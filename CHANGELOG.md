# Changelog

Format mengikuti [Keep a Changelog](https://keepachangelog.com/id/1.1.0/), dan nomor versi mengikuti
[Semantic Versioning](https://semver.org/lang/id/).

## [Unreleased]

## [1.0.0] - 2026-11-11

### Ditambahkan
- Program konsol ESTIMA: estimasi kebutuhan material dan biaya produksi komponen mesin. Masukan tiap komponen adalah
  nama, jumlah, berat per unit (kg), dan harga material per kg; keluarannya total berat dan total biaya tiap
  komponen, total keseluruhan, dan daftar komponen dengan biaya tertinggi (hasil seri ikut ditampilkan).
- Menu interaktif: tambah, ubah, dan hapus komponen; rekap material dan biaya; peringkat biaya dengan grafik batang;
  data contoh; daftar harga acuan material; simpan laporan ke berkas teks.
- Daftar harga acuan untuk sebelas material (baja SS400, S45C, SCM440; stainless SUS304 dan SUS316L; besi cor FC250;
  aluminium 6061-T6; kuningan C3604; perunggu BC6; nylon MC; POM) dari daftar harga yang dibuka pada November 2026.
  Sumber, tanggal, dan hitungan konversi tiap titik harga dicatat di `docs/sumber-data.md`.
- Pemeriksaan masukan yang ketat dengan pesan yang menyebut bentuk yang benar; masukan yang habis menghentikan
  program dengan rapi (kode keluar 1).
- Hitungan dalam gram dan pembulatan ke rupiah terdekat, sehingga hasil tepat untuk seluruh rentang data yang diterima.
- Tampilan kotak dan tabel berwarna yang menyesuaikan lebar terminal 40 sampai 120 kolom; opsi `--no-color` dan
  `--ascii`; dukungan `NO_COLOR` dan `FORCE_COLOR`.
- Kode modular (`include/estima`, `src`) yang hanya memakai fitur C++98 dan lolos sampai C++23, dan edisi satu berkas
  `amalgamation/UTS_2531070011.cpp` yang dibangkitkan `scripts/amalgamate.py`.
- Uji unit, uji ujung-ke-ujung dengan sesi terskrip, dan gate tata letak untuk setiap lebar terminal.
- CI di Linux x86_64 dan ARM64, macOS, dan Windows, termasuk build biner universal macOS yang sama dengan paket rilis;
  matriks GCC dan Clang untuk C++98 sampai C++23; build MSVC; dan job GCC 4.9 (generasi Dev-C++ 5.11) yang
  mengompilasi berkas tunggal tanpa opsi, lalu berkas tunggal dan sumber modular dengan
  `-std=c++98 -Wall -Wextra -pedantic -Werror`, kemudian menjalankan sesi uji singkat.
- Paket rilis untuk Windows x86_64, macOS 11 ke atas (universal: Apple Silicon dan Intel), Linux x86_64 dan aarch64,
  dan Android aarch64 (Termux), dengan `SHA256SUMS`; skrip `scripts/coba.sh` dan `scripts/coba.ps1` untuk menjalankan
  rilis terbaru tanpa memasang (`coba.sh` menolak macOS yang lebih lama dari 11 dengan pesan yang jelas).
- Dokumen rancangan: pseudocode (`docs/pseudocode.md`) dan flowchart (`docs/flowchart.md`).

[Unreleased]: https://github.com/fadhlillah-dli/UTS_2531070011/compare/v1.0.0...HEAD
[1.0.0]: https://github.com/fadhlillah-dli/UTS_2531070011/releases/tag/v1.0.0
