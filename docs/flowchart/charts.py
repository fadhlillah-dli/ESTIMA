"""Definisi flowchart ESTIMA; digambar oleh scripts/render_flowcharts.py.

Setiap chart dibangun dalam satu fungsi lalu didaftarkan di CHARTS. Simpul ditaruh
di sel grid (kolom, baris); jalur utama lurus di kolom 0, cabang ke kanan, dan garis
balik perulangan lewat lane kiri (-0.5). Ringkasan API ada di docstring
scripts/render_flowcharts.py.

Nama variabel dan fungsi mengikuti docs/pseudocode.md (larik dinomori mulai 1);
padanan nama fungsi C++ ditulis di judul tiap chart.
"""

from render_flowcharts import Chart


def program_utama() -> Chart:
    """Alur utama: perulangan menu sampai pengguna memilih keluar."""
    c = Chart("fc_01_program_utama", title="Alur utama program (runApp)")

    c.terminal("mulai", "Mulai", 0, 0)
    c.process("siap", "Siapkan terminal\ndaftar.banyak = 0", 0, 1)
    c.io("banner", "Tampilkan banner", 0, 2)
    c.junction("ulang", 0, 2.5)
    c.io("menu", "Tampilkan menu dan\nringkasan data", 0, 3)
    c.io("baca", "Baca pilihan (0 sampai 8),\ndiulang sampai sah", 0, 4)
    c.decision("nol", "pilihan = 0 ?", 0, 5)
    c.sub("aksi", "Jalankan aksi sesuai pilihan:\n1 tambah, 2 rekap, 3 peringkat, 4 ubah,\n5 hapus, 6 data contoh, 7 harga acuan,\n8 simpan laporan", 0, 6)
    c.io("tunggu", "Tunggu pengguna\nmenekan Enter", 0, 7)

    c.decision("yakin", "daftar kosong atau pengguna\nsetuju keluar ?", 1, 5)
    c.io("tutup", "Tampilkan\n\"Program selesai\"", 1, 6)
    c.terminal("selesai", "Selesai", 1, 7)

    c.flow("mulai", "siap", "banner", "ulang", "menu", "baca", "nol")
    c.edge("nol", "aksi", "Tidak")
    c.flow("aksi", "tunggu")
    c.edge("tunggu", "ulang", exit="left", enter="left", lane=-0.5)
    c.edge("nol", "yakin", "Ya", exit="right", enter="left")
    c.edge("yakin", "ulang", "Tidak", exit="top", enter="right")
    c.edge("yakin", "tutup", "Ya")
    c.flow("tutup", "selesai")
    return c


def tambah_komponen() -> Chart:
    """Menu 1: mengisi komponen satu per satu selama pengguna masih ingin menambah."""
    c = Chart("fc_02_tambah_komponen", title="Menu 1: menambah komponen (actionAdd)")

    c.terminal("mulai", "Mulai", 0, 0)
    c.io("petunjuk", "Tampilkan petunjuk pengisian", 0, 1)
    c.junction("ulang", 0, 1.5)
    c.decision("penuh", "daftar.banyak = 100 ?", 0, 2)
    c.io("pesan_penuh", "Tampilkan\n\"Daftar sudah penuh\"", 1, 2)
    c.sub("baca", "Baca nama, jumlah, berat per unit,\ndan harga per kg ke komponen k\n(tiap masukan diulang sampai sah)", 0, 3)
    c.decision("batas", "hitungTotalBiaya(k)\n> batas biaya ?", 0, 4)
    c.io("pesan_batas", "Tampilkan pesan\nbatas biaya", 1, 4)
    c.io("ringkas", "Tampilkan ringkasan k:\ntotal berat dan total biaya", 0, 5)
    c.decision("setuju", "Simpan komponen ini ?", 0, 6)
    c.process("simpan", "daftar.banyak = daftar.banyak + 1\ndaftar.butir[daftar.banyak] = k", 0, 7)
    c.junction("gabung", 0, 7.5)
    c.decision("lagi", "Tambah komponen lagi ?", 0, 8)
    c.terminal("selesai", "Selesai", 0, 9)

    c.flow("mulai", "petunjuk", "ulang", "penuh")
    c.edge("penuh", "pesan_penuh", "Ya", exit="right", enter="left")
    c.edge("pesan_penuh", "selesai", exit="right", enter="right", lane=1.5)
    c.edge("penuh", "baca", "Tidak")
    c.flow("baca", "batas")
    c.edge("batas", "pesan_batas", "Ya", exit="right", enter="left")
    c.edge("pesan_batas", "lagi", enter="right")
    c.edge("batas", "ringkas", "Tidak")
    c.flow("ringkas", "setuju")
    c.edge("setuju", "simpan", "Ya")
    c.edge("setuju", "gabung", "Tidak", exit="right", enter="right", lane=0.3)
    c.flow("simpan", "gabung", "lagi")
    c.edge("lagi", "ulang", "Ya", exit="left", enter="left", lane=-0.5)
    c.edge("lagi", "selesai", "Tidak")
    return c


def baca_masukan() -> Chart:
    """Pola baca-periksa-ulangi yang dipakai semua masukan; contohnya masukan jumlah."""
    c = Chart("fc_03_baca_masukan", title="Membaca jumlah sampai sah (readQuantity)")

    c.terminal("mulai", "Mulai", 0, 0)
    c.junction("ulang", 0, 0.5)
    c.io("tanya", "Tampilkan pertanyaan,\nbaca satu baris ketikan", 0, 1)
    c.decision("angka", "Semua karakter\nberupa angka ?", 0, 2)
    c.decision("rentang", "1 <= nilai <= 1.000.000 ?", 0, 3)
    c.io("galat", "Tampilkan pesan kesalahan\ndan contoh penulisan\nyang benar", 1, 3)
    c.process("hasil", "kembalikan nilai", 0, 4)
    c.terminal("selesai", "Selesai", 0, 5)

    c.flow("mulai", "ulang", "tanya", "angka")
    c.edge("angka", "rentang", "Ya")
    c.edge("angka", "galat", "Tidak", exit="right", enter="top")
    c.edge("rentang", "galat", "Tidak", exit="right", enter="left")
    c.edge("rentang", "hasil", "Ya")
    c.edge("galat", "ulang", exit="right", enter="right", lane=1.5)
    c.flow("hasil", "selesai")
    return c


def hitung_berat_biaya() -> Chart:
    """Dua fungsi hitung yang diminta soal, digambar berdampingan."""
    c = Chart("fc_04_hitung_berat_biaya",
              title="Fungsi hitungTotalBerat (calculateTotalWeight) dan hitungTotalBiaya (calculateTotalCost)")

    c.terminal("b_mulai", "Mulai\nhitungTotalBerat(k)", 0, 0)
    c.process("b_gram", "gramPerUnit =\nbulatkan(k.beratPerUnit x 1000)", 0, 1)
    c.process("b_total", "totalGram =\nk.jumlah x gramPerUnit", 0, 2)
    c.process("b_hasil", "kembalikan\ntotalGram / 1000", 0, 3)
    c.terminal("b_selesai", "Selesai", 0, 4)
    c.flow("b_mulai", "b_gram", "b_total", "b_hasil", "b_selesai")

    c.terminal("c_mulai", "Mulai\nhitungTotalBiaya(k)", 1, 0)
    c.process("c_gram", "totalGram = k.jumlah x\nbulatkan(k.beratPerUnit x 1000)", 1, 1)
    c.process("c_biaya", "biaya = bulatkanKeBawah(\n(totalGram x k.hargaPerKg + 500) / 1000)", 1, 2)
    c.process("c_hasil", "kembalikan biaya", 1, 3)
    c.terminal("c_selesai", "Selesai", 1, 4)
    c.flow("c_mulai", "c_gram", "c_biaya", "c_hasil", "c_selesai")
    return c


def komponen_termahal() -> Chart:
    """Dua perulangan berurutan: mencari nilai tertinggi, lalu mengumpulkan pemiliknya."""
    # Chart ini paling tinggi (15 baris), jadi jarak antarbaris dirapatkan sedikit supaya
    # teksnya tetap terbaca ketika dimuatkan ke satu halaman A4.
    c = Chart("fc_05_komponen_termahal",
              title="Fungsi cariKomponenTermahal (findMostExpensive)", row_gap=15)

    c.terminal("mulai", "Mulai", 0, 0)
    c.decision("kosong", "banyak <= 0 ?", 0, 1)
    c.process("nol", "kembalikan 0", 1, 1)
    c.process("awal1", "tertinggi = hitungTotalBiaya(butir[1])\ni = 2", 0, 2)

    # Perulangan pertama: mencari biaya tertinggi. Junction di celah baris 2-3 menjadi
    # titik masuk garis balik tepat di atas uji perulangan.
    c.junction("ulang1", 0, 2.5)
    c.decision("uji1", "i <= banyak ?", 0, 3)
    c.process("biaya", "biaya = hitungTotalBiaya(butir[i])", 0, 4)
    c.decision("lebih", "biaya > tertinggi ?", 0, 5)
    c.process("ganti", "tertinggi = biaya", 0, 6)
    c.process("maju1", "i = i + 1", 0, 7)

    # Perulangan kedua: mencatat nomor semua komponen yang biayanya sama dengan tertinggi.
    c.process("awal2", "ditemukan = 0\ni = 1", 0, 8)
    c.junction("ulang2", 0, 8.5)
    c.decision("uji2", "i <= banyak ?", 0, 9)
    c.decision("sama", "hitungTotalBiaya(butir[i])\n= tertinggi ?", 0, 10)
    c.process("catat", "ditemukan = ditemukan + 1\nhasil[ditemukan] = i", 0, 11)
    c.process("maju2", "i = i + 1", 0, 12)
    c.process("jumlah", "kembalikan ditemukan", 0, 13)
    c.terminal("selesai", "Selesai", 0, 14)

    c.flow("mulai", "kosong")
    c.edge("kosong", "nol", "Ya", exit="right", enter="left")
    c.edge("nol", "selesai", enter="right")
    c.edge("kosong", "awal1", "Tidak")
    c.flow("awal1", "ulang1", "uji1")

    c.edge("uji1", "biaya", "Ya")
    c.edge("uji1", "awal2", "Tidak", exit="right", lane=0.75)
    c.flow("biaya", "lebih")
    c.edge("lebih", "ganti", "Ya")
    c.edge("lebih", "maju1", "Tidak", exit="right", enter="right", lane=0.3)
    c.flow("ganti", "maju1")
    c.edge("maju1", "ulang1", exit="left", enter="left", lane=-0.5)

    c.flow("awal2", "ulang2", "uji2")
    c.edge("uji2", "sama", "Ya")
    c.edge("uji2", "jumlah", "Tidak", exit="right", lane=0.75)
    c.edge("sama", "catat", "Ya")
    c.edge("sama", "maju2", "Tidak", exit="right", enter="right", lane=0.3)
    c.flow("catat", "maju2")
    c.edge("maju2", "ulang2", exit="left", enter="left", lane=-0.5)
    c.flow("jumlah", "selesai")
    return c


def rekap() -> Chart:
    """Menu 2: rekap disusun dari fungsi-fungsi hitung, dicetak baris demi baris."""
    c = Chart("fc_06_rekap", title="Menu 2: menyusun rekap (printRecap)")

    c.terminal("mulai", "Mulai", 0, 0)
    c.decision("kosong", "daftar.banyak = 0 ?", 0, 1)
    c.io("pesan", "Tampilkan\n\"Belum ada komponen\"", 1, 1)
    c.sub("termahal", "nTermahal = cariKomponenTermahal(\nbutir, banyak, termahal)", 0, 2)
    c.sub("total", "totalBerat = jumlahTotalBerat(butir, banyak)\ntotalBiaya = jumlahTotalBiaya(butir, banyak)", 0, 3)
    c.process("awal", "i = 1", 0, 4)
    c.junction("ulang", 0, 4.5)
    c.decision("uji", "i <= banyak ?", 0, 5)
    c.io("baris", "Tampilkan baris ke-i: nama, jumlah,\nberat per unit, harga per kg,\nhitungTotalBerat(butir[i]),\nhitungTotalBiaya(butir[i])", 0, 6)
    c.process("maju", "i = i + 1", 0, 7)
    c.io("cetak_total", "Tampilkan jenis komponen, jumlah unit,\ntotalBerat, dan totalBiaya", 0, 8)
    c.io("cetak_termahal", "Tampilkan nTermahal komponen termahal\nbeserta biaya dan porsinya", 0, 9)
    c.terminal("selesai", "Selesai", 0, 10)

    c.flow("mulai", "kosong")
    c.edge("kosong", "pesan", "Ya", exit="right", enter="left")
    c.edge("pesan", "selesai", enter="right")
    c.edge("kosong", "termahal", "Tidak")
    c.flow("termahal", "total", "awal", "ulang", "uji")
    c.edge("uji", "baris", "Ya")
    c.flow("baris", "maju")
    c.edge("maju", "ulang", exit="left", enter="left", lane=-0.5)
    c.edge("uji", "cetak_total", "Tidak", exit="right", lane=0.75)
    c.flow("cetak_total", "cetak_termahal", "selesai")
    return c


CHARTS = [
    program_utama(),
    tambah_komponen(),
    baca_masukan(),
    hitung_berat_biaya(),
    komponen_termahal(),
    rekap(),
]
