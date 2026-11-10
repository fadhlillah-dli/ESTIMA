#!/usr/bin/env python3
"""Gate tata letak ESTIMA: tidak ada baris yang melebihi lebar layar pada lebar 40-120 kolom.

Pemakaian:
    python3 tests/e2e/check_layout.py --exe build/estima [--min 40] [--max 120] [--show 20]

Untuk setiap lebar, sesi menu di sessions.py (SCREEN_SESSIONS) dijalankan tiga kali: polos,
dengan FORCE_COLOR=1, dan dengan --ascii. Tiap keluaran diperiksa:
  - tidak ada baris buatan program yang lebih lebar dari COLUMNS-1 kolom setelah kode warna
    dibuang (kolom terakhir dibiarkan kosong supaya terminal tidak melipat baris sendiri);
  - semua baris dalam satu kotak selebar garis atasnya;
  - sesinya sampai ke akhir yang diharapkan, sehingga sesi yang diam-diam tersesat tidak lulus.
Kode keluar 0 hanya bila tidak ada pelanggaran.
"""

import argparse
import re
import sys
import time
from concurrent.futures import ThreadPoolExecutor
from dataclasses import dataclass, replace
from typing import Iterator, List, Optional, Sequence, Tuple

# Modul saudara (harness, sessions) diimpor dari folder ini; tanpa baris ini Python menulis
# folder __pycache__ ke dalam pohon sumber setiap kali uji dijalankan.
sys.dont_write_bytecode = True

from harness import (ASCII_BOX, ESC, UNICODE_BOX, Outcome, Session, SessionError, derailment,
                     existing_exe, layout_problems, run_session, strip_ansi, use_utf8_output)
from sessions import PROMPT_LABELS, SCREEN_SESSIONS

WORKERS = 8
MODES = ("polos", "warna", "ascii")


def prompt_pattern(labels: Sequence[str]) -> "re.Pattern[str]":
    """Pola baris pertanyaan: label, spasi perata (bila ada), lalu ": ".

    Tanda # pada label berarti angka. Program mencetak ulang jawaban di belakang pertanyaan
    ketika stdin bukan terminal; jawaban itu bukan buatan program, jadi tidak ikut diukur.
    """
    parts = [re.escape(label).replace(re.escape("#"), r"\d+") for label in labels]
    return re.compile("(?:%s) *: " % "|".join(parts))


PROMPT = prompt_pattern(PROMPT_LABELS)


def printed_width(line: str) -> int:
    """Lebar bagian baris yang dicetak program; pada baris pertanyaan hanya pertanyaannya."""
    found = PROMPT.match(line)
    return len(found.group(0)) if found else len(line)


@dataclass(frozen=True)
class Violation:
    width: int
    mode: str
    scenario: str
    line_number: int
    text: str
    complaint: str


def configure(session: Session, width: int, mode: str) -> Session:
    """Sesi yang sama untuk lebar dan mode tertentu."""
    if mode == "ascii":
        return replace(session, width=width, args=("--ascii",) + session.args)
    if mode == "warna":
        return replace(session, width=width, env=dict(session.env, FORCE_COLOR="1"))
    return replace(session, width=width)


def screen_problems(outcome: Outcome, width: int, mode: str) -> Iterator[Tuple[int, str, str]]:
    """Pelanggaran pada satu layar sebagai (nomor baris, baris, keluhan)."""
    if mode == "ascii" and any(ord(char) > 127 for char in outcome.stdout):
        yield 0, "", "mode --ascii mengeluarkan karakter di luar ASCII"
    lines = strip_ansi(outcome.stdout).split("\n")
    box = ASCII_BOX if mode == "ascii" else UNICODE_BOX
    for number, complaint in layout_problems(lines, width - 1, box, printed_width):
        yield number, lines[number - 1], complaint
    for number, line in enumerate(lines, start=1):
        if ESC in line:
            yield number, line, "kode ESC yang bukan warna SGR tertinggal di baris"


def check_one(exe: str, session: Session, width: int, mode: str) -> List[Violation]:
    """Menjalankan satu sesi pada satu lebar dan mode, lalu mengumpulkan pelanggarannya."""
    def found(number: int, text: str, complaint: str) -> Violation:
        return Violation(width, mode, session.name, number, text, complaint)

    configured = configure(session, width, mode)
    try:
        outcome = run_session(exe, configured)
    except SessionError as error:
        return [found(0, "", str(error))]
    problems = [found(*item) for item in screen_problems(outcome, width, mode)]
    derailed = derailment(configured, outcome)
    if derailed is not None:
        problems.append(found(0, "", "sesi tersesat: " + derailed))
    return problems


def check_all(exe: str, widths: Sequence[int]) -> Tuple[int, List[Violation]]:
    """Semua kombinasi lebar x mode x sesi; hasilnya banyaknya sesi dan pelanggaran terurut."""
    jobs = [(session, width, mode)
            for width in widths for mode in MODES for session in SCREEN_SESSIONS]
    with ThreadPoolExecutor(max_workers=WORKERS) as pool:
        results = pool.map(lambda job: check_one(exe, *job), jobs)
        violations = [violation for group in results for violation in group]
    return len(jobs), violations


def parse_options(argv: Optional[Sequence[str]] = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Gate lebar baris keluaran ESTIMA.")
    parser.add_argument("--exe", required=True, type=existing_exe, help="jalur biner estima")
    parser.add_argument("--min", type=int, default=40, help="lebar terkecil (bawaan 40)")
    parser.add_argument("--max", type=int, default=120, help="lebar terbesar (bawaan 120)")
    parser.add_argument("--show", type=int, default=20,
                        help="banyaknya pelanggaran pertama yang dicetak (bawaan 20)")
    options = parser.parse_args(argv)
    if options.min < 2 or options.min > options.max:
        parser.error("rentang lebar tidak masuk akal: %d-%d" % (options.min, options.max))
    return options


def main() -> int:
    options = parse_options()
    use_utf8_output()
    started = time.time()
    count, violations = check_all(options.exe, range(options.min, options.max + 1))
    seconds = time.time() - started

    for item in violations[:options.show]:
        where = "baris %d" % item.line_number if item.line_number else "sesi"
        print("[PELANGGARAN] lebar %d, mode %s, skenario %s, %s: %s"
              % (item.width, item.mode, item.scenario, where, item.complaint))
        if item.text:
            print("    %r" % item.text)
    if len(violations) > options.show:
        print("... dan %d pelanggaran lain tidak ditampilkan." % (len(violations) - options.show))

    summary = "%d sesi (lebar %d-%d, %d mode, %d skenario) diperiksa dalam %.1f detik: "
    print(summary % (count, options.min, options.max, len(MODES), len(SCREEN_SESSIONS), seconds)
          + ("%d pelanggaran." % len(violations) if violations else "tidak ada pelanggaran."))
    return 1 if violations else 0


if __name__ == "__main__":
    sys.exit(main())
