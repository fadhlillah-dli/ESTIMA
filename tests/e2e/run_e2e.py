#!/usr/bin/env python3
"""Uji ujung-ke-ujung ESTIMA: menjalankan biner yang sebenarnya dengan sesi ketikan terskrip.

Pemakaian:
    python3 tests/e2e/run_e2e.py --exe build/estima [--verbose] [--only nama ...]

Tiap skenario (lihat sessions.py) dijalankan di folder kerja sementara dengan lingkungan bersih,
lalu stdout, stderr, kode keluar, dan berkas yang ditulis program dibandingkan dengan harapannya.
Kode keluar skrip ini 0 hanya bila semua skenario lulus.
"""

import argparse
import os
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from typing import List, Optional, Sequence

# Modul saudara (harness, sessions) diimpor dari folder ini; tanpa baris ini Python menulis
# folder __pycache__ ke dalam pohon sumber setiap kali uji dijalankan.
sys.dont_write_bytecode = True

from harness import (ESC, Mismatch, Run, Session, SessionError, derailment, existing_exe,
                     first_missing, flatten, require, run_session, strip_ansi, use_utf8_output)
from sessions import ALL_SESSIONS

WORKERS = 8


def check_streams(session: Session, run: Run) -> None:
    """Saluran galat dan warna.

    Pesan galat hanya boleh ada bila skenario memintanya (dan lalu harus ada di stderr, bukan
    stdout). Kode ESC hanya boleh ada bila warna dinyalakan: kebocoran warna ke keluaran yang
    ditulis ke berkas atau dibaca program lain adalah kesalahan yang mudah lolos tanpa uji ini.
    """
    outcome = run.outcome
    if session.stderr:
        missing = first_missing(" ".join(strip_ansi(outcome.stderr).split()), session.stderr)
        require(missing is None, "stderr tidak memuat %r berurutan; isinya %r"
                % (missing, outcome.stderr))
    else:
        require(outcome.stderr == "", "stderr seharusnya kosong, isinya %r" % outcome.stderr)
    if session.colored:
        require(ESC in outcome.stdout, "layar tidak memuat kode warna ANSI")
    else:
        require(ESC not in outcome.stdout + outcome.stderr, "ada kode ESC padahal warna mati")


def check_screen(session: Session, run: Run) -> None:
    """Isi layar: teks berurutan (expect) dan potongan persis (exact)."""
    stdout = run.outcome.stdout
    missing = first_missing(flatten(stdout), session.expect)
    require(missing is None, "layar tidak memuat %r pada urutan yang diharapkan" % missing)
    plain = strip_ansi(stdout)
    for piece in session.exact:
        require(piece in plain, "layar tidak memuat potongan persis %r" % piece)


def check_files(session: Session, run: Run) -> None:
    """Berkas di folder kerja sesudah sesi.

    Laporan yang disimpan harus ASCII murni tanpa kode ESC dan memuat isi yang diharapkan.
    Berkas yang sudah ada dan tidak boleh ditimpa harus utuh, dan program tidak boleh
    meninggalkan berkas lain.
    """
    files = run.outcome.files
    for name in session.saved:
        require(name in files, "berkas %s tidak ditulis program; adanya %s" % (name, sorted(files)))
        data = files[name]
        require(ESC.encode() not in data, "berkas %s memuat kode ESC" % name)
        require(all(byte < 128 for byte in data), "berkas %s memuat byte di luar ASCII" % name)
        # Seperti keluaran layar, akhir baris bawaan sistem (CRLF di Windows) disamakan dulu.
        text = data.decode("ascii").replace(os.linesep, "\n")
        missing = first_missing(flatten(text), session.report)
        require(missing is None, "berkas %s tidak memuat %r berurutan" % (name, missing))
    for name, content in session.files.items():
        if name not in session.saved:
            require(files.get(name) == content.encode("utf-8"),
                    "berkas lama %s berubah padahal tidak boleh ditimpa" % name)
    stray = set(files) - set(session.saved) - set(session.files)
    require(not stray, "program meninggalkan berkas yang tidak diharapkan: %s" % sorted(stray))


def evaluate(session: Session, run: Run) -> None:
    """Semua pemeriksaan satu skenario; berhenti di keluhan pertama lewat Mismatch."""
    problem = derailment(session, run.outcome)
    require(problem is None, str(problem))
    check_streams(session, run)
    check_screen(session, run)
    check_files(session, run)
    for check in session.checks:
        check(run)


def execute(exe: str, session: Session, show_screen: bool) -> Optional[str]:
    """Menjalankan satu skenario; hasilnya None bila lulus, selain itu keterangan kegagalan.

    Dengan show_screen, layar lengkap ikut dilampirkan supaya penyebabnya bisa dibaca langsung.
    """
    try:
        run = Run(exe, session, run_session(exe, session))
    except SessionError as error:
        return str(error)
    try:
        evaluate(session, run)
    except Mismatch as error:
        if show_screen:
            return "%s\n--- layar ---\n%s--- akhir layar ---" % (error, run.outcome.stdout)
        return str(error)
    return None


def select(names: Sequence[str]) -> List[Session]:
    """Skenario yang dijalankan: semuanya, atau hanya yang disebut lewat --only."""
    if not names:
        return list(ALL_SESSIONS)
    chosen = [session for session in ALL_SESSIONS if session.name in names]
    unknown = sorted(set(names) - set(session.name for session in chosen))
    if unknown:
        raise SystemExit("skenario tidak dikenal: %s" % ", ".join(unknown))
    return chosen


def main() -> int:
    parser = argparse.ArgumentParser(description="Uji ujung-ke-ujung ESTIMA.")
    parser.add_argument("--exe", required=True, type=existing_exe, help="jalur biner estima")
    parser.add_argument("--verbose", action="store_true",
                        help="tampilkan tujuan tiap skenario dan layar skenario yang gagal")
    parser.add_argument("--only", nargs="+", default=[], metavar="NAMA",
                        help="jalankan hanya skenario dengan nama ini")
    options = parser.parse_args()
    use_utf8_output()

    names = [session.name for session in ALL_SESSIONS]
    if len(set(names)) != len(names):
        raise SystemExit("nama skenario di sessions.py tidak unik")

    sessions = select(options.only)
    started = time.time()
    with ThreadPoolExecutor(max_workers=WORKERS) as pool:
        results = list(pool.map(lambda session: execute(options.exe, session, options.verbose), sessions))

    failed = 0
    for session, problem in zip(sessions, results):
        label = "LULUS" if problem is None else "GAGAL"
        detail = "  (%s)" % session.purpose if options.verbose else ""
        print("[%s] %s%s" % (label, session.name, detail))
        if problem is not None:
            failed += 1
            print("      %s" % problem.replace("\n", "\n      "))

    print()
    print("%d skenario dijalankan dalam %.1f detik: %d lulus, %d gagal."
          % (len(sessions), time.time() - started, len(sessions) - failed, failed))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
