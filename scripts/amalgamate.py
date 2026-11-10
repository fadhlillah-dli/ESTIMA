#!/usr/bin/env python3
"""Menyusun berkas satu-sumber amalgamation/UTS_2531070011.cpp dari modul-modul program.

Program ESTIMA ditulis dalam beberapa modul (include/estima/*.h dan src/*.cpp), sedangkan soal
UTS meminta satu berkas .cpp. Skrip ini menyalin isi semua modul ke satu berkas dalam urutan
yang membuatnya bisa dikompilasi sendirian, tanpa opsi -I:

    g++ amalgamation/UTS_2531070011.cpp -o estima

Pemakaian:

    python3 scripts/amalgamate.py                  menyusun ulang berkas gabungan
    python3 scripts/amalgamate.py --check          memeriksa saja, tanpa menulis apa pun
    python3 scripts/amalgamate.py --output JALUR   menulis (atau memeriksa) berkas di JALUR

Susunan berkas gabungan:

    1. kepala berkas: identitas, cara mengompilasi, letak unsur yang diminta soal, daftar isi;
    2. isi platform.h, karena harus mendahului semua #include;
    3. satu blok berisi semua #include pustaka standar dari seluruh modul;
    4. tiap modul sebagai satu bagian bernomor: semua header dulu, lalu semua berkas sumber.

Dari isi modul, yang diubah hanya hal yang tidak diperlukan lagi di dalam satu berkas:

    - baris pertama modul (judul) menjadi kepala bagian;
    - #include "estima/..." dan penjaga include (#ifndef/#define/#endif) dibuang, berikut
      komentar yang menjelaskannya: komentar di ujung barisnya, baris komentar lanjutannya tepat
      di bawahnya, dan komentar di antara #ifndef dan #define penjaga;
    - #include pustaka standar di kolom 0 dipindahkan ke blok bersama, berikut komentar di
      ujung barisnya dan baris komentar lanjutannya;
    - deretan baris kosong dirapatkan menjadi satu.

Baris lain disalin apa adanya, termasuk #include yang dijorokkan (pustaka khusus sistem operasi
di dalam #ifdef), yang harus tetap di tempatnya.

Isi berkas gabungan hanya ditentukan oleh isi modul: masukan yang sama selalu menghasilkan
keluaran yang sama byte demi byte (ASCII, akhir baris LF). Hal yang tidak dipahami skrip ini
dilaporkan sebagai galat yang menyebut berkas dan nomor barisnya; skrip tidak menebak.

Kode keluar: 0 berhasil; 1 --check menemukan berkas gabungan yang tidak sesuai; 2 modul tidak
bisa digabung atau opsi salah.
"""

import argparse
import difflib
import re
import sys
from pathlib import Path
from typing import Dict, List, NamedTuple, NoReturn, Optional, Sequence, Tuple

PROGRAM = "amalgamate.py"

# Nama berkas yang dikumpulkan ditentukan soal (UTS_<NIM>.cpp). Kepala berkas selalu menyebut
# nama ini, juga ketika --output menunjuk ke jalur lain, supaya isi berkas hanya bergantung pada
# isi modul.
OUTPUT_NAME = "UTS_2531070011.cpp"
DEFAULT_OUTPUT = "amalgamation/" + OUTPUT_NAME

COURSE = "UTS Algoritma dan Pemrograman - Semester Ganjil TA. 2026/2027 - Kelas 26FA"
STUDENT_NAME = "Fadhlillah"
STUDENT_ID = "2531070011"
LECTURER = "Ali Muhammad, S.Kom., M.Kom."

HEADER_FOLDER = "include/estima"
SOURCE_FOLDER = "src"

# platform.h tidak bernomor: isinya ditaruh paling depan karena harus mendahului semua #include.
PLATFORM_HEADER = "platform"

# Urutan bagian bernomor. Sebuah header hanya boleh meng-include header yang muncul lebih dulu di
# daftar ini (diperiksa saat menyaring), dan semua header mendahului semua berkas sumber. Modul
# baru harus didaftarkan di sini; berkas yang belum terdaftar dilaporkan sebagai galat supaya
# tidak ada modul yang diam-diam tertinggal dari berkas gabungan.
HEADER_ORDER = (
    "app_info", "component", "calculation", "material_catalog", "text_format", "input_parse",
    "terminal", "ui", "prompt", "report", "app",
)
SOURCE_ORDER = (
    "component", "calculation", "material_catalog", "text_format", "input_parse", "terminal",
    "ui", "prompt", "report", "app", "main",
)

# Folder dan akhiran berkas yang diperiksa ketika mencari modul yang belum terdaftar.
MODULE_ROOTS = ("include", "src")
MODULE_SUFFIXES = (".h", ".hh", ".hpp", ".hxx", ".inl", ".c", ".cc", ".cpp", ".cxx")

EXIT_OK = 0
EXIT_OUTDATED = 1
EXIT_ERROR = 2

# Lebar kepala berkas mengikuti lebar komentar di modul (100 kolom); garis pembatas melebar
# sendiri bila ada judul bagian yang lebih panjang.
MIN_RULE_WIDTH = 100
PROSE_WIDTH = 95
DIFF_LIMIT = 40

TITLE_PATTERN = re.compile(r"^// ===== (\S+) - (.+) =====$")
DIRECTIVE_PATTERN = re.compile(r"^(\s*)#\s*([a-z]+)\b(.*)$")
INCLUDE_PATTERN = re.compile(r'^(\s*)#\s*include\s*(?:<([^<>"]+)>|"([^<>"]+)")\s*(//.*)?$')
LOCAL_INCLUDE_PATTERN = re.compile(r"^estima/(\w+)\.h$")
STATIC_NAME_PATTERN = re.compile(r"^static\b[^=;(\[]*?(\w+)\s*[=;(\[]")
APP_INFO_PATTERN = re.compile(r'^const char\* const (APP_[A-Z_]+) = "([^"\\]*)";')
UNSUPPORTED_BYTE = re.compile(rb"[^\t\n\x20-\x7e]")

# Komentar ujung baris untuk pustaka standar yang di-#include oleh lebih dari satu modul.
# Komentar di tiap modul hanya menyebut pemakaian di modul itu, padahal di blok bersama satu
# baris #include melayani seluruh program. Karena itu komentarnya ditulis di sini dan harus
# menyebut semua pemakaiannya di berkas gabungan. Baris kedua dan seterusnya menjadi baris
# lanjutan yang menjorok sampai kolom komentar. Bila ada pustaka baru yang dipakai beberapa
# modul tetapi belum punya komentar di sini, skrip berhenti dengan galat (tidak menebak).
SHARED_INCLUDE_COMMENTS: Dict[str, List[str]] = {
    "cmath": [
        "// <cmath> digunakan untuk std::floor (pembulatan ke bawah): dasar pembulatan berat dan biaya, pembulatan",
        "// rupiah, gram, dan persen saat ditulis sebagai teks, serta panjang batang grafik di printRanking().",
    ],
    "cstddef": [
        "// <cstddef> digunakan untuk tipe std::size_t (ukuran, lebar, dan posisi teks di hampir semua bagian) dan NULL,",
        "// yaitu nilai penunjuk kosong yang dipakai bersama std::getenv, std::strtol, std::time, dan std::localtime.",
    ],
    "cstring": [
        "// <cstring> digunakan untuk std::strcmp, pembanding dua teks gaya C (const char*), yang dipakai findMaterialByName()",
        "// serta decideColor() dan initTerminal() untuk memeriksa isi variabel lingkungan FORCE_COLOR dan TERM.",
    ],
    "fstream": [
        "// <fstream> digunakan untuk aliran berkas: std::ofstream menulis laporan .txt di saveReport(), dan std::ifstream",
        "// dipakai fileExists() untuk memeriksa apakah berkas laporan dengan nama itu sudah ada.",
    ],
    "iostream": [
        "// <iostream> digunakan untuk std::cout (keluaran ke layar), std::cin (sumber ketikan yang dibaca per baris oleh",
        "// readLine() dan waitForEnter()), dan std::cerr (saluran pesan galat, misalnya untuk opsi yang salah).",
    ],
    "ostream": [
        "// <ostream> digunakan untuk tipe std::ostream, yaitu aliran keluaran umum yang menjadi parameter out fungsi gambar",
        "// dan cetak, sehingga keluaran yang sama bisa ditulis ke std::cout, std::cerr, atau berkas laporan.",
    ],
    "string": [
        "// <string> digunakan untuk tipe std::string, yaitu teks yang panjangnya bisa berubah (nama komponen, ketikan,",
        "// pesan, dan baris tampilan), serta std::getline yang membaca satu baris utuh dari std::cin.",
    ],
    "vector": [
        "// <vector> digunakan untuk tipe std::vector, yaitu daftar yang panjangnya bisa bertambah: hasil wrapText(),",
        "// baris-baris isi kotak (StyledLine), serta kolom dan baris tabel.",
    ],
}

# Jenis baris sebelumnya, untuk mengenali komentar yang menempel pada baris yang tidak disalin.
# REMOVED_NOTED adalah baris yang tidak disalin dan punya komentar di ujungnya: baris komentar
# tepat di bawahnya dianggap lanjutan komentar itu dan ikut ke mana baris itu pergi.
BLANK, COMMENT, KEPT, REMOVED, REMOVED_NOTED = range(5)

# Tahap pembacaan penjaga include sebuah header.
GUARD_NONE, GUARD_AWAITED, GUARD_HALF_OPEN, GUARD_OPEN, GUARD_CLOSED = range(5)


def header_path(name: str) -> str:
    return "%s/%s.h" % (HEADER_FOLDER, name)


def source_path(name: str) -> str:
    return "%s/%s.cpp" % (SOURCE_FOLDER, name)


class ExamElement(NamedTuple):
    """Satu unsur yang diminta soal: modul tempatnya, pola baris penandanya, dan keterangannya."""

    path: str
    pattern: str
    description: str


# Unsur yang diminta soal dan tempatnya di kode. Nomor bagian dan nomor barisnya dihitung saat
# berkas disusun, dan skrip berhenti bila sebuah penanda tidak ditemukan lagi. Dengan begitu
# petunjuk di kepala berkas tidak bisa diam-diam menjadi usang ketika kodenya berubah.
EXAM_ELEMENTS = (
    ExamElement(header_path("component"), r"^struct Component\b",
                "struct Component - data satu komponen: nama, jumlah, berat, harga"),
    ExamElement(header_path("component"), r"^\s+Component items\[",
                "array items[] di dalam struct ComponentList - penyimpan semua komponen"),
    ExamElement(source_path("calculation"), r"^\w.*\bcalculateTotalWeight\(",
                "fungsi calculateTotalWeight() - total berat satu komponen"),
    ExamElement(source_path("calculation"), r"^\w.*\bsumTotalWeight\(",
                "fungsi sumTotalWeight() - total berat seluruh komponen"),
    ExamElement(source_path("calculation"), r"^\w.*\bcalculateTotalCost\(",
                "fungsi calculateTotalCost() - total biaya satu komponen"),
    ExamElement(source_path("calculation"), r"^\w.*\bsumTotalCost\(",
                "fungsi sumTotalCost() - total biaya seluruh komponen"),
    ExamElement(source_path("calculation"), r"^\w.*\bfindMostExpensive\(",
                "fungsi findMostExpensive() - mencari komponen dengan biaya tertinggi"),
    ExamElement(source_path("app"), r"^\w.*\brunMenuLoop\(",
                "runMenuLoop() - perulangan menu (while) dan percabangan pilihan (switch)"),
    ExamElement(source_path("report"), r"^\w.*\bprintRecap\(",
                "printRecap() - tampilan total tiap komponen, total keseluruhan, komponen termahal"),
)


class AmalgamationError(Exception):
    """Modul tidak bisa digabung; pesannya sudah menyebut berkas dan baris penyebabnya."""


def fail(path: str, number: Optional[int], message: str) -> NoReturn:
    location = path if number is None else "%s:%d" % (path, number)
    raise AmalgamationError("%s: %s" % (location, message))


class StandardInclude(NamedTuple):
    """Satu #include pustaka standar yang dipindah ke blok bersama, beserta komentarnya."""

    name: str
    # Blok komentar yang menempel tepat di atas baris #include.
    above: List[str]
    # Komentar di ujung baris #include, lalu baris lanjutannya tepat di bawahnya.
    trailing: List[str]


class Section(NamedTuple):
    """Hasil menyaring satu modul."""

    path: str
    title: str
    lines: List[str]
    includes: List[StandardInclude]
    static_names: List[Tuple[str, int]]
    has_platform_includes: bool


class ModuleFilter:
    """Menyaring satu modul baris demi baris: disalin, dibuang, atau dipindah ke blok bersama."""

    def __init__(self, path: str, visible_headers: Sequence[str]) -> None:
        self.path = path
        self.visible_headers = visible_headers
        self.is_header = path.endswith(".h")
        self.guard = "ESTIMA_%s_H" % Path(path).stem.upper()
        self.guard_state = GUARD_AWAITED if self.is_header else GUARD_NONE
        # Di dalam header, isi modul berada satu tingkat di dalam #ifndef penjaganya.
        self.base_depth = 1 if self.is_header else 0
        self.depth = 0
        # Baris kode atau direktif pertama yang disalin; #include di kolom 0 harus mendahuluinya.
        self.first_fixed_line = 0
        # Sebelum direktif pertama, komentar adalah pembuka modul dan selalu disalin.
        self.directive_seen = False
        self.previous = COMMENT
        self.lines: List[str] = []
        self.includes: List[StandardInclude] = []
        # Komentar pustaka standar yang menerima baris lanjutan; None bila lanjutannya dibuang.
        self.continued_comment: Optional[List[str]] = None
        self.static_names: List[Tuple[str, int]] = []
        self.has_platform_includes = False

    def run(self, lines: Sequence[str]) -> Section:
        name = Path(self.path).name
        title = TITLE_PATTERN.match(lines[0]) if lines else None
        if title is None or title.group(1) != name:
            fail(self.path, 1, 'baris pertama modul harus berbentuk "// ===== %s - <judul> =====".' % name)
        for number, line in enumerate(lines[1:], start=2):
            self._feed(number, line)
        if self.is_header and self.guard_state != GUARD_CLOSED:
            self._guard_problem(len(lines))
        if not self.is_header and self.depth != 0:
            fail(self.path, len(lines), "ada #if yang belum ditutup #endif sampai akhir berkas.")
        while self.lines and not self.lines[-1]:
            self.lines.pop()
        return Section(self.path, title.group(2), self.lines, self.includes, self.static_names,
                       self.has_platform_includes)

    def _feed(self, number: int, line: str) -> None:
        stripped = line.strip()
        if not stripped:
            self._keep("", BLANK)
        elif self.guard_state == GUARD_CLOSED:
            fail(self.path, number, "masih ada isi sesudah #endif penutup penjaga include.")
        elif stripped.startswith("//"):
            self._comment(number, line)
        elif stripped.startswith("/*"):
            fail(self.path, number, "komentar blok /* ... */ tidak didukung; tulis sebagai komentar //.")
        elif stripped.startswith("#"):
            self._directive(number, line)
        else:
            self._code(number, line)

    def _keep(self, line: str, kind: int) -> None:
        # Baris kosong hanya disalin bila baris sebelumnya berisi, sehingga deretan baris kosong
        # (termasuk yang muncul karena ada baris dibuang) tersisa satu.
        if line or not self.lines or self.lines[-1]:
            self.lines.append(line)
        self.previous = kind

    def _keep_fixed(self, number: int, line: str) -> None:
        if not self.first_fixed_line:
            self.first_fixed_line = number
        self._keep(line, KEPT)

    def _comment(self, number: int, line: str) -> None:
        # Komentar di antara #ifndef dan #define penjaga menjelaskan penjaga itu, jadi ikut dibuang.
        if self.guard_state == GUARD_HALF_OPEN:
            return
        if self.previous == REMOVED_NOTED:
            if self.continued_comment is not None:
                self.continued_comment.append(line.strip())
            return
        if self.previous == REMOVED:
            fail(self.path, number,
                 "komentar ini menempel di bawah baris %d, padahal baris itu tidak ikut disalin ke "
                 "bagiannya, sehingga komentar ini akan tertinggal sendirian. Jadikan komentar di ujung "
                 "baris %d, atau sisipkan baris kosong bila komentar ini milik kode di bawahnya."
                 % (number - 1, number - 1))
        self._keep(line, COMMENT)

    def _code(self, number: int, line: str) -> None:
        if self.guard_state in (GUARD_AWAITED, GUARD_HALF_OPEN):
            self._guard_problem(number)
        if not self.is_header:
            static = STATIC_NAME_PATTERN.match(line)
            if static is not None:
                self.static_names.append((static.group(1), number))
        self._keep_fixed(number, line)

    def _directive(self, number: int, line: str) -> None:
        match = DIRECTIVE_PATTERN.match(line)
        if match is None:
            fail(self.path, number, "baris praprosesor ini tidak dikenali.")
        indent, name, rest = match.groups()
        # Komentar di ujung baris dilepas dulu supaya nama penjaga bisa dicocokkan persis.
        operand = rest.split("//", 1)[0].strip()
        removed = False
        self.continued_comment = None
        if self.guard_state == GUARD_AWAITED:
            if indent or name != "ifndef" or operand != self.guard:
                self._guard_problem(number)
            self.guard_state = GUARD_HALF_OPEN
            self.depth = 1
            removed = True
        elif self.guard_state == GUARD_HALF_OPEN:
            if indent or name != "define" or operand != self.guard:
                self._guard_problem(number)
            self.guard_state = GUARD_OPEN
            removed = True
        elif name == "include":
            removed = self._include(number, line)
        elif name in ("if", "ifdef", "ifndef"):
            self.depth += 1
        elif name in ("elif", "elifdef", "elifndef", "else"):
            if self.depth == self.base_depth:
                fail(self.path, number, "#%s ini tidak punya #if pembuka di dalam modul." % name)
        elif name == "endif":
            self.depth -= 1
            if self.depth < self.base_depth:
                if not self.is_header:
                    fail(self.path, number, "#endif ini tidak punya #if pembuka di dalam modul.")
                self.guard_state = GUARD_CLOSED
                removed = True
        self.directive_seen = True
        if removed:
            self.previous = REMOVED_NOTED if "//" in rest else REMOVED
        else:
            self._keep_fixed(number, line)

    def _include(self, number: int, line: str) -> bool:
        """Memeriksa satu baris #include; hasil True berarti baris itu tidak disalin ke bagiannya."""
        match = INCLUDE_PATTERN.match(line)
        if match is None:
            fail(self.path, number,
                 'baris #include ini tidak dikenali. Bentuk yang didukung: #include <nama> atau '
                 '#include "estima/nama.h", boleh diikuti komentar //.')
        indent, standard, local, comment = match.groups()
        shown = "<%s>" % standard if standard else '"%s"' % local
        if indent:
            # Dijorokkan berarti sengaja diletakkan di tempat itu, misalnya di dalam #ifdef _WIN32.
            if local:
                fail(self.path, number,
                     "#include %s dijorokkan, padahal header modul selalu dibuang dari berkas gabungan. "
                     "Tulis di kolom 0 pada bagian atas modul." % shown)
            self.has_platform_includes = True
            return False
        if self.depth != self.base_depth:
            fail(self.path, number,
                 "#include %s di kolom 0 berada di dalam blok #if, sehingga tidak boleh dipindah ke luar "
                 "syaratnya. Jorokkan baris ini supaya tetap di tempatnya." % shown)
        if self.first_fixed_line:
            fail(self.path, number,
                 "#include %s muncul sesudah baris kode atau direktif pertama modul (baris %d), "
                 "sehingga tidak bisa dipindah tanpa mengubah urutan. Pindahkan ke bagian atas modul."
                 % (shown, self.first_fixed_line))
        # Sebelum direktif pertama, komentar adalah pembuka modul dan tetap di tempatnya.
        attached = self.previous == COMMENT and self.directive_seen
        if local:
            if attached:
                fail(self.path, number,
                     "komentar di baris %d menempel pada #include %s, padahal #include ini tidak ikut "
                     "disalin, sehingga komentar itu akan tertinggal sendirian. Jadikan komentar di ujung "
                     "baris #include, atau pisahkan dengan baris kosong." % (number - 1, shown))
            self._check_local_include(number, local)
        else:
            # Blok komentar yang menempel di atasnya dan komentar di ujung barisnya ikut pindah
            # bersama #include ini ke blok bersama.
            above = self._take_attached_comments() if attached else []
            trailing = [comment] if comment else []
            self.includes.append(StandardInclude(standard, above, trailing))
            self.continued_comment = trailing
        return True

    def _take_attached_comments(self) -> List[str]:
        """Mengambil kembali deretan baris komentar terakhir yang sudah disalin ke bagiannya."""
        start = len(self.lines)
        while start > 0 and self.lines[start - 1].strip().startswith("//"):
            start -= 1
        taken = [line.strip() for line in self.lines[start:]]
        del self.lines[start:]
        return taken

    def _check_local_include(self, number: int, local: str) -> None:
        match = LOCAL_INCLUDE_PATTERN.match(local)
        name = match.group(1) if match else ""
        if name != PLATFORM_HEADER and name not in HEADER_ORDER:
            fail(self.path, number,
                 '#include lokal tidak dikenal: "%s". Header modul harus terdaftar di HEADER_ORDER pada '
                 "scripts/amalgamate.py." % local)
        if name not in self.visible_headers:
            fail(self.path, number,
                 '"%s" baru muncul sesudah modul ini di berkas gabungan, jadi isinya belum dikenal di '
                 "sini. Perbaiki urutan HEADER_ORDER pada scripts/amalgamate.py." % local)

    def _guard_problem(self, number: int) -> NoReturn:
        fail(self.path, number,
             'header tanpa penjaga include yang diharapkan. Sesudah komentar pembuka harus ada '
             '"#ifndef %s", lalu "#define %s" (di antaranya hanya boleh ada komentar), dan modul '
             'harus ditutup "#endif" sebagai baris terakhir.' % (self.guard, self.guard))


def read_module(root: Path, path: str) -> List[str]:
    """Membaca satu modul sebagai daftar baris, sambil menolak isi yang bukan ASCII ber-LF."""
    file_path = root / path
    if not file_path.is_file():
        fail(path, None, "modul ini ada di daftar urutan, tetapi berkasnya tidak ditemukan.")
    data = file_path.read_bytes()
    unsupported = UNSUPPORTED_BYTE.search(data)
    if unsupported is not None:
        position = unsupported.start()
        number = data.count(b"\n", 0, position) + 1
        column = position - data.rfind(b"\n", 0, position)
        byte = data[position]
        if byte == 0x0D:
            fail(path, number, "akhir baris CRLF (\\r\\n); modul harus berakhiran baris LF.")
        if byte > 0x7F:
            fail(path, number,
                 "byte bukan ASCII (0x%02X) di kolom %d; tulis karakter UTF-8 sebagai \\x.. di dalam "
                 "literal." % (byte, column))
        fail(path, number, "karakter kendali 0x%02X di kolom %d tidak didukung." % (byte, column))
    lines = data.decode("ascii").split("\n")
    if lines[-1] == "":
        lines.pop()
    return lines


def reject_unlisted_modules(root: Path, listed: Sequence[str]) -> None:
    for folder in MODULE_ROOTS:
        for file_path in sorted((root / folder).rglob("*")):
            path = file_path.relative_to(root).as_posix()
            if file_path.is_file() and file_path.suffix.lower() in MODULE_SUFFIXES and path not in listed:
                fail(path, None,
                     "berkas modul ini belum terdaftar. Tambahkan ke HEADER_ORDER atau SOURCE_ORDER pada "
                     "scripts/amalgamate.py, pada urutan yang benar.")


def reject_static_name_clashes(sections: Sequence[Section]) -> None:
    """Menolak nama static yang sama di dua berkas sumber.

    Di tiap modul, nama static hanya terlihat oleh berkasnya sendiri sehingga dua modul boleh
    memakai nama yang sama. Di berkas gabungan semua modul menjadi satu unit kompilasi, jadi
    kedua nama itu bertabrakan (atau, untuk fungsi, diam-diam menjadi overload satu sama lain).
    """
    owners: Dict[str, Tuple[str, int]] = {}
    for section in sections:
        for name, number in section.static_names:
            owner_path, owner_line = owners.setdefault(name, (section.path, number))
            if owner_path != section.path:
                fail(section.path, number,
                     'nama static "%s" juga dipakai di %s:%d. Di berkas gabungan semua modul menjadi '
                     "satu unit kompilasi sehingga kedua nama itu bertabrakan. Beri nama yang berbeda "
                     "pada salah satunya." % (name, owner_path, owner_line))


def read_app_info(section: Section) -> Dict[str, str]:
    """Mengambil teks identitas program dari app_info.h, sumber tunggal nama dan nomor versi."""
    info: Dict[str, str] = {}
    for line in section.lines:
        match = APP_INFO_PATTERN.match(line)
        if match is not None:
            info[match.group(1)] = match.group(2)
    for name in ("APP_NAME", "APP_VERSION", "APP_TAGLINE", "APP_REPOSITORY"):
        if not info.get(name):
            fail(section.path, None,
                 'konstanta %s tidak ditemukan dalam bentuk const char* const %s = "...";' % (name, name))
    return info


def standard_include_lines(sections: Sequence[Section]) -> List[str]:
    """Menyatukan #include pustaka standar: tiap pustaka sekali, urut abjad, komentar diratakan.

    Blok komentar yang menempel di atas sebuah #include ditulis di atasnya lagi, dipisah satu baris
    kosong dari #include sebelumnya; baris lanjutan komentar di ujung baris ditulis di bawahnya,
    menjorok sampai kolom komentar itu. Untuk blok komentar di atas, yang dipakai adalah yang
    pertama ditemukan untuk pustaka itu. Komentar ujung baris diambil dari modulnya bila pustaka
    itu hanya di-#include satu modul; bila lebih dari satu modul, dipakai komentar bersama dari
    SHARED_INCLUDE_COMMENTS supaya semua pemakaiannya tersebut.

    Pengecualian: blok komentar yang diawali "// #include " menjelaskan perintah #include itu
    sendiri, bukan satu pustaka. Blok seperti itu ditulis satu kali di awal daftar, supaya
    penjelasannya terbaca sebelum #include pertama dan tidak terselip di tengah urutan abjad.
    """
    general: List[str] = []
    above: Dict[str, List[str]] = {}
    trailing: Dict[str, List[str]] = {}
    users: Dict[str, List[str]] = {}
    for section in sections:
        for include in section.includes:
            block = include.above
            if block and block[0].lstrip().startswith("// #include "):
                if not general:
                    general = block
                block = []
            if not above.get(include.name):
                above[include.name] = block
            if not trailing.get(include.name):
                trailing[include.name] = include.trailing
            users.setdefault(include.name, [])
            if section.path not in users[include.name]:
                users[include.name].append(section.path)
    for name, paths in users.items():
        if len(paths) > 1:
            if name not in SHARED_INCLUDE_COMMENTS:
                fail(paths[0], None,
                     "<%s> di-#include oleh %d modul (%s), tetapi belum punya komentar bersama di "
                     "SHARED_INCLUDE_COMMENTS. Tambahkan komentar yang menyebut semua pemakaiannya."
                     % (name, len(paths), ", ".join(paths)))
            trailing[name] = SHARED_INCLUDE_COMMENTS[name]
    width = max((len("#include <%s>" % name) for name in trailing), default=0)
    lines: List[str] = list(general)
    for name in sorted(trailing):
        if above[name] and lines:
            lines.append("")
        lines += above[name]
        directive = "#include <%s>" % name
        comment = trailing[name]
        if not comment:
            lines.append(directive)
            continue
        lines.append("%s %s" % (directive.ljust(width), comment[0]))
        lines += [" " * (width + 1) + more for more in comment[1:]]
    return lines


def wrap_words(text: str, width: int) -> List[str]:
    """Melipat teks per kata. Ditulis sendiri supaya hasilnya sama di semua versi Python."""
    lines: List[str] = []
    current = ""
    for word in text.split():
        if current and len(current) + 1 + len(word) > width:
            lines.append(current)
            current = word
        else:
            current = "%s %s" % (current, word) if current else word
    if current:
        lines.append(current)
    return lines


class Block(NamedTuple):
    """Satu blok berkepala di berkas gabungan: persiapan, pustaka standar, atau bagian bernomor."""

    number: int
    path: str
    heading: str
    summary: str
    lines: List[str]


def build_blocks(sections: Sequence[Section]) -> List[Block]:
    platform, modules = sections[0], sections[1:]
    blocks = [Block(0, platform.path,
                    "PERSIAPAN - %s - %s" % (platform.path, platform.title),
                    "Persiapan: %s - %s" % (Path(platform.path).name, platform.title),
                    platform.lines)]

    note: List[str] = []
    kept_in_place = [str(number) for number, section in enumerate(modules, start=1)
                     if section.has_platform_includes]
    if kept_in_place:
        note = ["// " + line for line in wrap_words(
            "Pustaka khusus sistem operasi tidak ikut dikumpulkan di sini: baris #include-nya tetap "
            "berada di dalam #ifdef pada Bagian %s." % ", ".join(kept_in_place), MIN_RULE_WIDTH - 3)]
    blocks.append(Block(0, "",
                        "PUSTAKA STANDAR - semua #include dari seluruh modul, dikumpulkan di satu tempat",
                        "Pustaka standar: semua #include dari seluruh modul",
                        note + standard_include_lines(sections)))

    for number, section in enumerate(modules, start=1):
        blocks.append(Block(number, section.path,
                            "BAGIAN %d - %s - %s" % (number, section.path, section.title),
                            "%s - %s" % (Path(section.path).name, section.title),
                            section.lines))
    return blocks


def locate_exam_elements(blocks: Sequence[Block]) -> List[Tuple[int, int]]:
    """Mencari tiap unsur soal: hasilnya (indeks blok, indeks baris di dalam blok itu)."""
    block_of = dict((block.path, index) for index, block in enumerate(blocks))
    places = []
    for element in EXAM_ELEMENTS:
        index = block_of.get(element.path, -1)
        lines = blocks[index].lines if index >= 0 else []
        offsets = [offset for offset, line in enumerate(lines) if re.search(element.pattern, line)]
        if not offsets:
            fail(element.path, None,
                 'penanda untuk "%s" tidak ditemukan (pola %s). Perbarui EXAM_ELEMENTS pada '
                 "scripts/amalgamate.py supaya petunjuk di kepala berkas tetap benar."
                 % (element.description, element.pattern))
        places.append((index, offsets[0]))
    return places


def table_row(section: str, line: str, text: str) -> str:
    return "  %6s  %5s  %s" % (section, line, text)


def file_header(info: Dict[str, str], contents: Sequence[Tuple[Block, int]],
                elements: Sequence[Tuple[ExamElement, int, int]]) -> List[str]:
    """Menyusun kepala berkas.

    contents berisi (blok, nomor baris kepalanya) dan elements berisi (unsur soal, nomor bagian,
    nomor baris). Nomor baris dikirim dari luar karena baru diketahui setelah seluruh berkas
    dirangkai.
    """
    text = [
        OUTPUT_NAME,
        "%s - %s" % (info["APP_NAME"], info["APP_TAGLINE"]),
        COURSE,
        "",
        "Nama  : " + STUDENT_NAME,
        "NIM   : " + STUDENT_ID,
        "Dosen : " + LECTURER,
        "Versi : " + info["APP_VERSION"],
        "",
        "Tentang berkas ini",
    ]
    text += ["  " + line for line in wrap_words(
        "Berkas tunggal ini adalah gabungan seluruh modul program, yaitu isi folder include/ dan src/ "
        "pada repositori %s. Penggabungannya dikerjakan oleh scripts/amalgamate.py supaya program bisa "
        "dikompilasi dengan satu perintah dan dikumpulkan sebagai satu berkas. Bila ada yang perlu "
        "diubah, perubahannya dikerjakan di modul asalnya, lalu berkas ini disusun ulang."
        % info["APP_REPOSITORY"], PROSE_WIDTH)]
    text += [
        "",
        "Cara mengompilasi",
        "  g++ (terminal)          : g++ %s -o estima" % OUTPUT_NAME,
        "  Dev-C++                 : buka berkas ini, lalu Execute > Compile & Run (F11)",
        "  Code::Blocks            : buka berkas ini, lalu Build > Build and run (F9)",
        "  Visual Studio           : cl /EHsc %s" % OUTPUT_NAME,
        "",
        "Letak unsur yang diminta soal (kolom Baris adalah nomor baris di berkas ini)",
        table_row("Bagian", "Baris", "Unsur"),
    ]
    text += [table_row(str(section), str(line), element.description) for element, section, line in elements]
    text += [
        "",
        "Daftar isi",
        table_row("Bagian", "Baris", "Isi"),
    ]
    for block, line in contents:
        if block.path == header_path(HEADER_ORDER[0]):
            text.append(table_row("", "", "Deklarasi: tipe data, konstanta, dan daftar fungsi (asal: %s/)"
                                  % HEADER_FOLDER))
        if block.path == source_path(SOURCE_ORDER[0]):
            text.append(table_row("", "", "Implementasi: isi fungsi (asal: %s/)" % SOURCE_FOLDER))
        text.append(table_row(str(block.number) if block.number else "-", str(line), block.summary))
    return ["// " + line if line else "//" for line in text]


def compose(info: Dict[str, str], blocks: Sequence[Block]) -> str:
    """Merangkai kepala berkas dan semua blok menjadi teks akhir."""
    places = locate_exam_elements(blocks)
    headings = ["// " + block.heading for block in blocks]

    def header_with(block_lines: Sequence[int], element_lines: Sequence[int]) -> List[str]:
        return file_header(info, list(zip(blocks, block_lines)),
                           [(element, blocks[index].number, line)
                            for element, (index, _), line in zip(EXAM_ELEMENTS, places, element_lines)])

    # Nomor baris di kepala berkas bergantung pada panjang kepala berkas itu sendiri. Kolom
    # nomornya berlebar tetap, jadi kepala berkas disusun dulu dengan nomor 0 untuk mengetahui
    # panjangnya, lalu disusun lagi dengan nomor yang sebenarnya.
    draft = header_with([0] * len(blocks), [0] * len(places))
    width = max([MIN_RULE_WIDTH] + [len(line) for line in draft + headings])
    rule = "// " + "=" * (width - 3)

    body: List[str] = []
    heading_at: List[int] = []
    for block, heading in zip(blocks, headings):
        if body:
            body.append("")
        body.append(rule)
        heading_at.append(len(body))
        body += [heading, rule]
        body += block.lines

    # Kepala berkas diapit dua garis dan diikuti satu baris kosong; nomor baris dihitung dari 1.
    first_body_line = len(draft) + 4
    block_lines = [first_body_line + at for at in heading_at]
    element_lines = [first_body_line + heading_at[index] + 2 + offset for index, offset in places]
    lines = [rule] + header_with(block_lines, element_lines) + [rule, ""] + body

    # Pemeriksaan silang: setiap nomor baris yang ditulis di kepala berkas harus benar-benar
    # menunjuk baris yang dimaksud.
    for line, heading in zip(block_lines, headings):
        if lines[line - 1] != heading:
            raise RuntimeError("nomor baris %d di daftar isi tidak menunjuk kepala bagiannya" % line)
    for line, element in zip(element_lines, EXAM_ELEMENTS):
        if not re.search(element.pattern, lines[line - 1]):
            raise RuntimeError("nomor baris %d di letak unsur soal tidak menunjuk penandanya" % line)
    return "\n".join(lines) + "\n"


def assemble(root: Path) -> str:
    """Membaca semua modul di bawah root dan mengembalikan isi berkas gabungan."""
    listed = ([header_path(PLATFORM_HEADER)] + [header_path(name) for name in HEADER_ORDER]
              + [source_path(name) for name in SOURCE_ORDER])
    reject_unlisted_modules(root, listed)

    sections: List[Section] = []
    visible_headers: List[str] = []
    for path in listed:
        sections.append(ModuleFilter(path, tuple(visible_headers)).run(read_module(root, path)))
        if path.endswith(".h"):
            visible_headers.append(Path(path).stem)
    reject_static_name_clashes(sections)

    app_info = next(section for section in sections if section.path == header_path("app_info"))
    return compose(read_app_info(app_info), build_blocks(sections))


def shown_path(path: Path, root: Path) -> str:
    try:
        return path.resolve().relative_to(root).as_posix()
    except ValueError:
        return str(path)


def check_output(output: Path, shown: str, expected: bytes) -> int:
    """Membandingkan berkas yang ada dengan hasil penggabungan, tanpa menulis apa pun."""
    if not output.is_file():
        print("%s: %s belum ada. Jalankan python3 scripts/amalgamate.py untuk menyusunnya."
              % (PROGRAM, shown), file=sys.stderr)
        return EXIT_OUTDATED
    actual = output.read_bytes()
    if actual == expected:
        print("%s sudah sesuai dengan modul." % shown)
        return EXIT_OK

    print("%s: %s tidak sama dengan hasil penggabungan modul saat ini.\n"
          "Jalankan python3 scripts/amalgamate.py untuk menyusunnya ulang, lalu sertakan hasilnya."
          % (PROGRAM, shown), file=sys.stderr)
    if actual.replace(b"\r\n", b"\n") == expected:
        print("Isinya sama, tetapi akhir barisnya CRLF; berkas gabungan harus berakhiran baris LF.",
              file=sys.stderr)
        return EXIT_OUTDATED
    diff = list(difflib.unified_diff(
        actual.decode("ascii", "backslashreplace").replace("\r", "\\r").split("\n"),
        expected.decode("ascii").split("\n"),
        fromfile=shown, tofile="hasil penggabungan modul saat ini", n=1, lineterm=""))
    print("Perbedaannya:", file=sys.stderr)
    for line in diff[:DIFF_LIMIT]:
        print(line, file=sys.stderr)
    if len(diff) > DIFF_LIMIT:
        print("... (%d baris perbedaan lainnya tidak ditampilkan)" % (len(diff) - DIFF_LIMIT),
              file=sys.stderr)
    return EXIT_OUTDATED


def write_output(output: Path, shown: str, expected: bytes) -> int:
    # Berkas yang sudah sesuai tidak ditulis ulang supaya waktu ubahnya tetap dan alat build
    # tidak mengompilasi ulang tanpa sebab.
    if output.is_file() and output.read_bytes() == expected:
        print("%s sudah sesuai dengan modul; tidak ada yang ditulis." % shown)
        return EXIT_OK
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(expected)
    print("%s disusun ulang: %d bagian, %d baris."
          % (shown, len(HEADER_ORDER) + len(SOURCE_ORDER), expected.count(b"\n")))
    return EXIT_OK


class IndonesianHelpFormatter(argparse.HelpFormatter):
    """Mengganti awalan "usage:" bawaan argparse supaya bantuan seluruhnya berbahasa Indonesia."""

    def add_usage(self, usage, actions, groups, prefix=None):
        super().add_usage(usage, actions, groups, prefix="Pemakaian: ")


class ArgumentParser(argparse.ArgumentParser):
    def error(self, message: str) -> NoReturn:
        self.print_usage(sys.stderr)
        self.exit(EXIT_ERROR, "%s: opsi tidak bisa dipakai (%s). Lihat --help.\n" % (self.prog, message))


def parse_arguments(arguments: Optional[Sequence[str]]) -> argparse.Namespace:
    parser = ArgumentParser(
        prog=PROGRAM, add_help=False, allow_abbrev=False, formatter_class=IndonesianHelpFormatter,
        description="Menyusun %s dari modul di %s/ dan %s/." % (DEFAULT_OUTPUT, HEADER_FOLDER, SOURCE_FOLDER))
    options = parser.add_argument_group("Opsi")
    options.add_argument("-h", "--help", action="help", help="Menampilkan bantuan ini lalu keluar.")
    options.add_argument("--check", action="store_true",
                         help="Memeriksa saja: keluar 1 dan menampilkan perbedaannya bila berkas gabungan "
                              "tidak sama dengan hasil penggabungan modul saat ini. Tidak ada yang ditulis.")
    options.add_argument("--output", metavar="JALUR",
                         help="Berkas yang ditulis atau diperiksa (bawaan: %s)." % DEFAULT_OUTPUT)
    return parser.parse_args(arguments)


def main(arguments: Optional[Sequence[str]] = None) -> int:
    options = parse_arguments(arguments)
    root = Path(__file__).resolve().parent.parent
    output = Path(options.output) if options.output else root / DEFAULT_OUTPUT
    shown = shown_path(output, root)
    try:
        expected = assemble(root).encode("ascii")
        if options.check:
            return check_output(output, shown, expected)
        return write_output(output, shown, expected)
    except AmalgamationError as error:
        print("%s: %s" % (PROGRAM, error), file=sys.stderr)
    except OSError as error:
        print("%s: berkas tidak bisa dibaca atau ditulis: %s" % (PROGRAM, error), file=sys.stderr)
    return EXIT_ERROR


if __name__ == "__main__":
    sys.exit(main())
