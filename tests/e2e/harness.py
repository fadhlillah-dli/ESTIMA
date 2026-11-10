"""Alat bersama uji ujung-ke-ujung (e2e) ESTIMA.

Modul ini dipakai oleh run_e2e.py (uji perilaku) dan check_layout.py (gate lebar baris):

  - Session, Outcome, Run : bentuk data satu skenario dan hasil menjalankannya;
  - run_session()         : menjalankan biner yang sebenarnya dengan ketikan terskrip;
  - alat teks             : membuang kode warna, merapatkan transkrip, memeriksa tata letak.

Hanya pustaka standar Python 3.8+ yang dipakai, supaya uji bisa dijalankan di Linux, macOS, dan
Windows tanpa memasang apa pun.
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field, replace
from pathlib import Path
from typing import Callable, Iterator, Mapping, NamedTuple, Optional, Sequence, Tuple

ESC = "\x1b"
DEFAULT_WIDTH = 100
TIMEOUT_SECONDS = 60

# Kalimat penutup program dan awalan pesan kesalahannya; hampir semua skenario memeriksanya.
PROGRAM_DONE = "[OK] Program selesai. Terima kasih."
ERROR_MARK = "[!] "

# Nomor versi hanya ditulis di satu tempat (kontrak proyek), jadi uji membacanya dari sana.
APP_INFO_HEADER = Path(__file__).resolve().parents[2] / "include" / "estima" / "app_info.h"

# Di Windows tiap proses anak diberi konsol tersembunyi miliknya sendiri. ESTIMA memindahkan
# code page konsol ke UTF-8 lalu mengembalikannya saat selesai; bila puluhan sesi berbagi konsol
# penguji, urutan simpan-kembalikan itu saling bertabrakan dan konsol penguji tertinggal di
# UTF-8. Di sistem lain nilainya 0 (tidak ada yang diubah).
CREATION_FLAGS = getattr(subprocess, "CREATE_NO_WINDOW", 0)

ANSI_COLOR = re.compile(r"\x1b\[[0-9;]*m")


class BoxGlyphs(NamedTuple):
    """Karakter penyusun kotak dan batang grafik pada satu mode gambar."""

    top_left: str
    top_right: str
    bottom_left: str
    bottom_right: str
    horizontal: str
    vertical: str
    bar: str


# Karakter UTF-8 ditulis sebagai kode supaya berkas ini terbaca utuh di penyunting mana pun.
UNICODE_BOX = BoxGlyphs("┌", "┐", "└", "┘", "─", "│", "█")
ASCII_BOX = BoxGlyphs("+", "+", "+", "+", "-", "|", "#")


@dataclass(frozen=True)
class Session:
    """Satu skenario: apa yang diketik, cara program dijalankan, dan apa yang harus terjadi."""

    name: str                       # nama pendek, dipakai di laporan uji
    purpose: str                    # satu kalimat: perilaku apa yang dibuktikan
    typed: Tuple[str, ...] = ()     # ketikan, satu butir per baris ("" = hanya menekan Enter)

    # Cara program dijalankan.
    args: Tuple[str, ...] = ()                              # opsi baris perintah
    env: Mapping[str, str] = field(default_factory=dict)    # variabel lingkungan tambahan
    width: Optional[int] = DEFAULT_WIDTH                    # nilai COLUMNS; None = tidak diatur
    newline: str = "\n"                                     # akhir baris pada ketikan
    unterminated: bool = False                              # True = baris terakhir tanpa akhir baris
    files: Mapping[str, str] = field(default_factory=dict)  # berkas yang sudah ada di folder kerja

    # Hal yang harus terjadi.
    exit_code: int = 0
    expect: Tuple[str, ...] = ()    # teks yang harus muncul BERURUTAN di layar (lihat flatten)
    exact: Tuple[str, ...] = ()     # potongan yang harus muncul persis, termasuk spasinya
    errors: int = 0                 # banyaknya pesan kesalahan "[!]" di layar
    final: str = PROGRAM_DONE       # teks penutup layar; bukti sesi berjalan sampai akhir
    stderr: Tuple[str, ...] = ()    # teks berurutan di saluran galat; kosong = harus kosong
    colored: bool = False           # True = layar harus memuat kode warna ANSI
    saved: Tuple[str, ...] = ()     # berkas laporan yang harus ditulis program
    report: Tuple[str, ...] = ()    # teks berurutan yang harus ada di tiap berkas laporan itu
    checks: Tuple[Callable[["Run"], None], ...] = ()    # pemeriksaan khusus (lihat checks.py)

    def stdin(self) -> bytes:
        """Ketikan dalam bentuk byte yang dikirim ke masukan standar program."""
        text = self.newline.join(self.typed)
        if self.typed and not self.unterminated:
            text += self.newline
        return text.encode("utf-8")


class Outcome(NamedTuple):
    """Hasil menjalankan satu sesi. Teks sudah didekode dan akhir barisnya selalu "\\n"."""

    exit_code: int
    stdout: str
    stderr: str
    files: Mapping[str, bytes]      # isi folder kerja setelah program selesai


class Run(NamedTuple):
    """Sesi yang sudah dijalankan, lengkap dengan cara mengulanginya dengan pengaturan lain."""

    exe: str
    session: Session
    outcome: Outcome

    def again(self, **changes) -> Outcome:
        """Menjalankan sesi yang sama dengan pengaturan lain, misalnya lebar atau opsi berbeda."""
        return run_session(self.exe, replace(self.session, **changes))


class SessionError(Exception):
    """Sesi tidak menghasilkan keluaran yang bisa diperiksa (macet atau bukan UTF-8)."""


class Mismatch(Exception):
    """Hasil sesi tidak sesuai harapan; pesannya menjelaskan apa yang berbeda."""


def require(condition: bool, message: str) -> None:
    """Menghentikan satu pemeriksaan dengan pesan bila syaratnya tidak terpenuhi."""
    if not condition:
        raise Mismatch(message)


def session_environment(session: Session) -> Mapping[str, str]:
    """Lingkungan proses untuk satu sesi.

    Pengaturan warna dan lebar milik terminal penguji dibuang dulu, supaya hasil uji tidak
    bergantung pada tempat uji dijalankan; setelah itu barulah pengaturan skenario dipasang.
    """
    environment = dict(os.environ)
    for name in ("NO_COLOR", "FORCE_COLOR", "COLUMNS"):
        environment.pop(name, None)
    if session.width is not None:
        environment["COLUMNS"] = str(session.width)
    environment.update(session.env)
    return environment


def decode_output(data: bytes) -> str:
    """Mengubah byte keluaran program menjadi teks.

    Keluaran wajib UTF-8 yang sah. Hanya akhir baris bawaan sistem yang disamakan menjadi "\\n"
    (di Windows "\\r\\n"); "\\r" lain dibiarkan supaya ketahuan sebagai kesalahan.
    """
    try:
        text = data.decode("utf-8")
    except UnicodeDecodeError as error:
        raise SessionError("keluaran program bukan UTF-8 yang sah (%s)" % error)
    return text.replace(os.linesep, "\n")


def run_session(exe: str, session: Session) -> Outcome:
    """Menjalankan biner dengan ketikan sesi dan mengembalikan semua yang dihasilkannya.

    Tiap sesi mendapat folder kerja sementara yang baru: laporan yang disimpan program tidak
    pernah mengotori repositori, dan sesi yang satu tidak bisa memengaruhi sesi yang lain.
    """
    with tempfile.TemporaryDirectory(prefix="estima-e2e-") as workdir:
        for name, content in session.files.items():
            Path(workdir, name).write_bytes(content.encode("utf-8"))
        try:
            done = subprocess.run(
                [exe] + list(session.args),
                input=session.stdin(),
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                env=session_environment(session),
                cwd=workdir,
                timeout=TIMEOUT_SECONDS,
                creationflags=CREATION_FLAGS,
                check=False,
            )
        except subprocess.TimeoutExpired:
            raise SessionError("program tidak selesai dalam %d detik" % TIMEOUT_SECONDS)
        files = {name: Path(workdir, name).read_bytes() for name in sorted(os.listdir(workdir))}
    return Outcome(done.returncode, decode_output(done.stdout), decode_output(done.stderr), files)


def strip_ansi(text: str) -> str:
    """Membuang kode warna ANSI, menyisakan teks yang terlihat di layar."""
    return ANSI_COLOR.sub("", text)


def squeeze(text: str) -> str:
    """Merapatkan semua deret spasi dan ganti baris menjadi satu spasi."""
    return " ".join(text.split())


def without_frame(line: str) -> str:
    """Isi satu baris layar tanpa bingkai kotaknya.

    Garis tepi kiri-kanan dibuang, garis atas/bawah disisakan judulnya saja, dan garis pemisah
    di dalam tabel dikosongkan. Baris di luar kotak dikembalikan apa adanya.
    """
    if len(line) < 2:
        return line
    for box in (UNICODE_BOX, ASCII_BOX):
        if line[0] == box.vertical and line[-1] == box.vertical:
            inner = line[1:-1]
            return inner if inner.strip(box.horizontal + " ") else ""
        if line[0] in (box.top_left, box.bottom_left) and line[-1] in (box.top_right, box.bottom_right):
            return line[1:-1].strip(box.horizontal + " ")
    return line


def flatten(text: str) -> str:
    """Menjadikan transkrip satu baris panjang yang mudah dicari isinya.

    Kode warna, bingkai kotak, dan garis tabel dibuang, lalu semua deret spasi dan ganti baris
    dirapatkan menjadi satu spasi. Dengan begitu harapan uji cukup menulis isi yang harus
    tampil, tanpa bergantung pada lebar kolom tabel atau di kata mana sebuah kalimat dilipat.
    """
    return squeeze(" ".join(without_frame(line) for line in strip_ansi(text).split("\n")))


def first_missing(flat_text: str, wanted: Sequence[str]) -> Optional[str]:
    """Mencari tiap teks secara berurutan; hasilnya teks pertama yang tidak ketemu, atau None.

    Urutan ikut diperiksa: pencarian teks berikutnya dimulai dari akhir teks sebelumnya, jadi
    pesan yang benar tetapi muncul di tempat yang salah tetap dianggap hilang.
    """
    position = 0
    for item in wanted:
        needle = squeeze(item)
        found = flat_text.find(needle, position)
        if found < 0:
            return item
        position = found + len(needle)
    return None


def count_errors(text: str) -> int:
    """Banyaknya pesan kesalahan di layar.

    Tiap pesan diawali "[!] " di kolom pertama; baris lanjutannya menjorok, jadi pesan yang
    dilipat menjadi beberapa baris tetap dihitung satu.
    """
    return sum(1 for line in strip_ansi(text).split("\n") if line.startswith(ERROR_MARK))


def layout_problems(lines: Sequence[str], limit: int, box: BoxGlyphs,
                    measure: Callable[[str], int] = len) -> Iterator[Tuple[int, str]]:
    """Mencari pelanggaran tata letak; hasilnya pasangan (nomor baris, keluhan).

    Dua aturan diperiksa pada baris-baris yang sudah bebas kode warna:
      1. tidak ada baris yang lebih lebar dari limit kolom;
      2. semua baris sebuah kotak sama lebar dengan garis atasnya dan bertepi di kedua ujung.
    Lebar dihitung per karakter karena semua karakter yang dicetak ESTIMA selebar satu kolom.
    """
    box_width = None    # lebar kotak yang sedang terbuka; None berarti sedang di luar kotak
    for number, line in enumerate(lines, start=1):
        width = measure(line)
        if width > limit:
            yield number, "%d kolom, batas %d" % (width, limit)

        if box_width is not None:
            closing = line.startswith(box.bottom_left)
            if closing or line.startswith(box.vertical):
                if len(line) != box_width:
                    yield number, "baris kotak %d kolom, garis atasnya %d kolom" % (len(line), box_width)
                if not line.endswith(box.bottom_right if closing else box.vertical):
                    yield number, "tepi kanan kotak tidak ada"
                if closing:
                    box_width = None
                continue
            yield number, "kotak terputus sebelum garis bawahnya"
            box_width = None

        if line.startswith(box.top_left):
            box_width = len(line)
            if not line.endswith(box.top_right):
                yield number, "garis atas kotak tidak berakhir di sudut kanan"
        elif line[:1] in (box.vertical, box.bottom_left):
            yield number, "baris kotak muncul tanpa garis atas"
    if box_width is not None:
        yield len(lines), "kotak terakhir tidak ditutup garis bawah"


def read_app_version() -> str:
    """Nomor versi program menurut include/estima/app_info.h."""
    try:
        header = APP_INFO_HEADER.read_text(encoding="utf-8")
    except OSError as error:
        raise Mismatch("%s tidak bisa dibaca (%s)" % (APP_INFO_HEADER, error))
    found = re.search(r'APP_VERSION\s*=\s*"([^"]+)"', header)
    if found is None:
        raise Mismatch("APP_VERSION tidak ditemukan di %s" % APP_INFO_HEADER)
    return found.group(1)


def existing_exe(path: str) -> str:
    """Jenis argumen --exe: jalur mutlak biner yang diuji, ditolak bila berkasnya tidak ada.

    Jalur dibuat mutlak karena tiap sesi dijalankan dari folder kerja sementara.
    """
    exe = os.path.abspath(path)
    if not os.path.isfile(exe):
        raise argparse.ArgumentTypeError("biner tidak ditemukan: %s" % exe)
    return exe


def use_utf8_output() -> None:
    """Menulis laporan uji sebagai UTF-8 di sistem apa pun.

    Transkrip yang ditampilkan saat uji gagal memuat karakter garis kotak. Tanpa pengaturan
    ini Python di Windows memakai code page lama ketika keluarannya dialihkan (misalnya oleh
    CTest) dan berhenti dengan UnicodeEncodeError tepat ketika transkrip paling dibutuhkan.
    """
    for stream in (sys.stdout, sys.stderr):
        stream.reconfigure(encoding="utf-8", errors="backslashreplace")


def derailment(session: Session, outcome: Outcome) -> Optional[str]:
    """Keluhan bila sesi tidak berjalan sampai akhir yang diharapkan, atau None bila sehat.

    Ini pemeriksaan paling dasar dan dipakai kedua skrip uji: kode keluar, banyaknya pesan
    kesalahan, dan teks penutup layar. Gate tata letak memakainya supaya sesi yang diam-diam
    tersesat (misalnya menu salah terbuka pada lebar tertentu) tidak dianggap lulus hanya
    karena barisnya cukup pendek.
    """
    if outcome.exit_code != session.exit_code:
        return "kode keluar %d, seharusnya %d" % (outcome.exit_code, session.exit_code)
    shown = count_errors(outcome.stdout)
    if shown != session.errors:
        return "ada %d pesan [!], seharusnya %d" % (shown, session.errors)
    flat = flatten(outcome.stdout)
    wanted = squeeze(session.final)
    if wanted and not flat.endswith(wanted):
        return "layar tidak berakhir dengan %r, melainkan %r" % (wanted, flat[-len(wanted) - 30:])
    return None
