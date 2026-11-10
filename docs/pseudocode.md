# Pseudocode ESTIMA

Pseudocode di bawah menggambarkan cara kerja program dengan kalimat terstruktur berbahasa Indonesia.
Nama di dalam kurung kurawal `{ ... }` adalah nama fungsi yang sama di kode C++, supaya tiap bagian
mudah dicocokkan dengan sumbernya di `src/`.

Notasi yang dipakai: `<-` mengisi nilai, `JIKA ... MAKA ... SELAIN ITU ... AKHIR JIKA` untuk
percabangan, `SELAMA`, `UNTUK`, dan `ULANGI ... SAMPAI` untuk perulangan, `KEMBALIKAN` untuk hasil
fungsi, dan `larik[1..n]` untuk array yang elemennya dinomori mulai 1 (di C++ nomornya mulai 0).

## 1. Data

```text
KAMUS
    konstanta MAKS_KOMPONEN = 100
    konstanta BATAS_BIAYA   = 999.999.999.999        { rupiah per komponen }

    tipe Komponen = rekaman                           { struct Component }
        nama         : teks                           { 1-30 karakter, unik }
        jumlah       : bilangan bulat                 { 1 .. 1.000.000 unit }
        beratPerUnit : bilangan pecahan               { 0,001 .. 10.000 kg }
        hargaPerKg   : bilangan pecahan               { Rp1 .. Rp100.000.000 }
    akhir rekaman

    tipe DaftarKomponen = rekaman                     { struct ComponentList }
        butir  : larik[1..MAKS_KOMPONEN] dari Komponen
        banyak : bilangan bulat                       { jumlah elemen yang terisi }
    akhir rekaman
```

## 2. Program utama

```text
PROGRAM Estima                                        { runApp, runMenuLoop }
KAMUS
    daftar  : DaftarKomponen
    pilihan : bilangan bulat
    selesai : logika
ALGORITMA
    siapkan terminal (warna, UTF-8, lebar jendela)
    daftar.banyak <- 0
    tampilkan banner identitas program
    selesai <- SALAH

    SELAMA BUKAN selesai LAKUKAN
        tampilkan menu utama dan ringkasan data (banyak komponen, total berat, total biaya)
        pilihan <- bacaPilihan(0, 8)                  { diulang sampai angkanya sah }

        PILIH pilihan
            1 : tambahKomponen(daftar)
            2 : tampilkanRekap(daftar)
            3 : tampilkanPeringkat(daftar)
            4 : ubahKomponen(daftar)
            5 : hapusKomponen(daftar)
            6 : muatDataContoh(daftar)
            7 : tampilkanHargaAcuan()
            8 : simpanLaporan(daftar)
            0 : JIKA daftar.banyak = 0 ATAU tanya("Keluar dari program?") MAKA
                    selesai <- BENAR
                AKHIR JIKA
        AKHIR PILIH

        JIKA BUKAN selesai DAN pilihan <> 0 MAKA
            tunggu pengguna menekan Enter
        AKHIR JIKA
    AKHIR SELAMA

    tampilkan "Program selesai. Terima kasih."
AKHIR PROGRAM
```

## 3. Fungsi perhitungan

Berat diketik dalam kilogram dengan paling banyak tiga angka desimal. Supaya hasil kali tidak
terganggu pecahan biner, berat diubah dulu ke gram (bilangan bulat), dikalikan, lalu dikembalikan
ke kilogram.

```text
FUNGSI hitungTotalBerat(k : Komponen) -> bilangan pecahan          { calculateTotalWeight }
    gramPerUnit <- bulatkan(k.beratPerUnit * 1000)
    KEMBALIKAN (k.jumlah * gramPerUnit) / 1000                     { kilogram }
AKHIR FUNGSI

FUNGSI hitungTotalBiaya(k : Komponen) -> bilangan pecahan          { calculateTotalCost }
    totalGram <- k.jumlah * bulatkan(k.beratPerUnit * 1000)
    { total berat (kg) x harga per kg, dibulatkan ke rupiah terdekat }
    KEMBALIKAN bulatkanKeBawah((totalGram * k.hargaPerKg + 500) / 1000)
AKHIR FUNGSI

{ sumTotalWeight }
FUNGSI jumlahTotalBerat(butir : larik Komponen, banyak : bilangan bulat) -> bilangan pecahan
    total <- 0
    UNTUK i <- 1 SAMPAI banyak LAKUKAN
        total <- total + hitungTotalBerat(butir[i])
    AKHIR UNTUK
    KEMBALIKAN total
AKHIR FUNGSI

{ sumTotalCost }
FUNGSI jumlahTotalBiaya(butir : larik Komponen, banyak : bilangan bulat) -> bilangan pecahan
    total <- 0
    UNTUK i <- 1 SAMPAI banyak LAKUKAN
        total <- total + hitungTotalBiaya(butir[i])
    AKHIR UNTUK
    KEMBALIKAN total
AKHIR FUNGSI
```

## 4. Mencari komponen termahal

Hasilnya bisa lebih dari satu komponen bila ada biaya tertinggi yang sama, jadi fungsi mengisi
larik `hasil` dengan nomor semua komponen itu dan mengembalikan banyaknya.

```text
{ findMostExpensive }
FUNGSI cariKomponenTermahal(butir : larik Komponen, banyak : bilangan bulat,
                            hasil : larik bilangan bulat) -> bilangan bulat
    JIKA banyak <= 0 MAKA
        KEMBALIKAN 0
    AKHIR JIKA

    { langkah 1: cari nilai biaya tertinggi }
    tertinggi <- hitungTotalBiaya(butir[1])
    UNTUK i <- 2 SAMPAI banyak LAKUKAN
        biaya <- hitungTotalBiaya(butir[i])
        JIKA biaya > tertinggi MAKA
            tertinggi <- biaya
        AKHIR JIKA
    AKHIR UNTUK

    { langkah 2: kumpulkan semua komponen yang biayanya sama dengan nilai itu }
    ditemukan <- 0
    UNTUK i <- 1 SAMPAI banyak LAKUKAN
        JIKA hitungTotalBiaya(butir[i]) = tertinggi MAKA
            ditemukan <- ditemukan + 1
            hasil[ditemukan] <- i
        AKHIR JIKA
    AKHIR UNTUK
    KEMBALIKAN ditemukan
AKHIR FUNGSI
```

## 5. Menyusun peringkat biaya

```text
PROSEDUR urutkanPeringkat(butir : larik Komponen, banyak : bilangan bulat,
                          urutan : larik bilangan bulat)                         { rankByCost }
    UNTUK i <- 1 SAMPAI banyak LAKUKAN
        urutan[i] <- i
    AKHIR UNTUK

    { insertion sort pada nomor urut, dari biaya terbesar; data komponen tidak dipindah }
    UNTUK i <- 2 SAMPAI banyak LAKUKAN
        sekarang <- urutan[i]
        biayaSekarang <- hitungTotalBiaya(butir[sekarang])
        posisi <- i - 1
        SELAMA posisi >= 1 DAN hitungTotalBiaya(butir[urutan[posisi]]) < biayaSekarang LAKUKAN
            urutan[posisi + 1] <- urutan[posisi]
            posisi <- posisi - 1
        AKHIR SELAMA
        urutan[posisi + 1] <- sekarang
    AKHIR UNTUK
AKHIR PROSEDUR
```

## 6. Menambah komponen

```text
PROSEDUR tambahKomponen(daftar : DaftarKomponen)                                 { actionAdd }
KAMUS
    k : Komponen
ALGORITMA
    tampilkan petunjuk pengisian
    ULANGI
        JIKA daftar.banyak = MAKS_KOMPONEN MAKA
            tampilkan "Daftar sudah penuh"
            hentikan perulangan
        AKHIR JIKA

        k.nama         <- bacaNama(daftar)       { diulang sampai sah dan belum dipakai }
        k.jumlah       <- bacaJumlah()           { bilangan bulat 1 .. 1.000.000 }
        k.beratPerUnit <- bacaBerat()            { 0,001 .. 10.000 kg, koma atau titik }
        k.hargaPerKg   <- bacaHarga()            { pilih harga acuan atau ketik sendiri }

        JIKA hitungTotalBiaya(k) > BATAS_BIAYA MAKA
            tampilkan "Total biaya melebihi batas yang bisa dihitung"
        SELAIN ITU
            tampilkan ringkasan: nama, jumlah, berat per unit, harga per kg,
                                 hitungTotalBerat(k), hitungTotalBiaya(k)
            JIKA tanya("Simpan komponen ini?") MAKA
                daftar.banyak <- daftar.banyak + 1
                daftar.butir[daftar.banyak] <- k
            AKHIR JIKA
        AKHIR JIKA
    SAMPAI BUKAN tanya("Tambah komponen lagi?")
AKHIR PROSEDUR
```

Setiap pembaca masukan memakai pola yang sama: tanya, periksa, ulangi sampai benar.

```text
{ readQuantity, checkQuantity }
FUNGSI bacaJumlah() -> bilangan bulat
    ULANGI
        baca teks dari keyboard
        JIKA teks hanya berisi angka DAN 1 <= nilai(teks) <= 1.000.000 MAKA
            KEMBALIKAN nilai(teks)
        AKHIR JIKA
        tampilkan pesan kesalahan beserta contoh penulisan yang benar
    SAMPAI masukan sah
AKHIR FUNGSI
```

## 7. Menampilkan rekap

```text
PROSEDUR tampilkanRekap(daftar : DaftarKomponen)                                 { printRecap }
KAMUS
    termahal  : larik[1..MAKS_KOMPONEN] dari bilangan bulat
    nTermahal : bilangan bulat
ALGORITMA
    JIKA daftar.banyak = 0 MAKA
        tampilkan "Belum ada komponen"
        selesai
    AKHIR JIKA

    nTermahal   <- cariKomponenTermahal(daftar.butir, daftar.banyak, termahal)
    totalBerat  <- jumlahTotalBerat(daftar.butir, daftar.banyak)
    totalBiaya  <- jumlahTotalBiaya(daftar.butir, daftar.banyak)

    UNTUK i <- 1 SAMPAI daftar.banyak LAKUKAN
        tampilkan i, nama, jumlah, berat per unit, harga per kg,
                  hitungTotalBerat(daftar.butir[i]), hitungTotalBiaya(daftar.butir[i])
    AKHIR UNTUK

    tampilkan jenis komponen, jumlah unit, totalBerat, totalBiaya

    UNTUK j <- 1 SAMPAI nTermahal LAKUKAN
        k <- daftar.butir[termahal[j]]
        tampilkan k.nama, hitungTotalBiaya(k), porsi terhadap totalBiaya
    AKHIR UNTUK
AKHIR PROSEDUR
```

## 8. Menghapus komponen

```text
{ actionDelete, removeComponent }
PROSEDUR hapusKomponen(daftar : DaftarKomponen)
    JIKA daftar.banyak = 0 MAKA
        tampilkan "Belum ada komponen"
        selesai
    AKHIR JIKA
    tampilkan daftar nomor dan nama komponen
    nomor <- bacaPilihan(0, daftar.banyak)       { 0 berarti batal }
    JIKA nomor = 0 ATAU BUKAN tanya("Hapus komponen ini?") MAKA
        selesai
    AKHIR JIKA

    { elemen di kanan nomor digeser satu langkah ke kiri supaya tidak ada lubang }
    UNTUK i <- nomor SAMPAI daftar.banyak - 1 LAKUKAN
        daftar.butir[i] <- daftar.butir[i + 1]
    AKHIR UNTUK
    daftar.banyak <- daftar.banyak - 1
AKHIR PROSEDUR
```
