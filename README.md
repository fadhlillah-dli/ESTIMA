# ESTIMA

[![C++98 sampai C++23](https://img.shields.io/badge/C%2B%2B-98%20%E2%80%93%2023-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20macOS%20%7C%20Linux%20%7C%20Android%20(Termux)-lightgrey)](#menjalankan)
[![CI](../../actions/workflows/ci.yml/badge.svg)](../../actions/workflows/ci.yml)

**ESTIMA** (Estimasi Material) adalah program konsol C++ untuk menghitung kebutuhan material dan memperkirakan
biaya produksi komponen mesin. Program ini adalah jawaban Ujian Tengah Semester mata kuliah **Algoritma dan
Pemrograman** (Teknik Informatika, Universitas Sains Indonesia, kelas 26FA, semester ganjil 2026/2027).

Untuk setiap komponen pengguna mengisi nama, jumlah, berat per unit (kg), dan harga material per kg. Program
menghitung total berat dan total biaya tiap komponen, total keseluruhan, dan menunjukkan komponen dengan biaya
produksi tertinggi, dalam menu interaktif dengan pemeriksaan masukan yang ketat dan tampilan tabel berwarna.

*ESTIMA is a C++ console program that estimates material needs and production cost for machine components
(a mid-term exam answer for an introductory programming course). It ships as a modular code base plus a
single-file edition, compiles cleanly from C++98 to C++23 on GCC, Clang, MinGW-w64 and MSVC, and has prebuilt
packages for Windows 10/11, macOS 11 or later, Linux and Android (Termux). The user interface is in Indonesian.*

## Demo

![Demo ESTIMA di terminal](docs/img/demo.gif)

```text
┌─ Rekap Material dan Biaya ───────────────────────────────────────────┐
│ No  Komponen  Jumlah  Berat/unit  Harga/kg  Total berat  Total biaya │
│ ──  ────────  ──────  ──────────  ────────  ───────────  ─────────── │
│  1  Gear          24    1,850 kg  Rp19.500    44,400 kg    Rp865.800 │
│  2  Shaft         12    4,200 kg  Rp27.000    50,400 kg  Rp1.360.800 │
│  3  Bracket       40    0,750 kg  Rp14.000    30,000 kg    Rp420.000 │
│ ──  ────────  ──────  ──────────  ────────  ───────────  ─────────── │
│     TOTAL         76                         124,800 kg  Rp2.646.600 │
└──────────────────────────────────────────────────────────────────────┘
┌─ Total Keseluruhan ──────────┐
│ Jenis komponen : 3           │
│ Jumlah unit    : 76          │
│ Total berat    : 124,800 kg  │
│ Total biaya    : Rp2.646.600 │
└──────────────────────────────┘
┌─ Biaya Produksi Tertinggi ─────────────────────┐
│ - Shaft : Rp1.360.800 (51,4% dari total biaya) │
└────────────────────────────────────────────────┘
```

## Fitur

- **Sesuai soal**: total berat dan total biaya tiap komponen, total keseluruhan, dan daftar komponen dengan biaya
  tertinggi (semua komponen yang biayanya sama-sama tertinggi ikut ditampilkan).
- **Menu lengkap**: tambah, ubah, dan hapus komponen; rekap; peringkat biaya dengan grafik batang; data contoh;
  daftar harga acuan; simpan laporan ke berkas teks.
- **Harga acuan material** untuk sebelas bahan yang umum dipakai bengkel mesin, dikumpulkan dari daftar harga yang
  dibuka pada November 2026 dan dicatat sumbernya satu per satu di [docs/sumber-data.md](docs/sumber-data.md). Harga
  acuan hanya titik awal: harga selalu bisa diketik sendiri.
- **Pemeriksaan masukan yang ketat**: setiap ketikan diperiksa, ditolak dengan pesan yang menyebut bentuk yang benar,
  lalu ditanyakan lagi. Masukan yang habis (Ctrl+D atau Ctrl+Z) menghentikan program dengan rapi.
- **Hitungan tepat**: berat dihitung dalam gram dan biaya dibulatkan ke rupiah terdekat, sehingga jumlah baris tabel
  selalu sama dengan totalnya.
- **Tampilan menyesuaikan lebar terminal** dari 40 sampai 120 kolom: di jendela sempit (misalnya Termux di ponsel)
  tabel berubah menjadi susunan ke bawah. Warna mati sendiri bila keluaran bukan terminal atau `NO_COLOR` disetel.
- **Portabel**: satu kode sumber yang lolos `-Wall -Wextra -pedantic -Werror` pada C++98 sampai C++23, sehingga
  bisa dikompilasi dari Dev-C++ lama sampai compiler terbaru.
- **Setiap baris kode berkomentar** dalam bahasa Indonesia: apa yang dipakai dan mengapa dipilih.

## Menjalankan

Satu perintah, tanpa memasang apa pun: paket rilis terbaru diunduh ke folder sementara, diperiksa dengan SHA-256,
dijalankan, lalu foldernya dihapus begitu program ditutup (juga bila dihentikan dengan Ctrl+C). Tidak perlu hak
Administrator dan PATH tidak disentuh.

macOS 11 (Big Sur) atau lebih baru, Linux (x86_64 atau aarch64/ARM64, misalnya Raspberry Pi 64-bit dan server ARM),
dan Android lewat Termux (aarch64; `pkg install curl` bila belum ada), di Terminal:

```bash
curl -fsSL https://raw.githubusercontent.com/fadhlillah-dli/UTS_2531070011/main/scripts/coba.sh | sh
```

Windows 10/11 (x64), di **PowerShell** (tekan tombol Windows, ketik `PowerShell`, Enter; bukan Command Prompt):

```powershell
irm https://raw.githubusercontent.com/fadhlillah-dli/UTS_2531070011/main/scripts/coba.ps1 | iex
```

Paket diunduh ulang setiap kali perintah dijalankan (sekitar 1 MB), jadi perlu koneksi internet. Paket macOS
jalan di Mac Apple Silicon maupun Intel. Di macOS yang lebih lama dari 11, `coba.sh` berhenti dan menjelaskan
alasannya; di sana program tetap bisa dikompilasi sendiri seperti di bawah.

## Kompilasi sendiri

Syaratnya hanya compiler C++ (standar apa pun dari C++98 ke atas).

- macOS: `xcode-select --install`
- Linux (Debian/Ubuntu): `sudo apt install build-essential`
- Android (Termux): `pkg install clang git`
- Windows: pasang [MSYS2](https://www.msys2.org/), lalu `pacman -S mingw-w64-ucrt-x86_64-gcc` dan tambahkan
  `C:\msys64\ucrt64\bin` ke PATH; atau pakai Dev-C++, Code::Blocks, maupun Visual Studio

**Satu berkas.** [amalgamation/UTS_2531070011.cpp](amalgamation/UTS_2531070011.cpp) memuat seluruh program, jadi
cukup satu perintah:

```bash
g++ amalgamation/UTS_2531070011.cpp -o estima && ./estima
```

Di Dev-C++, buka berkas itu lalu pilih *Execute > Compile & Run* (F11); di Code::Blocks, *Build > Build and run*
(F9). Dengan compiler Visual Studio: `cl /EHsc amalgamation\UTS_2531070011.cpp`.

**Modular.** Kode yang sama, dipisah per modul di `include/` dan `src/`:

```bash
git clone https://github.com/fadhlillah-dli/UTS_2531070011.git
cd UTS_2531070011
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude src/*.cpp -o estima && ./estima
```

| Cara | Perintah |
| --- | --- |
| CMake | `cmake --preset default && cmake --build --preset default && ctest --preset default` |
| Make | `make` (program), `make single` (dari satu berkas), `make test` (uji), `make run` |
| VS Code | Open Folder, lalu Ctrl+Shift+B (build) atau Terminal > Run Task > "Build & Run" |

Opsi CMake: `-DESTIMA_CXX_STANDARD=98|11|14|17|20|23` (bawaan 17) dan `-DESTIMA_WARNINGS_AS_ERRORS=ON`.

Preset CMake di atas butuh CMake 3.21 ke atas. Untuk CMake 3.16 sampai 3.20, tulis langkahnya satu per satu:
`cmake -S . -B build/default`, lalu `cmake --build build/default`, lalu `cd build/default && ctest`. Di Windows,
Makefile dijalankan dengan `mingw32-make` dari MSYS2 (paket `mingw-w64-ucrt-x86_64-make`), bukan `make`.

## Cara pakai

1. Pilih menu dengan mengetik nomornya lalu Enter.
2. Pada **Tambah komponen**, isi nama, jumlah, dan berat per unit. Program lalu menampilkan harga acuan: pilih
   nomornya (Enter menerima harga itu, atau ketik harga lain), atau pilih `0` untuk langsung mengetik harga sendiri.
3. **Rekap material & biaya** menampilkan semua hasil hitung; **Peringkat biaya** mengurutkan komponen dari biaya
   terbesar; **Simpan laporan** menulis keduanya ke berkas `.txt` di folder tempat program dijalankan.

| Masukan | Aturan |
| --- | --- |
| Nama komponen | 1-30 karakter: huruf, angka, spasi, dan tanda `- _ . , / ( ) + # &`; tidak boleh sama dengan komponen lain |
| Jumlah | bilangan bulat 1 sampai 1.000.000 |
| Berat per unit | 0,001 sampai 10.000 kg; desimal dengan koma atau titik, paling banyak tiga angka di belakangnya |
| Harga per kg | rupiah bulat Rp1 sampai Rp100.000.000, ditulis tanpa titik (contoh `18500`) |

Opsi baris perintah: `-h`/`--help`, `-v`/`--version`, `--no-color`, dan `--ascii` (menggambar kotak dengan `+ - |`
untuk terminal tanpa UTF-8).

Data uji dan hasil yang diharapkan:

| Data | Hasil |
| --- | --- |
| Gear 24 x 1,85 kg x Rp18.500 dan Shaft 12 x 4,2 kg x Rp32.000 (harga diketik sendiri) | total berat 94,800 kg; total biaya Rp2.434.200; termahal Shaft Rp1.612.800 (66,3%) |
| Gear 24 x 1,85 kg, Shaft 12 x 4,2 kg, Bracket 40 x 0,75 kg dengan harga acuan S45C, SCM440, dan SS400 | 124,800 kg; Rp2.646.600; termahal Shaft Rp1.360.800 (51,4%) |
| Menu **Muat data contoh** | lima komponen; 217,932 kg; Rp7.265.664; termahal Housing Rp2.067.200 |

## Harga acuan material

| Material | Standar | Harga acuan per kg |
| --- | --- | ---: |
| Baja SS400 | JIS G3101 | Rp14.000 |
| Baja karbon S45C | JIS G4051 | Rp19.500 |
| Baja paduan SCM440 | JIS G4053 | Rp27.000 |
| Stainless SUS304 | JIS G4303 | Rp53.000 |
| Stainless SUS316L | JIS G4303 | Rp100.000 |
| Besi cor FC250 | JIS G5501 | Rp38.000 |
| Aluminium 6061-T6 | JIS H4040 | Rp135.000 |
| Kuningan C3604 | JIS H3250 | Rp250.000 |
| Perunggu BC6 | JIS H5120 | Rp325.000 |
| Nylon MC (PA6) | ASTM D5989 | Rp120.000 |
| POM (polyacetal) | ASTM D6100 | Rp100.000 |

Angka di atas adalah nilai tengah harga dari daftar harga distributor dan toko daring di Indonesia yang dibuka pada
10 November 2026, lalu dibulatkan. Banyaknya harga di balik tiap angka tidak sama: sebagian besar material memakai tiga
harga atau lebih, tetapi S45C hanya memakai dua harga distributor dan SCM440 hanya satu. Angka SCM440, besi cor FC250,
dan perunggu BC6 berkeyakinan rendah (harga FC250 hampir semuanya dari satu toko, dan harga BC6 sebarannya lebar), jadi
sebaiknya dianggap perkiraan kasar. Setiap titik harga, tautan sumbernya, hitungan konversi ke rupiah per kilogram, dan
tingkat keyakinannya ada di [docs/sumber-data.md](docs/sumber-data.md), beserta cara memperbaruinya.

## Rancangan

- [docs/pseudocode.md](docs/pseudocode.md): pseudocode program utama dan fungsi-fungsi intinya (perhitungan,
  pencarian komponen termahal, peringkat, tambah, pembacaan masukan, rekap, dan hapus).
- [docs/flowchart.md](docs/flowchart.md): flowchart alur utama, pengisian komponen, pembacaan masukan, fungsi hitung,
  pencarian komponen termahal, dan penyusunan rekap.

Data satu komponen disimpan dalam `struct Component`; daftarnya adalah array berukuran tetap di dalam
`struct ComponentList`. Tiga fungsi yang diminta soal ada di modul `calculation`:

| Fungsi | Tugas |
| --- | --- |
| `calculateTotalWeight()` | total berat satu komponen (jumlah x berat per unit) |
| `calculateTotalCost()` | total biaya satu komponen (total berat x harga per kg, dibulatkan ke rupiah) |
| `findMostExpensive()` | nomor semua komponen yang biayanya tertinggi |

`sumTotalWeight()`, `sumTotalCost()`, dan `rankByCost()` melengkapinya untuk total keseluruhan dan peringkat.

## Struktur proyek

```text
UTS_2531070011/
├── include/estima/          deklarasi tiap modul (.h)
├── src/                     isi tiap modul (.cpp) dan main.cpp
├── amalgamation/            UTS_2531070011.cpp: seluruh program dalam satu berkas
├── tests/
│   ├── unit/                uji fungsi satu per satu (kerangka uji kecil buatan sendiri)
│   └── e2e/                 uji program utuh lewat sesi ketikan terskrip dan gate tata letak
├── scripts/
│   ├── amalgamate.py        menyusun berkas tunggal dari include/ dan src/
│   ├── render_flowcharts.py menggambar flowchart dari docs/flowchart/charts.py
│   └── coba.sh · coba.ps1   menjalankan rilis terbaru sekali jalan tanpa memasang
├── docs/                    pseudocode, flowchart, sumber data harga, gambar
├── .github/                 workflows/ (CI dan pembuatan paket rilis), dependabot.yml
├── .vscode/                 tasks.json dan c_cpp_properties.json untuk VS Code
├── CMakeLists.txt · CMakePresets.json · Makefile
├── .editorconfig · .gitattributes · .gitignore
└── README.md · CHANGELOG.md · CONTRIBUTING.md · SECURITY.md · LICENSE
```

| Modul | Isi |
| --- | --- |
| `component` | `struct Component`, daftar komponen, operasi tambah, hapus, dan cari |
| `calculation` | rumus berat dan biaya, pencarian komponen termahal, penyusunan peringkat |
| `material_catalog` | daftar harga acuan dan data contoh |
| `input_parse` | pemeriksaan teks ketikan dan pengubahannya menjadi angka |
| `prompt` | tanya jawab dengan pengguna; pertanyaan diulang sampai jawabannya sah |
| `text_format` | penulisan angka, rupiah, dan berat; pelipatan teks panjang menjadi beberapa baris |
| `ui` | kotak, tabel, dan warna |
| `terminal` | urusan dengan sistem operasi: warna, UTF-8, lebar jendela |
| `report` | rekap, peringkat, daftar harga acuan, laporan ke berkas |
| `app` | menu utama dan alur tiap pilihan |

Berkas tunggal di `amalgamation/` dibangkitkan dari modul-modul itu, jadi perubahan selalu dibuat di `include/` dan
`src/`, lalu berkas tunggal disusun ulang dengan `python3 scripts/amalgamate.py`. CI menolak perubahan bila keduanya
tidak lagi sama.

## Pengujian

```bash
make test                                              # uji unit, lalu uji ujung-ke-ujung
python3 tests/e2e/run_e2e.py --exe build/estima        # sesi terskrip terhadap biner mana pun
python3 tests/e2e/check_layout.py --exe build/estima   # gate tata letak 40-120 kolom
python3 scripts/amalgamate.py --check                  # berkas tunggal masih sama dengan modulnya?
```

Skrip uji butuh Python 3.8 ke atas. Di Windows, perintah Python-nya `python`, bukan `python3`.

- **Uji unit** memeriksa tiap fungsi dengan angka yang jawabannya dihitung tangan: rumus, pembulatan, hasil seri,
  setiap aturan pemeriksaan masukan, pelipatan teks panjang, dan penggambaran tabel.
- **Uji ujung-ke-ujung** (`tests/e2e`) menjalankan program utuh lewat puluhan skenario ketikan, termasuk setiap jenis
  masukan salah, masukan yang habis di tengah jalan, dan penyimpanan laporan.
- **Gate tata letak** menjalankan program pada setiap lebar terminal dari 40 sampai 120 kolom (polos, berwarna, dan
  mode ASCII) dan gagal bila ada satu baris pun yang melebihi lebar terminal.
- **CI** (GitHub Actions) mengompilasi dan menguji di Linux x86_64 dan ARM64, macOS, dan Windows, dan di macOS juga
  membangun biner universal persis seperti paket rilis; mengompilasi dengan GCC dan Clang pada C++98, 11, 14, 17, 20,
  dan 23, serta dengan MSVC; mengompilasi dengan GCC 4.9 (generasi compiler bawaan Dev-C++ 5.11), yaitu berkas
  tunggal tanpa opsi apa pun seperti di Dev-C++, lalu berkas tunggal dan sumber modular dengan
  `-std=c++98 -Wall -Wextra -pedantic -Werror`, kemudian menjalankan sesi uji singkat; dan memeriksa skrip `coba`.

## Konvensi kode

| Jenis | Gaya | Contoh |
| --- | --- | --- |
| Tipe | PascalCase | `Component`, `ComponentList`, `MaterialReference` |
| Fungsi dan variabel | lowerCamelCase | `calculateTotalCost()`, `unitWeightKg` |
| Konstanta | UPPER_SNAKE_CASE | `MAX_COMPONENTS`, `MATERIAL_CATALOG` |

Identifier berbahasa Inggris; komentar dan teks tampilan berbahasa Indonesia. Kode hanya memakai fitur C++ yang sudah
ada sejak C++98 (tanpa `auto`, lambda, atau fitur lain yang baru ada sejak C++11), dan berkas sumbernya ASCII murni:
karakter garis kotak ditulis sebagai kode byte, sehingga hasil kompilasinya sama di compiler mana pun.

## Catatan tampilan

- Garis kotak dan grafik batang memakai UTF-8. Di Windows program mengatur konsolnya sendiri ke UTF-8 saat mulai dan
  mengembalikannya saat selesai, jadi tidak perlu `chcp 65001`. Untuk terminal lama yang tidak mengenal UTF-8,
  jalankan dengan `--ascii`.
- Warna mengikuti kebiasaan program terminal: mati bila keluaran dialihkan ke berkas atau `NO_COLOR` disetel,
  dipaksa menyala dengan `FORCE_COLOR`, dan bisa dimatikan dengan `--no-color`.

## Berkontribusi

Lihat [CONTRIBUTING.md](CONTRIBUTING.md). Masalah keamanan dilaporkan secara privat, lihat [SECURITY.md](SECURITY.md).

## Lisensi

[MIT](LICENSE) © 2026 Fadhlillah
