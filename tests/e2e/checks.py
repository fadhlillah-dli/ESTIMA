"""Pemeriksaan khusus untuk skenario yang tidak cukup diperiksa dengan daftar teks.

Tiap pemeriksaan menerima Run (sesi yang sudah dijalankan) dan berhenti lewat require() dengan
pesan yang menjelaskan bedanya bila hasil tidak sesuai. Nama yang di tabel skenario dipanggil
dengan kurung, misalnya same_stdout_as(newline="\\n"), adalah pembuat pemeriksaan: hasilnya
fungsi pemeriksa untuk pengaturan itu.
"""

import re
from itertools import zip_longest
from typing import Callable, List, Tuple

from harness import (ASCII_BOX, UNICODE_BOX, BoxGlyphs, Run, flatten, read_app_version, require,
                     squeeze, strip_ansi, without_frame)

Check = Callable[[Run], None]

# Lebar terminal baku. Teks bantuan tidak dilipat mengikuti COLUMNS, jadi harus muat di sini.
CLASSIC_TERMINAL_WIDTH = 80

# Judul kotak keluaran (4) soal: komponen dengan biaya produksi tertinggi.
TOP_TITLE = "Biaya Produksi Tertinggi"

# Bentuk baris tabel rekap, baris TOTAL-nya, baris tabel harga, dan tawaran harga acuan.
RECAP_ROW = re.compile(r"(\d+) +(.+?) +([\d.]+) +([\d.]+,\d{3}) kg +Rp([\d.]+)"
                       r" +([\d.]+,\d{3}) kg +Rp([\d.]+)")
RECAP_TOTAL = re.compile(r"TOTAL +([\d.]+) +([\d.]+,\d{3}) kg +Rp([\d.]+)")
PRICE_ROW = re.compile(r"(\d+) +(.+?) +(Rp[\d.]+)(?: {2,}.*)?")
PRICE_OFFER = re.compile(r"Enter = pakai Rp([\d.]+) \((.+?)\), atau ketik harga lain\.")


# ---------------------------------------------------------------------------------------------
# Angka. Uji menghitung sendiri dengan bilangan bulat (gram dan rupiah) lalu membandingkannya
# dengan tulisan di layar, sehingga hitungan pembanding tidak membawa galat pembulatan.
# ---------------------------------------------------------------------------------------------

def to_number(text: str) -> int:
    """Teks "1.250.000" menjadi 1250000."""
    return int(text.replace(".", ""))


def to_gram(text: str) -> int:
    """Teks berat "1.234,500" (kilogram) menjadi 1234500 gram."""
    whole, _, fraction = text.partition(",")
    return to_number(whole) * 1000 + int(fraction)


def thousands(number: int) -> str:
    """Bilangan 1250000 menjadi "1.250.000"."""
    return format(number, ",").replace(",", ".")


def rupiah(amount: int) -> str:
    return "Rp" + thousands(amount)


def kilogram(gram: int) -> str:
    return "%s,%03d kg" % (thousands(gram // 1000), gram % 1000)


def percent(part: int, whole: int) -> str:
    """Porsi part terhadap whole dengan satu angka desimal; setengah dibulatkan ke atas."""
    tenths = (part * 2000 + whole) // (2 * whole) if whole > 0 else 0
    return "%d,%d%%" % divmod(tenths, 10)


# ---------------------------------------------------------------------------------------------
# Pembanding dengan sesi lain
# ---------------------------------------------------------------------------------------------

def first_difference(actual: str, wanted: str) -> str:
    """Keterangan baris pertama yang berbeda di antara dua layar."""
    pairs = zip_longest(actual.split("\n"), wanted.split("\n"))
    for number, (left, right) in enumerate(pairs, start=1):
        if left != right:
            return "baris %d tertulis %r, pembandingnya %r" % (number, left, right)
    return "tidak ada baris yang berbeda"


def same_stdout_as(**changes) -> Check:
    """Layar harus sama persis dengan sesi yang sama yang dijalankan dengan pengaturan lain."""
    def check(run: Run) -> None:
        other = run.again(**changes).stdout
        require(run.outcome.stdout == other,
                "layar berbeda dari sesi pembanding %r: %s"
                % (changes, first_difference(run.outcome.stdout, other)))
    return check


def colors_change_nothing_else(run: Run) -> None:
    """Warna hanya hiasan: tanpa kode warnanya, layar harus sama persis dengan mode polos.

    Ini juga menjamin lebar kolom dan pelipatan baris tidak ikut menghitung kode warna.
    """
    plain = run.again(env={}).stdout
    shown = strip_ansi(run.outcome.stdout)
    require(shown == plain,
            "layar berwarna berbeda dari layar polos: " + first_difference(shown, plain))


def ascii_mirrors_unicode(run: Run) -> None:
    """Opsi --ascii hanya menukar karakter gambar, tidak mengubah isi maupun tata letak.

    Layar mode biasa diambil, karakter garis dan batangnya diganti padanan ASCII, lalu hasilnya
    harus sama persis dengan layar mode --ascii.
    """
    swap = str.maketrans(dict(zip(UNICODE_BOX, ASCII_BOX)))
    wanted = run.again(args=()).stdout.translate(swap)
    require(run.outcome.stdout == wanted,
            "layar --ascii berbeda dari layar biasa: "
            + first_difference(run.outcome.stdout, wanted))


def only_ascii(run: Run) -> None:
    """Mode --ascii dibuat untuk terminal tanpa UTF-8, jadi semua karakternya harus ASCII."""
    foreign = sorted(set(char for char in run.outcome.stdout if ord(char) > 127))
    require(not foreign, "ada karakter di luar ASCII: %r" % "".join(foreign))


def nothing_on_stdout(run: Run) -> None:
    """Opsi yang salah tidak boleh sempat membuka menu: stdout harus kosong."""
    require(run.outcome.stdout == "",
            "stdout seharusnya kosong, isinya diawali %r" % run.outcome.stdout[:60])


def gone_after(marker: str, text: str) -> Check:
    """Setelah kemunculan terakhir marker, text tidak boleh tampil lagi di layar."""
    def check(run: Run) -> None:
        flat = flatten(run.outcome.stdout)
        position = flat.rfind(squeeze(marker))
        require(position >= 0, "penanda %r tidak tampil" % marker)
        require(squeeze(text) not in flat[position + len(squeeze(marker)):],
                "%r masih tampil setelah %r" % (text, marker))
    return check


# ---------------------------------------------------------------------------------------------
# Versi dan bantuan
# ---------------------------------------------------------------------------------------------

def prints_version(run: Run) -> None:
    """Opsi versi mencetak tepat satu baris "ESTIMA <versi>"; versinya dibaca dari app_info.h."""
    wanted = "ESTIMA %s\n" % read_app_version()
    require(run.outcome.stdout == wanted,
            "stdout %r, seharusnya %r" % (run.outcome.stdout, wanted))


def help_names_version(run: Run) -> None:
    """Baris pertama bantuan menyebut nama dan versi yang sama dengan app_info.h."""
    wanted = "ESTIMA %s - " % read_app_version()
    require(run.outcome.stdout.startswith(wanted), "bantuan tidak diawali %r" % wanted)


def fits_classic_terminal(run: Run) -> None:
    """Teks bantuan (di stdout atau stderr) harus muat di terminal baku 80 kolom."""
    for line in (run.outcome.stdout + run.outcome.stderr).split("\n"):
        require(len(line) < CLASSIC_TERMINAL_WIDTH,
                "baris bantuan selebar %d kolom tidak muat di terminal %d kolom: %r"
                % (len(line), CLASSIC_TERMINAL_WIDTH, line))


# ---------------------------------------------------------------------------------------------
# Tabel di layar. Harga katalog masih bisa berubah, jadi yang diperiksa strukturnya: isi tabel
# harus cocok satu sama lain dan cocok dengan hitungan uji sendiri.
# ---------------------------------------------------------------------------------------------

def boxes_titled(screen: str, title: str, box: BoxGlyphs = UNICODE_BOX) -> List[List[str]]:
    """Isi semua kotak berjudul title; tiap kotak berupa daftar baris tanpa tepi.

    Bawaannya kotak bergaris UTF-8 (layar mode biasa); berkas laporan memakai ASCII_BOX.
    """
    lines = strip_ansi(screen).split("\n")
    boxes = []
    for index, line in enumerate(lines):
        if not line.startswith(box.top_left) or without_frame(line) != title:
            continue
        body = []
        for inner in lines[index + 1:]:
            if not inner.startswith(box.vertical):
                break
            body.append(inner[1:-1].strip())
        boxes.append(body)
    return boxes


def top_colons_aligned(run: Run) -> None:
    """Daftar biaya tertinggi yang berisi beberapa komponen seri harus rata pada tanda titik dua.

    Diperiksa di setiap kotak "Biaya Produksi Tertinggi" di layar dan di setiap berkas laporan
    yang disimpan: semua baris "- Nama : RpX (...)" punya " : " pada kolom yang sama, dan
    sedikitnya ada dua baris seperti itu (skenario ini memang tentang seri). Pemeriksaan ini
    untuk layar yang cukup lebar; di layar sempit program sengaja tidak meratakan nama.
    """
    places = [("layar", body) for body in boxes_titled(run.outcome.stdout, TOP_TITLE)]
    for name in run.session.saved:
        # Di Windows akhir baris berkas adalah CRLF; disamakan dulu menjadi LF seperti layar.
        text = run.outcome.files[name].decode("ascii").replace("\r\n", "\n")
        places += [(name, body) for body in boxes_titled(text, TOP_TITLE, ASCII_BOX)]
    require(bool(places), "kotak %r tidak tampil" % TOP_TITLE)
    for where, body in places:
        items = [line for line in body if line.startswith("- ") and " : " in line]
        require(len(items) >= 2, "%s: daftar biaya tertinggi tidak berisi komponen seri" % where)
        columns = sorted(set(line.index(" : ") for line in items))
        require(len(columns) == 1,
                "%s: tanda titik dua daftar biaya tertinggi tidak sejajar (kolom %s)" % (where, columns))


def price_rows(screen: str, title: str) -> List[Tuple[int, str, str]]:
    """Baris (nomor, material, harga) dari kotak harga berjudul title yang pertama tampil."""
    boxes = boxes_titled(screen, title)
    require(bool(boxes), "kotak %r tidak tampil" % title)
    found = [PRICE_ROW.fullmatch(line) for line in boxes[0]]
    return [(int(row.group(1)), row.group(2), row.group(3)) for row in found if row]


def catalog_tables_agree(run: Run) -> None:
    """Daftar harga di menu 7 dan tabel pilihan di menu Tambah harus menampilkan data yang sama.

    Material bernomor urut dari 1, kedua tabel memuat nama dan harga yang sama, dan batas atas
    pada pertanyaan "Pilih harga [0-N]" sama dengan banyaknya material.
    """
    catalog = price_rows(run.outcome.stdout, "Harga Acuan Material")
    choices = price_rows(run.outcome.stdout, "Harga Acuan per kg")
    require(bool(catalog), "daftar harga tidak tampil sebagai tabel pada lebar ini")
    require([number for number, _, _ in catalog] == list(range(1, len(catalog) + 1)),
            "nomor material tidak berurutan mulai dari 1")
    require(choices == catalog, "tabel di menu Tambah berbeda dari daftar harga di menu 7")
    require("Pilih harga [0-%d] :" % len(catalog) in flatten(run.outcome.stdout),
            "batas pilihan harga tidak sama dengan banyaknya material (%d)" % len(catalog))


def reference_price_is_used(run: Run) -> None:
    """Jalur harga acuan, tanpa bergantung pada angka katalog.

    Skenarionya memilih material no. 1 lalu menekan Enter untuk komponen 2 unit x 1,5 kg. Harga
    yang ditawarkan dibaca dari layar, lalu harus (1) sama dengan harga material no. 1 di tabel,
    (2) dipakai di ringkasan, dan (3) menghasilkan total biaya tiga kali harga itu (berat 3 kg).
    """
    flat = flatten(run.outcome.stdout)
    offer = PRICE_OFFER.search(flat)
    require(offer is not None, "tawaran harga acuan tidak tampil")
    price = to_number(offer.group(1))
    require("Harga/kg 1 %s %s " % (offer.group(2), rupiah(price)) in flat,
            "harga yang ditawarkan bukan harga material no. 1 di tabel")
    wanted = ("Harga/kg : %s Total berat : 3,000 kg Total biaya : %s"
              % (rupiah(price), rupiah(3 * price)))
    require(wanted in flat, "ringkasan tidak memuat %r" % wanted)


def sample_recap_adds_up(run: Run) -> None:
    """Rekap data contoh harus cocok dengan hitungan uji, berapa pun harga katalognya.

    Yang dibaca adalah rekap terakhir di layar. Tiap barisnya harus memenuhi jumlah x berat/unit
    = total berat dan total berat x harga = total biaya (dibulatkan ke rupiah terdekat). Baris
    TOTAL, kotak Total Keseluruhan, kotak Biaya Produksi Tertinggi, dan pesan "N komponen contoh
    dimuat" harus sesuai dengan penjumlahan baris-baris itu.
    """
    recaps = boxes_titled(run.outcome.stdout, "Rekap Material dan Biaya")
    require(bool(recaps), "kotak rekap tidak tampil")
    rows = [row for row in map(RECAP_ROW.fullmatch, recaps[-1]) if row]
    totals = [row for row in map(RECAP_TOTAL.fullmatch, recaps[-1]) if row]
    require(bool(rows) and len(totals) == 1, "rekap tidak tampil sebagai tabel pada lebar ini")

    sum_quantity = sum_gram = sum_cost = 0
    costs = []
    for position, row in enumerate(rows, start=1):
        name = row.group(2)
        quantity, unit_gram, price = (to_number(row.group(3)), to_gram(row.group(4)),
                                      to_number(row.group(5)))
        total_gram, cost = to_gram(row.group(6)), to_number(row.group(7))
        require(int(row.group(1)) == position, "nomor baris %s bukan %d" % (name, position))
        require(quantity * unit_gram == total_gram,
                "total berat %s bukan jumlah x berat/unit" % name)
        require((total_gram * price + 500) // 1000 == cost,
                "total biaya %s bukan total berat x harga" % name)
        sum_quantity += quantity
        sum_gram += total_gram
        sum_cost += cost
        costs.append((name, cost))

    shown = totals[0]
    require((to_number(shown.group(1)), to_gram(shown.group(2)), to_number(shown.group(3)))
            == (sum_quantity, sum_gram, sum_cost), "baris TOTAL bukan penjumlahan semua baris")

    highest = max(cost for _, cost in costs)
    wanted = ["[OK] %d komponen contoh dimuat" % len(rows),
              "Jenis komponen : %d Jumlah unit : %s Total berat : %s Total biaya : %s"
              % (len(rows), thousands(sum_quantity), kilogram(sum_gram), rupiah(sum_cost))]
    wanted += ["- %s : %s (%s dari total biaya)" % (name, rupiah(cost), percent(cost, sum_cost))
               for name, cost in costs if cost == highest]
    flat = flatten(run.outcome.stdout)
    for text in wanted:
        require(text in flat, "layar tidak memuat %r" % text)
