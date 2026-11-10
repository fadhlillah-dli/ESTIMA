"""Tabel skenario uji ujung-ke-ujung ESTIMA.

Kedua skrip uji memakai tabel yang sama, jadi sesi yang diperiksa isinya juga diperiksa lebarnya:

  SCREEN_SESSIONS - sesi menu biasa. run_e2e.py memeriksa isinya pada lebar 100 kolom, lalu
                    check_layout.py menjalankannya lagi pada setiap lebar 40-120 kolom.
  LAUNCH_SESSIONS - sesi tentang cara program dijalankan: opsi, warna, lebar, dan bentuk
                    masukan. Hanya dipakai run_e2e.py.

Cara membaca satu skenario: typed adalah ketikan pengguna baris demi baris, expect adalah teks
yang harus tampil berurutan di layar (bingkai kotak dan pelipatan baris diabaikan, lihat
harness.flatten), dan errors adalah banyaknya pesan "[!]" yang boleh muncul.

Harga selalu diketik sendiri. Harga katalog masih bisa diperbarui, jadi skenario yang
menyentuh katalog atau data contoh hanya memeriksa strukturnya (lihat checks.py).
"""

from checks import (ascii_mirrors_unicode, catalog_tables_agree, colors_change_nothing_else,
                    fits_classic_terminal, gone_after, help_names_version, nothing_on_stdout,
                    only_ascii, prints_version, reference_price_is_used, same_stdout_as,
                    sample_recap_adds_up, top_colons_aligned)
from harness import ERROR_MARK, PROGRAM_DONE, UNICODE_BOX, Session

ENTER = ""      # menekan Enter tanpa mengetik apa pun
BAR = UNICODE_BOX.bar

# ---------------------------------------------------------------------------------------------
# Teks program yang dipakai berulang
# ---------------------------------------------------------------------------------------------

PAUSE = "Tekan Enter untuk kembali ke menu..."
NO_DATA = "Data: belum ada komponen."
EMPTY_LIST = ("[i] Belum ada komponen. Pilih menu 1 untuk menambah komponen atau menu 6 untuk "
              "memuat data contoh.")
INPUT_CLOSED = "Input berakhir sebelum program ditutup lewat menu Keluar. Program dihentikan."
STORED = '[OK] Komponen "%s" disimpan sebagai no. %d.'
DUPLICATE = ('Nama "%s" sudah dipakai komponen no. %d. Beri nama lain, misalnya dengan menambah '
             "kode ukuran.")
COST_LIMIT = ("Total biaya komponen ini melebihi Rp999.999.999.999, batas yang bisa dihitung "
              "program. Periksa lagi jumlah, berat, dan harganya.")
LIST_FULL = ("Daftar sudah penuh (100 komponen). Hapus dulu komponen yang tidak dipakai lewat "
             "menu 5.")
REPOSITORY_LINE = "Dokumentasi: https://github.com/fadhlillah-dli/UTS_2531070011"

# Label pertanyaan yang dijawab sesi-sesi di bawah. Gate tata letak memakainya untuk mengenali
# baris pertanyaan: di baris itu jawaban ikut tercetak, padahal yang harus muat di layar hanya
# bagian "label : ". Tanda # berarti angka yang mengikuti isi katalog. Pertanyaan baru yang
# dijawab sebuah sesi harus didaftarkan di sini.
PROMPT_LABELS = (
    "Pilihan Anda [0-8]",
    "Nama komponen",
    "Jumlah (unit)",
    "Berat per unit (kg)",
    "Pilih harga [0-#]",
    "Harga per kg (Rp)",
    "Simpan komponen ini? [Y/t]",
    "Tambah komponen lagi? [y/T]",
    "Nomor komponen [0 = batal]",
    "Simpan perubahan? [Y/t]",
    "Hapus komponen ini? [y/T]",
    "Ganti dengan data contoh? [y/T]",
    "Nama berkas",
    "Timpa berkas itu? [y/T]",
    "Keluar dari program? [y/T]",
)


# ---------------------------------------------------------------------------------------------
# Penyusun ketikan
# ---------------------------------------------------------------------------------------------

def part(name, quantity, weight, price):
    """Ketikan satu komponen: harga diketik sendiri (pilihan 0), lalu Enter untuk menyimpan."""
    return (name, quantity, weight, "0", price, ENTER)


def keep_adding(parts):
    """Ketikan dari menu utama: buka Tambah komponen lalu isi semua komponen.

    Pertanyaan "Tambah komponen lagi?" selalu dijawab y, jadi di akhir ketikan ini program
    sedang menunggu nama komponen berikutnya.
    """
    typed = ("1",)
    for one in parts:
        typed += one + ("y",)
    return typed


def add_parts(*parts):
    """Seperti keep_adding, tetapi setelah komponen terakhir menjawab t dan kembali ke menu."""
    return keep_adding(parts)[:-1] + ("t", ENTER)


def typed_of(cases):
    """Ketikan dari daftar pasangan (ketikan, pesan kesalahan)."""
    return tuple(typed for typed, _ in cases)


def rejected(label, cases):
    """Harapan untuk ketikan yang ditolak: tiap ketikan langsung dijawab pesan kesalahannya.

    Hasilnya teks "label : ketikan [!] pesan" untuk tiap pasangan, sehingga pesan yang benar
    tetapi muncul untuk ketikan yang lain tetap ketahuan.
    """
    return tuple("%s : %s %s%s" % (label, typed, ERROR_MARK, message) for typed, message in cases)


# ---------------------------------------------------------------------------------------------
# Data yang dipakai banyak skenario
# ---------------------------------------------------------------------------------------------

# Sesi asap baku, ditulis apa adanya dari kontrak proyek (dipakai juga oleh CI dan dokumentasi).
SMOKE_KEYS = (
    "1", "Gear", "24", "1,85", "0", "18500", ENTER, "y",
    "Shaft", "12", "4.2", "0", "32000", ENTER, "t", ENTER,
    "2", ENTER,
    "0", "y",
)
SMOKE_LINES = (
    "Total berat    : 94,800 kg",
    "Total biaya    : Rp2.434.200",
    "- Shaft : Rp1.612.800 (66,3% dari total biaya)",
)

GEAR = part("Gear", "24", "1,85", "18500")          # 44,400 kg, Rp821.400
SHAFT = part("Shaft", "12", "4.2", "32000")         # 50,400 kg, Rp1.612.800
BRACKET = part("Bracket", "40", "0,96", "14500")    # 38,400 kg, Rp556.800
MIDDLE = part("Poros Tengah", "2", "3", "10000")    # 6,000 kg, Rp60.000
PLATE_A = part("Pelat A", "10", "2", "5000")        # 20,000 kg, Rp100.000
PLATE_B = part("Pelat B", "8", "5", "2500")         # 40,000 kg, Rp100.000 (seri dengan Pelat A)
BOLT = part("Baut", "100", "0,05", "10000")         # 5,000 kg, Rp50.000
FRAME = part("Rangka", "1", "40", "10000")          # 40,000 kg, Rp400.000
PIN = part("Pin", "100", "0,5", "2000")             # 50,000 kg, Rp100.000
# Nama 30 karakter (batas terpanjang) yang seri dengan Pin, untuk menguji perataan titik dua.
LONG_TIE_NAME = "Rumah Bantalan Poros Utama 120"
LONG_TIE = part(LONG_TIE_NAME, "4", "5", "5000")    # 20,000 kg, Rp100.000 (seri dengan Pin)

TWO_PARTS = add_parts(GEAR, SHAFT)                  # sama dengan 16 ketikan pertama sesi asap
TWO_PARTS_STATUS = "Data: 2 komponen | 94,800 kg | Rp2.434.200"
TWO_PARTS_ROWS = (
    "1 Gear 24 1,850 kg Rp18.500 44,400 kg Rp821.400",
    "2 Shaft 12 4,200 kg Rp32.000 50,400 kg Rp1.612.800",
    "TOTAL 36 94,800 kg Rp2.434.200",
)
QUIT = ("0", "y")       # daftar berisi, jadi program meminta konfirmasi sebelum keluar

# Isi berkas laporan untuk data dua komponen. Laporan selalu memakai garis ASCII dan batang "#".
TWO_PARTS_REPORT = (
    "Rekap Material dan Biaya",
) + TWO_PARTS_ROWS + (
    "Total Keseluruhan Jenis komponen : 2 Jumlah unit : 36 Total berat : 94,800 kg"
    " Total biaya : Rp2.434.200",
    "Biaya Produksi Tertinggi - Shaft : Rp1.612.800 (66,3% dari total biaya)",
    "Peringkat Biaya Produksi",
    "1 Shaft Rp1.612.800 66,3% " + "#" * 24 + " 2 Gear Rp821.400 33,7% " + "#" * 12 + " Porsi =",
    "Catatan: harga per kg adalah harga yang dipakai saat laporan dibuat (harga acuan atau harga yang diketik sendiri).",
)
OLD_REPORT = {"laporan-estima.txt": "isi lama yang tidak boleh hilang tanpa izin\n"}

# ---------------------------------------------------------------------------------------------
# Ketikan yang harus ditolak, berpasangan dengan pesan yang harus menjawabnya
# ---------------------------------------------------------------------------------------------

NAME_EMPTY = "Nama komponen belum diisi."
NAME_START = "Nama harus diawali huruf atau angka."
NAME_CHARS = "Nama hanya boleh berisi huruf A-Z, angka, spasi, dan tanda - _ . , / ( ) + # &"
BAD_NAMES = (
    ("", NAME_EMPTY),
    ("   ", NAME_EMPTY),                    # spasi saja sama dengan kosong
    ("-Gear", NAME_START),
    ("(Gear)", NAME_START),
    ("Gear@1", NAME_CHARS),
    ("Gear*2", NAME_CHARS),
    ("Poros Penggerak Utama Sisi Kiri",     # 31 karakter, satu lebih dari batas
     "Nama terlalu panjang (31 karakter); paling banyak 30 karakter."),
)
ALL_ALLOWED_MARKS = "Pin-A_1.5,B/C (2)+#&"  # memuat semua tanda baca yang diizinkan

SAME_AS_GEAR = (
    ("gear", DUPLICATE % ("gear", 1)),
    ("GEAR", DUPLICATE % ("GEAR", 1)),
    ("  Gear  ", DUPLICATE % ("Gear", 1)),  # spasi di tepi tidak membuat nama jadi berbeda
    ("gEAR", DUPLICATE % ("gEAR", 1)),
)

QUANTITY_FORMAT = "Jumlah harus bilangan bulat tanpa titik, koma, spasi, atau huruf. Contoh: 24"
QUANTITY_RANGE = "Jumlah harus antara 1 dan 1.000.000 unit."
BAD_QUANTITIES = (
    ("", "Jumlah belum diisi."),
    ("dua", QUANTITY_FORMAT),
    ("2,5", QUANTITY_FORMAT),
    ("2.5", QUANTITY_FORMAT),
    ("1.000", QUANTITY_FORMAT),             # titik ribuan tidak diterima
    ("-3", QUANTITY_FORMAT),
    ("+3", QUANTITY_FORMAT),
    ("1 0", QUANTITY_FORMAT),
    ("24 unit", QUANTITY_FORMAT),
    ("0", QUANTITY_RANGE),
    ("1000001", QUANTITY_RANGE),
    ("99999999999999999999", QUANTITY_RANGE),   # lebih besar dari daya tampung bilangan bulat
)

WEIGHT_FORMAT = "Berat ditulis dengan angka; desimal boleh memakai koma atau titik. Contoh: 1,85"
WEIGHT_DECIMALS = ("Berat paling teliti sampai gram, jadi paling banyak 3 angka di belakang koma. "
                   "Contoh: 0,125")
WEIGHT_RANGE = "Berat per unit harus antara 0,001 kg dan 10.000,000 kg."
BAD_WEIGHTS = (
    ("", "Berat per unit belum diisi."),
    ("berat", WEIGHT_FORMAT),
    ("1,2,3", WEIGHT_FORMAT),
    ("1.234,5", WEIGHT_FORMAT),             # titik ribuan bersama koma desimal
    (",5", WEIGHT_FORMAT),
    ("5,", WEIGHT_FORMAT),
    ("-1,5", WEIGHT_FORMAT),
    ("1,5 kg", WEIGHT_FORMAT),
    ("1e3", WEIGHT_FORMAT),
    ("0,0005", WEIGHT_DECIMALS),
    ("1,2345", WEIGHT_DECIMALS),
    ("0", WEIGHT_RANGE),
    ("0,000", WEIGHT_RANGE),
    ("10000,001", WEIGHT_RANGE),
    ("99999999999", WEIGHT_RANGE),
)

PRICE_FORMAT = "Harga ditulis dalam rupiah bulat tanpa Rp, titik, koma, atau spasi. Contoh: 18500"
PRICE_RANGE = "Harga per kg harus antara Rp1 dan Rp100.000.000."
BAD_PRICES = (
    ("", "Harga per kg belum diisi."),
    ("mahal", PRICE_FORMAT),
    ("Rp18500", PRICE_FORMAT),
    ("18.500", PRICE_FORMAT),
    ("18500,50", PRICE_FORMAT),
    ("18 500", PRICE_FORMAT),
    ("-18500", PRICE_FORMAT),
    ("0", PRICE_RANGE),
    ("100000001", PRICE_RANGE),
    ("99999999999", PRICE_RANGE),
)

MENU_UNKNOWN = "Pilihan tidak dikenal. Ketik angka 0 sampai 8 lalu tekan Enter."
BAD_MENU_CHOICES = (
    ("", MENU_UNKNOWN),
    ("9", MENU_UNKNOWN),
    ("-1", MENU_UNKNOWN),
    ("1,0", MENU_UNKNOWN),
    ("satu", MENU_UNKNOWN),
    ("1 2", MENU_UNKNOWN),
    ("99999999999", MENU_UNKNOWN),
)
# Batas atas pilihan harga mengikuti isi katalog, jadi pesannya hanya diperiksa sampai "sampai".
PRICE_CHOICE_UNKNOWN = ERROR_MARK + "Pilihan tidak dikenal. Ketik angka 0 sampai"
ONLY_ONE_PART = "Pilihan tidak dikenal. Ketik angka 0 sampai 1 lalu tekan Enter."

YES_NO_UNKNOWN = "Jawab dengan y (ya) atau t (tidak), lalu tekan Enter."
BAD_SAVE_ANSWERS = (
    ("x", YES_NO_UNKNOWN),
    ("1", YES_NO_UNKNOWN),
    ("yy", YES_NO_UNKNOWN),
    ("ya tidak", YES_NO_UNKNOWN),
)

FILE_CHARS = ("Nama berkas hanya boleh berisi huruf, angka, tanda hubung (-), garis bawah (_), "
              "dan titik, tanpa spasi dan tanpa nama folder.")
FILE_DOTS = "Nama berkas tidak boleh diawali atau diakhiri titik."
FILE_RESERVED = ("Nama itu dicadangkan Windows untuk perangkat. Pilih nama lain, misalnya "
                 "laporan-estima.")
LONGEST_FILE_NAME = "x" * 60
BAD_FILE_NAMES = (
    ("hasil uji", FILE_CHARS),
    ("../laporan", FILE_CHARS),             # tidak boleh keluar dari folder kerja
    ("sub\\laporan", FILE_CHARS),
    ("lap*ran?", FILE_CHARS),
    (".tersembunyi", FILE_DOTS),
    ("laporan.", FILE_DOTS),
    ("CON", FILE_RESERVED),
    ("nul.txt", FILE_RESERVED),
    ("Lpt1.2026", FILE_RESERVED),
    (LONGEST_FILE_NAME + "x", "Nama berkas terlalu panjang; paling banyak 60 karakter."),
)

# ---------------------------------------------------------------------------------------------
# Nama yang panjangnya diatur supaya pesan "nama sudah dipakai" jatuh tepat di tepi layar.
#
# Pesannya: [!] Nama "<nama>" sudah dipakai komponen no. N. Beri nama lain, misalnya dengan
# menambah kode ukuran.  Potongan [!] Nama "<nama>" selebar 11 + panjang nama, potongan sampai
# kata "lain," selebar 57 + panjang nama, dan seluruh pesan selebar 95 + panjang nama. Layar
# selebar W kolom boleh diisi sampai W - 1 kolom, sehingga:
#
#   lebar 40  (batas 39):  nama 28 huruf pas, nama 29 huruf kelebihan satu kolom
#   lebar 80  (batas 79):  nama 22 huruf pas, nama 23 huruf kelebihan satu kolom
#   lebar 120 (batas 119): nama 24 huruf pas, nama 25 huruf kelebihan satu kolom
#
# Pasangan pas/kelebihan itu membuktikan program memakai lebar yang diminta sampai kolom
# terakhirnya, tidak kurang dan tidak lebih. Bila kalimat pesannya diubah, panjang nama dan
# potongan di EDGE_AT_* perlu dihitung ulang.
# ---------------------------------------------------------------------------------------------

EDGE_NAMES = (
    "Pelat Dudukan Motor A1",           # 22 huruf, komponen no. 1
    "Pelat Dudukan Motor A12",          # 23 huruf, no. 2
    "Poros Penggerak Utama K1",         # 24 huruf, no. 3
    "Poros Penggerak Utama K12",        # 25 huruf, no. 4
    "Rumah Bantalan Sisi Kanan B1",     # 28 huruf, no. 5
    "Rumah Bantalan Sisi Kanan B12",    # 29 huruf, no. 6
)
EDGE_KEYS = (
    keep_adding(part(name, "1", "1", "1000") for name in EDGE_NAMES)
    + EDGE_NAMES                        # komponen ke-7: keenam nama ditolak karena sudah dipakai
    + part("Tutup Ujung", "1", "1", "1000") + ("t", ENTER)
    + QUIT
)
EDGE_REJECTIONS = tuple((name, DUPLICATE % (name, number))
                        for number, name in enumerate(EDGE_NAMES, start=1))
EDGE_AT_40 = (
    '[!] Nama "Rumah Bantalan Sisi Kanan B1"\n    sudah dipakai',
    '[!] Nama "Rumah Bantalan Sisi Kanan\n    B12" sudah dipakai',
)
EDGE_AT_80 = (
    "komponen no. 1. Beri nama lain,\n    misalnya dengan",
    "komponen no. 2. Beri nama\n    lain, misalnya dengan",
)
EDGE_AT_120 = (
    "komponen no. 3. Beri nama lain, misalnya dengan menambah kode ukuran.\nNama komponen",
    "komponen no. 4. Beri nama lain, misalnya dengan menambah kode\n    ukuran.\nNama komponen",
)

# Seratus komponen bernama 30 huruf, masing-masing tepat di batas biaya Rp999.999.999.999
# (27.027 unit x 1 kg x Rp37.000.037). Daftar jadi penuh, semua kolom angka selebar mungkin,
# dan jumlah seratus biaya terbesar itu harus tetap tepat sampai rupiah terakhir.
FULL_LIST = tuple(part("Komponen Uji Tata Letak No %03d" % number, "27027", "1", "37000037")
                  for number in range(1, 101))
NAME_30 = "Roda Gigi Miring M2,5 Z40 (#7)"


# ---------------------------------------------------------------------------------------------
# Sesi menu: diperiksa isinya oleh run_e2e.py dan lebarnya oleh check_layout.py.
# Ketikannya harus ASCII, karena pada mode --ascii gate menuntut layar yang ASCII murni.
# ---------------------------------------------------------------------------------------------

SCREEN_SESSIONS = (
    Session(
        "asap-baku",
        "Sesi asap baku: dua komponen, rekap, lalu keluar dengan kode 0",
        typed=SMOKE_KEYS,
        exact=SMOKE_LINES + (
            # Pada 100 kolom menu masih memuat keterangan tiap butir di baris yang sama.
            "[1] Tambah komponen         Isi nama, jumlah, berat per unit, dan harga per kg",
        ),
        expect=(
            NO_DATA,
            "Komponen ke-1",
            "Total berat : 44,400 kg Total biaya : Rp821.400",
            STORED % ("Gear", 1),
            "Komponen ke-2",
            "Total berat : 50,400 kg Total biaya : Rp1.612.800",
            STORED % ("Shaft", 2),
            TWO_PARTS_STATUS,
            "Rekap Material dan Biaya",
            "No Komponen Jumlah Berat/unit Harga/kg Total berat Total biaya",
        ) + TWO_PARTS_ROWS + (
            "Total Keseluruhan Jenis komponen : 2 Jumlah unit : 36 Total berat : 94,800 kg"
            " Total biaya : Rp2.434.200",
            "Biaya Produksi Tertinggi - Shaft : Rp1.612.800 (66,3% dari total biaya)",
            PAUSE,
            "Keluar dari program? [y/T] : y",
        ),
    ),

    # ----- Validasi: tiap bentuk ketikan yang salah ditolak dengan pesannya sendiri -----
    Session(
        "validasi-nama",
        "Nama kosong, berawalan tanda, berkarakter terlarang, atau terlalu panjang ditolak",
        typed=("1",) + typed_of(BAD_NAMES)
        + part(ALL_ALLOWED_MARKS, "1", "1", "1000") + ("y",)
        + part("  Gear    Box  ", "1", "1", "1000") + ("t", ENTER)
        + QUIT,
        expect=rejected("Nama komponen", BAD_NAMES) + (
            "Nama komponen : " + ALL_ALLOWED_MARKS + " Jumlah (unit) : 1",
            "Ringkasan Komponen Nama : " + ALL_ALLOWED_MARKS + " Jumlah : 1 unit",
            STORED % (ALL_ALLOWED_MARKS, 1),
        ),
        # Spasi di tepi dibuang dan deret spasi di tengah dirapatkan sebelum nama disimpan.
        exact=("Nama        : Gear Box ", STORED % ("Gear Box", 2)),
        errors=len(BAD_NAMES),
    ),
    Session(
        "nama-kembar",
        "Nama yang sudah dipakai ditolak tanpa membedakan huruf besar dan kecil",
        typed=keep_adding([GEAR]) + typed_of(SAME_AS_GEAR)
        + part("Gear M2", "1", "1", "1000") + ("y", "gear   m2")
        + SHAFT + ("t", ENTER)
        + QUIT,
        expect=(STORED % ("Gear", 1), "Komponen ke-2")
        + rejected("Nama komponen", SAME_AS_GEAR)
        + (STORED % ("Gear M2", 2), "Komponen ke-3")
        + rejected("Nama komponen", (("gear m2", DUPLICATE % ("gear m2", 2)),))
        + (STORED % ("Shaft", 3), "Data: 3 komponen | 95,800 kg | Rp2.435.200"),
        errors=len(SAME_AS_GEAR) + 1,
    ),
    Session(
        "validasi-jumlah",
        "Jumlah kosong, bukan bilangan bulat, atau di luar 1 sampai 1.000.000 ditolak",
        typed=("1", "Uji Jumlah") + typed_of(BAD_QUANTITIES)
        + ("1000000", "0,001", "0", "1", ENTER, "t", ENTER)
        + QUIT,
        expect=rejected("Jumlah (unit)", BAD_QUANTITIES) + (
            "Jumlah (unit) : 1000000 Berat per unit (kg) : 0,001",
            "Jumlah : 1.000.000 unit Berat/unit : 0,001 kg Harga/kg : Rp1"
            " Total berat : 1.000,000 kg Total biaya : Rp1.000",
        ),
        errors=len(BAD_QUANTITIES),
    ),
    Session(
        "validasi-berat",
        "Berat kosong, salah bentuk, lebih teliti dari gram, atau di luar rentang ditolak",
        typed=("1", "Uji Berat", "1") + typed_of(BAD_WEIGHTS)
        + ("10000.000", "0", "1", ENTER, "t", ENTER)
        + QUIT,
        expect=rejected("Berat per unit (kg)", BAD_WEIGHTS) + (
            "Berat per unit (kg) : 10000.000",
            "Berat/unit : 10.000,000 kg Harga/kg : Rp1"
            " Total berat : 10.000,000 kg Total biaya : Rp10.000",
        ),
        errors=len(BAD_WEIGHTS),
    ),
    Session(
        "validasi-harga",
        "Harga kosong, bukan rupiah bulat, atau di luar Rp1 sampai Rp100.000.000 ditolak",
        typed=("1", "Uji Harga", "1", "1", "0") + typed_of(BAD_PRICES)
        + ("100000000", ENTER, "t", ENTER)
        + QUIT,
        expect=rejected("Harga per kg (Rp)", BAD_PRICES) + (
            "Harga per kg (Rp) : 100000000",
            "Harga/kg : Rp100.000.000 Total berat : 1,000 kg Total biaya : Rp100.000.000",
        ),
        errors=len(BAD_PRICES),
    ),
    Session(
        "validasi-pilihan",
        "Nomor di luar pilihan ditolak di menu utama, pilihan harga, dan nomor komponen",
        typed=typed_of(BAD_MENU_CHOICES)
        + ("1", "Uji Pilihan", "2", "3", ENTER, "x", "999", "0", "1000", ENTER, "t", ENTER)
        + ("4", ENTER, "2", "x", "0", ENTER)
        + ("5", "2", "0", ENTER)
        + QUIT,
        expect=rejected("Pilihan Anda [0-8]", BAD_MENU_CHOICES) + (
            "Pilihan Anda [0-8] : 1",
            "Pilih harga [0-",
            # Enter kosong juga ditolak: pilihan harga tidak punya nilai bawaan.
            ": " + PRICE_CHOICE_UNKNOWN,
            ": x " + PRICE_CHOICE_UNKNOWN,
            ": 999 " + PRICE_CHOICE_UNKNOWN,
            ": 0 Harga per kg (Rp) : 1000",
        ) + rejected("Nomor komponen [0 = batal]",
                     (("", ONLY_ONE_PART), ("2", ONLY_ONE_PART), ("x", ONLY_ONE_PART))) + (
            "Nomor komponen [0 = batal] : 0 [i] Tidak ada yang diubah.",
        ) + rejected("Nomor komponen [0 = batal]", (("2", ONLY_ONE_PART),)) + (
            "Nomor komponen [0 = batal] : 0 [i] Tidak ada yang dihapus.",
        ),
        errors=len(BAD_MENU_CHOICES) + 3 + 3 + 1,
    ),
    Session(
        "validasi-ya-tidak",
        "Jawaban selain ya/tidak ditolak; y, ya, yes, t, tidak, n, no diterima apa pun hurufnya",
        typed=("1", "Gear", "24", "1,85", "0", "18500") + typed_of(BAD_SAVE_ANSWERS)
        + ("YA", "mungkin", "Yes")
        + ("Shaft", "12", "4.2", "0", "32000", "Tidak", "N", ENTER)
        + ("0", "no", "0", "T", "0", "Y"),
        expect=rejected("Simpan komponen ini? [Y/t]", BAD_SAVE_ANSWERS) + (
            "Simpan komponen ini? [Y/t] : YA " + STORED % ("Gear", 1),
            "Tambah komponen lagi? [y/T] : mungkin " + ERROR_MARK + YES_NO_UNKNOWN,
            "Tambah komponen lagi? [y/T] : Yes Komponen ke-2",
            "Simpan komponen ini? [Y/t] : Tidak [i] Komponen tidak disimpan.",
            "Tambah komponen lagi? [y/T] : N " + PAUSE,
            "Data: 1 komponen | 44,400 kg | Rp821.400",
            "Keluar dari program? [y/T] : no Menu Utama",
            "Keluar dari program? [y/T] : T Menu Utama",
            "Keluar dari program? [y/T] : Y " + PROGRAM_DONE,
        ),
        errors=len(BAD_SAVE_ANSWERS) + 1,
    ),
    Session(
        "batas-biaya",
        "Biaya tepat Rp999.999.999.999 diterima; satu unit lebih banyak ditolak (tambah dan ubah)",
        typed=("1", "Tepat di Batas", "27027", "1", "0", "37000037", ENTER, "y")
        + ("Lewat Batas", "27028", "1", "0", "37000037", "t", ENTER)
        + ("4", "1", ENTER, "27028", ENTER, ENTER, ENTER)
        + QUIT,
        expect=(
            "Jumlah : 27.027 unit Berat/unit : 1,000 kg Harga/kg : Rp37.000.037"
            " Total berat : 27.027,000 kg Total biaya : Rp999.999.999.999",
            STORED % ("Tepat di Batas", 1),
            # Saat menambah: komponen ditolak, lalu program langsung menawarkan komponen lain.
            "Harga per kg (Rp) : 37000037 " + ERROR_MARK + COST_LIMIT
            + " Tambah komponen lagi? [y/T] : t",
            "Data: 1 komponen | 27.027,000 kg | Rp999.999.999.999",
            # Saat mengubah: perubahan ditolak dan data lama tidak tersentuh.
            "Jumlah (unit) : 27028",
            "Harga per kg (Rp) : " + ERROR_MARK + COST_LIMIT + " " + PAUSE,
            "Data: 1 komponen | 27.027,000 kg | Rp999.999.999.999",
        ),
        errors=2,
    ),

    # ----- Menu 1: harga dari daftar acuan -----
    Session(
        "harga-acuan",
        "Memilih material lalu Enter memakai harga acuannya; mengetik angka menggantinya",
        typed=("1", "Poros", "2", "1,5", "1", ENTER, ENTER, "y")
        + ("Pasak", "4", "0,25", "2", "12345", ENTER, "t", ENTER)
        + ("2", ENTER)
        + QUIT,
        expect=(
            "Harga Acuan per kg No Material Harga/kg 1",
            "0 Ketik harga sendiri -",
            ": 1 [i] Enter = pakai Rp",
            "atau ketik harga lain. Harga per kg (Rp) : Ringkasan Komponen Nama : Poros"
            " Jumlah : 2 unit Berat/unit : 1,500 kg Harga/kg : Rp",
            STORED % ("Poros", 1),
            ": 2 [i] Enter = pakai Rp",
            "atau ketik harga lain. Harga per kg (Rp) : 12345",
            "Nama : Pasak Jumlah : 4 unit Berat/unit : 0,250 kg Harga/kg : Rp12.345"
            " Total berat : 1,000 kg Total biaya : Rp12.345",
            STORED % ("Pasak", 2),
            "2 Pasak 4 0,250 kg Rp12.345 1,000 kg Rp12.345",
        ),
        checks=(reference_price_is_used,),
    ),

    # ----- Menu 2 dan 3: rekap dan peringkat -----
    Session(
        "rekap-seri-tertinggi",
        "Dua komponen dengan biaya tertinggi yang sama ditampilkan keduanya, berperingkat 1",
        typed=add_parts(PLATE_A, PLATE_B, BOLT) + ("2", ENTER, "3", ENTER) + QUIT,
        expect=(
            "Data: 3 komponen | 65,000 kg | Rp250.000",
            "1 Pelat A 10 2,000 kg Rp5.000 20,000 kg Rp100.000",
            "2 Pelat B 8 5,000 kg Rp2.500 40,000 kg Rp100.000",
            "3 Baut 100 0,050 kg Rp10.000 5,000 kg Rp50.000",
            "TOTAL 118 65,000 kg Rp250.000",
            "Biaya Produksi Tertinggi Ada 2 komponen dengan biaya tertinggi yang sama:"
            " - Pelat A : Rp100.000 (40,0% dari total biaya)"
            " - Pelat B : Rp100.000 (40,0% dari total biaya)",
            # Seri di puncak: keduanya peringkat 1 dan peringkat 2 dilompati.
            "1 Pelat A Rp100.000 40,0% " + BAR * 24
            + " 1 Pelat B Rp100.000 40,0% " + BAR * 24
            + " 3 Baut Rp50.000 20,0% " + BAR * 12 + " Porsi =",
        ),
    ),
    Session(
        "rekap-seri-nama-beda-panjang",
        "Seri dengan nama berbeda panjang: titik dua daftar tertinggi sejajar di layar dan laporan",
        typed=add_parts(PIN, LONG_TIE, BOLT) + ("2", ENTER, "8", ENTER, ENTER) + QUIT,
        expect=(
            "Data: 3 komponen | 75,000 kg | Rp250.000",
            "Biaya Produksi Tertinggi Ada 2 komponen dengan biaya tertinggi yang sama:"
            " - Pin : Rp100.000 (40,0% dari total biaya)"
            " - " + LONG_TIE_NAME + " : Rp100.000 (40,0% dari total biaya)",
            "[OK] Laporan disimpan ke laporan-estima.txt.",
        ),
        # Pin ditambah 27 spasi sampai selebar nama 30 karakter, jadi titik duanya lurus.
        exact=(
            "- Pin" + " " * 27 + " : Rp100.000 (40,0% dari total biaya)",
            "- " + LONG_TIE_NAME + " : Rp100.000 (40,0% dari total biaya)",
        ),
        saved=("laporan-estima.txt",),
        report=(
            "Biaya Produksi Tertinggi Ada 2 komponen dengan biaya tertinggi yang sama:"
            " - Pin : Rp100.000 (40,0% dari total biaya)"
            " - " + LONG_TIE_NAME + " : Rp100.000 (40,0% dari total biaya)",
        ),
        checks=(top_colons_aligned,),
    ),
    Session(
        "rekap-seri-layar-sempit",
        "Seri di layar 60 kolom: baris yang diratakan tidak muat di kotak, jadi nama ditulis apa adanya",
        typed=add_parts(PIN, LONG_TIE, BOLT) + ("2", ENTER) + QUIT,
        width=60,
        expect=(
            "Biaya Produksi Tertinggi Ada 2 komponen dengan biaya tertinggi yang sama:"
            " - Pin : Rp100.000 (40,0% dari total biaya)"
            " - " + LONG_TIE_NAME + " : Rp100.000 (40,0% dari total biaya)",
        ),
        # Tanpa spasi tambahan sesudah Pin: barisnya tetap satu baris utuh, karena isi kotak pada lebar
        # 60 paling lebar 55 kolom (60 - 5) sedangkan baris yang diratakan butuh 69 kolom.
        exact=(
            "- Pin : Rp100.000 (40,0% dari total biaya)",
        ),
    ),
    Session(
        "rekap-seri-tepat-muat",
        "Seri di layar 74 kolom: baris yang diratakan tepat selebar isi kotak (69 kolom), jadi tetap rata",
        typed=add_parts(PIN, LONG_TIE, BOLT) + ("2", ENTER) + QUIT,
        width=74,
        expect=(
            "Biaya Produksi Tertinggi Ada 2 komponen dengan biaya tertinggi yang sama:"
            " - Pin : Rp100.000 (40,0% dari total biaya)"
            " - " + LONG_TIE_NAME + " : Rp100.000 (40,0% dari total biaya)",
        ),
        exact=(
            "- Pin" + " " * 27 + " : Rp100.000 (40,0% dari total biaya)",
            "- " + LONG_TIE_NAME + " : Rp100.000 (40,0% dari total biaya)",
        ),
        checks=(top_colons_aligned,),
    ),
    Session(
        "peringkat-seri",
        "Peringkat diurutkan dari biaya terbesar; biaya sama berbagi peringkat (1, 2, 2, 4)",
        typed=add_parts(PLATE_A, BOLT, FRAME, PLATE_B) + ("3", ENTER, "2", ENTER) + QUIT,
        expect=(
            "Data: 4 komponen | 105,000 kg | Rp650.000",
            "Peringkat Biaya Produksi No Komponen Total biaya Porsi Grafik",
            # Pelat A diisi lebih dulu daripada Pelat B, jadi tetap tampil lebih dulu.
            "1 Rangka Rp400.000 61,5% " + BAR * 24
            + " 2 Pelat A Rp100.000 15,4% " + BAR * 6
            + " 2 Pelat B Rp100.000 15,4% " + BAR * 6
            + " 4 Baut Rp50.000 7,7% " + BAR * 3 + " Porsi =",
            "TOTAL 119 105,000 kg Rp650.000",
            "Biaya Produksi Tertinggi - Rangka : Rp400.000 (61,5% dari total biaya)",
        ),
    ),
    Session(
        "daftar-kosong",
        "Menu 2, 3, 4, 5, dan 8 menjelaskan bahwa daftar masih kosong; keluar tanpa konfirmasi",
        typed=("2", ENTER, "3", ENTER, "4", ENTER, "5", ENTER, "8", ENTER, "0"),
        expect=tuple("Pilihan Anda [0-8] : %s %s %s" % (choice, EMPTY_LIST, PAUSE)
                     for choice in "23458")
        + (NO_DATA, "Pilihan Anda [0-8] : 0 " + PROGRAM_DONE),
    ),

    # ----- Menu 4: ubah komponen -----
    Session(
        "ubah-tanpa-perubahan",
        "Menekan Enter di setiap isian mempertahankan semua nilai lama",
        typed=TWO_PARTS + ("4", "1", ENTER, ENTER, ENTER, ENTER, ENTER, ENTER)
        + ("2", ENTER) + QUIT,
        expect=(
            "Daftar Komponen No Komponen Total biaya 1 Gear Rp821.400 2 Shaft Rp1.612.800",
            "Nomor komponen [0 = batal] : 1"
            " [i] Tekan Enter tanpa mengetik untuk mempertahankan nilai lama."
            " [i] Nama sekarang: Gear Nama komponen :"
            " [i] Jumlah sekarang: 24 unit Jumlah (unit) :"
            " [i] Berat per unit sekarang: 1,850 kg Berat per unit (kg) :"
            " [i] Harga per kg sekarang: Rp18.500 Harga per kg (Rp) :"
            " Ringkasan Komponen Nama : Gear Jumlah : 24 unit Berat/unit : 1,850 kg"
            " Harga/kg : Rp18.500 Total berat : 44,400 kg Total biaya : Rp821.400",
            "Simpan perubahan? [Y/t] : [OK] Komponen no. 1 diperbarui.",
            TWO_PARTS_STATUS,
        ) + TWO_PARTS_ROWS,
    ),
    Session(
        "ubah-semua-nilai",
        "Nama, jumlah, berat, dan harga komponen diganti; rekap memakai nilai barunya",
        typed=TWO_PARTS + ("4", "2", "Poros Utama", "10", "2,5", "40000", "y", ENTER)
        + ("2", ENTER) + QUIT,
        expect=(
            "Nomor komponen [0 = batal] : 2",
            "[i] Nama sekarang: Shaft Nama komponen : Poros Utama"
            " [i] Jumlah sekarang: 12 unit Jumlah (unit) : 10"
            " [i] Berat per unit sekarang: 4,200 kg Berat per unit (kg) : 2,5"
            " [i] Harga per kg sekarang: Rp32.000 Harga per kg (Rp) : 40000"
            " Ringkasan Komponen Nama : Poros Utama Jumlah : 10 unit Berat/unit : 2,500 kg"
            " Harga/kg : Rp40.000 Total berat : 25,000 kg Total biaya : Rp1.000.000",
            "Simpan perubahan? [Y/t] : y [OK] Komponen no. 2 diperbarui.",
            "Data: 2 komponen | 69,400 kg | Rp1.821.400",
            "1 Gear 24 1,850 kg Rp18.500 44,400 kg Rp821.400",
            "2 Poros Utama 10 2,500 kg Rp40.000 25,000 kg Rp1.000.000",
            "TOTAL 34 69,400 kg Rp1.821.400",
            "- Poros Utama : Rp1.000.000 (54,9% dari total biaya)",
        ),
        checks=(gone_after("Komponen no. 2 diperbarui.", "Shaft"),),
    ),
    Session(
        "ubah-dibatalkan",
        "Batal di nomor komponen atau di konfirmasi tidak mengubah data; nama kembar ditolak",
        typed=TWO_PARTS + ("4", "0", ENTER)
        + ("4", "1", "shaft", "GEAR", "5", ENTER, ENTER, "t", ENTER)
        + ("2", ENTER) + QUIT,
        expect=(
            "Nomor komponen [0 = batal] : 0 [i] Tidak ada yang diubah. " + PAUSE,
            # Nama komponen lain ditolak, tetapi nama sendiri dengan huruf berbeda boleh.
            "Nama komponen : shaft " + ERROR_MARK + DUPLICATE % ("shaft", 2)
            + " Nama komponen : GEAR [i] Jumlah sekarang: 24 unit Jumlah (unit) : 5",
            "Nama : GEAR Jumlah : 5 unit Berat/unit : 1,850 kg Harga/kg : Rp18.500"
            " Total berat : 9,250 kg Total biaya : Rp171.125",
            "Simpan perubahan? [Y/t] : t [i] Perubahan dibatalkan. " + PAUSE,
            TWO_PARTS_STATUS,
        ) + TWO_PARTS_ROWS,
        errors=1,
    ),

    # ----- Menu 5: hapus komponen -----
    Session(
        "hapus-batal-lalu-setuju",
        "Hapus dibatalkan tiga cara, lalu disetujui: komponen sesudahnya naik satu nomor",
        typed=add_parts(GEAR, MIDDLE, BRACKET)
        + ("5", "0", ENTER)
        + ("5", "2", ENTER, ENTER)
        + ("5", "2", "t", ENTER)
        + ("5", "2", "y", ENTER)
        + ("5", "2", "y", ENTER)
        + ("5", "1", "y", ENTER)
        + ("5", ENTER)
        + ("0",),
        expect=(
            "Data: 3 komponen | 88,800 kg | Rp1.438.200",
            "Daftar Komponen No Komponen Total biaya"
            " 1 Gear Rp821.400 2 Poros Tengah Rp60.000 3 Bracket Rp556.800",
            "Nomor komponen [0 = batal] : 0 [i] Tidak ada yang dihapus. " + PAUSE,
            "Nomor komponen [0 = batal] : 2 [i] Komponen yang dipilih: Poros Tengah"
            " Hapus komponen ini? [y/T] : [i] Tidak ada yang dihapus. " + PAUSE,
            "Hapus komponen ini? [y/T] : t [i] Tidak ada yang dihapus. " + PAUSE,
            "Data: 3 komponen | 88,800 kg | Rp1.438.200",
            'Hapus komponen ini? [y/T] : y [OK] Komponen "Poros Tengah" dihapus.'
            " Sisa 2 komponen.",
            "Data: 2 komponen | 82,800 kg | Rp1.378.200",
            "Daftar Komponen No Komponen Total biaya 1 Gear Rp821.400 2 Bracket Rp556.800",
            '[OK] Komponen "Bracket" dihapus. Sisa 1 komponen.',
            "Data: 1 komponen | 44,400 kg | Rp821.400",
            '[OK] Komponen "Gear" dihapus. Sisa 0 komponen.',
            NO_DATA,
            "Pilihan Anda [0-8] : 5 " + EMPTY_LIST,
            # Daftar sudah kosong lagi, jadi keluar tidak perlu konfirmasi.
            "Pilihan Anda [0-8] : 0 " + PROGRAM_DONE,
        ),
        checks=(gone_after('"Poros Tengah" dihapus. Sisa 2 komponen.', "Poros Tengah"),),
    ),

    # ----- Menu 6 dan 7: data contoh dan daftar harga (strukturnya saja) -----
    Session(
        "data-contoh",
        "Data contoh dimuat dan rekapnya cocok dengan hitungan uji sendiri",
        typed=("6", ENTER, "3", ENTER) + QUIT,
        width=120,      # lebar terbesar, supaya rekap tampil sebagai tabel yang bisa dibaca uji
        expect=(
            "Pilihan Anda [0-8] : 6 [OK]",
            "komponen contoh dimuat dengan harga acuan",
            "Rekap Material dan Biaya",
            "No Komponen Jumlah Berat/unit Harga/kg Total berat Total biaya",
            # Kelima komponen yang dijanjikan keterangan menu 6.
            "1 Gear", "2 Shaft", "3 Bracket", "4 Housing", "5 Bushing",
            "TOTAL",
            "Total Keseluruhan Jenis komponen : 5",
            "Biaya Produksi Tertinggi",
            PAUSE,
            "Peringkat Biaya Produksi No Komponen Total biaya Porsi Grafik 1",
            "Porsi = bagian biaya komponen dari total biaya.",
        ),
        checks=(sample_recap_adds_up,),
    ),
    Session(
        "data-contoh-menimpa",
        "Memuat data contoh di atas data yang ada: batal menyisakan data lama, setuju menggantinya",
        typed=add_parts(part("Komponen Lama", "3", "2", "1000"))
        + ("6", ENTER, ENTER)
        + ("6", "y", ENTER)
        + QUIT,
        width=120,
        expect=(
            "Data: 1 komponen | 6,000 kg | Rp6.000",
            "[i] Daftar saat ini berisi 1 komponen dan akan diganti."
            " Ganti dengan data contoh? [y/T] : [i] Data tidak diubah. " + PAUSE,
            "Data: 1 komponen | 6,000 kg | Rp6.000",
            "[i] Daftar saat ini berisi 1 komponen dan akan diganti."
            " Ganti dengan data contoh? [y/T] : y [OK]",
            "komponen contoh dimuat dengan harga acuan",
            "Rekap Material dan Biaya",
            "Total Keseluruhan Jenis komponen : 5",
        ),
        checks=(sample_recap_adds_up, gone_after("komponen contoh dimuat", "Komponen Lama")),
    ),
    Session(
        "daftar-harga",
        "Daftar harga acuan tampil lengkap dan sama dengan tabel pilihan harga di menu Tambah",
        typed=("7", ENTER) + add_parts(part("Uji Katalog", "1", "1", "1000")) + QUIT,
        width=120,
        expect=(
            "Harga Acuan Material No Material Harga/kg Standar Kegunaan umum 1",
            "Periode data:",
            PAUSE,
            "Harga Acuan per kg No Material Harga/kg 1",
            "0 Ketik harga sendiri - Harga acuan",
            "keterangannya ada di menu 7.",
        ),
        checks=(catalog_tables_agree,),
    ),

    # ----- Menu 8: simpan laporan -----
    Session(
        "simpan-nama-bawaan",
        "Enter pada nama berkas menyimpan laporan sebagai laporan-estima.txt",
        typed=TWO_PARTS + ("8", ENTER, ENTER) + QUIT,
        expect=(
            "[i] Laporan ditulis sebagai berkas teks di folder tempat program dijalankan."
            " [i] Enter = laporan-estima.txt"
            " Nama berkas : [OK] Laporan disimpan ke laporan-estima.txt. " + PAUSE,
        ),
        saved=("laporan-estima.txt",),
        report=TWO_PARTS_REPORT,
    ),
    Session(
        "simpan-nama-sendiri",
        "Nama tanpa akhiran diberi .txt; akhiran yang sudah diketik dipakai apa adanya",
        typed=TWO_PARTS + ("8", "rekap-mesin_v2", ENTER, "8", "Rekap.Akhir.TXT", ENTER) + QUIT,
        expect=(
            "Nama berkas : rekap-mesin_v2 [OK] Laporan disimpan ke rekap-mesin_v2.txt.",
            "Nama berkas : Rekap.Akhir.TXT [OK] Laporan disimpan ke Rekap.Akhir.TXT.",
        ),
        saved=("rekap-mesin_v2.txt", "Rekap.Akhir.TXT"),
        report=TWO_PARTS_REPORT,
    ),
    Session(
        "simpan-nama-tidak-sah",
        "Nama berkas berspasi, berfolder, bertitik di tepi, dicadangkan, atau kepanjangan ditolak",
        typed=TWO_PARTS + ("8",) + typed_of(BAD_FILE_NAMES) + (LONGEST_FILE_NAME, ENTER) + QUIT,
        expect=rejected("Nama berkas", BAD_FILE_NAMES) + (
            "[OK] Laporan disimpan ke " + LONGEST_FILE_NAME + ".txt.",
        ),
        errors=len(BAD_FILE_NAMES),
        saved=(LONGEST_FILE_NAME + ".txt",),
        report=TWO_PARTS_REPORT,
    ),
    Session(
        "simpan-timpa-ditolak",
        "Berkas yang sudah ada tidak ditimpa bila pengguna menekan Enter atau menjawab t",
        typed=TWO_PARTS + ("8", ENTER, ENTER, ENTER, "8", ENTER, "t", ENTER) + QUIT,
        files=OLD_REPORT,
        expect=(
            "Nama berkas : [i] Berkas laporan-estima.txt sudah ada."
            " Timpa berkas itu? [y/T] : [i] Laporan tidak disimpan. " + PAUSE,
            "Nama berkas : [i] Berkas laporan-estima.txt sudah ada."
            " Timpa berkas itu? [y/T] : t [i] Laporan tidak disimpan. " + PAUSE,
        ),
    ),
    Session(
        "simpan-timpa-disetujui",
        "Berkas yang sudah ada diganti laporan baru setelah pengguna menjawab y",
        typed=TWO_PARTS + ("8", ENTER, "y", ENTER) + QUIT,
        files=OLD_REPORT,
        expect=(
            "Nama berkas : [i] Berkas laporan-estima.txt sudah ada."
            " Timpa berkas itu? [y/T] : y [OK] Laporan disimpan ke laporan-estima.txt.",
        ),
        saved=("laporan-estima.txt",),
        report=TWO_PARTS_REPORT,
    ),

    # ----- Angka di ujung rentang -----
    Session(
        "angka-ekstrem",
        "Nama 30 huruf dan angka terbesar maupun terkecil tetap dihitung tepat dan tertata",
        typed=add_parts(
            part(NAME_30, "1000000", "10000", "99"),
            part("Harga Tertinggi", "1", "9999,999", "100000000"),
            part("Tepat di Batas Biaya", "27027", "1", "37000037"),
            part("Paling Ringan", "1", "0,001", "1"),
        ) + ("2", ENTER, "3", ENTER, "8", "ekstrem", ENTER) + QUIT,
        expect=(
            "Nama : " + NAME_30 + " Jumlah : 1.000.000 unit Berat/unit : 10.000,000 kg"
            " Harga/kg : Rp99 Total berat : 10.000.000.000,000 kg Total biaya : Rp990.000.000.000",
            STORED % (NAME_30, 1),
            "Nama : Harga Tertinggi Jumlah : 1 unit Berat/unit : 9.999,999 kg"
            " Harga/kg : Rp100.000.000 Total berat : 9.999,999 kg Total biaya : Rp999.999.900.000",
            "Nama : Tepat di Batas Biaya Jumlah : 27.027 unit Berat/unit : 1,000 kg"
            " Harga/kg : Rp37.000.037 Total berat : 27.027,000 kg Total biaya : Rp999.999.999.999",
            # Satu gram seharga Rp1 per kg bernilai sepersepuluh sen: dibulatkan menjadi Rp0.
            "Nama : Paling Ringan Jumlah : 1 unit Berat/unit : 0,001 kg"
            " Harga/kg : Rp1 Total berat : 0,001 kg Total biaya : Rp0",
            "Data: 4 komponen | 10.000.037.027,000 kg | Rp2.989.999.899.999",
            # Tabel rekap selebar ini tidak muat di 100 kolom, jadi tiap komponen tampil
            # sebagai kartu.
            "1. " + NAME_30 + " Jumlah : 1.000.000 Berat/unit : 10.000,000 kg Harga/kg : Rp99"
            " Total berat : 10.000.000.000,000 kg Total biaya : Rp990.000.000.000",
            "4. Paling Ringan Jumlah : 1 Berat/unit : 0,001 kg Harga/kg : Rp1"
            " Total berat : 0,001 kg Total biaya : Rp0",
            "Jenis komponen : 4 Jumlah unit : 1.027.029 Total berat : 10.000.037.027,000 kg"
            " Total biaya : Rp2.989.999.899.999",
            "- Tepat di Batas Biaya : Rp999.999.999.999 (33,4% dari total biaya)",
            "1 Tepat di Batas Biaya Rp999.999.999.999 33,4% " + BAR * 24
            + " 2 Harga Tertinggi Rp999.999.900.000 33,4% " + BAR * 24
            + " 3 " + NAME_30 + " Rp990.000.000.000 33,1% " + BAR * 24
            # Komponen berbiaya Rp0 tidak mendapat batang.
            + " 4 Paling Ringan Rp0 0,0% Porsi =",
            "[OK] Laporan disimpan ke ekstrem.txt.",
        ),
        saved=("ekstrem.txt",),
        report=(
            "1. " + NAME_30 + " Jumlah : 1.000.000",
            "Total berat : 10.000.037.027,000 kg Total biaya : Rp2.989.999.899.999",
            "- Tepat di Batas Biaya : Rp999.999.999.999 (33,4% dari total biaya)",
            "3 " + NAME_30 + " Rp990.000.000.000 33,1% " + "#" * 24 + " 4 Paling Ringan Rp0 0,0%",
        ),
    ),
    Session(
        "daftar-penuh",
        "Daftar menampung tepat 100 komponen; yang ke-101 ditolak sampai ada yang dihapus",
        typed=keep_adding(FULL_LIST) + (ENTER,)
        + ("1", ENTER)
        + ("2", ENTER)
        + ("5", "100", "y", ENTER)
        + keep_adding([part("Pengganti", "1", "1", "1000")]) + (ENTER,)
        + QUIT,
        expect=(
            STORED % ("Komponen Uji Tata Letak No 001", 1),
            "Komponen ke-100",
            STORED % ("Komponen Uji Tata Letak No 100", 100),
            "Tambah komponen lagi? [y/T] : y " + ERROR_MARK + LIST_FULL + " " + PAUSE,
            # Seratus biaya terbesar dijumlahkan tanpa kehilangan satu rupiah pun.
            "Data: 100 komponen | 2.702.700,000 kg | Rp99.999.999.999.900",
            "Pilihan Anda [0-8] : 1 Tambah Komponen",
            ERROR_MARK + LIST_FULL + " " + PAUSE,
            "Jenis komponen : 100 Jumlah unit : 2.702.700 Total berat : 2.702.700,000 kg"
            " Total biaya : Rp99.999.999.999.900",
            "Ada 100 komponen dengan biaya tertinggi yang sama:",
            "- Komponen Uji Tata Letak No 100 : Rp999.999.999.999 (1,0% dari total biaya)",
            '[OK] Komponen "Komponen Uji Tata Letak No 100" dihapus. Sisa 99 komponen.',
            "Komponen ke-100",
            STORED % ("Pengganti", 100),
            "Tambah komponen lagi? [y/T] : y " + ERROR_MARK + LIST_FULL + " " + PAUSE,
            "Data: 100 komponen | 2.675.674,000 kg | Rp99.000.000.000.901",
        ),
        errors=3,
    ),
    Session(
        "tepi-layar",
        "Pesan yang panjangnya pas di tepi layar tetap utuh pada setiap lebar",
        typed=EDGE_KEYS,
        expect=rejected("Nama komponen", EDGE_REJECTIONS) + (STORED % ("Tutup Ujung", 7),),
        errors=len(EDGE_NAMES),
    ),

    # ----- Menu 0 dan masukan yang habis -----
    Session(
        "keluar-batal-lalu-setuju",
        "Keluar saat masih ada data meminta konfirmasi; Enter atau t kembali ke menu",
        typed=add_parts(GEAR) + ("0", ENTER, "0", "t", "0", "y"),
        expect=(
            "Pilihan Anda [0-8] : 0 [i] Data komponen hanya tersimpan selama program berjalan."
            " Simpan laporan lewat menu 8 bila perlu. Keluar dari program? [y/T] : Menu Utama",
            "Data: 1 komponen | 44,400 kg | Rp821.400",
            "Keluar dari program? [y/T] : t Menu Utama",
            "Data: 1 komponen | 44,400 kg | Rp821.400",
            "Keluar dari program? [y/T] : y " + PROGRAM_DONE,
        ),
    ),
    Session(
        "eof-di-menu",
        "Masukan habis di menu utama: berhenti dengan kode 1 dan pesan di saluran galat",
        typed=(),
        exit_code=1,
        final="Ketik nomor menu lalu tekan Enter. Pilihan Anda [0-8] :",
        stderr=(INPUT_CLOSED,),
    ),
    Session(
        "eof-saat-menambah",
        "Masukan habis di tengah pengisian komponen: berhenti dengan kode 1",
        typed=("1", "Gear", "24"),
        exit_code=1,
        final="Nama komponen : Gear Jumlah (unit) : 24 Berat per unit (kg) :",
        stderr=(INPUT_CLOSED,),
    ),
    Session(
        "eof-saat-jeda",
        'Masukan habis di jeda "Tekan Enter": berhenti dengan kode 1',
        typed=("2",),
        exit_code=1,
        final=EMPTY_LIST + " " + PAUSE,
        stderr=(INPUT_CLOSED,),
    ),
)


# ---------------------------------------------------------------------------------------------
# Sesi tentang cara program dijalankan: hanya dipakai run_e2e.py.
# ---------------------------------------------------------------------------------------------

def smoke(name, purpose, **setup):
    """Sesi asap baku yang dijalankan dengan pengaturan lain.

    Tiga baris wajibnya diperiksa persis kecuali skenario menggantinya lewat exact: pada layar
    sempit baris itu dilipat, jadi di sana cukup diperiksa lewat expect yang tidak peduli lipatan.
    """
    setup.setdefault("exact", SMOKE_LINES)
    return Session(name, purpose, typed=SMOKE_KEYS, **setup)


def edge(name, purpose, exact, **setup):
    """Sesi tepi-layar pada lebar tertentu; exact berisi potongan layar pada lebar itu."""
    return Session(name, purpose, typed=EDGE_KEYS, errors=len(EDGE_NAMES), exact=exact, **setup)


HELP_TEXT = (
    "Pemakaian: estima [opsi]",
    "-h, --help",
    "-v, --version",
    "--no-color",
    "NO_COLOR",
    "--ascii",
    "Tanpa opsi, program membuka menu interaktif.",
)

LAUNCH_SESSIONS = (
    # ----- Opsi baris perintah -----
    Session(
        "bantuan",
        "--help menjelaskan semua opsi lalu selesai tanpa membuka menu",
        args=("--help",),
        expect=HELP_TEXT,
        final=REPOSITORY_LINE,
        checks=(help_names_version, fits_classic_terminal),
    ),
    Session(
        "bantuan-pendek",
        "-h mencetak bantuan yang sama persis dengan --help",
        args=("-h",),
        final=REPOSITORY_LINE,
        checks=(same_stdout_as(args=("--help",)),),
    ),
    Session(
        "versi",
        '--version mencetak "ESTIMA <versi>" sesuai app_info.h',
        args=("--version",),
        final="",
        checks=(prints_version,),
    ),
    Session(
        "versi-pendek",
        "-v sama dengan --version",
        args=("-v",),
        final="",
        checks=(prints_version,),
    ),
    Session(
        "opsi-tidak-dikenal",
        "Opsi yang tidak dikenal: kode 2, pesan dan bantuan di saluran galat, menu tidak dibuka",
        args=("--warna",),
        typed=("0",),
        exit_code=2,
        final="",
        stderr=("Opsi tidak dikenal: --warna",) + HELP_TEXT + (REPOSITORY_LINE,),
        checks=(nothing_on_stdout, fits_classic_terminal),
    ),
    Session(
        "opsi-salah-setelah-opsi-benar",
        "Satu argumen salah membatalkan seluruh perintah walaupun opsi sebelumnya benar",
        args=("--ascii", "data.txt"),
        typed=("0",),
        exit_code=2,
        final="",
        stderr=("Opsi tidak dikenal: data.txt", "Pemakaian: estima [opsi]"),
        checks=(nothing_on_stdout,),
    ),

    # ----- Warna -----
    smoke("warna-no-color-env", "NO_COLOR mematikan warna", env={"NO_COLOR": "1"}),
    smoke("warna-no-color-menang", "NO_COLOR tetap menang bila FORCE_COLOR juga dipasang",
          env={"NO_COLOR": "1", "FORCE_COLOR": "1"}),
    smoke("warna-opsi-no-color", "--no-color mematikan warna", args=("--no-color",)),
    smoke("warna-opsi-no-color-menang", "--no-color tetap menang bila FORCE_COLOR dipasang",
          args=("--no-color",), env={"FORCE_COLOR": "1"}),
    smoke("warna-force-color", "FORCE_COLOR=1 menyalakan warna walaupun keluaran berupa pipa",
          env={"FORCE_COLOR": "1"}, colored=True, checks=(colors_change_nothing_else,)),
    smoke("warna-force-color-nol", "FORCE_COLOR=0 tidak menyalakan warna",
          env={"FORCE_COLOR": "0"}),

    # ----- Mode ASCII -----
    smoke("ascii", "--ascii menggambar kotak dan batang dengan karakter ASCII saja",
          args=("--ascii",), checks=(only_ascii, ascii_mirrors_unicode)),
    smoke("ascii-berwarna", "--ascii bisa dipakai bersama warna",
          args=("--ascii",), env={"FORCE_COLOR": "1"}, colored=True,
          checks=(only_ascii, ascii_mirrors_unicode, colors_change_nothing_else)),

    # ----- Bentuk masukan -----
    smoke("masukan-crlf", "Ketikan berakhiran baris Windows (CRLF) dibaca sama seperti LF",
          newline="\r\n", checks=(same_stdout_as(newline="\n"),)),
    smoke("masukan-tanpa-baris-akhir", "Jawaban terakhir tanpa akhir baris tetap dibaca",
          unterminated=True, checks=(same_stdout_as(unterminated=False),)),
    Session(
        "nama-bukan-ascii",
        "Nama berisi karakter di luar ASCII ditolak dan layar tetap UTF-8 yang sah",
        typed=("1", "Poros ø20") + part("Poros 20", "1", "1", "1000") + ("t", ENTER) + QUIT,
        expect=("Nama komponen : Poros ø20 " + ERROR_MARK + NAME_CHARS,
                STORED % ("Poros 20", 1)),
        errors=1,
    ),

    # ----- Lebar layar (lihat penjelasan di atas EDGE_NAMES) -----
    edge("lebar-40", "Lebar 40: baris diisi sampai kolom ke-39, tidak lebih", EDGE_AT_40,
         width=40),
    edge("lebar-80", "Lebar 80: baris diisi sampai kolom ke-79, tidak lebih", EDGE_AT_80,
         width=80),
    edge("lebar-120", "Lebar 120: baris diisi sampai kolom ke-119, tidak lebih", EDGE_AT_120,
         width=120),
    edge("lebar-di-bawah-batas", "COLUMNS di bawah 40 diperlakukan sebagai 40", EDGE_AT_40,
         width=10),
    edge("lebar-di-atas-batas", "COLUMNS di atas 120 diperlakukan sebagai 120", EDGE_AT_120,
         width=500),
    edge("lebar-tanpa-columns", "Tanpa COLUMNS, keluaran ke pipa memakai 80 kolom", EDGE_AT_80,
         width=None),
    edge("lebar-bukan-angka", "COLUMNS yang bukan angka diabaikan dan 80 kolom dipakai",
         EDGE_AT_80, env={"COLUMNS": "lebar"}),
    smoke("lebar-40-kartu", "Di layar 40 kolom menu tanpa keterangan dan tabel menjadi kartu",
          width=40, exact=(),
          expect=(
              "1. Gear Jumlah : 24 Berat/unit : 1,850 kg Harga/kg : Rp18.500"
              " Total berat : 44,400 kg Total biaya : Rp821.400",
              "2. Shaft Jumlah : 12 Berat/unit : 4,200 kg Harga/kg : Rp32.000"
              " Total berat : 50,400 kg Total biaya : Rp1.612.800",
              "Total berat : 94,800 kg Total biaya : Rp2.434.200",
              "- Shaft : Rp1.612.800 (66,3% dari total biaya)",
          )),
)

ALL_SESSIONS = SCREEN_SESSIONS + LAUNCH_SESSIONS
