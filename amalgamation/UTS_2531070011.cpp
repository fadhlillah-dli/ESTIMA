// ===============================================================================================================
// UTS_2531070011.cpp
// ESTIMA - Estimasi Material dan Biaya Produksi Komponen Mesin
// UTS Algoritma dan Pemrograman - Semester Ganjil TA. 2026/2027 - Kelas 26FA
//
// Nama  : Fadhlillah
// NIM   : 2531070011
// Dosen : Ali Muhammad, S.Kom., M.Kom.
// Versi : 1.0.0
//
// Tentang berkas ini
//   Berkas tunggal ini adalah gabungan seluruh modul program, yaitu isi folder include/ dan src/
//   pada repositori https://github.com/fadhlillah-dli/UTS_2531070011. Penggabungannya dikerjakan
//   oleh scripts/amalgamate.py supaya program bisa dikompilasi dengan satu perintah dan dikumpulkan
//   sebagai satu berkas. Bila ada yang perlu diubah, perubahannya dikerjakan di modul asalnya, lalu
//   berkas ini disusun ulang.
//
// Cara mengompilasi
//   g++ (terminal)          : g++ UTS_2531070011.cpp -o estima
//   Dev-C++                 : buka berkas ini, lalu Execute > Compile & Run (F11)
//   Code::Blocks            : buka berkas ini, lalu Build > Build and run (F9)
//   Visual Studio           : cl /EHsc UTS_2531070011.cpp
//
// Letak unsur yang diminta soal (kolom Baris adalah nomor baris di berkas ini)
//   Bagian  Baris  Unsur
//        2    206  struct Component - data satu komponen: nama, jumlah, berat, harga
//        2    234  array items[] di dalam struct ComponentList - penyimpan semua komponen
//       13    882  fungsi calculateTotalWeight() - total berat satu komponen
//       13    900  fungsi sumTotalWeight() - total berat seluruh komponen
//       13    888  fungsi calculateTotalCost() - total biaya satu komponen
//       13    911  fungsi sumTotalCost() - total biaya seluruh komponen
//       13    931  fungsi findMostExpensive() - mencari komponen dengan biaya tertinggi
//       21   3211  runMenuLoop() - perulangan menu (while) dan percabangan pilihan (switch)
//       20   2514  printRecap() - tampilan total tiap komponen, total keseluruhan, komponen termahal
//
// Daftar isi
//   Bagian  Baris  Isi
//        -     67  Persiapan: platform.h - penyesuaian compiler yang harus dibaca paling awal
//        -     96  Pustaka standar: semua #include dari seluruh modul
//                  Deklarasi: tipe data, konstanta, dan daftar fungsi (asal: include/estima/)
//        1    132  app_info.h - identitas program
//        2    167  component.h - data satu komponen mesin dan daftar penyimpannya
//        3    277  calculation.h - rumus perhitungan berat dan biaya
//        4    337  material_catalog.h - harga acuan material dan data contoh
//        5    377  text_format.h - pembentukan teks: angka, rupiah, berat, perataan, dan pelipatan baris
//        6    438  input_parse.h - pemeriksaan dan pengubahan teks ketikan menjadi data
//        7    498  terminal.h - kemampuan terminal: warna, UTF-8, lebar jendela, dan jenis masukan
//        8    538  ui.h - komponen tampilan: gaya teks, kotak, tabel, dan pesan
//        9    649  prompt.h - tanya jawab dengan pengguna
//       10    690  report.h - penyusunan laporan dari data komponen
//       11    733  app.h - alur utama program
//                  Implementasi: isi fungsi (asal: src/)
//       12    758  component.cpp - operasi dasar pada daftar komponen
//       13    850  calculation.cpp - rumus perhitungan berat dan biaya
//       14    996  material_catalog.cpp - harga acuan material dan data contoh
//       15   1108  text_format.cpp - pembentukan teks: angka, rupiah, berat, perataan, dan pelipatan baris
//       16   1379  input_parse.cpp - pemeriksaan dan pengubahan teks ketikan menjadi data
//       17   1708  terminal.cpp - kemampuan terminal: warna, UTF-8, lebar jendela, dan jenis masukan
//       18   1925  ui.cpp - komponen tampilan: gaya teks, kotak, tabel, dan pesan
//       19   2299  prompt.cpp - tanya jawab dengan pengguna
//       20   2446  report.cpp - penyusunan laporan dari data komponen
//       21   2775  app.cpp - alur utama program: menu dan aksi tiap pilihannya
//       22   3310  main.cpp - titik masuk program
// ===============================================================================================================

// ===============================================================================================================
// PERSIAPAN - include/estima/platform.h - penyesuaian compiler yang harus dibaca paling awal
// ===============================================================================================================
// Isi platform.h harus dibaca compiler paling awal, sebelum header pustaka standar. Pada versi modul,
// semua berkas .cpp di folder src/ dibuka dengan #include "estima/platform.h"; pada berkas tunggal, isinya
// ditaruh sebelum blok #include pustaka standar. Alasannya: pengaturan di bawah hanya berpengaruh bila
// sudah ada SEBELUM header pustaka standar dibaca compiler.

// Visual C++ menandai fungsi pustaka standar seperti std::getenv dan std::localtime sebagai
// "tidak aman" (peringatan C4996, yang berubah menjadi galat bila pemeriksaan SDL aktif).
// Program ini memakainya dengan cara yang aman (hasilnya langsung dibaca lalu disalin), jadi
// peringatan itu dimatikan di sini. Compiler lain (GCC, Clang) melewati blok ini.
// std::getenv dipakai di src/terminal.cpp (membaca NO_COLOR, FORCE_COLOR, TERM, COLUMNS), sedangkan
// std::localtime dipakai di src/report.cpp (tanggal dan jam di kepala laporan berkas).
//
// #if berfungsi untuk kompilasi bersyarat: baris sampai #endif pasangannya hanya ikut dikompilasi bila syaratnya benar.
// defined(NAMA) bernilai benar bila makro NAMA sudah didefinisikan, baik oleh compiler sendiri maupun oleh kode sebelumnya.
// #endif berfungsi untuk menutup blok #if (atau #ifndef/#ifdef) yang terdekat di atasnya.
// Operator logika dipakai untuk menggabungkan syarat: && artinya "dan" (benar bila kedua syarat benar),
// || artinya "atau" (benar bila salah satu syarat benar), dan ! artinya "tidak" (membalik benar menjadi salah dan sebaliknya).
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS) // _MSC_VER adalah makro yang didefinisikan oleh Visual C++ (MSVC), jadi syarat pertama benar di MSVC, dan
// !defined(_CRT_SECURE_NO_WARNINGS) artinya makro itu belum dibuat, misalnya lewat opsi /D di baris perintah;
// pemeriksaan kedua ini mencegah peringatan "makro didefinisikan ulang".
// #define berfungsi untuk membuat makro, yaitu nama yang dikenal oleh preprocessor (tahap sebelum kompilasi);
// bisa tanpa isi (cukup sebagai penanda) atau dengan isi, yang nanti menggantikan nama itu di dalam kode.
#define _CRT_SECURE_NO_WARNINGS 1 // _CRT_SECURE_NO_WARNINGS 1 adalah makro resmi Visual C++ yang mematikan peringatan C4996 untuk
// fungsi seperti std::getenv dan std::localtime; nilainya 1 sekadar penanda bahwa pengaturan ini dinyalakan.
#endif // akhir blok #if khusus Visual C++

// ===============================================================================================================
// PUSTAKA STANDAR - semua #include dari seluruh modul, dikumpulkan di satu tempat
// ===============================================================================================================
// Pustaka khusus sistem operasi tidak ikut dikumpulkan di sini: baris #include-nya tetap berada di
// dalam #ifdef pada Bagian 17.
// #include berfungsi untuk menyalin isi sebuah header ke berkas ini sebelum dikompilasi, supaya nama-nama
// yang dideklarasikan di header itu bisa dipakai. Bentuk #include <...> (kurung sudut) dipakai untuk header
// pustaka standar C++ yang ikut terpasang bersama compiler, sedangkan #include "..." (tanda kutip) dipakai
// untuk header milik proyek ini sendiri, misalnya "estima/text_format.h" yang dicari di folder include/ pada versi
// modul. Di berkas tunggal tidak ada #include "...", karena isi header proyek sudah disalin langsung sebagai bagian
// Persiapan dan Bagian 1-11.

// <cmath> adalah header pustaka standar untuk fungsi matematika; dari sini dipakai std::floor.
#include <cmath>    // <cmath> digunakan untuk std::floor (pembulatan ke bawah): dasar pembulatan berat dan biaya, pembulatan
                    // rupiah, gram, dan persen saat ditulis sebagai teks, serta panjang batang grafik di printRanking().
#include <cstddef>  // <cstddef> digunakan untuk tipe std::size_t (ukuran, lebar, dan posisi teks di hampir semua bagian) dan NULL,
                    // yaitu nilai penunjuk kosong yang dipakai bersama std::getenv, std::strtol, std::time, dan std::localtime.
#include <cstdlib>  // <cstdlib> digunakan untuk std::getenv (membaca variabel lingkungan), std::atexit (mendaftarkan fungsi penutup), dan std::strtol (teks ke long)

// <cstring> adalah header pustaka standar untuk mengolah teks gaya C (deretan char yang diakhiri karakter nol), seperti nilai const char*.
#include <cstring>  // <cstring> digunakan untuk std::strcmp, pembanding dua teks gaya C (const char*), yang dipakai findMaterialByName()
                    // serta decideColor() dan initTerminal() untuk memeriksa isi variabel lingkungan FORCE_COLOR dan TERM.
#include <ctime>    // <ctime> digunakan untuk membaca jam komputer: std::time, std::localtime, std::strftime, beserta tipe std::time_t dan std::tm
#include <fstream>  // <fstream> digunakan untuk aliran berkas: std::ofstream menulis laporan .txt di saveReport(), dan std::ifstream
                    // dipakai fileExists() untuk memeriksa apakah berkas laporan dengan nama itu sudah ada.
#include <iomanip>  // <iomanip> digunakan untuk pengatur tampilan angka: std::setprecision, std::setw, dan std::setfill.
#include <iostream> // <iostream> digunakan untuk std::cout (keluaran ke layar), std::cin (sumber ketikan yang dibaca per baris oleh
                    // readLine() dan waitForEnter()), dan std::cerr (saluran pesan galat, misalnya untuk opsi yang salah).
#include <ostream>  // <ostream> digunakan untuk tipe std::ostream, yaitu aliran keluaran umum yang menjadi parameter out fungsi gambar
                    // dan cetak, sehingga keluaran yang sama bisa ditulis ke std::cout, std::cerr, atau berkas laporan.
#include <sstream>  // <sstream> digunakan untuk std::ostringstream, yaitu "kertas" tempat menulis angka menjadi teks.
#include <string>   // <string> digunakan untuk tipe std::string, yaitu teks yang panjangnya bisa berubah (nama komponen, ketikan,
                    // pesan, dan baris tampilan), serta std::getline yang membaca satu baris utuh dari std::cin.
#include <vector>   // <vector> digunakan untuk tipe std::vector, yaitu daftar yang panjangnya bisa bertambah: hasil wrapText(),
                    // baris-baris isi kotak (StyledLine), serta kolom dan baris tabel.

// ===============================================================================================================
// BAGIAN 1 - include/estima/app_info.h - identitas program
// ===============================================================================================================
// Semua teks identitas dikumpulkan di satu tempat supaya banner, bantuan (--help), nomor versi,
// dan kepala laporan selalu memakai tulisan yang sama dan cukup diubah di sini.
// Header ini di-#include oleh src/app.cpp (banner pembuka, --help, --version) dan src/report.cpp
// (baris pertama laporan berkas .txt).

// Tujuh konstanta teks di bawah sengaja disimpan sebagai konstanta bernama, bukan diketik langsung di
// setiap tempat yang membutuhkannya: bila versi naik atau nama mata kuliah berubah, cukup satu baris
// di sini yang diganti dan semua tampilan ikut berubah.
//
// const berfungsi untuk membuat nilai yang tidak boleh diubah lagi setelah diisi; bila ada kode yang
// mencoba mengubahnya, compiler langsung menolak.
// Tanda * pada tipe menandai pointer (penunjuk), yaitu variabel yang menyimpan ALAMAT sebuah data di
// memori, bukan datanya sendiri. Teks dalam tanda kutip seperti "ESTIMA" disimpan compiler sebagai
// deretan karakter di memori, dan const char* menunjuk ke karakter pertamanya.
// const char* const dibaca dari kanan: konstanta yang berupa pointer (const yang kanan: alamatnya tidak
// bisa diganti) ke char yang const (const yang kiri: huruf-hurufnya tidak bisa diubah). Jadi teks maupun
// penunjuknya sama-sama tetap.
// Alasan teknis const yang kanan: konstanta const di tingkat berkas hanya berlaku di berkas .cpp yang
// meng-#include-nya, sehingga header ini aman dipakai oleh beberapa berkas .cpp sekaligus tanpa galat
// "didefinisikan ganda" saat penautan (linking).
const char* const APP_NAME = "ESTIMA"; // APP_NAME = "ESTIMA" adalah nama program; dipakai di judul banner, keluaran --version, bantuan, dan laporan berkas.
const char* const APP_VERSION = "1.0.0"; // APP_VERSION = "1.0.0" adalah nomor versi program, satu-satunya sumber nomor versi;
// opsi -v/--version mencetaknya sebagai "ESTIMA 1.0.0".
const char* const APP_TAGLINE = "Estimasi Material dan Biaya Produksi Komponen Mesin"; // APP_TAGLINE adalah keterangan singkat program;
// tampil sebagai baris pertama di dalam kotak banner, di baris pertama bantuan --help (sesudah nama dan versi), dan di kepala laporan berkas.
const char* const APP_COURSE = "UTS Algoritma dan Pemrograman - Kelas 26FA"; // APP_COURSE adalah nama ujian dan kelas; ditampilkan di banner pembuka.
const char* const APP_AUTHOR = "Fadhlillah (2531070011)"; // APP_AUTHOR adalah nama pembuat beserta NIM; ditampilkan di banner pembuka.
const char* const APP_INSTITUTION = "Teknik Informatika - Universitas Sains Indonesia"; // APP_INSTITUTION adalah program studi dan kampus;
// ditampilkan di banner pembuka.
const char* const APP_REPOSITORY = "https://github.com/fadhlillah-dli/UTS_2531070011"; // APP_REPOSITORY adalah alamat repositori kode sumber;
// dicetak di baris "Dokumentasi:" pada bantuan --help.

// ===============================================================================================================
// BAGIAN 2 - include/estima/component.h - data satu komponen mesin dan daftar penyimpannya
// ===============================================================================================================
// Header ini adalah dasar seluruh program: bentuk data satu komponen (struct Component), wadah
// penyimpannya (struct ComponentList berisi array), batas-batas data, dan empat operasi dasar daftar.
// Hampir semua bagian lain (perhitungan, katalog, laporan, menu) meng-#include header ini.

// Batas-batas data. Ditulis sebagai konstanta bernama supaya angka yang sama dipakai oleh
// validasi input, pesan kesalahan, dan pengujian, dan cukup diubah di satu tempat.
// Pemakainya: src/input_parse.cpp (memeriksa ketikan), src/app.cpp (petunjuk pengisian, batas daftar penuh,
// dan batas biaya), serta src/report.cpp (ukuran array bantu).
// int adalah tipe data bilangan bulat (tanpa pecahan), misalnya 0, 24, atau -1; di semua platform sasaran
// program ini int memakai 32 bit, jadi bisa menyimpan nilai sampai sekitar 2,1 miliar.
const int MAX_COMPONENTS = 100; // MAX_COMPONENTS = 100 adalah banyaknya komponen paling banyak dalam satu daftar; sekaligus menjadi
// ukuran array items di ComponentList. Karena dipakai sebagai ukuran array, nilainya harus konstanta yang sudah pasti saat kompilasi.
const int MAX_NAME_LENGTH = 30; // MAX_NAME_LENGTH = 30 adalah panjang nama komponen paling banyak (dalam karakter); nama yang lebih
// panjang ditolak oleh checkComponentName() di src/input_parse.cpp.
const int MIN_QUANTITY = 1; // MIN_QUANTITY = 1 adalah jumlah unit paling sedikit; artinya jumlah 0 (atau kurang) ditolak saat diketik.
const int MAX_QUANTITY = 1000000; // MAX_QUANTITY = 1000000 adalah jumlah unit paling banyak (satu juta); masih jauh di bawah batas int.
// double adalah tipe data bilangan pecahan (desimal), misalnya 1.85; dipakai untuk berat karena berat bisa
// berupa pecahan sampai ketelitian gram (tiga angka di belakang koma).
const double MIN_UNIT_WEIGHT_KG = 0.001; // MIN_UNIT_WEIGHT_KG = 0.001 artinya berat per unit paling ringan 0,001 kg, yaitu 1 gram.
const double MAX_UNIT_WEIGHT_KG = 10000.0; // MAX_UNIT_WEIGHT_KG = 10000.0 artinya berat per unit paling berat 10.000 kg (10 ton).
const double MIN_PRICE_PER_KG = 1.0; // MIN_PRICE_PER_KG = 1.0 artinya harga material paling murah Rp1 per kg.
const double MAX_PRICE_PER_KG = 100000000.0; // MAX_PRICE_PER_KG = 100000000.0 artinya harga material paling mahal Rp100.000.000 per kg.
// Harga disimpan sebagai double, sama seperti berat, supaya perkalian harga x berat dan biaya hasilnya langsung
// dihitung dalam double. Biaya itu bisa mencapai ratusan miliar rupiah, jauh melewati batas int (sekitar 2,1 miliar).
// Harga yang diterima program sendiri selalu bilangan bulat rupiah.
// Biaya satu komponen dibatasi di bawah Rp1 triliun. Dengan batas ini seluruh hitungan (termasuk
// total 100 komponen) masih berupa bilangan bulat yang bisa disimpan double tanpa galat pembulatan.
const double MAX_COMPONENT_COST = 999999999999.0; // MAX_COMPONENT_COST = 999999999999.0 adalah biaya satu komponen paling besar
// (Rp999.999.999.999); diperiksa oleh costWithinLimit() di src/app.cpp sebelum komponen disimpan.

// [Kriteria soal: struktur data penyimpan komponen]
// Component adalah struct (record): empat data milik satu komponen dibungkus menjadi satu
// variabel. Dipilih struct, bukan empat array terpisah (nama[], jumlah[], berat[], harga[]),
// supaya data satu komponen tidak bisa tertukar barisnya dan mudah dikirim ke fungsi sebagai
// satu kesatuan.
// struct berfungsi untuk membuat tipe data baru yang berisi beberapa data (disebut anggota) sekaligus.
// Anggotanya diambil dengan tanda titik, misalnya draft.name atau list.count, artinya "anggota name milik draft".
struct Component { // Component adalah nama tipe struct untuk satu komponen mesin; isinya empat anggota di bawah.
    // std:: adalah awalan untuk nama yang berasal dari pustaka standar C++; tanda :: artinya "milik",
    // jadi std::string adalah string milik pustaka standar.
    // std::string adalah tipe teks yang panjangnya bisa berubah; dipilih untuk nama karena panjang nama
    // berbeda-beda dan std::string mengurus sendiri tempat penyimpanannya di memori.
    std::string name; // name adalah nama komponen, misalnya "Gear"; dipakai juga untuk mencari komponen lewat findComponentByName().
    int quantity; // quantity adalah jumlah unit komponen; memakai int karena jumlah selalu bilangan bulat (1 sampai 1.000.000).
    double unitWeightKg; // unitWeightKg adalah berat satu unit dalam kilogram; memakai double karena berat bisa pecahan, misalnya 1,85 kg.
    double pricePerKg; // pricePerKg adalah harga material per kilogram dalam rupiah; memakai double sama seperti berat, supaya hasil kali
    // keduanya dihitung dalam double (lihat penjelasan di atas MAX_COMPONENT_COST).

    // Konstruktor adalah fungsi khusus bernama sama dengan struct-nya dan tanpa tipe hasil. Ia berjalan otomatis
    // setiap kali variabel struct itu dibuat, sehingga cocok untuk memberi nilai awal semua anggota.
    // Di sini konstruktor hanya dideklarasikan; isinya ditulis di src/component.cpp.
    Component(); // Component() adalah konstruktor tanpa parameter; membuat nama kosong dan semua angka 0.
}; // akhir struct Component

// [Kriteria soal: array penyimpan komponen]
// ComponentList adalah daftar komponen berbasis ARRAY berukuran tetap ditambah pencacah isi.
// Dipilih array karena jumlah komponen dalam satu estimasi kecil dan batasnya jelas (100),
// setiap elemen bisa diambil langsung lewat nomor urutnya, dan tidak perlu memesan memori saat
// program berjalan. count mencatat berapa elemen yang sudah terisi; elemen sesudahnya kosong.
// Array dan pencacahnya dibungkus dalam satu struct supaya keduanya selalu berjalan bersama: fungsi yang
// menerima satu ComponentList otomatis tahu isi array sekaligus berapa yang terisi.
struct ComponentList { // ComponentList adalah nama tipe struct untuk daftar komponen; daftar milik program dibuat di runMenuLoop() (src/app.cpp).
    // Array adalah sederet variabel bertipe sama yang disimpan berdampingan dan diberi satu nama. Ukurannya
    // ditulis dalam kurung siku saat deklarasi, misalnya items[100]. Elemennya diambil dengan nomor urut
    // (indeks) dalam kurung siku yang dimulai dari 0: items[0] elemen pertama, items[99] elemen terakhir.
    Component items[MAX_COMPONENTS]; // items[MAX_COMPONENTS] adalah array berisi 100 Component; komponen ke-1 disimpan di items[0],
    // komponen ke-2 di items[1], dan seterusnya.
    int count; // count adalah banyaknya elemen items yang sudah terisi (0 sampai 100); elemen terisi selalu berada di items[0] sampai items[count - 1].

    ComponentList(); // ComponentList() adalah konstruktor yang membuat daftar kosong (count = 0); isinya ada di src/component.cpp.
}; // akhir struct ComponentList

// Empat fungsi di bawah adalah operasi dasar pada daftar: menambah, menghapus, mengosongkan, dan mencari.
// Menambah dan menghapus dikerjakan hanya oleh fungsi-fungsi ini, sehingga aturan "elemen terisi selalu rapat
// di depan" cukup dijaga di satu tempat. (Mengganti isi satu elemen yang sudah ada, seperti pada menu Ubah,
// dilakukan langsung lewat items[index].)
// Di header ini fungsi hanya dideklarasikan (prototipe): nama, tipe hasil, dan parameternya saja, diakhiri
// titik koma. Prototipe memberi tahu compiler bahwa fungsi itu ada, supaya berkas .cpp lain yang meng-#include
// header ini boleh memanggilnya. Definisinya (isi fungsi) ditulis di src/component.cpp.
//
// Menambah komponen di ujung daftar. Hasil false berarti daftar sudah penuh.
// bool adalah tipe data yang hanya punya dua nilai: true (benar) atau false (salah); cocok untuk hasil
// yang menjawab "berhasil atau tidak".
// Tanda & pada tipe parameter menandai referensi: parameter itu menjadi nama lain untuk variabel milik
// pemanggil, bukan salinannya. Jadi perubahan pada list di dalam fungsi langsung mengubah daftar pemanggil.
// const ... & artinya data dikirim tanpa disalin (hemat waktu dan memori, karena struct bisa besar) tetapi
// fungsi tidak boleh mengubahnya; compiler menolak bila fungsi mencoba mengubahnya.
bool addComponent(ComponentList& list, const Component& component); // addComponent() berfungsi untuk menyalin component ke ujung list;
// list memakai & karena isinya bertambah, component memakai const & karena hanya dibaca. Dipanggil oleh menu Tambah (src/app.cpp)
// dan oleh loadSampleComponents() (src/material_catalog.cpp).

// Menghapus komponen pada posisi index (mulai 0) lalu merapatkan elemen sesudahnya.
// Hasil false berarti index di luar isi daftar.
bool removeComponent(ComponentList& list, int index); // removeComponent() berfungsi untuk membuang komponen nomor index dari list;
// dipanggil oleh menu Hapus komponen di src/app.cpp.

// Mengosongkan daftar.
// void adalah penanda bahwa fungsi tidak mengembalikan nilai apa pun; fungsi seperti ini dipanggil untuk efeknya saja.
void clearComponents(ComponentList& list); // clearComponents() berfungsi untuk mengosongkan list; dipakai loadSampleComponents()
// sebelum memuat data contoh supaya daftar lama terganti, bukan tertumpuk.

// Mencari komponen berdasarkan nama tanpa membedakan huruf besar/kecil.
// Hasilnya posisi komponen (mulai 0), atau -1 bila tidak ada.
int findComponentByName(const ComponentList& list, const std::string& name); // findComponentByName() berfungsi untuk mencari posisi
// komponen bernama name; -1 dipilih sebagai tanda "tidak ada" karena tidak mungkin menjadi indeks array. Dipakai saat mengisi nama
// di src/app.cpp supaya dua komponen tidak bernama sama ("gear" dan "Gear" dianggap sama).

// ===============================================================================================================
// BAGIAN 3 - include/estima/calculation.h - rumus perhitungan berat dan biaya
// ===============================================================================================================
// Semua fungsi di sini murni menghitung: tidak membaca keyboard dan tidak mencetak ke layar.
// Karena itu fungsi-fungsi ini bisa dipakai ulang oleh tampilan layar (menu rekap dan peringkat) maupun
// laporan berkas, dan bisa diuji otomatis dengan angka yang sudah diketahui jawabannya.
// Header ini hanya berisi deklarasi (prototipe); isi setiap fungsi ditulis di src/calculation.cpp.

// [Kriteria soal: fungsi menghitung total berat]
// Total berat satu komponen (kg) = jumlah x berat per unit.
// Hasilnya double karena berat bisa berpecahan (misalnya 44,400 kg). Komponen dikirim sebagai const
// Component& supaya struct-nya tidak disalin dan dijamin tidak diubah oleh fungsi ini.
double calculateTotalWeight(const Component& component); // calculateTotalWeight(component) berfungsi untuk menghitung total berat satu komponen dalam kg;
// dipakai di tabel rekap dan ringkasan komponen (src/report.cpp).

// [Kriteria soal: fungsi menghitung total biaya]
// Total biaya satu komponen (rupiah) = total berat x harga per kg, dibulatkan ke rupiah terdekat.
// Tipe hasilnya double, bukan int: biaya satu komponen bisa mencapai hampir Rp1 triliun (batasnya MAX_COMPONENT_COST),
// jauh di atas batas int (sekitar 2,1 miliar), sedangkan double masih menyimpan bilangan bulat sebesar itu dengan tepat.
double calculateTotalCost(const Component& component); // calculateTotalCost(component) berfungsi untuk menghitung total biaya satu komponen dalam rupiah bulat;
// dipakai di rekap, peringkat, ringkasan komponen, dan pemeriksaan batas MAX_COMPONENT_COST di src/app.cpp.

// Total berat seluruh komponen (kg). Parameter items[] adalah array komponen dan count jumlah
// elemen yang terisi.
// Parameter array (items[]) bersama jumlah elemennya (count) adalah cara C++ mengirim sebuah array ke fungsi:
// yang sebenarnya dikirim hanya alamat elemen pertamanya, sehingga fungsi tidak tahu panjang array
// kecuali diberi tahu lewat parameter tambahan seperti count. Pemanggil menulis sumTotalWeight(list.items, list.count).
double sumTotalWeight(const Component items[], int count); // sumTotalWeight(items, count) berfungsi untuk menjumlahkan total berat count komponen pertama di array items;
// const di depan Component artinya elemen array hanya dibaca, tidak diubah.

// Total biaya seluruh komponen (rupiah).
double sumTotalCost(const Component items[], int count); // sumTotalCost(items, count) berfungsi untuk menjumlahkan total biaya semua komponen;
// hasilnya dipakai sebagai total keseluruhan biaya di rekap dan sebagai dasar porsi persen.

// Jumlah unit seluruh komponen.
// long adalah tipe bilangan bulat yang jangkauannya tidak pernah lebih kecil dari int; standar C++ menjamin long
// sanggup menyimpan paling sedikit sampai sekitar 2,1 miliar (di Linux dan macOS 64-bit bahkan jauh lebih besar).
// Jumlah unit terbesar yang mungkin adalah 100 komponen x 1.000.000 unit = 100.000.000 unit, jadi selalu muat di long.
long sumQuantity(const Component items[], int count); // sumQuantity(items, count) berfungsi untuk menjumlahkan jumlah unit semua komponen;
// hasilnya ditampilkan sebagai "Jumlah unit" dan baris TOTAL di rekap.

// [Kriteria soal: fungsi menemukan komponen termahal]
// Menemukan komponen dengan biaya tertinggi. Posisi komponennya ditulis ke resultIndexes[]
// (harus muat count elemen) dan banyaknya dikembalikan. Hasilnya bisa lebih dari satu bila ada
// beberapa komponen dengan biaya tertinggi yang sama; hasil 0 berarti daftar kosong.
// Hasil berupa array posisi (bukan satu Component) dipilih supaya komponen yang biayanya kembar tidak ada yang terlewat,
// dan posisi itu bisa dipakai untuk menyorot barisnya di tabel rekap.
int findMostExpensive(const Component items[], int count, int resultIndexes[]); // findMostExpensive(...) berfungsi untuk mencari komponen termahal;
// resultIndexes[] tanpa const karena array ini justru diisi oleh fungsi, dan nilai kembaliannya (int) adalah banyaknya posisi yang ditulis.

// Menyusun peringkat biaya dari yang terbesar. order[] (harus muat count elemen) diisi posisi
// komponen sesuai urutan peringkat; data aslinya tidak dipindah.
// Komponen yang biayanya sama tetap berurutan sesuai urutan pengisiannya.
void rankByCost(const Component items[], int count, int order[]); // rankByCost(items, count, order) berfungsi untuk mengisi order[] dengan posisi komponen;
// urutannya dari biaya terbesar. void karena hasilnya dikirim lewat array order[], bukan lewat return. Dipakai oleh menu Peringkat biaya (printRanking).

// Porsi sebuah biaya terhadap total, dalam persen (0 bila total 0).
double costSharePercent(double cost, double totalCost); // costSharePercent(cost, totalCost) berfungsi untuk menghitung cost / totalCost x 100;
// dipakai untuk kolom "Porsi" di peringkat dan keterangan "(..% dari total biaya)" di rekap.

// ===============================================================================================================
// BAGIAN 4 - include/estima/material_catalog.h - harga acuan material dan data contoh
// ===============================================================================================================
// Header ini memperkenalkan katalog harga acuan material (dipakai menu Tambah komponen dan menu 7 Harga acuan material)
// serta fungsi pemuat data contoh (menu 6). Isinya ditulis di src/material_catalog.cpp.

// Satu baris daftar harga acuan. Anggotanya const char* (teks tetap) karena isi katalog sudah
// diketahui saat program ditulis dan tidak pernah diubah saat program berjalan.
// Dibuat struct supaya nama, standar, kegunaan, dan harga satu material selalu berada dalam satu baris yang sama.
struct MaterialReference { // MaterialReference adalah struct (record) untuk satu material di tabel harga acuan.
    const char* name; // name adalah nama material yang tampil di daftar pilihan harga, misalnya "Baja karbon S45C"; juga dipakai untuk mencarinya.
    const char* standard; // standard adalah kode standar bahan (misalnya JIS G4051) yang tampil di kolom "Standar" menu harga acuan.
    const char* typicalUse; // typicalUse adalah contoh kegunaan umum bahan itu, tampil di kolom "Kegunaan umum".
    double pricePerKg; // pricePerKg adalah harga acuan per kg dalam rupiah; double supaya sama dengan tipe pricePerKg di struct Component.
}; // akhir struct MaterialReference

// Katalog disimpan sebagai array konstan: isinya tetap, urutannya menjadi nomor pilihan di
// layar, dan pencarian cukup dengan perulangan karena barisnya hanya belasan.
// extern berfungsi untuk memberi tahu compiler bahwa sebuah variabel ADA, tetapi isinya (definisinya) ditulis di tempat lain,
// yaitu di src/material_catalog.cpp. Dengan begitu katalog cukup ditulis satu kali, dan semua bagian program yang memakainya
// (misalnya app.cpp dan report.cpp) memakai katalog yang sama.
extern const MaterialReference MATERIAL_CATALOG[]; // MATERIAL_CATALOG[] adalah array katalog; tanda [] kosong karena ukurannya baru diketahui
// di material_catalog.cpp, tempat barisnya ditulis.
extern const int MATERIAL_COUNT; // MATERIAL_COUNT adalah banyaknya baris katalog; dipakai sebagai batas perulangan dan batas nomor pilihan harga.

// Periode data harga dan keterangan sumbernya, ditampilkan di bawah tabel harga acuan.
extern const char* const CATALOG_PERIOD; // CATALOG_PERIOD adalah teks periode data harga (bulan dan tahun), juga muncul di catatan tabel pilihan harga.
extern const char* const CATALOG_SOURCE_NOTE; // CATALOG_SOURCE_NOTE adalah keterangan singkat asal angka harga, dicetak di bawah tabel menu 7.

// Mencari material berdasarkan nama persisnya. Hasilnya posisi di katalog, atau -1 bila tidak ada.
// "Persis" artinya huruf besar/kecil dan spasi harus sama. Parameternya const char* karena nama yang dicari
// berasal dari tabel data contoh, yang juga berupa teks tetap.
int findMaterialByName(const char* name); // findMaterialByName(name) berfungsi untuk mencari posisi material bernama name; -1 dipakai sebagai tanda
// "tidak ada" karena posisi yang sah selalu 0 atau lebih.

// Mengganti isi daftar dengan data contoh (gear, shaft, bracket, dan lain-lain) yang harganya
// diambil dari katalog. Hasilnya jumlah komponen yang dimuat.
int loadSampleComponents(ComponentList& list); // loadSampleComponents(list) berfungsi untuk mengosongkan list lalu mengisinya dengan data contoh;
// list dikirim sebagai referensi (&) tanpa const karena isinya memang diubah.

// ===============================================================================================================
// BAGIAN 5 - include/estima/text_format.h - pembentukan teks: angka, rupiah, berat, perataan, dan pelipatan baris
// ===============================================================================================================
// Header ini berisi daftar fungsi "tukang tulis" program: fungsi yang mengubah angka menjadi teks yang enak dibaca orang
// Indonesia (titik ribuan, koma desimal, awalan Rp) dan fungsi yang mengatur lebar teks supaya tabel dan kotak tetap lurus.
// Semuanya hanya menyusun teks lalu mengembalikannya; tidak ada yang mencetak ke layar. Jadi hasilnya bisa dipakai untuk layar
// maupun untuk berkas laporan, dan bisa diuji otomatis dengan membandingkan teks hasilnya.

// Kelompok pertama: pengubah angka menjadi teks. Dibuat fungsi tersendiri (bukan langsung mencetak angka dengan <<) karena
// aturan tulis Indonesia berbeda dari bawaan C++: ribuan dipisah titik dan desimal memakai koma. Dengan fungsi bersama,
// rekap, peringkat, ringkasan, pesan kesalahan, dan laporan berkas pasti menulis angka dengan cara yang sama.
// Semua parameternya double karena nilai yang ditulis (biaya hingga Rp999.999.999.999, berat berdesimal, persen) bisa
// melebihi daya tampung int atau memang berpecahan.
// Menulis bilangan bulat tidak negatif dengan titik pemisah ribuan: 1250000 menjadi "1.250.000".
std::string formatThousands(double wholeNumber); // formatThousands(double wholeNumber) berfungsi untuk menulis bilangan bulat dengan titik ribuan;
// dipakai untuk nomor urut, jumlah unit, dan banyaknya komponen, dan juga menjadi bagian dalam fungsi-fungsi format di bawahnya.

// Menulis nilai uang: 821400 menjadi "Rp821.400" (tanpa spasi dan tanpa desimal).
std::string formatRupiah(double amount); // formatRupiah(double amount) berfungsi untuk menulis nilai uang dengan awalan Rp; amount dibulatkan dulu ke rupiah terdekat.

// Menulis berat dengan tiga angka desimal (ketelitian gram): 44.4 menjadi "44,400 kg".
std::string formatWeight(double weightKg); // formatWeight(double weightKg) berfungsi untuk menulis berat dalam kg, selalu dengan tiga angka di belakang koma.

// Menulis persen dengan satu angka desimal: 56.512 menjadi "56,5%".
std::string formatPercent(double percent); // formatPercent(double percent) berfungsi untuk menulis porsi biaya dalam persen, dibulatkan ke satu desimal.

// Kelompok kedua: pengatur lebar teks untuk tabel dan kotak.
// Lebar teks di layar dalam kolom. Berbeda dari jumlah byte karena satu karakter garis kotak
// UTF-8 memakai tiga byte tetapi hanya satu kolom.
// std::size_t adalah tipe bilangan bulat tanpa tanda negatif yang dipakai pustaka standar untuk ukuran dan panjang
// (misalnya hasil text.size()). Dipilih di sini karena lebar dan banyaknya kolom tidak pernah negatif, dan memakai tipe
// yang sama dengan size() menghindari peringatan compiler saat keduanya dibandingkan.
std::size_t displayWidth(const std::string& text); // displayWidth(const std::string& text) berfungsi untuk menghitung lebar text di layar (dalam kolom);
// dipakai di mana-mana untuk mengukur lebar kolom tabel, lebar kotak, dan panjang nama menu.

// Menambah spasi di kanan (rata kiri) atau di kiri (rata kanan) sampai selebar width kolom.
// Dua fungsi ini dipakai untuk meratakan isi sel tabel: teks rata kiri, angka rata kanan, supaya digitnya lurus.
std::string padRight(const std::string& text, std::size_t width); // padRight(text, width) berfungsi untuk membuat teks rata kiri dengan menambah spasi di kanannya.
std::string padLeft(const std::string& text, std::size_t width); // padLeft(text, width) berfungsi untuk membuat teks rata kanan dengan menambah spasi di kirinya.

// Mengulang sepotong teks sebanyak times kali, misalnya untuk menggambar garis.
std::string repeatText(const std::string& piece, std::size_t times); // repeatText(piece, times) berfungsi untuk mengulang piece sebanyak times kali;
// piece berupa teks (bukan satu char) karena garis kotak UTF-8 dan blok batang grafik terdiri dari tiga byte.

// Memotong teks yang lebih lebar dari maxWidth dan menandainya dengan ".." di ujung.
std::string truncateText(const std::string& text, std::size_t maxWidth); // truncateText(text, maxWidth) berfungsi untuk memendekkan text supaya muat maxWidth kolom;
// dipakai untuk judul kotak yang terlalu panjang.

// Mengubah huruf A-Z menjadi a-z; karakter lain dibiarkan.
std::string toLowerAscii(const std::string& text); // toLowerAscii(text) berfungsi untuk menghasilkan salinan text dalam huruf kecil;
// dipakai untuk membandingkan tanpa membedakan huruf besar/kecil (nama komponen, jawaban y/t, nama berkas).

// Melipat teks menjadi beberapa baris yang masing-masing paling lebar maxWidth kolom. Baris
// lanjutan menjorok hangingIndent spasi. Hasilnya std::vector karena jumlah barisnya baru
// diketahui setelah teks dilipat.
// std::vector<std::string> adalah daftar berurutan berisi std::string. Bedanya dengan array: panjangnya tidak perlu
// ditentukan di awal, karena setiap pemanggilan push_back(nilai) menambah satu elemen di ujung daftar dan vector mengatur
// memorinya sendiri. Elemennya tetap diambil dengan [nomor] seperti array, dan size() memberi banyaknya elemen.
std::vector<std::string> wrapText(const std::string& text, std::size_t maxWidth, std::size_t hangingIndent); // wrapText(...) berfungsi untuk melipat text;
// dipakai oleh drawBox(), drawTableBox(), dan printWrapped() di ui.cpp supaya tidak ada baris yang melewati lebar terminal.

// ===============================================================================================================
// BAGIAN 6 - include/estima/input_parse.h - pemeriksaan dan pengubahan teks ketikan menjadi data
// ===============================================================================================================
// Fungsi di sini hanya menerima teks dan mengembalikan hasil pemeriksaan; tidak ada satu pun yang
// membaca keyboard. Pembacaan keyboard dikerjakan readLine() di prompt.cpp, lalu teks hasil bacaannya
// diserahkan (lewat prompt.cpp dan app.cpp) ke fungsi-fungsi di sini. Pemisahan ini membuat setiap aturan validasi bisa diuji
// otomatis satu per satu (lihat tests/unit/test_input_parse.cpp) tanpa perlu mengetik apa pun.

// trimSpaces() dipakai readLine() di prompt.cpp untuk merapikan setiap baris yang diketik pengguna,
// sehingga " 24 " dan "24" dianggap jawaban yang sama.
std::string trimSpaces(const std::string& text); // trimSpaces(text) berfungsi untuk membuang spasi, tab, dan tanda akhir baris (\r, \n) di awal dan akhir teks;
// hasilnya teks baru, sedangkan text aslinya tidak diubah (const &).

// collapseSpaces() dipakai khusus untuk nama komponen di app.cpp, supaya "Gear   Besar" dan
// "Gear Besar" dianggap nama yang sama ketika diperiksa keunikannya.
std::string collapseSpaces(const std::string& text); // collapseSpaces(text) berfungsi seperti trimSpaces, ditambah merapatkan deret spasi/tab di tengah teks menjadi satu spasi

// isAllDigits() menjadi pemeriksaan dasar yang dipakai ulang oleh pemeriksa jumlah, berat, harga,
// dan pilihan menu, sehingga aturan "hanya angka" cukup ditulis di satu tempat.
bool isAllDigits(const std::string& text); // isAllDigits(text) menghasilkan true bila teks tidak kosong dan seluruh karakternya angka 0-9; teks kosong menghasilkan false

// Empat pemeriksa data komponen di bawah memakai pola yang sama: hasilnya teks kosong bila ketikan
// sah, atau pesan kesalahan yang menjelaskan bentuk yang benar bila tidak sah. Pesan itu langsung
// ditampilkan oleh pemanggil (prompt.cpp dan app.cpp), jadi pengguna tahu apa yang harus diperbaiki.
// Hasil berupa teks dipilih, bukan bool, karena satu nilai sekaligus membawa dua hal: sah atau
// tidak (kosong atau tidak) dan alasannya. Angka hasil ubahan dikirim lewat parameter referensi
// (int& / double&) yang hanya diisi bila ketikan sah; checkComponentName tidak punya parameter
// keluaran karena nama yang sah dipakai apa adanya.
std::string checkComponentName(const std::string& name); // checkComponentName(name) berfungsi untuk memeriksa nama komponen: tidak kosong, paling panjang 30 karakter
// (MAX_NAME_LENGTH), diawali huruf atau angka, dan hanya berisi huruf, angka, spasi, serta tanda - _ . , / ( ) + # &
std::string checkQuantity(const std::string& text, int& quantity); // checkQuantity(text, quantity) berfungsi untuk memeriksa jumlah unit (bilangan bulat 1 sampai 1.000.000);
// bila sah, nilainya ditulis ke quantity. int dipakai karena jumlah unit selalu bulat.
std::string checkUnitWeight(const std::string& text, double& weightKg); // checkUnitWeight(text, weightKg) berfungsi untuk memeriksa berat per unit (0,001 sampai 10.000 kg,
// paling banyak 3 angka desimal, boleh koma atau titik); double dipakai karena berat bisa berpecahan.
std::string checkPricePerKg(const std::string& text, double& pricePerKg); // checkPricePerKg(text, pricePerKg) berfungsi untuk memeriksa harga per kg (rupiah bulat, Rp1 sampai
// Rp100.000.000); hasilnya disimpan sebagai double, sama dengan tipe pricePerKg di struct Component.

// Nama berkas laporan diperiksa terpisah dari data komponen karena aturannya lain: tidak boleh ada
// spasi atau nama folder, dan tidak boleh memakai nama perangkat yang dicadangkan Windows.
std::string checkReportFileName(const std::string& text, std::string& fileName); // checkReportFileName(text, fileName) berfungsi untuk memeriksa nama berkas laporan;
// bila sah, fileName diisi nama yang sudah berakhiran .txt (akhiran ditambahkan bila belum ada).

// parseMenuChoice() menghasilkan bool, bukan pesan, karena untuk pilihan menu cukup diketahui sah
// atau tidak; pesan kesalahannya ("Pilihan tidak dikenal...") disusun oleh readChoice() di prompt.cpp.
bool parseMenuChoice(const std::string& text, int minValue, int maxValue, int& choice); // parseMenuChoice(...) berfungsi untuk membaca nomor pilihan menu dalam rentang
// minValue sampai maxValue; hasil true berarti sah dan nomornya sudah ditulis ke choice.

// Jawaban pertanyaan ya/tidak punya TIGA kemungkinan: ya, tidak, atau tidak dikenal (misalnya
// pengguna mengetik "mungkin"). bool hanya punya dua nilai, jadi dipakai enum supaya keadaan
// "tidak dikenal" bisa dibedakan dan pertanyaannya diulang oleh askYesNo() di prompt.cpp.
// enum berfungsi untuk membuat tipe data baru yang isinya daftar nama tetap; setiap nama mewakili
// sebuah bilangan bulat. Kode jadi lebih mudah dibaca: ANSWER_YES jauh lebih jelas daripada angka 1.
enum YesNoAnswer { // enum YesNoAnswer adalah tipe hasil parseYesNo(); nilainya selalu salah satu dari tiga nama di bawah
    ANSWER_UNKNOWN = 0, // ANSWER_UNKNOWN = 0 artinya ketikan bukan jawaban ya atau tidak yang dikenal, jadi pertanyaan perlu diulang
    ANSWER_YES = 1, // ANSWER_YES = 1 artinya pengguna menjawab ya (y, ya, atau yes)
    ANSWER_NO = 2 // ANSWER_NO = 2 artinya pengguna menjawab tidak (t, tidak, n, atau no); nama terakhir sengaja tanpa koma karena
    // koma sesudah nama terakhir enum belum dibolehkan di C++98 dan memicu peringatan -pedantic.
}; // akhir enum YesNoAnswer (titik koma setelah kurung kurawal penutup enum wajib ditulis)
YesNoAnswer parseYesNo(const std::string& text); // parseYesNo(text) berfungsi untuk mengubah ketikan menjadi salah satu nilai YesNoAnswer; huruf besar dan kecil tidak dibedakan

// ===============================================================================================================
// BAGIAN 7 - include/estima/terminal.h - kemampuan terminal: warna, UTF-8, lebar jendela, dan jenis masukan
// ===============================================================================================================
// Modul terminal (terminal.h dan src/terminal.cpp) adalah satu-satunya bagian program yang berbicara langsung
// dengan sistem operasi.
// Windows dan macOS/Linux/Android punya cara berbeda untuk hal yang sama (menyalakan warna,
// memakai UTF-8, menanyakan lebar jendela), jadi perbedaan itu dikurung di modul ini; bagian lain
// program cukup memanggil lima fungsi di bawah tanpa perlu tahu sedang berjalan di sistem apa.
// Isi fungsinya ada di src/terminal.cpp.

// Batas lebar tata letak, dipakai oleh terminalWidth() untuk menjepit lebar jendela yang terbaca.
// Di bawah 40 kolom terminal sendiri yang melipat baris; di atas 120 kolom teks tidak dilebarkan
// lagi supaya tetap enak dibaca. Ditulis sebagai konstanta bernama (bukan angka langsung di dalam
// fungsi) supaya batasnya terlihat jelas dan cukup diubah di satu tempat.
const int MIN_TERMINAL_WIDTH = 40; // MIN_TERMINAL_WIDTH = 40 adalah lebar tata letak paling kecil (kolom); jendela yang lebih sempit tetap digambar selebar 40
const int MAX_TERMINAL_WIDTH = 120; // MAX_TERMINAL_WIDTH = 120 adalah lebar tata letak paling besar; jendela yang lebih lebar tetap digambar selebar 120
const int DEFAULT_TERMINAL_WIDTH = 80; // DEFAULT_TERMINAL_WIDTH = 80 adalah lebar cadangan bila lebar jendela tidak bisa diketahui sama sekali;
// 80 kolom dipilih karena itu ukuran jendela terminal yang paling umum.

// Menyiapkan terminal satu kali di awal program (dipanggil runApp() di src/app.cpp setelah opsi baris
// perintah dibaca). disableColor mematikan warna (opsi --no-color); forceAscii memakai garis ASCII
// biasa (opsi --ascii). Hasil pemeriksaannya diingat, jadi fungsi-fungsi di bawah tinggal membacanya.
void initTerminal(bool disableColor, bool forceAscii); // initTerminal(bool disableColor, bool forceAscii) berfungsi untuk memeriksa terminal dan menyimpan hasilnya;
// void karena fungsi ini tidak mengembalikan nilai, ia hanya menyiapkan keadaan.

// true bila keluaran boleh memakai kode warna ANSI. Dipakai screenStyle() di src/ui.cpp.
bool colorsEnabled(); // colorsEnabled() mengembalikan bool: true artinya teks boleh diberi warna, false artinya teks ditulis polos

// true bila garis kotak digambar dengan karakter UTF-8; false berarti memakai + - |.
// Dipakai screenStyle() di src/ui.cpp untuk memilih jenis garis kotak.
bool unicodeEnabled(); // unicodeEnabled() mengembalikan true bila garis kotak memakai karakter UTF-8, false bila memakai karakter ASCII biasa

// true bila masukan berasal dari keyboard; false bila dialihkan dari berkas atau pipa.
// Dipakai src/prompt.cpp untuk memutuskan apakah jawaban perlu dicetak ulang ke layar.
bool inputIsInteractive(); // inputIsInteractive() mengembalikan true bila pengguna mengetik langsung di terminal, false bila masukan datang dari berkas/pipa

// Lebar jendela terminal saat ini dalam kolom, selalu di antara batas minimal dan maksimal di atas.
// Dipakai screenStyle() di src/ui.cpp dan pesan "Input berakhir..." di runApp() supaya tampilan selebar jendela.
int terminalWidth(); // terminalWidth() mengembalikan int, yaitu banyaknya kolom (40 sampai 120) yang dipakai untuk menggambar tampilan

// ===============================================================================================================
// BAGIAN 8 - include/estima/ui.h - komponen tampilan: gaya teks, kotak, tabel, dan pesan
// ===============================================================================================================
// Header ini berisi "alat gambar" yang dipakai seluruh tampilan program: gaya teks (warna), kotak berjudul, tabel,
// dan pesan singkat. Di sini hanya ada deklarasi; isi fungsinya ditulis di src/ui.cpp.
// Pemakainya: src/report.cpp (rekap, peringkat, harga acuan, laporan berkas), src/app.cpp (banner, menu, pesan),
// dan src/prompt.cpp (label pertanyaan dan pesan kesalahan). Dengan alat yang sama, semua layar tampil seragam.

// Gaya teks yang dikenal tampilan. Nilainya berurutan mulai 0 karena dipakai sebagai nomor
// elemen pada array kode warna (ANSI_CODES di src/ui.cpp); STYLE_COUNT di ujung otomatis bernilai banyaknya gaya.
// Pada enum, nama yang tidak diberi nilai otomatis bernilai satu lebih besar dari nama sebelumnya (1, 2, 3, ...).
// Dipakai enum, bukan angka biasa, supaya pemanggil menulis nama yang jelas seperti STYLE_ERROR, dan compiler menolak
// angka int sembarang di tempat yang meminta TextStyle.
enum TextStyle { // TextStyle adalah enum berisi semua gaya teks yang bisa dipakai saat mencetak.
    STYLE_NORMAL = 0, // STYLE_NORMAL = 0 adalah teks biasa tanpa kode warna; ditulis = 0 supaya jelas penomoran dimulai dari 0.
    STYLE_TITLE, // STYLE_TITLE (bernilai 1) adalah gaya judul: tebal dan berwarna cyan, misalnya judul kotak.
    STYLE_BOLD, // STYLE_BOLD (2) adalah teks tebal, misalnya judul kolom tabel, baris TOTAL, dan label pertanyaan.
    STYLE_DIM, // STYLE_DIM (3) adalah abu-abu yang tidak mencolok, untuk garis kotak, catatan, dan pesan keterangan [i].
    STYLE_SUCCESS, // STYLE_SUCCESS (4) adalah hijau tebal untuk pesan berhasil [OK].
    STYLE_ERROR, // STYLE_ERROR (5) adalah merah tebal untuk pesan kesalahan [!].
    STYLE_HINT, // STYLE_HINT (6) adalah kuning untuk ajakan "Tekan Enter untuk kembali ke menu...".
    STYLE_HIGHLIGHT, // STYLE_HIGHLIGHT (7) adalah kuning tebal untuk menyorot komponen berbiaya tertinggi di rekap dan peringkat.
    STYLE_COUNT // STYLE_COUNT (8) bukan gaya, melainkan banyaknya gaya di atasnya; dipakai sebagai ukuran array kode warna.
}; // akhir enum TextStyle

// Tujuan keluaran: seberapa lebar, boleh berwarna atau tidak, dan memakai garis UTF-8 atau ASCII.
// Dengan mengirim keterangan ini ke fungsi gambar, kode yang sama bisa menulis ke layar
// (berwarna, selebar jendela) maupun ke berkas laporan (polos, lebar tetap).
// Ketiga keterangan dibungkus dalam satu struct supaya setiap fungsi gambar cukup menerima satu parameter, bukan tiga.
struct OutputStyle { // OutputStyle adalah struct berisi tiga keterangan tentang tujuan keluaran.
    int width; // width adalah lebar keluaran dalam kolom; int sudah cukup karena nilainya kecil (40 sampai 120 kolom).
    bool useColor; // useColor bernilai true bila kode warna ANSI boleh ditulis; selalu false untuk berkas laporan.
    bool useUnicode; // useUnicode bernilai true bila garis kotak memakai karakter UTF-8; false berarti memakai + - | biasa.
}; // akhir struct OutputStyle

// Satu baris isi kotak beserta gayanya. Teks dan gaya disimpan berdampingan dalam satu struct, jadi saat baris
// yang panjang dilipat, setiap potongannya tetap membawa gaya yang sama.
struct StyledLine { // StyledLine adalah struct untuk satu baris isi kotak.
    std::string text; // text adalah isi baris yang akan ditulis.
    TextStyle style; // style adalah gaya (warna) baris itu, salah satu nilai enum TextStyle.

    StyledLine(const std::string& lineText, TextStyle lineStyle); // StyledLine(lineText, lineStyle) adalah konstruktor supaya baris bisa dibuat dan diisi
    // dalam satu ekspresi, misalnya lines.push_back(StyledLine("Total", STYLE_BOLD)).
}; // akhir struct StyledLine

// Keterangan satu kolom tabel: judul dan arah perataan (angka rata kanan, teks rata kiri).
struct TableColumn { // TableColumn adalah struct untuk keterangan satu kolom tabel.
    std::string header; // header adalah judul kolom, misalnya "Komponen" atau "Total biaya".
    bool alignRight; // alignRight bernilai true untuk kolom rata kanan (angka, supaya satuan dan ribuannya lurus), false untuk rata kiri (teks).

    TableColumn(const std::string& headerText, bool rightAligned); // TableColumn(headerText, rightAligned) adalah konstruktor, dipakai seperti
    // table.columns.push_back(TableColumn("No", true)).
}; // akhir struct TableColumn

// Satu baris tabel: isi tiap sel sebagai teks, ditambah gaya barisnya. Sel disimpan sebagai teks yang sudah
// dibentuk (misalnya "Rp821.400"), jadi fungsi gambar tidak perlu tahu arti angkanya.
struct TableRow { // TableRow adalah struct untuk satu baris tabel.
    std::vector<std::string> cells; // cells adalah isi sel dari kolom pertama sampai terakhir; vector karena banyak kolomnya berbeda tiap tabel.
    TextStyle style; // style adalah gaya seluruh baris, misalnya STYLE_HIGHLIGHT untuk komponen termahal.

    TableRow(); // TableRow() adalah konstruktor tanpa parameter yang mengisi style dengan STYLE_NORMAL, supaya baris baru tidak bergaya acak.
}; // akhir struct TableRow

// Tabel umum. Baris dan kolomnya disimpan dalam std::vector karena banyaknya baru diketahui
// saat program berjalan (mengikuti jumlah komponen). Satu struktur ini dipakai oleh rekap,
// peringkat biaya, daftar harga acuan, dan daftar pilihan komponen.
struct Table { // Table adalah struct yang menyatukan kolom, baris, dan baris penutup sebuah tabel.
    std::vector<TableColumn> columns; // columns adalah keterangan setiap kolom, berurutan dari kiri ke kanan.
    std::vector<TableRow> rows; // rows adalah baris-baris data tabel, berurutan dari atas ke bawah.
    bool hasFooter; // hasFooter bernilai true bila tabel punya baris penutup (misalnya baris TOTAL pada rekap).
    TableRow footer; // footer adalah isi baris penutup itu; hanya digambar bila hasFooter true.

    Table(); // Table() adalah konstruktor yang membuat tabel kosong dengan hasFooter bernilai false.
}; // akhir struct Table

// Gaya keluaran untuk layar (mengikuti terminal) dan untuk berkas teks polos selebar width.
OutputStyle screenStyle(); // screenStyle() berfungsi untuk menyusun gaya layar dari keadaan terminal: lebar jendela, warna, dan UTF-8.
OutputStyle plainStyle(int width); // plainStyle(width) berfungsi untuk menyusun gaya polos tanpa warna dengan garis ASCII, dipakai untuk laporan berkas.

// Membungkus teks dengan kode warna sesuai gayanya; teks dikembalikan apa adanya bila warna mati.
std::string colorize(const OutputStyle& style, TextStyle textStyle, const std::string& text); // colorize(style, textStyle, text) mengembalikan text
// yang sudah diberi kode warna textStyle; semua parameter dikirim sebagai const & atau nilai kecil, jadi tidak ada yang diubah.

// Karakter pengisi batang grafik: blok penuh pada mode UTF-8, tanda # pada mode ASCII.
const char* barGlyph(const OutputStyle& style); // barGlyph(style) mengembalikan teks tetap untuk satu kotak batang grafik di tabel peringkat biaya.

// Lebar isi terbesar sebuah kotak pada gaya keluaran ini.
std::size_t boxInnerLimit(const OutputStyle& style); // boxInnerLimit(style) mengembalikan lebar keluaran dikurangi bingkai kotak dan margin kanan.

// Menggambar kotak berjudul. Baris yang terlalu panjang dilipat mengikuti lebar keluaran.
// std::ostream& sebagai parameter berarti fungsi menerima aliran keluaran apa pun lewat referensi: std::cout (layar),
// std::cerr (saluran pesan galat), atau std::ofstream (berkas). Karena itu satu fungsi gambar cukup ditulis sekali dan
// dipakai untuk layar maupun laporan berkas. Tidak diberi const karena menulis ke aliran mengubah keadaan aliran itu.
void drawBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawBox(out, style, title, lines): out tujuan tulisan, style gaya keluaran,
             const std::vector<StyledLine>& lines); // title judul kotak, dan lines baris-baris isinya; void karena fungsi ini hanya menulis, tanpa hasil.

// Menggambar tabel di dalam kotak. Bila tabel tidak muat, setiap baris ditampilkan sebagai
// "kartu" bersusun ke bawah (dua kolom pertama menjadi judul kartu). notes dicetak di bawah tabel.
void drawTableBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawTableBox(out, style, title, table, notes): tiga parameter
                  const Table& table, const std::vector<StyledLine>& notes); // pertama sama dengan drawBox(), ditambah tabelnya dan catatan (boleh kosong).

// Mencetak teks yang dilipat selebar keluaran; baris lanjutan menjorok hangingIndent spasi.
void printWrapped(std::ostream& out, const OutputStyle& style, TextStyle textStyle, // printWrapped(out, style, textStyle, text, hangingIndent) mencetak
                  const std::string& text, std::size_t hangingIndent); // text tanpa kotak dengan gaya textStyle, dilipat bila lebih lebar dari keluaran.

// Pesan singkat ke layar dengan awalan [!] (salah), [OK] (berhasil), dan [i] (keterangan).
// Ketiganya hanya menerima teks pesan karena tujuan (layar) dan gayanya sudah pasti, jadi pemanggilannya pendek.
void printError(const std::string& message); // printError(message) menampilkan pesan kesalahan berwarna merah dengan awalan [!].
void printSuccess(const std::string& message); // printSuccess(message) menampilkan pesan berhasil berwarna hijau dengan awalan [OK].
void printInfo(const std::string& message); // printInfo(message) menampilkan keterangan berwarna abu-abu dengan awalan [i].

// ===============================================================================================================
// BAGIAN 9 - include/estima/prompt.h - tanya jawab dengan pengguna
// ===============================================================================================================
// Setiap fungsi baca di sini mengulang pertanyaannya sampai jawabannya sah, jadi pemanggil
// selalu menerima data yang sudah benar. Pemeriksaan isinya sendiri dikerjakan oleh fungsi di
// input_parse.h; modul ini hanya mengurus bagian "bertanya, membaca ketikan, lalu mengulang bila
// salah". Isi fungsinya ada di src/prompt.cpp dan pemakainya terutama src/app.cpp.

// Dilempar (throw) ketika masukan habis: Ctrl+D di macOS/Linux, Ctrl+Z lalu Enter di Windows,
// atau berkas masukan sudah sampai ujung. Ditangkap satu kali di runApp() supaya program
// berhenti dengan rapi dari pertanyaan mana pun, tanpa memeriksa keadaan itu di setiap pemanggil.
// Struct ini sengaja kosong (tanpa anggota): ia tidak membawa data apa pun, yang penting hanya
// JENISNYA, karena catch di runApp() mengenali kejadian "masukan habis" dari nama tipe ini.
// Cara kerja throw dan catch dijelaskan di src/prompt.cpp, tempat InputClosed dilempar.
struct InputClosed { // InputClosed adalah tipe penanda "masukan sudah habis"; benda bertipe ini dilempar oleh readLine() dan waitForEnter()
}; // akhir struct InputClosed (isinya memang kosong)

// Menampilkan label pertanyaan lalu membaca satu baris ketikan. Spasi di awal dan akhir jawaban sudah
// dibuang. Fungsi ini menjadi dasar semua fungsi baca lain di bawah.
std::string readLine(const std::string& label); // readLine(const std::string& label) berfungsi untuk bertanya dengan teks label lalu mengembalikan jawabannya sebagai std::string

// Menahan layar sampai pengguna menekan Enter (dipakai sesudah setiap aksi menu, supaya hasilnya sempat dibaca).
void waitForEnter(); // waitForEnter() berfungsi untuk menampilkan ajakan "Tekan Enter..." lalu menunggu satu baris ketikan; void karena tidak ada hasil yang dikembalikan

// Membaca nomor pilihan minValue..maxValue. Bila hasDefault true, Enter kosong berarti defaultValue.
int readChoice(const std::string& label, int minValue, int maxValue, bool hasDefault, int defaultValue); // readChoice(...) berfungsi untuk membaca nomor
// pilihan menu atau nomor komponen; hasilnya int yang pasti berada di antara minValue dan maxValue (atau defaultValue).

// Pertanyaan ya/tidak. defaultYes menentukan jawaban untuk Enter kosong.
bool askYesNo(const std::string& question, bool defaultYes); // askYesNo(...) berfungsi untuk mengajukan pertanyaan ya/tidak; hasil true berarti ya, false berarti tidak

// Pembaca data komponen. Bila hasDefault true, Enter kosong mempertahankan defaultValue
// (dipakai saat mengubah komponen yang sudah ada). Jumlah memakai int karena unit komponen selalu
// bilangan bulat. Berat memakai double karena boleh berdesimal (sampai 3 angka di belakang koma). Harga
// hanya menerima rupiah bulat, tetapi tetap double karena disimpan di Component sebagai double dan
// dikalikan dengan berat yang juga double.
int readQuantity(const std::string& label, bool hasDefault, int defaultValue); // readQuantity(...) berfungsi untuk membaca jumlah unit komponen (1 sampai 1.000.000)
double readUnitWeight(const std::string& label, bool hasDefault, double defaultValue); // readUnitWeight(...) membaca berat per unit dalam kg (0,001 sampai 10.000)
double readPricePerKg(const std::string& label, bool hasDefault, double defaultValue); // readPricePerKg(...) membaca harga material per kg
// dalam rupiah bulat (1 sampai 100.000.000).

// ===============================================================================================================
// BAGIAN 10 - include/estima/report.h - penyusunan laporan dari data komponen
// ===============================================================================================================
// Header ini berisi daftar fungsi yang menyusun laporan dari data komponen: rekap, peringkat biaya,
// daftar harga acuan, ringkasan satu komponen, daftar pilihan komponen, dan penyimpanan ke berkas.
// Fungsi cetak menerima std::ostream& (aliran keluaran apa pun), sehingga laporan yang sama bisa
// dikirim ke layar (std::cout) atau ke berkas (std::ofstream) tanpa menulis kodenya dua kali.
// Isi setiap fungsi ditulis di src/report.cpp; yang memanggilnya adalah menu di src/app.cpp.

// Pesan untuk daftar yang masih kosong; dipakai bersama oleh laporan dan menu.
// Ditulis extern di sini dan diisi sekali di src/report.cpp, supaya kalimatnya sama persis di
// rekap, peringkat, dan menu ubah, hapus, serta simpan laporan di src/app.cpp.
extern const char* const EMPTY_LIST_MESSAGE; // EMPTY_LIST_MESSAGE adalah teks tetap "Belum ada komponen..."; extern artinya isinya ada di src/report.cpp

// Rekap lengkap: total berat dan biaya tiap komponen, total keseluruhan, dan komponen dengan
// biaya produksi tertinggi. Inilah keluaran (1) sampai (4) yang diminta soal, dalam satu fungsi.
// list dikirim sebagai const ComponentList& karena rekap hanya membaca data, tidak mengubahnya.
void printRecap(std::ostream& out, const OutputStyle& style, const ComponentList& list); // printRecap() berfungsi untuk mencetak rekap ke out dengan gaya style;
// dipanggil dari menu 2, setelah data contoh dimuat (menu 6), dan oleh saveReport() untuk isi berkas laporan.

// Peringkat biaya dari yang terbesar, dengan porsi terhadap total dan grafik batang.
void printRanking(std::ostream& out, const OutputStyle& style, const ComponentList& list); // printRanking() berfungsi untuk mencetak tabel peringkat biaya;
// dipanggil dari menu 3 dan oleh saveReport(). Urutannya dihitung rankByCost() tanpa memindah data asli.

// Daftar harga acuan material beserta periode dan sumber datanya.
// Tidak perlu parameter daftar komponen karena isinya diambil dari katalog MATERIAL_CATALOG.
void printCatalog(std::ostream& out, const OutputStyle& style); // printCatalog() berfungsi untuk mencetak tabel harga acuan; dipanggil dari menu 7

// Ringkasan satu komponen (dipakai sebelum menyimpan komponen baru atau perubahan).
// Parameternya satu Component saja, karena yang diringkas adalah salinan (draft) yang belum masuk daftar.
void printComponentSummary(std::ostream& out, const OutputStyle& style, const Component& component); // printComponentSummary() berfungsi untuk mencetak data
// dan hasil hitung satu komponen di dalam kotak, supaya pengguna bisa memeriksa sebelum menjawab "Simpan?".

// Daftar singkat nomor dan nama komponen untuk dipilih (menu ubah dan hapus).
void printComponentPicker(std::ostream& out, const OutputStyle& style, const ComponentList& list); // printComponentPicker() berfungsi untuk mencetak tabel
// No, Komponen, dan Total biaya; nomor yang tampil mulai dari 1, sedangkan posisi di array mulai dari 0.

// Menyimpan laporan lengkap ke berkas teks. Hasil false berarti gagal dan errorMessage berisi sebabnya.
// Hasilnya bool (berhasil atau tidak) ditambah pesan lewat referensi, supaya fungsi ini tidak
// mencetak apa pun sendiri: menu di src/app.cpp yang memutuskan menampilkan pesan sukses atau galat.
bool saveReport(const ComponentList& list, const std::string& fileName, std::string& errorMessage); // saveReport() berfungsi untuk menulis rekap dan peringkat
// ke berkas fileName; errorMessage memakai & tanpa const karena fungsi ini mengisinya saat gagal.

// ===============================================================================================================
// BAGIAN 11 - include/estima/app.h - alur utama program
// ===============================================================================================================
// Header ini adalah "pintu depan" modul app: isinya hanya kode keluar program dan satu fungsi runApp().
// Berkas main.cpp cukup meng-include header ini lalu memanggil runApp(); seluruh menu dan aksinya ada di app.cpp.

// Kode keluar program yang dikirim ke sistem operasi.
// Kode keluar adalah angka yang diterima sistem operasi saat program selesai; skrip, uji otomatis, dan CI membacanya
// untuk tahu program berhenti dengan cara apa. Ketiganya ditulis sebagai konstanta bernama supaya runApp() menulis
// "return EXIT_CODE_BAD_USAGE" yang jelas maksudnya, bukan angka 2 yang harus ditebak artinya.
// Tipenya int karena kode keluar program memang berupa bilangan bulat (hasil fungsi main() juga int).
const int EXIT_CODE_OK = 0; // EXIT_CODE_OK = 0 artinya program selesai normal: pengguna menutupnya lewat menu Keluar,
// atau hanya meminta bantuan (--help) atau nomor versi (--version). Angka 0 adalah tanda "berhasil" yang dipakai semua sistem operasi.
const int EXIT_CODE_INPUT_CLOSED = 1; // EXIT_CODE_INPUT_CLOSED = 1 artinya masukan habis (Ctrl+D, Ctrl+Z lalu Enter, atau berkas masukan
// sudah sampai ujung) sebelum pengguna memilih Keluar; dikembalikan runApp() saat menangkap InputClosed.
const int EXIT_CODE_BAD_USAGE = 2; // EXIT_CODE_BAD_USAGE = 2 artinya program dipanggil dengan opsi yang tidak dikenal; nilai 2 lazim
// dipakai program baris perintah untuk "cara pemakaian salah".

// Menjalankan program: membaca opsi baris perintah, menyiapkan terminal, lalu melayani menu
// sampai pengguna memilih keluar. Hasilnya kode keluar di atas.
// Parameternya sama persis dengan parameter main() (argc dan argv), karena main() hanya meneruskan keduanya ke sini.
// Dengan begitu seluruh alur ada di modul app, dan main.cpp tetap berisi satu perintah saja.
int runApp(int argc, char* argv[]); // runApp(int argc, char* argv[]) adalah deklarasi fungsi utama program; argc adalah banyaknya kata
// pada baris perintah dan argv adalah array teks berisi kata-kata itu. Hasil int-nya salah satu kode keluar di atas; definisinya ada di app.cpp.

// ===============================================================================================================
// BAGIAN 12 - src/component.cpp - operasi dasar pada daftar komponen
// ===============================================================================================================
// Berkas ini berisi definisi (isi) dari semua yang dideklarasikan di include/estima/component.h: dua konstruktor
// dan empat fungsi pengelola daftar. Isinya sengaja pendek dan tidak membaca keyboard maupun mencetak ke layar,
// jadi bisa dipakai oleh menu, katalog data contoh, dan uji otomatis tanpa perubahan.

// Konstruktor memberi nilai awal yang pasti, supaya komponen yang belum diisi tidak pernah
// berisi angka sisa memori.
// Tanpa konstruktor ini, anggota int dan double milik 100 elemen array items bisa berisi angka acak.
// Definisi konstruktor ditulis NamaStruct::NamaStruct() (tanda :: artinya "milik", jadi konstruktor milik struct itu).
// Bagian sesudah titik dua adalah daftar inisialisasi anggota: setiap anggota langsung diberi nilai awal dalam
// kurung, misalnya quantity(0) artinya quantity dimulai dari 0. Isi kurung kurawalnya boleh kosong karena
// semua pekerjaan sudah selesai di daftar itu.
Component::Component() : name(), quantity(0), unitWeightKg(0.0), pricePerKg(0.0) { // name() artinya nama dibuat sebagai teks kosong,
// sedangkan quantity, unitWeightKg, dan pricePerKg dimulai dari 0.
} // akhir konstruktor Component()

ComponentList::ComponentList() : count(0) { // count(0) artinya daftar baru selalu kosong; elemen items sendiri sudah diberi nilai awal
// oleh konstruktor Component() di atas, yang dijalankan otomatis untuk setiap elemen array.
} // akhir konstruktor ComponentList()

// addComponent(): menambah satu komponen di ujung daftar. Elemen baru selalu ditaruh tepat sesudah elemen
// terakhir yang terisi, jadi isi daftar tetap rapat di depan dan urutannya sama dengan urutan pengisian.
bool addComponent(ComponentList& list, const Component& component) { // addComponent() berfungsi untuk menyalin component ke posisi kosong
// pertama di list; hasil true berarti berhasil, false berarti daftar sudah penuh.
    // if berfungsi untuk menjalankan perintah di dalam kurung kurawal hanya bila syarat di dalam kurung bernilai benar.
    // Pasangannya else (bila ada) berisi perintah yang dijalankan bila syaratnya salah.
    // Operator perbandingan menghasilkan true atau false: == sama dengan, != tidak sama dengan, < kurang dari,
    // > lebih dari, <= kurang dari atau sama dengan, >= lebih dari atau sama dengan.
    if (list.count >= MAX_COMPONENTS) { // list.count >= MAX_COMPONENTS artinya ke-100 tempat sudah terisi, jadi tidak ada tempat lagi;
    // dengan >= (bukan ==) keadaan count yang sudah melewati batas pun ikut tertolak.
        // return berfungsi untuk mengakhiri fungsi saat itu juga dan memberikan nilai hasilnya kepada pemanggil.
        return false; // return false artinya komponen tidak ditambahkan karena daftar penuh; isi daftar tidak berubah sama sekali.
    } // akhir if daftar penuh
    // Elemen baru menempati posisi count, yaitu tempat kosong pertama, lalu pencacah dinaikkan.
    list.items[list.count] = component; // list.items[list.count] = component artinya seluruh anggota component (nama, jumlah, berat,
    // harga) disalin sekaligus ke elemen nomor count; struct bisa disalin utuh dengan satu tanda =.
    list.count = list.count + 1; // list.count = list.count + 1 artinya pencacah bertambah satu karena ada satu elemen baru yang terisi.
    return true; // return true artinya komponen berhasil ditambahkan.
} // akhir fungsi addComponent()

// removeComponent(): menghapus satu komponen dari tengah, depan, atau ujung daftar tanpa meninggalkan tempat kosong.
bool removeComponent(ComponentList& list, int index) { // removeComponent() berfungsi untuk membuang komponen pada posisi index (mulai 0);
// hasil true berarti berhasil, false berarti index tidak menunjuk ke komponen yang ada.
    if (index < 0 || index >= list.count) { // index < 0 || index >= list.count artinya posisinya negatif atau berada di luar elemen
    // yang terisi (elemen terisi hanya 0 sampai count - 1), jadi tidak ada yang bisa dihapus.
        return false; // return false artinya tidak ada yang dihapus dan daftar tetap seperti semula.
    } // akhir if index tidak sah
    // Menghapus dari array berarti menimpa: setiap elemen di kanan index digeser satu langkah
    // ke kiri, sehingga tidak ada "lubang" di tengah dan urutan komponen lain tetap sama.
    // for berfungsi untuk mengulang perintah dengan pencacah. Isi kurungnya tiga bagian yang dipisah titik koma:
    // nilai awal pencacah, syarat untuk terus mengulang, dan langkah yang dijalankan setiap selesai satu putaran.
    // ++ berfungsi untuk menambah nilai variabel dengan 1, jadi ++i sama dengan i = i + 1.
    // += berfungsi untuk menambah nilai variabel dengan angka di kanannya, jadi total += x sama dengan total = total + x.
    for (int i = index; i < list.count - 1; ++i) { // i = index artinya penggeseran dimulai dari tempat yang dihapus;
    // i < list.count - 1 karena setiap putaran mengambil items[i + 1], dan elemen terisi terakhir adalah items[count - 1].
        list.items[i] = list.items[i + 1]; // list.items[i] = list.items[i + 1] artinya elemen di sebelah kanan disalin ke kiri satu langkah.
    } // akhir perulangan for penggeseran
    list.count = list.count - 1; // list.count = list.count - 1 artinya isi daftar berkurang satu.
    list.items[list.count] = Component(); // Component() membuat satu komponen baru bernilai awal (nama kosong, angka 0); di sini dipakai
    // untuk mengosongkan elemen terakhir lama (isinya sudah tersalin ke kiri, atau itu komponen yang baru dihapus bila yang
    // dihapus komponen terakhir), supaya tidak ada data sisa sesudah count.
    return true; // return true artinya komponen berhasil dihapus.
} // akhir fungsi removeComponent()

// clearComponents(): mengosongkan seluruh daftar, misalnya sebelum data contoh dimuat.
void clearComponents(ComponentList& list) { // clearComponents() berfungsi untuk mengembalikan list menjadi daftar kosong.
    for (int i = 0; i < list.count; ++i) { // i = 0 sampai i < list.count artinya hanya elemen yang terisi (items[0] sampai items[count - 1])
    // yang perlu dikosongkan; elemen sesudahnya memang sudah kosong.
        list.items[i] = Component(); // list.items[i] = Component() artinya elemen ke-i ditimpa komponen bernilai awal (nama kosong, angka 0).
    } // akhir perulangan for pengosongan
    list.count = 0; // list.count = 0 artinya daftar kini tidak berisi komponen sama sekali.
} // akhir fungsi clearComponents()

// findComponentByName(): mencari komponen lewat namanya; dipakai menu Tambah dan Ubah di src/app.cpp untuk menolak nama kembar.
int findComponentByName(const ComponentList& list, const std::string& name) { // findComponentByName() berfungsi untuk mencari posisi
// komponen bernama name; list memakai const & karena daftar hanya dibaca, tidak diubah.
    // Pencarian berurutan (linear search): daftar paling banyak 100 elemen dan tidak terurut,
    // jadi memeriksa satu per satu dari depan adalah cara paling sederhana yang tetap cepat.
    // Supaya "Gear", "GEAR", dan "gear" dianggap nama yang sama, kedua nama diubah dulu ke huruf kecil lalu dibandingkan.
    const std::string wanted = toLowerAscii(name); // wanted adalah nama yang dicari dalam huruf kecil; dihitung sekali di luar perulangan
    // supaya tidak diulang di setiap putaran, dan const karena nilainya tidak berubah lagi.
    for (int i = 0; i < list.count; ++i) { // i = 0 sampai i < list.count artinya memeriksa setiap komponen yang terisi, dari depan.
        if (toLowerAscii(list.items[i].name) == wanted) { // toLowerAscii(list.items[i].name) == wanted artinya nama komponen ke-i,
        // setelah diubah ke huruf kecil, sama persis dengan nama yang dicari.
            return i; // return i artinya posisi komponen yang cocok langsung diberikan dan pencarian berhenti di temuan pertama.
        } // akhir if nama cocok
    } // akhir perulangan for pencarian
    return -1; // return -1 artinya seluruh daftar sudah diperiksa dan tidak ada nama yang cocok; -1 tidak mungkin menjadi posisi array.
} // akhir fungsi findComponentByName()

// ===============================================================================================================
// BAGIAN 13 - src/calculation.cpp - rumus perhitungan berat dan biaya
// ===============================================================================================================
// Berkas ini berisi isi (definisi) fungsi-fungsi yang dideklarasikan di include/estima/calculation.h:
// rumus total berat, total biaya, penjumlahan seluruh komponen, pencarian komponen termahal, dan peringkat biaya.

// Berat ditulis pengguna dalam kilogram dengan paling banyak tiga angka desimal (ketelitian gram).
// Pecahan desimal seperti 1,85 tidak bisa disimpan tepat dalam bentuk biner, sehingga 24 x 1,85
// yang dihitung langsung menghasilkan 44,400000000000006. Supaya hasil hitung tepat, berat
// diubah dulu ke gram (bilangan bulat), dikalikan, lalu dikembalikan ke kilogram di langkah akhir.
// static pada fungsi atau variabel tingkat berkas berfungsi untuk membatasi nama itu pada satu unit kompilasi, yaitu satu
// berkas .cpp yang dikompilasi beserta semua header yang di-#include-nya. Pada versi modul, berkas .cpp lain tidak bisa
// memanggil toWholeGram dan totalWeightGram; pada berkas tunggal, seluruh program memang satu unit kompilasi, jadi static di
// sini menjadi penanda bahwa keduanya hanya pembantu rumus di bawah, bukan bagian dari daftar fungsi di calculation.h.
static double toWholeGram(double weightKg) { // toWholeGram(weightKg) berfungsi untuk mengubah berat kg menjadi gram bulat, misalnya 1.85 menjadi 1850.
    // std::floor(x) adalah fungsi pembulatan ke bawah: membuang pecahan dan mengambil bilangan bulat terdekat yang tidak lebih besar dari x,
    // misalnya std::floor(1850.4) = 1850. Hasilnya tetap bertipe double.
    return std::floor(weightKg * 1000.0 + 0.5); // weightKg * 1000.0 + 0.5 lalu dibulatkan ke bawah artinya dibulatkan ke gram terdekat;
    // + 0.5 diperlukan karena hasil kali desimal di komputer kadang sedikit kurang dari angka yang benar, misalnya 1.005 x 1000 menjadi
    // 1004,9999999999999; tanpa + 0.5 hasil itu akan terpotong menjadi 1004 gram, bukan 1005 gram.
} // akhir fungsi toWholeGram()

// totalWeightGram menghitung total berat satu komponen dalam gram bulat. Dibuat sebagai fungsi tersendiri karena
// rumus yang sama dipakai tiga kali: untuk total berat, untuk total biaya, dan untuk penjumlahan berat seluruh komponen.
static double totalWeightGram(const Component& component) { // totalWeightGram(component) berfungsi untuk menghitung jumlah x berat per unit dalam gram.
    // static_cast<tipe>(nilai) berfungsi untuk mengubah sebuah nilai ke tipe lain secara terang-terangan (tertulis jelas di kode).
    return static_cast<double>(component.quantity) * toWholeGram(component.unitWeightKg); // static_cast<double>(component.quantity) mengubah jumlah (int) menjadi double
    // sebelum dikalikan. Tanpa static_cast pun int akan diubah otomatis saat dikali double; penulisan tegas ini hanya
    // membuat perubahan tipenya terlihat jelas oleh pembaca.
    // Hasil terbesarnya 1.000.000 unit x 10.000.000 gram, masih bilangan bulat yang bisa disimpan double dengan tepat.
} // akhir fungsi totalWeightGram()

// [Kriteria soal: fungsi menghitung total berat] Total berat = jumlah x berat per unit, dalam kg.
double calculateTotalWeight(const Component& component) { // calculateTotalWeight(component) berfungsi untuk menghitung total berat satu komponen dalam kg.
    return totalWeightGram(component) / 1000.0; // totalWeightGram(component) / 1000.0 artinya gram dikembalikan ke kilogram; ditulis 1000.0 (bukan 1000)
    // supaya jelas pembagiannya pembagian desimal, sehingga 44400 gram menjadi 44.4 kg.
} // akhir fungsi calculateTotalWeight()

// [Kriteria soal: fungsi menghitung total biaya] Total biaya = total berat x harga per kg, dibulatkan ke rupiah terdekat.
double calculateTotalCost(const Component& component) { // calculateTotalCost(component) berfungsi untuk menghitung total biaya satu komponen dalam rupiah bulat.
    // gram x (rupiah per kg) memberi biaya dalam seperseribu rupiah, masih berupa bilangan bulat.
    // Menambah 500 lalu membagi 1000 dan membuang pecahannya sama dengan membulatkan ke rupiah
    // terdekat (setengah rupiah ke atas dibulatkan naik).
    const double costPerMille = totalWeightGram(component) * component.pricePerKg; // costPerMille adalah biaya dalam seperseribu rupiah (gram x harga per kg);
    // bilangan bulat karena gram dan harga sama-sama bulat. const karena nilainya tidak diubah lagi.
    return std::floor((costPerMille + 500.0) / 1000.0); // (costPerMille + 500.0) / 1000.0 lalu std::floor artinya dibagi 1000 dan dibulatkan ke rupiah terdekat;
    // contoh: 1500 seperseribu rupiah (Rp1,5) menjadi floor(2000 / 1000) = Rp2.
} // akhir fungsi calculateTotalCost()

// Total berat seluruh komponen. Penjumlahannya dilakukan dalam gram bulat lebih dulu, baru dibagi 1000 sekali di akhir,
// supaya hasilnya tetap tepat (menjumlahkan banyak angka kg berpecahan bisa menimbulkan galat kecil).
double sumTotalWeight(const Component items[], int count) { // sumTotalWeight(items, count) berfungsi untuk menjumlahkan total berat semua komponen dalam kg.
    // Akumulasi: totalGram mulai dari 0 lalu ditambah berat tiap komponen satu per satu.
    double totalGram = 0.0; // totalGram = 0.0 adalah penampung jumlah berat dalam gram; harus dimulai dari 0 sebelum ditambah.
    for (int i = 0; i < count; ++i) { // for dari i = 0 sampai i < count artinya mengunjungi setiap komponen yang terisi, dari posisi 0 sampai count - 1.
        totalGram += totalWeightGram(items[i]); // totalGram += totalWeightGram(items[i]) artinya berat komponen ke-i (gram) ditambahkan ke penampung.
    } // akhir perulangan for
    return totalGram / 1000.0; // totalGram / 1000.0 artinya jumlah gram diubah sekali ke kilogram lalu dikembalikan.
} // akhir fungsi sumTotalWeight()

// Total biaya seluruh komponen = jumlah dari total biaya tiap komponen (yang masing-masing sudah dibulatkan ke rupiah),
// sehingga angka total di rekap selalu sama dengan penjumlahan angka per baris yang terlihat di tabel.
double sumTotalCost(const Component items[], int count) { // sumTotalCost(items, count) berfungsi untuk menjumlahkan total biaya semua komponen dalam rupiah.
    double total = 0.0; // total = 0.0 adalah penampung jumlah biaya, dimulai dari 0.
    for (int i = 0; i < count; ++i) { // for i = 0 sampai count - 1 artinya setiap komponen dikunjungi satu kali.
        total += calculateTotalCost(items[i]); // total += calculateTotalCost(items[i]) artinya biaya komponen ke-i ditambahkan ke total.
    } // akhir perulangan for
    return total; // return total; mengembalikan jumlah biaya seluruh komponen.
} // akhir fungsi sumTotalCost()

// Jumlah unit seluruh komponen, untuk baris "Jumlah unit" dan baris TOTAL di rekap.
long sumQuantity(const Component items[], int count) { // sumQuantity(items, count) berfungsi untuk menjumlahkan jumlah unit semua komponen.
    long total = 0; // total = 0 bertipe long, sama dengan tipe hasil fungsi, sebagai penampung jumlah unit.
    for (int i = 0; i < count; ++i) { // for i = 0 sampai count - 1 artinya mengunjungi setiap komponen yang terisi.
        total += items[i].quantity; // total += items[i].quantity artinya jumlah unit komponen ke-i (int) ditambahkan ke total (long).
    } // akhir perulangan for
    return total; // return total; mengembalikan jumlah unit seluruh komponen.
} // akhir fungsi sumQuantity()

// [Kriteria soal: fungsi menemukan komponen termahal]
// Dikerjakan dalam dua langkah (cari nilai tertinggi, lalu kumpulkan semua yang sama dengan nilai itu) supaya
// komponen yang biayanya kembar ikut terlaporkan semuanya, bukan hanya yang pertama ditemukan.
int findMostExpensive(const Component items[], int count, int resultIndexes[]) { // findMostExpensive(...) berfungsi untuk mengisi resultIndexes[] dengan posisi
    // komponen termahal dan mengembalikan banyaknya.
    if (count <= 0) { // count <= 0 artinya daftar kosong, jadi tidak ada komponen yang bisa dibandingkan.
        return 0; // return 0; berarti tidak ada komponen termahal yang ditemukan; resultIndexes[] tidak disentuh.
    } // akhir if daftar kosong
    // Langkah 1: cari nilai biaya tertinggi. Elemen pertama dianggap tertinggi dulu, lalu
    // dibandingkan dengan setiap elemen berikutnya.
    double highest = calculateTotalCost(items[0]); // highest diisi biaya komponen pertama (posisi 0) sebagai nilai tertinggi sementara.
    for (int i = 1; i < count; ++i) { // for dimulai dari i = 1 karena komponen posisi 0 sudah dipakai sebagai nilai awal highest.
        const double cost = calculateTotalCost(items[i]); // cost adalah biaya komponen ke-i, disimpan dulu supaya tidak dihitung dua kali di baris berikutnya.
        if (cost > highest) { // cost > highest memakai lebih besar (bukan >=) karena biaya yang sama tidak mengubah nilai tertinggi.
            highest = cost; // highest = cost; artinya nilai tertinggi diganti dengan biaya yang lebih besar ini.
        } // akhir if pembanding
    } // akhir perulangan langkah 1
    // Langkah 2: kumpulkan SEMUA komponen yang biayanya sama dengan nilai tertinggi itu, karena
    // bisa saja ada lebih dari satu. Biaya selalu bilangan bulat rupiah, jadi aman dibandingkan
    // dengan ==.
    int found = 0; // found = 0 adalah pencacah komponen termahal yang sudah ditemukan, sekaligus posisi kosong berikutnya di resultIndexes[].
    for (int i = 0; i < count; ++i) { // for dimulai lagi dari i = 0 karena kali ini SEMUA komponen, termasuk yang pertama, harus diperiksa.
        if (calculateTotalCost(items[i]) == highest) { // calculateTotalCost(items[i]) == highest artinya komponen ke-i termasuk yang termahal.
            resultIndexes[found] = i; // resultIndexes[found] = i; artinya posisi komponen ini dicatat di tempat kosong berikutnya.
            found = found + 1; // found = found + 1; artinya pencacah naik satu, jadi catatan berikutnya tidak menimpa yang ini.
        } // akhir if biaya sama dengan tertinggi
    } // akhir perulangan langkah 2
    return found; // return found; mengembalikan banyaknya komponen termahal (paling sedikit 1 bila daftar tidak kosong);
    // posisinya tercatat urut dari depan daftar.
} // akhir fungsi findMostExpensive()

// rankByCost tidak memindahkan isi array komponen: yang diurutkan hanya array order[] berisi nomor posisi.
// Dengan begitu nomor urut komponen di daftar tetap sama setelah peringkat ditampilkan.
void rankByCost(const Component items[], int count, int order[]) { // rankByCost(items, count, order) berfungsi untuk mengisi order[] dengan posisi komponen
    // dari biaya terbesar ke terkecil.
    for (int i = 0; i < count; ++i) { // for i = 0 sampai count - 1 berfungsi untuk mengisi order[] dengan urutan awal 0, 1, 2, ... (sesuai urutan pengisian).
        order[i] = i; // order[i] = i; artinya posisi ke-i mula-mula ditempati komponen nomor i.
    } // akhir perulangan pengisian awal
    // Insertion sort pada array nomor urut: setiap nomor disisipkan ke posisi yang benar di
    // antara nomor-nomor sebelumnya yang sudah terurut. Dipilih karena datanya sedikit (paling
    // banyak 100), langkahnya mudah diikuti, dan komponen dengan biaya sama tetap pada urutan
    // pengisiannya. Yang diurutkan nomor urutnya saja, jadi data komponen tidak ikut dipindah.
    for (int i = 1; i < count; ++i) { // for dimulai dari i = 1 karena satu elemen pertama (order[0]) sendirian sudah dianggap terurut.
        const int current = order[i]; // current adalah nomor komponen yang sedang disisipkan, disimpan karena tempatnya di order[] bisa tertimpa saat penggeseran.
        const double currentCost = calculateTotalCost(items[current]); // currentCost adalah biaya komponen current, yang dibandingkan dengan biaya di sebelah kirinya.
        int position = i - 1; // position = i - 1 artinya pemeriksaan dimulai dari elemen tepat di kiri current.
        // while berfungsi untuk mengulang perintah di dalamnya selama syaratnya masih benar; dipakai di sini karena banyaknya
        // langkah geser tidak diketahui sebelumnya (berhenti begitu ketemu tempat yang pas).
        while (position >= 0 && calculateTotalCost(items[order[position]]) < currentCost) { // syarat while: masih ada elemen di kiri (position >= 0)
            // DAN biaya elemen itu lebih kecil dari currentCost. position >= 0 ditulis lebih dulu karena && berhenti memeriksa bila syarat pertama salah,
            // jadi order[-1] tidak pernah dibaca. Pembanding < (bukan <=) membuat biaya yang sama tidak digeser, sehingga urutan pengisiannya tetap.
            order[position + 1] = order[position]; // order[position + 1] = order[position]; artinya nomor yang biayanya lebih kecil digeser satu langkah ke kanan.
            position = position - 1; // position = position - 1; artinya pemeriksaan pindah satu langkah ke kiri.
        } // akhir perulangan while
        order[position + 1] = current; // order[position + 1] = current; artinya current diletakkan di celah yang sudah dikosongkan, tepat di kanan elemen
        // yang biayanya tidak lebih kecil (atau di posisi 0 bila current paling mahal sejauh ini).
    } // akhir perulangan insertion sort
} // akhir fungsi rankByCost()

// Porsi biaya dalam persen, dengan penjagaan supaya tidak terjadi pembagian dengan nol.
double costSharePercent(double cost, double totalCost) { // costSharePercent(cost, totalCost) berfungsi untuk menghitung berapa persen cost dari totalCost.
    if (totalCost <= 0.0) { // totalCost <= 0.0 artinya total biaya nol, sehingga pembagian tidak boleh dilakukan (membagi dengan nol).
        return 0.0; // return 0.0; artinya porsinya dianggap 0 persen.
    } // akhir if penjaga pembagian nol
    return cost / totalCost * 100.0; // cost / totalCost * 100.0 artinya perbandingan biaya terhadap total dikali 100 supaya menjadi persen, misalnya 50 / 200 x 100 = 25.
} // akhir fungsi costSharePercent()

// ===============================================================================================================
// BAGIAN 14 - src/material_catalog.cpp - harga acuan material dan data contoh
// ===============================================================================================================
// Berkas ini berisi isi katalog harga acuan yang diperkenalkan di include/estima/material_catalog.h, tabel data contoh,
// dan dua fungsi: pencari material berdasarkan nama dan pemuat data contoh ke daftar komponen.

// Harga acuan per kilogram untuk bahan setengah jadi (batang bulat, pelat, atau batang cor) yang
// biasa dibeli bengkel mesin di Indonesia. Tiap angka adalah nilai tengah (median) beberapa harga
// dari daftar harga distributor dan toko daring yang dibuka pada November 2026, lalu dibulatkan;
// sumber, tanggal, dan cara pembulatan tiap angka dicatat di docs/sumber-data.md.
// Harga pasar berubah, jadi angka ini hanya titik awal: pengguna selalu bisa mengetik harga
// sendiri saat mengisi komponen.
// Array struct diisi langsung dengan daftar di antara { }: setiap { ... } di dalamnya adalah satu elemen MaterialReference,
// dan nilainya mengisi anggota struct sesuai urutan deklarasi (name, standard, typicalUse, pricePerKg).
// Ukuran array tidak ditulis di dalam [] karena compiler menghitungnya sendiri dari banyaknya baris (di sini 11).
// Deklarasi extern di material_catalog.h sudah dibaca compiler lebih dulu, jadi definisi const di bawah ini berlaku untuk
// seluruh program; tanpa extern, variabel const di luar fungsi hanya berlaku di unit kompilasi tempat ia ditulis.
const MaterialReference MATERIAL_CATALOG[] = { // MATERIAL_CATALOG[] adalah array konstan berisi 11 material; nomor pilihan di layar = posisi + 1.
    { "Baja SS400", "JIS G3101", "bracket, rangka, base plate", 14000.0 }, // Baja SS400: baja konstruksi umum, Rp14.000 per kg (pilihan nomor 1).
    { "Baja karbon S45C", "JIS G4051", "poros, roda gigi, pin", 19500.0 }, // Baja karbon S45C: baja karbon untuk komponen mesin, Rp19.500 per kg.
    { "Baja paduan SCM440", "JIS G4053", "poros dan gear beban berat", 27000.0 }, // Baja paduan SCM440: baja paduan krom-molibdenum, Rp27.000 per kg.
    { "Stainless SUS304", "JIS G4303", "komponen tahan karat", 53000.0 }, // Stainless SUS304: baja tahan karat yang paling umum, Rp53.000 per kg.
    { "Stainless SUS316L", "JIS G4303", "tahan bahan kimia, air laut", 100000.0 }, // Stainless SUS316L: baja tahan karat untuk lingkungan lebih keras, Rp100.000 per kg.
    { "Besi cor FC250", "JIS G5501", "housing, bed mesin, puli", 38000.0 }, // Besi cor FC250: besi cor kelabu untuk benda cor, Rp38.000 per kg.
    { "Aluminium 6061-T6", "JIS H4040", "bracket ringan, cover, jig", 135000.0 }, // Aluminium 6061-T6: paduan aluminium untuk bagian ringan, Rp135.000 per kg.
    { "Kuningan C3604", "JIS H3250", "bushing, fitting, mur", 250000.0 }, // Kuningan C3604: kuningan yang mudah dibubut, Rp250.000 per kg.
    { "Perunggu BC6", "JIS H5120", "bantalan luncur, bushing", 325000.0 }, // Perunggu BC6: perunggu cor untuk bantalan, Rp325.000 per kg.
    { "Nylon MC (PA6)", "ASTM D5989", "roda gigi ringan, roller", 120000.0 }, // Nylon MC (PA6): plastik teknik nilon cor, Rp120.000 per kg.
    { "POM (polyacetal)", "ASTM D6100", "gear presisi, bagian geser", 100000.0 } // POM (polyacetal): plastik teknik yang licin dan kaku, Rp100.000 per kg;
    // baris terakhir tidak diberi koma di belakangnya.
}; // akhir array MATERIAL_CATALOG

// Banyaknya baris katalog dihitung dari ukuran array, sehingga menambah atau menghapus baris
// di atas tidak menuntut angka lain ikut diubah.
// sizeof berfungsi untuk mengetahui ukuran sebuah variabel atau tipe dalam byte. sizeof(MATERIAL_CATALOG) adalah ukuran seluruh array,
// sedangkan sizeof(MATERIAL_CATALOG[0]) adalah ukuran satu elemennya; hasil baginya sama dengan banyaknya elemen.
// Hasil sizeof bertipe std::size_t (bilangan tanpa tanda), jadi diubah ke int dengan static_cast supaya cocok dengan tipe MATERIAL_COUNT.
const int MATERIAL_COUNT = static_cast<int>(sizeof(MATERIAL_CATALOG) / sizeof(MATERIAL_CATALOG[0])); // MATERIAL_COUNT = ukuran array / ukuran satu elemen,
// yaitu 11 untuk katalog di atas.

// Periode data dan keterangan sumber ditulis sebagai teks tetap (const char* const): penunjuknya tidak bisa diarahkan ke teks lain
// dan hurufnya tidak bisa diubah.
const char* const CATALOG_PERIOD = "November 2026"; // CATALOG_PERIOD = "November 2026" adalah bulan pengambilan data harga katalog.
const char* const CATALOG_SOURCE_NOTE = // CATALOG_SOURCE_NOTE adalah keterangan sumber harga yang dicetak di bawah tabel menu 7.
    // Dua teks dalam tanda kutip yang ditulis berurutan tanpa tanda apa pun di antaranya otomatis digabung compiler menjadi satu teks;
    // cara ini dipakai supaya teks panjang bisa dipecah ke beberapa baris kode.
    "Nilai tengah harga dari daftar harga distributor dan toko daring di Indonesia, dibulatkan; " // bagian pertama keterangan; spasi di ujungnya
    // menjadi pemisah dengan bagian kedua setelah digabung.
    "rinciannya ada di docs/sumber-data.md pada repositori program."; // bagian kedua keterangan, menunjuk berkas yang memuat rincian sumber harga.

// Data contoh: lima komponen mesin yang beratnya dihitung dari ukuran dan massa jenis bahannya
// (lihat docs/sumber-data.md). Bahan dirujuk lewat NAMA di katalog, bukan lewat harga, supaya
// harga contoh selalu mengikuti katalog di atas.
// Struct ini ditulis di berkas .cpp (bukan di header) karena hanya dipakai di berkas ini.
struct SampleComponent { // SampleComponent adalah struct untuk satu baris data contoh.
    const char* name; // name adalah nama komponen contoh, misalnya "Gear"; teks tetap karena tidak pernah berubah.
    int quantity; // quantity adalah jumlah unit komponen contoh.
    double unitWeightKg; // unitWeightKg adalah berat per unit dalam kg, dengan paling banyak tiga angka desimal.
    const char* materialName; // materialName adalah nama bahan yang harus sama persis dengan salah satu name di MATERIAL_CATALOG.
}; // akhir struct SampleComponent

// SAMPLE_COMPONENTS memakai static karena tabel ini hanya dibaca oleh loadSampleComponents() di bawah; berkas lain tidak perlu melihatnya.
static const SampleComponent SAMPLE_COMPONENTS[] = { // SAMPLE_COMPONENTS[] adalah array konstan berisi 5 komponen contoh (nama, jumlah, berat/unit, bahan).
    { "Gear", 24, 2.497, "Baja karbon S45C" }, // Gear: 24 unit, 2,497 kg per unit, bahan Baja karbon S45C.
    { "Shaft", 12, 4.932, "Baja paduan SCM440" }, // Shaft: 12 unit, 4,932 kg per unit, bahan Baja paduan SCM440.
    { "Bracket", 40, 0.965, "Baja SS400" }, // Bracket: 40 unit, 0,965 kg per unit, bahan Baja SS400.
    { "Housing", 8, 6.800, "Besi cor FC250" }, // Housing: 8 unit, 6,800 kg per unit, bahan Besi cor FC250.
    { "Bushing", 30, 0.194, "Perunggu BC6" } // Bushing: 30 unit, 0,194 kg per unit, bahan Perunggu BC6 (baris terakhir, tanpa koma).
}; // akhir array SAMPLE_COMPONENTS

// SAMPLE_COUNT dihitung dengan cara sizeof yang sama seperti MATERIAL_COUNT, jadi ikut berubah bila baris contoh ditambah atau dikurangi.
static const int SAMPLE_COUNT = static_cast<int>(sizeof(SAMPLE_COMPONENTS) / sizeof(SAMPLE_COMPONENTS[0])); // SAMPLE_COUNT adalah banyaknya baris data contoh
// (5); static karena hanya dipakai di berkas ini.

// Pencarian berurutan (linear search) dari baris pertama katalog: barisnya sedikit dan tidak diurutkan menurut nama,
// jadi memeriksa satu per satu sudah cukup cepat dan paling mudah diikuti.
int findMaterialByName(const char* name) { // findMaterialByName(name) berfungsi untuk mencari posisi material bernama name di katalog.
    for (int i = 0; i < MATERIAL_COUNT; ++i) { // for i = 0 sampai MATERIAL_COUNT - 1 artinya setiap baris katalog diperiksa dari depan.
        // std::strcmp(a, b) berfungsi untuk membandingkan dua teks gaya C huruf demi huruf; hasilnya 0 bila keduanya sama persis
        // (huruf besar dan kecil dibedakan). Dipakai karena name dan nama di katalog berupa const char*, dan tanda == pada dua
        // const char* hanya membandingkan alamat memorinya, bukan isi teksnya.
        if (std::strcmp(MATERIAL_CATALOG[i].name, name) == 0) { // std::strcmp(...) == 0 artinya nama di baris ke-i sama persis dengan name.
            return i; // return i; langsung mengembalikan posisi yang ditemukan dan menghentikan pencarian.
        } // akhir if nama cocok
    } // akhir perulangan for
    return -1; // return -1; artinya seluruh katalog sudah diperiksa dan nama itu tidak ada.
} // akhir fungsi findMaterialByName()

// Isi daftar lama dibuang dulu, lalu setiap baris data contoh diubah menjadi Component dengan harga dari katalog.
// Bila daftar sudah berisi, pertanyaan "Ganti dengan data contoh?" diajukan oleh pemanggilnya (menu 6 di app.cpp) sebelum fungsi ini dipanggil.
int loadSampleComponents(ComponentList& list) { // loadSampleComponents(list) berfungsi untuk mengganti isi list dengan data contoh dan mengembalikan banyaknya.
    clearComponents(list); // clearComponents(list) berfungsi untuk mengosongkan daftar lebih dulu, supaya data contoh tidak tercampur data lama.
    for (int i = 0; i < SAMPLE_COUNT; ++i) { // for i = 0 sampai SAMPLE_COUNT - 1 artinya setiap baris data contoh diproses satu per satu.
        const int materialIndex = findMaterialByName(SAMPLE_COMPONENTS[i].materialName); // materialIndex adalah posisi bahan komponen contoh ke-i di katalog,
        // atau -1 bila namanya tidak ditemukan.
        if (materialIndex < 0) { // materialIndex < 0 artinya bahan tidak ada di katalog, sehingga harganya tidak bisa diambil.
            // Hanya terjadi bila nama bahan di tabel contoh salah ketik; uji otomatis menjaganya.
            // continue berfungsi untuk melompati sisa isi perulangan pada putaran ini dan langsung lanjut ke putaran berikutnya.
            continue; // continue; artinya komponen contoh ini dilewati (tidak dimasukkan ke daftar) dan perulangan lanjut ke baris contoh berikutnya.
        } // akhir if bahan tidak ditemukan
        Component component; // component adalah komponen baru; konstruktornya mengisi nilai awal (nama kosong, angka 0) sebelum diisi di bawah.
        component.name = SAMPLE_COMPONENTS[i].name; // component.name diisi nama contoh; teks const char* disalin menjadi std::string.
        component.quantity = SAMPLE_COMPONENTS[i].quantity; // component.quantity diisi jumlah unit dari baris contoh.
        component.unitWeightKg = SAMPLE_COMPONENTS[i].unitWeightKg; // component.unitWeightKg diisi berat per unit dari baris contoh.
        component.pricePerKg = MATERIAL_CATALOG[materialIndex].pricePerKg; // component.pricePerKg diambil dari katalog lewat materialIndex,
        // jadi harga data contoh selalu sama dengan harga acuan yang sedang berlaku.
        addComponent(list, component); // addComponent(list, component) berfungsi untuk menambahkan komponen ke ujung daftar; daftar baru saja
        // dikosongkan dan contohnya hanya 5, jadi batas 100 komponen tidak mungkin terlampaui dan hasil true/false-nya tidak perlu diperiksa.
    } // akhir perulangan for
    return list.count; // return list.count; mengembalikan banyaknya komponen yang benar-benar dimuat (5 bila semua nama bahan cocok).
} // akhir fungsi loadSampleComponents()

// ===============================================================================================================
// BAGIAN 15 - src/text_format.cpp - pembentukan teks: angka, rupiah, berat, perataan, dan pelipatan baris
// ===============================================================================================================
// Berkas ini berisi isi (definisi) fungsi-fungsi yang daftarnya ada di text_format.h. Fungsi di sini hanya menyusun teks
// lalu mengembalikannya; pencetakan ke layar atau ke berkas dikerjakan pemanggilnya (ui.cpp, report.cpp, app.cpp, prompt.cpp).

// formatThousands() menjadi dasar semua penulisan angka di program. Angka diubah dulu menjadi deret digit memakai
// std::ostringstream, bukan std::to_string, karena std::to_string belum ada di C++98 yang mungkin dipakai compiler lama.
// Sesudah itu titik ribuan disisipkan sendiri, karena aturan C++ bawaan tidak mengenal titik ribuan gaya Indonesia.
std::string formatThousands(double wholeNumber) { // formatThousands(double wholeNumber) berfungsi untuk menulis angka dengan titik ribuan, misalnya "1.250.000".
    // Angka diubah dulu menjadi deret digit tanpa desimal, misalnya "1250000".
    // std::ostringstream adalah aliran keluaran yang menulis ke dalam sebuah teks, bukan ke layar. Cara menulisnya sama
    // dengan mencetak (memakai <<), lalu teks hasilnya diambil dengan .str(). Cocok untuk mengubah angka menjadi teks.
    std::ostringstream stream; // stream adalah ostringstream kosong yang akan menampung angka dalam bentuk teks.
    // std::fixed berfungsi untuk memaksa angka ditulis biasa (bukan bentuk ilmiah seperti 1.25e+06), dan
    // std::setprecision(n) berfungsi untuk menentukan banyaknya angka di belakang titik desimal saat std::fixed aktif.
    stream << std::fixed << std::setprecision(0) << wholeNumber; // std::fixed << std::setprecision(0) artinya wholeNumber ditulis utuh tanpa desimal;
    // tanpa std::fixed, setprecision(0) berarti hanya satu angka penting, sehingga 1250000 tertulis "1e+06": nilainya ikut berubah
    // dan tidak bisa diberi titik ribuan.
    const std::string digits = stream.str(); // digits = stream.str() artinya teks yang ditulis ke stream diambil, misalnya "1250000"; const karena tidak diubah.

    // Titik disisipkan tepat sebelum digit yang banyak digit dari dirinya sampai ujung kanan merupakan kelipatan tiga: 1.250.000.
    std::string result; // result adalah teks hasil yang disusun satu karakter demi satu karakter; awalnya kosong.
    // Fungsi anggota std::string yang dipakai di program ini (dipanggil dengan tanda titik, misalnya teks.size()):
    // size() memberi banyaknya byte (karakter) di dalam teks; substr(awal, panjang) mengambil potongan teks;
    // find(x) mencari posisi x di dalam teks (hasilnya std::string::npos bila tidak ketemu); erase(awal, panjang) membuang
    // sebagian teks; assign(n, c) mengganti seluruh isi teks dengan n buah karakter c; compare(...) membandingkan sebagian
    // teks dengan teks lain (hasil 0 berarti sama); empty() bernilai true bila teks kosong.
    // size() dan substr() dipakai di text_format.cpp, ui.cpp, dan input_parse.cpp; find() di ui.cpp dan input_parse.cpp; compare()
    // hanya di input_parse.cpp; erase() dan assign() hanya di text_format.cpp; empty() dipakai di banyak bagian program.
    const std::size_t length = digits.size(); // length = digits.size() adalah banyaknya digit; disimpan sekali karena dipakai berulang di dalam perulangan.
    for (std::size_t i = 0; i < length; ++i) { // for (i = 0; i < length; ++i) berfungsi untuk mendatangi setiap digit dari kiri ke kanan.
        // % (sisa bagi) berfungsi untuk menghasilkan sisa pembagian; misalnya 7 % 3 = 1, dan 6 % 3 = 0 (habis dibagi).
        if (i > 0 && (length - i) % 3 == 0) { // (length - i) % 3 == 0 artinya banyaknya digit dari posisi i sampai ujung kanan adalah kelipatan tiga;
            // i > 0 mencegah titik di paling depan (misalnya ".250" untuk angka 250).
            // char adalah tipe data untuk SATU karakter (satu byte), ditulis di antara tanda petik tunggal seperti '.' atau 'A'.
            // Berbeda dengan "..." (petik ganda) yang berupa teks berisi nol karakter atau lebih.
            result += '.'; // result += '.' artinya karakter titik ditambahkan di ujung result sebagai pemisah ribuan.
        } // akhir if penyisipan titik
        result += digits[i]; // result += digits[i] artinya digit ke-i (diambil dengan [nomor] seperti elemen array) disalin ke ujung result.
    } // akhir perulangan for setiap digit
    return result; // return result; mengembalikan angka yang sudah bertitik ribuan.
} // akhir fungsi formatThousands()

// formatRupiah() cukup menambahkan awalan "Rp" di depan hasil formatThousands(). Nilai dibulatkan lebih dulu sebagai
// pengaman, supaya yang ditulis selalu rupiah utuh walaupun nilai yang dikirim berpecahan.
std::string formatRupiah(double amount) { // formatRupiah(double amount) berfungsi untuk menulis amount sebagai uang, misalnya 821400 menjadi "Rp821.400".
    return "Rp" + formatThousands(std::floor(amount + 0.5)); // std::floor(amount + 0.5) artinya membulatkan ke rupiah terdekat (setengah ke atas naik);
    // tanda + di sini menyambung teks "Rp" dengan angka yang sudah bertitik ribuan.
} // akhir fungsi formatRupiah()

// formatWeight() menulis berat dengan tiga angka desimal karena berat diketik sampai ketelitian gram (maksimal 3 desimal).
std::string formatWeight(double weightKg) { // formatWeight(double weightKg) berfungsi untuk menulis berat, misalnya 44.4 menjadi "44,400 kg".
    // Berat dipecah menjadi kilogram utuh dan sisa gram supaya bagian desimalnya selalu tiga
    // angka (1,850) dan bagian utuhnya tetap mendapat titik ribuan (1.234,500).
    const double totalGram = std::floor(weightKg * 1000.0 + 0.5); // totalGram adalah berat dalam gram, dibulatkan ke gram terdekat; misalnya 44.4 kg menjadi 44400 gram.
    const double wholeKg = std::floor(totalGram / 1000.0); // wholeKg adalah bagian kilogram utuh; 44400 / 1000 = 44,4 lalu dibulatkan ke bawah menjadi 44.
    const int gramPart = static_cast<int>(totalGram - wholeKg * 1000.0); // gramPart adalah sisa gram (0 sampai 999); 44400 - 44000 = 400.
    // Dijadikan int karena nilainya sudah pasti bilangan bulat 0 sampai 999, jadi std::setw dan std::setfill di bawah bekerja pada angka bulat.

    std::ostringstream stream; // stream adalah ostringstream baru tempat menyusun teks berat.
    // std::setw(n) berfungsi untuk mengatur lebar minimum nilai BERIKUTNYA yang ditulis (hanya berlaku untuk satu nilai), dan
    // std::setfill(c) berfungsi untuk menentukan karakter pengisi bila nilainya lebih pendek dari lebar itu (bawaannya spasi).
    stream << formatThousands(wholeKg) << ',' << std::setw(3) << std::setfill('0') << gramPart << " kg"; // kg utuh, koma, gram 3 digit, lalu " kg".
    // std::setw(3) << std::setfill('0') << gramPart artinya gram selalu ditulis tiga digit dengan nol di depan: 5 gram menjadi "005", jadi 1,005 kg.
    return stream.str(); // return stream.str(); mengembalikan teks yang sudah tersusun, misalnya "44,400 kg".
} // akhir fungsi formatWeight()

// formatPercent() memakai cara yang sama dengan formatWeight(), tetapi dengan satu angka desimal (persepuluhan).
std::string formatPercent(double percent) { // formatPercent(double percent) berfungsi untuk menulis persen, misalnya 56.512 menjadi "56,5%".
    const double tenths = std::floor(percent * 10.0 + 0.5); // tenths adalah persen dalam satuan persepuluhan, dibulatkan; 56.512 x 10 = 565,12 lalu menjadi 565.
    const double whole = std::floor(tenths / 10.0); // whole adalah bagian persen yang utuh; 565 / 10 = 56,5 lalu dibulatkan ke bawah menjadi 56.
    const int tenthPart = static_cast<int>(tenths - whole * 10.0); // tenthPart adalah satu angka di belakang koma (0 sampai 9); 565 - 560 = 5.

    std::ostringstream stream; // stream adalah ostringstream baru tempat menyusun teks persen.
    stream << formatThousands(whole) << ',' << tenthPart << '%'; // bagian utuh, koma, satu angka desimal, lalu tanda '%'; hasilnya misalnya "56,5%".
    return stream.str(); // return stream.str(); mengembalikan teks persen yang sudah tersusun.
} // akhir fungsi formatPercent()

// true bila byte ini adalah byte lanjutan UTF-8 (berpola 10xxxxxx), yaitu bagian dari karakter
// yang byte awalnya sudah dihitung.
// Dasarnya: dalam UTF-8, satu karakter ditulis dengan 1 sampai 4 byte. Byte pertama sebuah karakter tidak pernah berpola
// 10xxxxxx, sedangkan byte kedua dan seterusnya selalu berpola itu. Jadi dengan melewati byte lanjutan, satu karakter
// terhitung tepat satu kali. Fungsi ini static karena hanya dipakai oleh fungsi lain di berkas ini.
static bool isContinuationByte(char byte) { // isContinuationByte(char byte) berfungsi untuk memeriksa apakah byte adalah byte lanjutan UTF-8.
    // unsigned char adalah char tanpa tanda negatif (nilai 0 sampai 255). Dipakai karena di banyak compiler char biasa
    // bertanda, sehingga byte UTF-8 (nilai 128 ke atas) terbaca negatif.
    // 0xC0 dan 0x80 adalah bilangan heksadesimal (basis 16): 0xC0 = 11000000 dan 0x80 = 10000000 dalam biner.
    // & (AND per bit) berfungsi untuk menyisakan bit yang bernilai 1 di kedua sisi; & 0xC0 artinya hanya dua bit teratas yang dilihat.
    return (static_cast<unsigned char>(byte) & 0xC0) == 0x80; // (byte & 0xC0) == 0x80 artinya dua bit teratas byte adalah 10, yaitu pola byte lanjutan.
} // akhir fungsi isContinuationByte()

// displayWidth() menghitung karakter, bukan byte, dengan menganggap setiap karakter selebar satu kolom. Anggapan ini
// benar untuk huruf ASCII dan untuk karakter garis kotak serta blok grafik yang dipakai program ini.
std::size_t displayWidth(const std::string& text) { // displayWidth(const std::string& text) berfungsi untuk menghitung lebar text di layar dalam kolom.
    std::size_t width = 0; // width adalah pencacah kolom; mulai 0 lalu bertambah satu untuk setiap karakter.
    for (std::size_t i = 0; i < text.size(); ++i) { // for (i = 0; i < text.size(); ++i) berfungsi untuk memeriksa setiap byte text dari awal sampai akhir.
        if (!isContinuationByte(text[i])) { // !isContinuationByte(text[i]) artinya byte ini adalah awal sebuah karakter, bukan byte lanjutan.
            width = width + 1; // width = width + 1 artinya satu karakter (satu kolom) lagi terhitung.
        } // akhir if awal karakter
    } // akhir perulangan for setiap byte
    return width; // return width; mengembalikan lebar text dalam kolom.
} // akhir fungsi displayWidth()

// padRight() dan padLeft() meratakan teks dengan spasi. Teks yang sudah selebar width atau lebih dikembalikan utuh,
// tidak dipotong, supaya isi sel tidak pernah hilang; lebar kolom tabel sendiri sudah dihitung dari isi terlebarnya.
std::string padRight(const std::string& text, std::size_t width) { // padRight(text, width) berfungsi untuk membuat text rata kiri selebar width kolom.
    const std::size_t current = displayWidth(text); // current adalah lebar text sekarang (dalam kolom, bukan byte).
    if (current >= width) { // current >= width artinya text sudah cukup lebar, jadi tidak perlu ditambah spasi.
        return text; // return text; mengembalikan text apa adanya.
    } // akhir if teks sudah cukup lebar
    // std::string(n, ' ') adalah cara membuat teks berisi n buah karakter yang sama, di sini n buah spasi.
    return text + std::string(width - current, ' '); // text + std::string(width - current, ' ') artinya kekurangan lebarnya diisi spasi di kanan.
} // akhir fungsi padRight()

std::string padLeft(const std::string& text, std::size_t width) { // padLeft(text, width) berfungsi untuk membuat text rata kanan selebar width kolom (kolom angka).
    const std::size_t current = displayWidth(text); // current adalah lebar text sekarang dalam kolom.
    if (current >= width) { // current >= width artinya text sudah cukup lebar, jadi dikembalikan tanpa spasi tambahan.
        return text; // return text; mengembalikan text apa adanya.
    } // akhir if teks sudah cukup lebar
    return std::string(width - current, ' ') + text; // std::string(width - current, ' ') + text artinya spasi ditaruh di KIRI, sehingga ujung kanan teks lurus.
} // akhir fungsi padLeft()

// repeatText() menerima sepotong teks (bukan satu char) karena garis kotak dan blok grafik UTF-8 terdiri dari tiga byte.
std::string repeatText(const std::string& piece, std::size_t times) { // repeatText(piece, times) berfungsi untuk mengulang piece sebanyak times kali.
    std::string result; // result adalah teks hasil; awalnya kosong.
    for (std::size_t i = 0; i < times; ++i) { // for (i = 0; i < times; ++i) berfungsi untuk mengulang tepat times kali.
        result += piece; // result += piece artinya piece disambungkan sekali lagi di ujung result.
    } // akhir perulangan for
    return result; // return result; mengembalikan teks yang sudah diulang, misalnya garis "----------".
} // akhir fungsi repeatText()

// Banyaknya byte di awal text yang lebarnya paling banyak maxWidth kolom, tanpa membelah satu
// karakter UTF-8 di tengah.
// Hasilnya berupa banyaknya byte (bukan kolom) karena substr() dan erase() bekerja dengan posisi byte.
static std::size_t prefixBytesForWidth(const std::string& text, std::size_t maxWidth) { // dipakai truncateText() dan wrapText() untuk memotong teks.
    std::size_t width = 0; // width adalah banyaknya kolom (karakter) yang sudah diambil.
    std::size_t index = 0; // index adalah posisi byte berikutnya; di akhir fungsi nilainya sama dengan banyaknya byte yang diambil.
    while (index < text.size() && width < maxWidth) { // while (...) berfungsi untuk mengambil satu karakter per putaran selama text belum habis dan lebar belum penuh.
        index = index + 1; // index = index + 1 artinya byte awal karakter ini diambil.
        while (index < text.size() && isContinuationByte(text[index])) { // while (...) berfungsi untuk ikut mengambil semua byte lanjutan karakter yang sama.
            index = index + 1; // index = index + 1 artinya satu byte lanjutan lagi diambil, supaya karakter UTF-8 tidak terbelah.
        } // akhir while byte lanjutan
        width = width + 1; // width = width + 1 artinya satu karakter utuh (satu kolom) sudah diambil.
    } // akhir while pengambilan karakter
    return index; // return index; mengembalikan banyaknya byte yang muat dalam maxWidth kolom.
} // akhir fungsi prefixBytesForWidth()

// truncateText() dipakai untuk judul kotak yang terlalu panjang. Tanda ".." (dua titik) dipakai sebagai penanda bahwa
// teks dipotong, dan memakan dua kolom; karena itu isi yang tersisa paling lebar maxWidth - 2 kolom.
std::string truncateText(const std::string& text, std::size_t maxWidth) { // truncateText(text, maxWidth) berfungsi untuk memendekkan text supaya muat maxWidth kolom.
    if (displayWidth(text) <= maxWidth) { // displayWidth(text) <= maxWidth artinya text sudah muat, jadi tidak perlu dipotong.
        return text; // return text; mengembalikan text utuh.
    } // akhir if teks sudah muat
    if (maxWidth <= 2) { // maxWidth <= 2 artinya ruangnya terlalu sempit untuk isi ditambah "..", jadi teks dipotong tanpa penanda.
        return text.substr(0, prefixBytesForWidth(text, maxWidth)); // text.substr(0, ...) artinya ambil potongan dari byte ke-0 sebanyak byte yang muat maxWidth kolom.
    } // akhir if ruang sangat sempit
    return text.substr(0, prefixBytesForWidth(text, maxWidth - 2)) + ".."; // potongan selebar maxWidth - 2 kolom ditambah "..", jadi lebarnya tepat maxWidth.
} // akhir fungsi truncateText()

// toLowerAscii() hanya mengubah huruf A-Z dan tidak bergantung pada pengaturan bahasa sistem (berbeda dengan std::tolower
// dari <cctype>), jadi hasilnya sama di semua komputer. Itu cukup, karena nama komponen dan nama berkas yang diterima
// program ini memang dibatasi pada karakter ASCII. Hasilnya dipakai untuk membandingkan teks tanpa membedakan huruf
// besar/kecil: nama komponen kembar, jawaban y/t, dan nama berkas laporan.
std::string toLowerAscii(const std::string& text) { // toLowerAscii(text) berfungsi untuk menghasilkan salinan text dengan huruf A-Z diganti a-z.
    std::string result = text; // result = text artinya text disalin dulu, karena text adalah const & sehingga tidak boleh diubah langsung.
    for (std::size_t i = 0; i < result.size(); ++i) { // for (i = 0; i < result.size(); ++i) berfungsi untuk memeriksa setiap karakter result.
        if (result[i] >= 'A' && result[i] <= 'Z') { // result[i] >= 'A' && result[i] <= 'Z' artinya karakter ini huruf besar; huruf A-Z berurutan di tabel ASCII.
            result[i] = static_cast<char>(result[i] - 'A' + 'a'); // result[i] - 'A' + 'a' artinya posisi huruf dalam abjad (0 untuk A) ditambahkan ke 'a'.
            // Hasil hitungan antar-char berupa int, jadi static_cast<char> mengembalikannya menjadi char sebelum disimpan.
        } // akhir if huruf besar
    } // akhir perulangan for setiap karakter
    return result; // return result; mengembalikan salinan yang sudah huruf kecil.
} // akhir fungsi toLowerAscii()

// wrapText() melipat teks per kata, mirip pengolah kata: kata dimasukkan ke baris yang sedang disusun selama masih muat,
// dan bila tidak muat baris itu ditutup lalu baris baru dimulai. Hasilnya dikumpulkan dalam std::vector karena banyaknya
// baris baru diketahui setelah selesai melipat. Parameter maxWidth dan hangingIndent dikirim sebagai salinan (bukan
// referensi), jadi boleh dikoreksi di dalam fungsi tanpa mengubah variabel milik pemanggil.
std::vector<std::string> wrapText(const std::string& text, std::size_t maxWidth, std::size_t hangingIndent) { // wrapText(...) berfungsi untuk melipat text
    // menjadi baris-baris selebar paling banyak maxWidth kolom, dengan baris lanjutan menjorok hangingIndent spasi.
    std::vector<std::string> lines; // lines adalah daftar baris hasil lipatan; awalnya kosong lalu diisi dengan push_back().
    if (maxWidth == 0) { // maxWidth == 0 artinya tidak ada ruang sama sekali; nilai ini tidak masuk akal untuk melipat.
        maxWidth = 1; // maxWidth = 1 artinya lebar dinaikkan ke satu kolom, supaya pemotongan kata di bawah selalu maju dan tidak berulang tanpa henti.
    } // akhir if lebar nol
    // Teks yang sudah muat dikembalikan apa adanya, termasuk spasi perata di dalamnya.
    if (displayWidth(text) <= maxWidth) { // displayWidth(text) <= maxWidth artinya seluruh text muat dalam satu baris.
        lines.push_back(text); // lines.push_back(text) artinya text dimasukkan utuh sebagai satu-satunya baris.
        return lines; // return lines; langsung mengembalikan daftar berisi satu baris itu.
    } // akhir if teks sudah muat
    // Indentasi harus menyisakan paling sedikit satu kolom untuk isi.
    if (hangingIndent + 1 > maxWidth) { // hangingIndent + 1 > maxWidth artinya setelah menjorok tidak tersisa satu kolom pun untuk kata.
        hangingIndent = 0; // hangingIndent = 0 artinya baris lanjutan tidak dibuat menjorok.
    } // akhir if indentasi terlalu besar

    // Spasi di awal teks dipertahankan sebagai indentasi baris pertama.
    std::size_t position = 0; // position adalah posisi byte di text yang sedang dibaca; maju terus dari kiri ke kanan.
    while (position < text.size() && text[position] == ' ') { // while (...) berfungsi untuk melewati spasi di awal text sambil menghitungnya.
        position = position + 1; // position = position + 1 artinya satu spasi awal dilewati.
    } // akhir while spasi awal
    std::size_t firstIndent = position; // firstIndent adalah banyaknya spasi di awal text, yang menjadi indentasi baris pertama.
    if (firstIndent + 1 > maxWidth) { // firstIndent + 1 > maxWidth artinya indentasi awal tidak menyisakan ruang untuk isi.
        firstIndent = 0; // firstIndent = 0 artinya baris pertama dimulai tanpa indentasi.
    } // akhir if indentasi awal terlalu besar

    // Tiga variabel di bawah adalah "keadaan" baris yang sedang disusun.
    std::string current(firstIndent, ' '); // current adalah isi baris yang sedang disusun, diawali firstIndent buah spasi.
    std::size_t currentWidth = firstIndent; // currentWidth adalah lebar current dalam kolom; disimpan terpisah supaya tidak perlu menghitung ulang.
    bool lineHasWord = false; // lineHasWord bernilai true bila current sudah berisi paling sedikit satu kata (bukan hanya indentasi).

    while (position < text.size()) { // while (position < text.size()) berfungsi untuk memproses kata demi kata sampai seluruh text terbaca.
        // Hitung spasi pemisah di depan kata, lalu ambil satu kata (deret karakter bukan spasi).
        std::size_t spaces = 0; // spaces adalah banyaknya spasi di antara kata sebelumnya dan kata berikutnya.
        while (position < text.size() && text[position] == ' ') { // while (...) berfungsi untuk melewati spasi pemisah sambil menghitungnya.
            position = position + 1; // position = position + 1 artinya satu spasi dilewati.
            spaces = spaces + 1; // spaces = spaces + 1 artinya spasi itu dihitung, supaya jarak asli bisa ditiru.
        } // akhir while spasi pemisah
        std::size_t wordEnd = position; // wordEnd mula-mula sama dengan awal kata, lalu digeser sampai ujung kata.
        while (wordEnd < text.size() && text[wordEnd] != ' ') { // while (...) berfungsi untuk mencari ujung kata, yaitu spasi berikutnya atau akhir text.
            wordEnd = wordEnd + 1; // wordEnd = wordEnd + 1 artinya satu karakter kata dilewati.
        } // akhir while pencarian ujung kata
        // break berfungsi untuk keluar dari perulangan saat itu juga, walaupun syarat perulangannya masih benar.
        if (wordEnd == position) { // wordEnd == position artinya tidak ada kata lagi; yang tersisa di ujung text hanya spasi.
            break; // break; keluar dari perulangan while karena tidak ada kata yang perlu dilipat.
        } // akhir if tidak ada kata lagi
        std::string word = text.substr(position, wordEnd - position); // word = text.substr(...) artinya kata diambil dari position sepanjang wordEnd - position byte.
        position = wordEnd; // position = wordEnd artinya pembacaan dilanjutkan dari ujung kata ini.

        // Di tengah baris, spasi asli dipertahankan apa adanya (termasuk spasi perata seperti
        // pada "Nama   : isi"); di awal baris tidak ada spasi yang ditambahkan.
        // operator ?: berfungsi untuk memilih salah satu dari dua nilai: syarat ? nilaiBilaBenar : nilaiBilaSalah.
        const std::size_t gap = lineHasWord ? spaces : 0; // gap artinya jarak sebelum kata = spaces bila baris sudah berisi kata, 0 bila belum.
        if (currentWidth + gap + displayWidth(word) <= maxWidth) { // currentWidth + gap + lebar kata <= maxWidth artinya kata masih muat di baris ini.
            // Kata masih muat di baris yang sedang disusun.
            current += std::string(gap, ' '); // current += std::string(gap, ' ') artinya spasi pemisah ditambahkan sebanyak gap.
            current += word; // current += word artinya kata disambungkan ke baris.
            currentWidth = currentWidth + gap + displayWidth(word); // currentWidth diperbarui dengan lebar spasi dan kata yang baru ditambahkan.
            lineHasWord = true; // lineHasWord = true artinya baris ini kini sudah berisi kata.
            continue; // continue; langsung lanjut ke kata berikutnya, karena bagian di bawah hanya untuk kata yang tidak muat.
        } // akhir if kata muat

        // Kata tidak muat: baris yang sedang disusun ditutup dan baris baru dimulai menjorok.
        if (lineHasWord) { // lineHasWord artinya baris sekarang sudah berisi kata, jadi baris itu bisa ditutup.
            lines.push_back(current); // lines.push_back(current) artinya baris yang sudah jadi disimpan ke daftar hasil.
            current.assign(hangingIndent, ' '); // current.assign(hangingIndent, ' ') artinya isi current diganti dengan hangingIndent buah spasi untuk baris baru.
            currentWidth = hangingIndent; // currentWidth = hangingIndent artinya lebar baris baru mula-mula selebar indentasinya.
            lineHasWord = false; // lineHasWord = false artinya baris baru belum berisi kata.
        } // akhir if menutup baris
        // Kata yang lebih lebar dari satu baris penuh (misalnya alamat web) dipotong paksa.
        // Di titik ini currentWidth selalu lebih kecil dari maxWidth, jadi maxWidth - currentWidth (sisa ruang) tidak negatif.
        while (displayWidth(word) > maxWidth - currentWidth) { // displayWidth(word) > maxWidth - currentWidth artinya kata masih lebih lebar dari sisa ruang baris.
            const std::size_t bytes = prefixBytesForWidth(word, maxWidth - currentWidth); // bytes adalah banyaknya byte awal kata yang tepat mengisi sisa ruang.
            lines.push_back(current + word.substr(0, bytes)); // potongan awal kata disambung ke baris lalu baris itu disimpan.
            word.erase(0, bytes); // word.erase(0, bytes) artinya potongan yang sudah ditulis dibuang dari word, sisanya diproses lagi.
            current.assign(hangingIndent, ' '); // current.assign(hangingIndent, ' ') artinya baris baru dimulai dengan indentasi lanjutan.
            currentWidth = hangingIndent; // currentWidth = hangingIndent artinya lebar baris baru diatur ulang.
        } // akhir while pemotongan kata panjang
        if (!word.empty()) { // !word.empty() artinya masih ada sisa kata (yang kini pasti muat) untuk ditulis.
            current += word; // current += word artinya sisa kata ditaruh di baris baru.
            currentWidth = currentWidth + displayWidth(word); // currentWidth bertambah selebar sisa kata itu.
            lineHasWord = true; // lineHasWord = true artinya baris baru sudah berisi kata.
        } // akhir if sisa kata
    } // akhir while pemrosesan kata

    if (lineHasWord || lines.empty()) { // lineHasWord || lines.empty() artinya baris terakhir berisi kata, atau belum ada baris sama sekali.
        // Syarat lines.empty() menjamin hasilnya tidak pernah kosong, sehingga pemanggil selalu menerima paling sedikit satu baris.
        lines.push_back(current); // lines.push_back(current) artinya baris terakhir yang masih disusun ikut disimpan.
    } // akhir if baris terakhir
    return lines; // return lines; mengembalikan semua baris hasil lipatan.
} // akhir fungsi wrapText()

// ===============================================================================================================
// BAGIAN 16 - src/input_parse.cpp - pemeriksaan dan pengubahan teks ketikan menjadi data
// ===============================================================================================================
// Berkas ini berisi isi (definisi) fungsi-fungsi yang dideklarasikan di input_parse.h. Semua
// fungsinya bekerja pada teks yang sudah dibaca prompt.cpp, jadi di sini
// tidak ada std::cin sama sekali.

// Angka dengan lebih dari sembilan digit pasti di luar semua rentang program ini (batas terbesar,
// harga Rp100.000.000, hanya sembilan digit). Batas ini diperiksa sebelum teks diubah menjadi
// bilangan supaya hasilnya tidak pernah melampaui daya tampung int (sekitar 2,1 miliar, sepuluh
// digit). Tiga konstanta pertama di blok ini bertipe std::size_t karena dibandingkan dengan size() milik
// std::string, yang juga std::size_t, sehingga perbandingannya tidak mencampur tipe bertanda dan
// tak bertanda. REPORT_EXTENSION di bawahnya hanya teks akhiran, jadi bukan std::size_t.
static const std::size_t MAX_SAFE_DIGITS = 9; // MAX_SAFE_DIGITS = 9 adalah batas banyak digit yang boleh diubah menjadi bilangan; 999.999.999 masih muat di int
static const std::size_t MAX_WEIGHT_DECIMALS = 3; // MAX_WEIGHT_DECIMALS = 3 artinya berat paling banyak 3 angka di belakang koma, yaitu ketelitian gram (0,001 kg)
static const std::size_t MAX_FILE_NAME_LENGTH = 60; // MAX_FILE_NAME_LENGTH = 60 adalah panjang maksimal nama berkas yang diketik (belum termasuk .txt tambahan)
static const char* const REPORT_EXTENSION = ".txt"; // REPORT_EXTENSION = ".txt" adalah akhiran yang ditambahkan ke nama berkas laporan bila belum ada

// Nama perangkat yang dicadangkan Windows: berkas bernama CON.txt atau NUL.txt tidak bisa dibuat,
// karena Windows menganggap nama itu sebagai perangkat (misalnya CON adalah layar/keyboard).
// Disimpan dalam array teks supaya pemeriksaannya cukup satu perulangan, bukan 22 perbandingan
// yang ditulis satu per satu. Semua ditulis huruf kecil karena nama ketikan pengguna juga diubah
// ke huruf kecil dulu sebelum dibandingkan (Windows tidak membedakan CON dan con).
static const char* const RESERVED_FILE_NAMES[] = { // RESERVED_FILE_NAMES[] adalah array berisi 22 teks tetap; ukurannya tidak ditulis karena compiler menghitungnya dari isi
    "con", "prn", "aux", "nul", // "con", "prn", "aux", "nul" adalah perangkat konsol, printer, port tambahan, dan perangkat kosong
    "com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8", "com9", // "com1" sampai "com9" adalah nama port serial
    "lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9" // "lpt1" sampai "lpt9" adalah nama port printer paralel; elemen terakhir tanpa koma
}; // akhir array RESERVED_FILE_NAMES
static const std::size_t RESERVED_FILE_NAME_COUNT = sizeof(RESERVED_FILE_NAMES) / sizeof(RESERVED_FILE_NAMES[0]); // RESERVED_FILE_NAME_COUNT adalah banyaknya elemen array
// di atas (22), dihitung otomatis dari ukuran seluruh array dibagi ukuran satu elemen, jadi tetap benar bila daftarnya ditambah.

// Empat fungsi kecil berikut memeriksa jenis satu karakter. Ditulis sendiri dengan perbandingan
// langsung, sehingga hanya karakter ASCII biasa yang dianggap huruf atau angka, apa pun pengaturan
// bahasa komputernya. Keempatnya hanya fungsi bantu di berkas ini dan tidak dideklarasikan di
// input_parse.h.
static bool isBlankChar(char character) { // isBlankChar(character) berfungsi untuk menentukan apakah character termasuk karakter kosong
    return character == ' ' || character == '\t' || character == '\r' || character == '\n'; // ' ' spasi, '\t' tab, '\r' dan '\n' tanda akhir baris;
    // '\r' ikut dihitung karena teks bergaya Windows berakhiran \r\n, sehingga \r bisa tertinggal di ujung ketikan.
} // akhir fungsi isBlankChar()

static bool isAsciiDigit(char character) { // isAsciiDigit(character) berfungsi untuk menentukan apakah character adalah angka 0-9
    return character >= '0' && character <= '9'; // '0' sampai '9' punya kode berurutan di tabel ASCII (48 sampai 57), jadi cukup diperiksa rentangnya
} // akhir fungsi isAsciiDigit()

static bool isAsciiLetter(char character) { // isAsciiLetter(character) berfungsi untuk menentukan apakah character adalah huruf A-Z atau a-z (tanpa aksen)
    return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'); // huruf besar dan huruf kecil diperiksa sebagai dua rentang terpisah,
    // karena di tabel ASCII keduanya tidak bersambung (di antaranya ada tanda seperti [ dan _).
} // akhir fungsi isAsciiLetter()

// isOneOf() dipakai untuk tanda baca yang diizinkan: daftarnya cukup ditulis sebagai satu teks
// (misalnya "-_.") daripada menulis banyak perbandingan character == '-' || character == '_' ...
static bool isOneOf(char character, const std::string& allowed) { // isOneOf(character, allowed) menghasilkan true bila character ada di dalam teks allowed
    // std::string::npos adalah nilai khusus yang dikembalikan find() bila yang dicari tidak ditemukan.
    return allowed.find(character) != std::string::npos; // allowed.find(character) != std::string::npos artinya character ditemukan di suatu posisi di allowed
} // akhir fungsi isOneOf()

// Membuang angka 0 di depan ("007" menjadi "7") tetapi menyisakan satu digit ("000" menjadi "0").
// Angka 0 di depan tidak mengubah nilai, jadi dibuang dulu sebelum digit dihitung; dengan begitu
// "0000000024" (sepuluh digit) tetap dianggap 24 dan tidak ditolak oleh batas MAX_SAFE_DIGITS.
static std::string stripLeadingZeros(const std::string& digits) { // stripLeadingZeros(digits) mengembalikan teks digits tanpa angka 0 di depannya
    std::size_t start = 0; // start adalah posisi digit pertama yang akan disimpan; mulai dari posisi 0 (karakter paling kiri)
    while (start + 1 < digits.size() && digits[start] == '0') { // start + 1 < digits.size() menjaga digit terakhir tidak ikut dibuang;
        // digits[start] == '0' artinya digit di posisi start adalah nol.
        start = start + 1; // start = start + 1 berfungsi untuk bergeser satu digit ke kanan, melewati angka 0 tadi
    } // akhir perulangan while
    return digits.substr(start); // digits.substr(start) mengambil potongan teks mulai posisi start sampai ujung
} // akhir fungsi stripLeadingZeros()

// Mengubah deret digit menjadi bilangan: tiap digit menggeser nilai lama satu tempat (x 10)
// lalu ditambahkan. Contoh "185": 0 x 10 + 1 = 1, lalu 1 x 10 + 8 = 18, lalu 18 x 10 + 5 = 185.
// Pemanggil sudah memastikan teksnya hanya digit dan tidak terlalu panjang (paling banyak 9 digit).
// Hasilnya double karena berat dan harga memang disimpan sebagai double (weightKg, pricePerKg) dan
// dibandingkan dengan batas bertipe double; bilangan bulat 9 digit masih disimpan double dengan tepat.
static double digitsToNumber(const std::string& digits) { // digitsToNumber(digits) mengembalikan nilai bilangan dari teks yang seluruhnya digit
    double value = 0.0; // value = 0.0 adalah hasil sementara; mulai dari nol sebelum ada digit yang dibaca
    for (std::size_t i = 0; i < digits.size(); ++i) { // perulangan for membaca digit satu per satu dari kiri (i = 0) sampai digit terakhir
        // digits[i] - '0' adalah cara mengubah karakter angka menjadi nilainya: kode '7' (55) dikurangi kode '0' (48) hasilnya 7.
        value = value * 10.0 + static_cast<double>(digits[i] - '0'); // value * 10.0 menggeser nilai lama satu tempat ke kiri, lalu nilai digit baru ditambahkan
    } // akhir perulangan for
    return value; // return value mengembalikan bilangan yang sudah tersusun lengkap
} // akhir fungsi digitsToNumber()

// trimSpaces() mencari posisi karakter bukan-kosong pertama dari kiri (begin) dan dari kanan (end),
// lalu mengambil potongan di antaranya. Cara ini hanya sekali menyalin teks.
std::string trimSpaces(const std::string& text) { // trimSpaces(text) mengembalikan text tanpa karakter kosong di awal dan akhirnya
    std::size_t begin = 0; // begin adalah posisi awal potongan; mulai dari karakter pertama (posisi 0)
    std::size_t end = text.size(); // end adalah posisi SESUDAH karakter terakhir potongan; mulai dari panjang teks (tepat di luar ujung)
    while (begin < end && isBlankChar(text[begin])) { // selama karakter di posisi begin kosong (dan begin belum bertemu end), lewati karakter itu
        begin = begin + 1; // begin = begin + 1 berfungsi untuk maju satu karakter ke kanan
    } // akhir perulangan while dari kiri
    while (end > begin && isBlankChar(text[end - 1])) { // text[end - 1] adalah karakter terakhir potongan; bila kosong, potongan dipendekkan dari kanan
        end = end - 1; // end = end - 1 berfungsi untuk mundur satu karakter ke kiri
    } // akhir perulangan while dari kanan
    return text.substr(begin, end - begin); // text.substr(begin, end - begin) mengambil end - begin karakter mulai posisi begin; teks yang seluruhnya spasi menjadi kosong
} // akhir fungsi trimSpaces()

// collapseSpaces() menyusun teks baru karakter demi karakter. Deret karakter kosong tidak langsung
// ditulis, tetapi hanya dicatat (pendingSpace); satu spasi baru ditulis ketika muncul karakter
// lain sesudahnya. Dengan begitu spasi di awal, spasi ganda, dan spasi di akhir hilang sekaligus.
std::string collapseSpaces(const std::string& text) { // collapseSpaces(text) mengembalikan text yang sudah dirapikan spasinya
    std::string result; // result adalah teks hasil; std::string yang baru dibuat isinya kosong
    bool pendingSpace = false; // pendingSpace bernilai true bila ada spasi yang masih "ditahan" dan perlu ditulis sebelum karakter berikutnya
    for (std::size_t i = 0; i < text.size(); ++i) { // perulangan for memeriksa setiap karakter text dari kiri ke kanan
        if (isBlankChar(text[i])) { // isBlankChar(text[i]) artinya karakter ini spasi, tab, atau tanda akhir baris
            // Spasi baru ditulis nanti, hanya bila masih ada karakter lain sesudahnya.
            pendingSpace = !result.empty(); // !result.empty() bernilai false selama result masih kosong, jadi spasi di awal teks tidak pernah ditahan
        } else { // else: karakter ini bukan karakter kosong
            if (pendingSpace) { // pendingSpace artinya sebelumnya ada spasi yang ditahan
                result += ' '; // result += ' ' menulis tepat satu spasi, berapa pun panjang deret spasi aslinya
            } // akhir if pendingSpace
            result += text[i]; // result += text[i] menyalin karakter ini ke hasil
            pendingSpace = false; // pendingSpace = false karena spasi yang ditahan sudah ditulis
        } // akhir if-else karakter kosong
    } // akhir perulangan for
    return result; // return result; spasi yang masih ditahan di ujung tidak pernah ditulis, jadi spasi di akhir teks ikut hilang
} // akhir fungsi collapseSpaces()

bool isAllDigits(const std::string& text) { // isAllDigits(text) menghasilkan true hanya bila text tidak kosong dan semua karakternya angka 0-9
    if (text.empty()) { // text.empty() artinya teks tidak berisi karakter apa pun
        return false; // return false karena teks kosong tidak bisa dianggap bilangan
    } // akhir if teks kosong
    for (std::size_t i = 0; i < text.size(); ++i) { // perulangan for memeriksa setiap karakter text
        if (!isAsciiDigit(text[i])) { // !isAsciiDigit(text[i]) artinya karakter ini bukan angka
            return false; // return false langsung: satu karakter bukan angka sudah cukup untuk menolak, sisanya tidak perlu diperiksa
        } // akhir if bukan angka
    } // akhir perulangan for
    return true; // return true karena semua karakter sudah lolos pemeriksaan
} // akhir fungsi isAllDigits()

// Urutan pemeriksaan nama: kosong, karakter pertama, seluruh karakter, lalu panjangnya.
// Panjang sengaja diperiksa paling akhir: sesudah semua karakter dipastikan huruf, angka, spasi, atau tanda yang diizinkan
// (semuanya satu byte), name.size() pasti sama dengan banyaknya karakter, sehingga angka di pesan "terlalu panjang" selalu tepat.
// Pemanggil (readComponentName() di app.cpp) sudah merapikan spasinya dengan collapseSpaces().
std::string checkComponentName(const std::string& name) { // checkComponentName(name) mengembalikan "" bila nama sah, atau pesan kesalahan bila tidak
    if (name.empty()) { // name.empty() artinya pengguna belum mengetik nama
        return "Nama komponen belum diisi."; // pesan untuk nama kosong
    } // akhir if nama kosong
    if (!isAsciiLetter(name[0]) && !isAsciiDigit(name[0])) { // name[0] adalah karakter pertama; aman diambil karena nama sudah pasti tidak kosong
        return "Nama harus diawali huruf atau angka."; // pesan bila nama diawali tanda baca atau spasi
    } // akhir if karakter pertama
    for (std::size_t i = 0; i < name.size(); ++i) { // perulangan for memeriksa setiap karakter nama
        const char character = name[i]; // character adalah salinan karakter ke-i; disimpan supaya kondisi di bawah lebih pendek dibaca
        if (!isAsciiLetter(character) && !isAsciiDigit(character) && !isOneOf(character, " -_.,/()+#&")) { // karakter ditolak bila bukan huruf, bukan angka,
            // dan tidak ada di daftar " -_.,/()+#&" (spasi dan sepuluh tanda baca yang diizinkan).
            return "Nama hanya boleh berisi huruf A-Z, angka, spasi, dan tanda - _ . , / ( ) + # &"; // pesan yang sekaligus menyebut semua karakter yang boleh
        } // akhir if karakter terlarang
    } // akhir perulangan for
    if (name.size() > static_cast<std::size_t>(MAX_NAME_LENGTH)) { // MAX_NAME_LENGTH (30, bertipe int) diubah ke std::size_t supaya sama tipenya dengan name.size()
        return "Nama terlalu panjang (" + formatThousands(static_cast<double>(name.size())) + " karakter); paling banyak " // panjang nama ikut disebut di pesan;
        // static_cast<double> diperlukan karena formatThousands() menerima double.
            + formatThousands(MAX_NAME_LENGTH) + " karakter."; // formatThousands(MAX_NAME_LENGTH) menulis batasnya (30) dari konstanta, bukan angka yang diketik ulang
    } // akhir if nama terlalu panjang
    return ""; // return "" (teks kosong) artinya nama sah
} // akhir fungsi checkComponentName()

// Jumlah diperiksa dua tahap: bentuknya dulu (hanya digit), baru nilainya (rentang 1-1.000.000),
// supaya pesan kesalahannya tepat sasaran.
std::string checkQuantity(const std::string& text, int& quantity) { // checkQuantity(text, quantity) mengembalikan "" dan mengisi quantity bila sah, atau pesan bila tidak
    if (text.empty()) { // text.empty() artinya pengguna hanya menekan Enter
        return "Jumlah belum diisi."; // pesan untuk jumlah kosong
    } // akhir if kosong
    if (!isAllDigits(text)) { // !isAllDigits(text) artinya ada karakter selain angka (titik, koma, minus, huruf, spasi)
        return "Jumlah harus bilangan bulat tanpa titik, koma, spasi, atau huruf. Contoh: 24"; // pesan bentuk yang salah beserta contoh yang benar
    } // akhir if bukan angka
    const std::string rangeMessage = "Jumlah harus antara " + formatThousands(MIN_QUANTITY) + " dan " // rangeMessage adalah pesan rentang; disusun sekali karena
        // dipakai di dua tempat di bawah (terlalu banyak digit, atau nilainya di luar rentang).
        + formatThousands(MAX_QUANTITY) + " unit."; // angka batas diambil dari MIN_QUANTITY dan MAX_QUANTITY di component.h
    const std::string digits = stripLeadingZeros(text); // digits adalah ketikan tanpa angka 0 di depan, supaya panjangnya mencerminkan besar angkanya
    if (digits.size() > MAX_SAFE_DIGITS) { // digits.size() > MAX_SAFE_DIGITS artinya angkanya lebih dari 9 digit, pasti melebihi 1.000.000
        return rangeMessage; // ditolak sebelum diubah menjadi bilangan
    } // akhir if terlalu banyak digit
    const double value = digitsToNumber(digits); // value adalah nilai bilangan dari ketikan
    if (value < MIN_QUANTITY || value > MAX_QUANTITY) { // value < MIN_QUANTITY || value > MAX_QUANTITY artinya di bawah 1 (yaitu 0) atau di atas 1.000.000
        return rangeMessage; // pesan rentang
    } // akhir if di luar rentang
    quantity = static_cast<int>(value); // static_cast<int>(value) mengubah value ke int; aman karena value pasti bulat dan di dalam rentang
    return ""; // "" artinya sah, dan quantity sudah terisi
} // akhir fungsi checkQuantity()

// Berat boleh ditulis dengan koma atau titik desimal ("1,85" atau "1.85"), karena orang Indonesia
// biasa memakai koma sedangkan kalkulator dan komputer memakai titik.
std::string checkUnitWeight(const std::string& text, double& weightKg) { // checkUnitWeight(...) mengembalikan "" dan mengisi weightKg bila sah, atau pesan bila tidak
    if (text.empty()) { // text.empty() artinya berat belum diketik
        return "Berat per unit belum diisi."; // pesan untuk berat kosong
    } // akhir if kosong
    const std::string formatMessage = // formatMessage adalah pesan untuk bentuk ketikan yang salah; disimpan karena dipakai di dua tempat
        "Berat ditulis dengan angka; desimal boleh memakai koma atau titik. Contoh: 1,85"; // isi pesan beserta contoh yang benar

    // Teks dipecah di tanda desimal pertama: bagian utuh di kiri, bagian pecahan di kanan.
    std::string wholePart = text; // wholePart adalah bagian utuh (kilogram); mula-mula seluruh teks, untuk ketikan tanpa tanda desimal
    std::string fractionPart; // fractionPart adalah bagian pecahan (di belakang koma); mula-mula kosong
    // find_first_of(".,") adalah fungsi std::string yang mencari posisi pertama dari SALAH SATU karakter di dalam ".,".
    const std::size_t separator = text.find_first_of(".,"); // separator adalah posisi titik atau koma pertama, atau npos bila tidak ada
    if (separator != std::string::npos) { // separator != std::string::npos artinya ketikan memang memakai tanda desimal
        wholePart = text.substr(0, separator); // text.substr(0, separator) mengambil semua karakter sebelum tanda desimal
        fractionPart = text.substr(separator + 1); // text.substr(separator + 1) mengambil semua karakter sesudah tanda desimal
        if (!isAllDigits(fractionPart)) { // pecahan harus angka semua; ini juga menolak "1," (pecahan kosong) dan "1,2,3" (tanda desimal kedua)
            return formatMessage; // pesan bentuk yang salah
        } // akhir if pecahan bukan angka
    } // akhir if ada tanda desimal
    if (!isAllDigits(wholePart)) { // bagian utuh juga harus angka semua; ini menolak huruf, minus, dan ",5" (bagian utuh kosong)
        return formatMessage; // pesan bentuk yang salah
    } // akhir if bagian utuh bukan angka
    if (fractionPart.size() > MAX_WEIGHT_DECIMALS) { // fractionPart.size() > MAX_WEIGHT_DECIMALS artinya lebih dari 3 angka di belakang koma
        return "Berat paling teliti sampai gram, jadi paling banyak " + formatThousands(static_cast<double>(MAX_WEIGHT_DECIMALS)) // pesan ketelitian;
        // angka 3 ditulis dari konstanta MAX_WEIGHT_DECIMALS (static_cast<double> karena formatThousands() menerima double), jadi pesan
        // dan aturan tidak bisa berbeda.
            + " angka di belakang koma. Contoh: 0,125"; // sambungan pesan beserta contoh berat yang sah (125 gram)
    } // akhir if terlalu banyak desimal

    const std::string rangeMessage = "Berat per unit harus antara " + formatWeight(MIN_UNIT_WEIGHT_KG) + " dan " // rangeMessage adalah pesan rentang berat;
        // formatWeight() menulis batasnya dengan tiga desimal: "0,001 kg" dan "10.000,000 kg".
        + formatWeight(MAX_UNIT_WEIGHT_KG) + "."; // batas atas diambil dari MAX_UNIT_WEIGHT_KG di component.h
    wholePart = stripLeadingZeros(wholePart); // angka 0 di depan bagian utuh dibuang supaya panjangnya mencerminkan besar angkanya
    if (wholePart.size() > MAX_SAFE_DIGITS) { // wholePart.size() > MAX_SAFE_DIGITS artinya bagian utuhnya lebih dari 9 digit, pasti di atas 10.000 kg
        return rangeMessage; // pesan rentang
    } // akhir if terlalu banyak digit
    // Bagian pecahan dilengkapi sampai tiga digit ("85" menjadi "850"), sehingga isinya gram.
    // Nilai disusun dalam gram (bilangan bulat) lalu dibagi 1000 sekali saja, supaya hasilnya
    // adalah nilai kilogram yang paling dekat dengan ketikan pengguna.
    while (fractionPart.size() < MAX_WEIGHT_DECIMALS) { // selama pecahan kurang dari 3 digit, tambahkan 0 di kanannya (nilainya tidak berubah: 0,85 = 0,850)
        fractionPart += '0'; // fractionPart += '0' menambah satu angka 0 di ujung kanan
    } // akhir perulangan while
    const double value = (digitsToNumber(wholePart) * 1000.0 + digitsToNumber(fractionPart)) / 1000.0; // value adalah berat dalam kg; contoh "1,85":
    // (1 x 1000 + 850) / 1000 = 1850 gram / 1000 = 1,85 kg.
    if (value < MIN_UNIT_WEIGHT_KG || value > MAX_UNIT_WEIGHT_KG) { // value di luar 0,001 sampai 10.000 kg (misalnya "0" atau "0,000") ditolak
        return rangeMessage; // pesan rentang
    } // akhir if di luar rentang
    weightKg = value; // weightKg = value; parameter keluaran baru diisi setelah semua pemeriksaan lolos
    return ""; // "" artinya sah
} // akhir fungsi checkUnitWeight()

// Harga diperiksa dengan pola yang sama seperti jumlah: bentuk dulu, baru rentang. Harga hanya
// menerima rupiah bulat, jadi "18.500" atau "Rp18500" ditolak dengan pesan yang menjelaskan sebabnya.
std::string checkPricePerKg(const std::string& text, double& pricePerKg) { // checkPricePerKg(...) mengembalikan "" dan mengisi pricePerKg bila sah, atau pesan bila tidak
    if (text.empty()) { // text.empty() artinya harga belum diketik
        return "Harga per kg belum diisi."; // pesan untuk harga kosong
    } // akhir if kosong
    if (!isAllDigits(text)) { // !isAllDigits(text) artinya ada karakter selain angka, misalnya "Rp", titik ribuan, atau koma
        return "Harga ditulis dalam rupiah bulat tanpa Rp, titik, koma, atau spasi. Contoh: 18500"; // pesan bentuk yang salah beserta contoh
    } // akhir if bukan angka
    const std::string rangeMessage = "Harga per kg harus antara " + formatRupiah(MIN_PRICE_PER_KG) + " dan " // rangeMessage adalah pesan rentang harga;
        // formatRupiah() menulis batasnya sebagai "Rp1" dan "Rp100.000.000".
        + formatRupiah(MAX_PRICE_PER_KG) + "."; // batas atas diambil dari MAX_PRICE_PER_KG di component.h
    const std::string digits = stripLeadingZeros(text); // digits adalah ketikan tanpa angka 0 di depan
    if (digits.size() > MAX_SAFE_DIGITS) { // lebih dari 9 digit pasti di atas Rp100.000.000
        return rangeMessage; // pesan rentang
    } // akhir if terlalu banyak digit
    const double value = digitsToNumber(digits); // value adalah nilai harga dari ketikan
    if (value < MIN_PRICE_PER_KG || value > MAX_PRICE_PER_KG) { // value di bawah Rp1 (yaitu 0) atau di atas Rp100.000.000 ditolak
        return rangeMessage; // pesan rentang
    } // akhir if di luar rentang
    pricePerKg = value; // pricePerKg = value; parameter keluaran diisi setelah semua pemeriksaan lolos
    return ""; // "" artinya sah
} // akhir fungsi checkPricePerKg()

// Nama berkas laporan dibatasi ketat (huruf, angka, - _ .) supaya berkas pasti bisa dibuat di
// Windows, macOS, dan Linux, dan supaya laporan tidak tertulis ke folder lain lewat tanda / atau \.
std::string checkReportFileName(const std::string& text, std::string& fileName) { // checkReportFileName(text, fileName) mengembalikan "" dan mengisi fileName bila sah
    if (text.empty()) { // text.empty() artinya nama berkas kosong (app.cpp sudah mengganti Enter kosong dengan nama bawaan, jadi ini jaga-jaga)
        return "Nama berkas belum diisi."; // pesan untuk nama kosong
    } // akhir if kosong
    if (text.size() > MAX_FILE_NAME_LENGTH) { // text.size() > MAX_FILE_NAME_LENGTH artinya nama lebih dari 60 karakter
        return "Nama berkas terlalu panjang; paling banyak " + formatThousands(static_cast<double>(MAX_FILE_NAME_LENGTH)) + " karakter."; // angka 60 di pesan
        // ditulis dari konstanta MAX_FILE_NAME_LENGTH, sehingga pesan selalu sama dengan batas yang diperiksa di atas.
    } // akhir if terlalu panjang
    for (std::size_t i = 0; i < text.size(); ++i) { // perulangan for memeriksa setiap karakter nama berkas
        const char character = text[i]; // character adalah karakter ke-i yang sedang diperiksa
        if (!isAsciiLetter(character) && !isAsciiDigit(character) && !isOneOf(character, "-_.")) { // karakter ditolak bila bukan huruf, bukan angka, dan bukan - _ .
            // Dua teks "..." yang ditulis berurutan digabung compiler menjadi satu (lihat material_catalog.cpp); di sini cara itu
            // dipakai supaya pesan yang panjang bisa dipecah ke dua baris kode.
            return "Nama berkas hanya boleh berisi huruf, angka, tanda hubung (-), garis bawah (_), dan titik, " // bagian pertama pesan
                   "tanpa spasi dan tanpa nama folder."; // bagian kedua pesan, disambung langsung ke bagian pertama
        } // akhir if karakter terlarang
    } // akhir perulangan for
    if (text[0] == '.' || text[text.size() - 1] == '.') { // text[0] karakter pertama, text[text.size() - 1] karakter terakhir; keduanya tidak boleh titik
        return "Nama berkas tidak boleh diawali atau diakhiri titik."; // nama berawalan titik menjadi berkas tersembunyi di macOS/Linux; berakhiran titik dibuang Windows
    } // akhir if titik di ujung

    // Bagian sebelum titik pertama dibandingkan dengan nama perangkat Windows.
    const std::string baseName = toLowerAscii(text.substr(0, text.find('.'))); // baseName adalah nama sebelum titik pertama, dalam huruf kecil; bila tidak ada titik,
    // find() menghasilkan npos dan substr(0, npos) mengambil seluruh teks.
    for (std::size_t i = 0; i < RESERVED_FILE_NAME_COUNT; ++i) { // perulangan for mencocokkan baseName dengan setiap elemen RESERVED_FILE_NAMES
        if (baseName == RESERVED_FILE_NAMES[i]) { // baseName == RESERVED_FILE_NAMES[i] membandingkan isi teks (std::string bisa dibandingkan langsung dengan teks "...")
            return "Nama itu dicadangkan Windows untuk perangkat. Pilih nama lain, misalnya laporan-estima."; // pesan untuk nama perangkat seperti con atau lpt1
        } // akhir if nama dicadangkan
    } // akhir perulangan for

    // Akhiran .txt ditambahkan bila belum ada, supaya berkasnya langsung dikenali penyunting teks.
    const std::string extension = REPORT_EXTENSION; // extension adalah salinan ".txt" sebagai std::string, supaya bisa memakai size() dan dibandingkan
    const std::string lowered = toLowerAscii(text); // lowered adalah nama dalam huruf kecil, supaya akhiran ".TXT" atau ".Txt" juga dikenali
    const bool hasExtension = lowered.size() > extension.size() // hasExtension bernilai true bila nama lebih panjang dari ".txt" (masih ada nama di depannya)
        && lowered.compare(lowered.size() - extension.size(), extension.size(), extension) == 0; // dan 4 karakter terakhirnya sama dengan ".txt";
        // compare(posisi, panjang, teks) menghasilkan 0 bila potongan itu sama persis dengan teks pembanding.
    fileName = hasExtension ? text : text + extension; // bila sudah berakhiran .txt, nama dipakai apa adanya (huruf aslinya); bila belum, ".txt" ditambahkan
    return ""; // "" artinya sah, dan fileName sudah terisi
} // akhir fungsi checkReportFileName()

// parseMenuChoice() memakai langkah yang sama dengan checkQuantity(), tetapi hanya menjawab sah
// atau tidak; batas rentangnya diterima dari pemanggil karena jumlah pilihan tiap menu berbeda-beda.
bool parseMenuChoice(const std::string& text, int minValue, int maxValue, int& choice) { // parseMenuChoice(...) menghasilkan true dan mengisi choice bila nomornya sah
    if (!isAllDigits(text)) { // !isAllDigits(text) artinya bukan angka; teks kosong juga tertolak di sini
        return false; // false artinya pilihan tidak sah
    } // akhir if bukan angka
    const std::string digits = stripLeadingZeros(text); // digits adalah ketikan tanpa angka 0 di depan ("03" menjadi "3")
    if (digits.size() > MAX_SAFE_DIGITS) { // lebih dari 9 digit pasti di luar rentang menu mana pun
        return false; // ditolak sebelum diubah menjadi bilangan
    } // akhir if terlalu banyak digit
    const double value = digitsToNumber(digits); // value adalah nomor yang diketik
    if (value < minValue || value > maxValue) { // value < minValue || value > maxValue artinya nomor di luar daftar pilihan
        return false; // nomor di luar rentang tidak sah
    } // akhir if di luar rentang
    choice = static_cast<int>(value); // choice diisi nomor pilihan sebagai int
    return true; // true artinya pilihan sah
} // akhir fungsi parseMenuChoice()

// parseYesNo() menerima beberapa bentuk jawaban yang lazim (bahasa Indonesia dan Inggris, huruf
// besar atau kecil), karena yang dicari adalah maksud pengguna, bukan ketikan yang persis sama.
YesNoAnswer parseYesNo(const std::string& text) { // parseYesNo(text) mengembalikan ANSWER_YES, ANSWER_NO, atau ANSWER_UNKNOWN
    const std::string answer = toLowerAscii(text); // answer adalah ketikan dalam huruf kecil, jadi "Y", "YA", dan "Yes" ikut dikenali
    if (answer == "y" || answer == "ya" || answer == "yes") { // tiga bentuk jawaban ya
        return ANSWER_YES; // pengguna menjawab ya
    } // akhir if jawaban ya
    if (answer == "t" || answer == "tidak" || answer == "n" || answer == "no") { // empat bentuk jawaban tidak
        return ANSWER_NO; // pengguna menjawab tidak
    } // akhir if jawaban tidak
    return ANSWER_UNKNOWN; // selain itu (termasuk teks kosong) jawabannya tidak dikenal; askYesNo() menangani Enter kosong lebih dulu
} // akhir fungsi parseYesNo()

// ===============================================================================================================
// BAGIAN 17 - src/terminal.cpp - kemampuan terminal: warna, UTF-8, lebar jendela, dan jenis masukan
// ===============================================================================================================
// Berkas ini berisi isi dari lima fungsi yang dijanjikan include/estima/terminal.h. Di sinilah
// program bertanya langsung ke sistem operasi: apakah keluaran berupa terminal, apakah terminal
// mengerti kode warna, berapa lebar jendelanya, dan apakah masukan diketik dari keyboard.

// Windows dan sistem keluarga Unix (macOS, Linux, Android) memakai pustaka yang berbeda untuk
// bertanya ke terminal, jadi pustakanya dipilih saat kompilasi.
// #ifdef NAMA berfungsi untuk bertanya ke compiler apakah NAMA sudah didefinisikan. Bila ya, baris-baris
// sesudahnya sampai #else ikut dikompilasi; bila tidak, yang dikompilasi adalah baris di antara #else dan
// #endif. #endif menutup pilihan itu. Dengan cara ini satu berkas sumber bisa berisi kode khusus Windows
// dan kode khusus Unix sekaligus, dan compiler hanya mengambil bagian yang cocok dengan sistemnya.
#ifdef _WIN32 // _WIN32 adalah nama yang otomatis didefinisikan compiler saat membuat program untuk Windows (32 maupun 64 bit); artinya blok ini khusus Windows
    // windows.h punya kebiasaan membuat makro min/max yang bentrok dengan pustaka standar C++;
    // NOMINMAX mencegahnya, dan WIN32_LEAN_AND_MEAN melewatkan bagian Windows yang tidak dipakai.
    // #ifndef ("if not defined") berfungsi untuk bertanya ke compiler apakah sebuah nama BELUM didefinisikan;
    // bila belum, baris sampai #endif pasangannya ikut dibaca, bila sudah, baris-baris itu dilewati.
    #ifndef NOMINMAX // NOMINMAX diperiksa dulu karena bisa saja sudah didefinisikan sebelumnya (misalnya lewat opsi -DNOMINMAX), jadi tidak didefinisikan dua kali
    #define NOMINMAX 1 // NOMINMAX 1 artinya windows.h diminta tidak membuat makro min dan max, supaya std::min dan std::max tidak rusak
    #endif // akhir pemeriksaan NOMINMAX
    #ifndef WIN32_LEAN_AND_MEAN // WIN32_LEAN_AND_MEAN diperiksa dulu dengan alasan yang sama: hanya didefinisikan bila belum ada
    #define WIN32_LEAN_AND_MEAN 1 // WIN32_LEAN_AND_MEAN 1 artinya windows.h hanya memuat bagian inti, jadi kompilasi lebih cepat dan nama yang ikut masuk lebih sedikit
    #endif // akhir pemeriksaan WIN32_LEAN_AND_MEAN
    #include <windows.h> // <windows.h> adalah pustaka resmi Windows; dari sini dipakai fungsi konsol untuk code page UTF-8, mode warna, dan lebar jendela
    #include <io.h> // <io.h> digunakan untuk _isatty, yaitu pemeriksa apakah sebuah saluran terhubung ke konsol (versi Windows dari isatty)
    #include <stdio.h> // <stdio.h> digunakan untuk stdin (saluran masukan standar) dan _fileno, yang memberi nomor saluran tersebut
    // Header Windows lama (misalnya bawaan Dev-C++ 5) belum mengenal mode ini; nilainya tetap.
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING // ENABLE_VIRTUAL_TERMINAL_PROCESSING hanya didefinisikan sendiri bila windows.h yang dipakai belum menyediakannya
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004 // 0x0004 adalah nilai resmi bendera mode ini; awalan 0x artinya angka ditulis dalam heksadesimal (basis 16), nilainya 4
    #endif // akhir pemeriksaan ENABLE_VIRTUAL_TERMINAL_PROCESSING
#else // #else artinya bukan Windows (macOS, Linux, Android): yang dikompilasi adalah dua include di bawah ini
    #include <sys/ioctl.h> // <sys/ioctl.h> digunakan untuk ioctl, TIOCGWINSZ, dan struct winsize, yaitu cara menanyakan ukuran jendela ke terminal
    #include <unistd.h> // <unistd.h> digunakan untuk isatty serta STDIN_FILENO dan STDOUT_FILENO (nomor saluran masukan dan keluaran standar)
#endif // akhir pemilihan pustaka sesuai sistem operasi

// Keadaan terminal diperiksa sekali di initTerminal() lalu diingat di tiga variabel ini. static membatasi
// variabel ini pada satu unit kompilasi (lihat penjelasan static di calculation.cpp), jadi pada versi modul
// hanya terminal.cpp yang bisa mengubahnya. Bagian lain program cukup membacanya lewat colorsEnabled(),
// unicodeEnabled(), dan inputIsInteractive(). Awalan g_ adalah kebiasaan penamaan untuk menandai variabel
// yang hidup di luar fungsi.
// Nilai awalnya adalah pilihan aman sebelum initTerminal() dipanggil: tanpa warna, garis UTF-8, dan
// masukan dianggap bukan dari keyboard.
static bool g_colorEnabled = false; // g_colorEnabled menyimpan keputusan boleh berwarna atau tidak; awalnya false (tanpa warna)
static bool g_unicodeEnabled = true; // g_unicodeEnabled menyimpan apakah garis kotak memakai karakter UTF-8; awalnya true
static bool g_inputInteractive = false; // g_inputInteractive menyimpan apakah masukan diketik langsung di terminal; awalnya false

// true bila variabel lingkungan name ada dan isinya bukan teks kosong.
// Variabel lingkungan adalah pengaturan berbentuk NAMA=isi yang diwariskan terminal (atau shell) ke
// setiap program yang dijalankannya, misalnya NO_COLOR=1. Fungsi kecil ini dibuat karena pemeriksaan
// yang sama dipakai untuk NO_COLOR dan FORCE_COLOR. Parameternya const char* karena std::getenv
// memang meminta nama dalam bentuk teks gaya C.
static bool hasEnvironmentValue(const char* name) { // hasEnvironmentValue(const char* name) berfungsi untuk memeriksa apakah variabel lingkungan bernama name terisi
    // std::getenv(nama) berfungsi untuk membaca variabel lingkungan. Hasilnya penunjuk ke isi variabel itu
    // (teks gaya C), atau NULL bila variabel tersebut tidak ada sama sekali.
    // NULL adalah nilai penunjuk kosong, artinya penunjuk itu tidak menunjuk ke data apa pun; penunjuk
    // seperti ini tidak boleh dibaca isinya, jadi selalu diperiksa dulu sebelum dipakai.
    const char* value = std::getenv(name); // value adalah penunjuk ke isi variabel lingkungan name, atau NULL bila variabel itu tidak ada
    return value != NULL && value[0] != '\0'; // value != NULL diperiksa lebih dulu; karena && berhenti begitu syarat pertama salah, value[0] hanya dibaca bila
    // penunjuknya sah. value[0] != '\0' artinya karakter pertamanya bukan karakter penutup teks ('\0'), jadi isinya tidak kosong.
} // akhir fungsi hasEnvironmentValue()

// Aturan warna mengikuti kebiasaan program terminal: opsi --no-color dan variabel NO_COLOR
// mematikan warna, FORCE_COLOR (selain "0") memaksanya menyala, dan selebihnya warna hanya
// dipakai bila keluaran benar-benar terminal yang mengerti kode warna. Aturan ini dipisah ke fungsi
// sendiri karena dipakai oleh kedua versi initTerminal() (Windows dan Unix); yang berbeda hanya cara
// mengetahui terminalSupportsColor.
static bool decideColor(bool disableColor, bool terminalSupportsColor) { // decideColor(...) berfungsi untuk memutuskan warna dipakai atau tidak; hasil true berarti berwarna
    if (disableColor || hasEnvironmentValue("NO_COLOR")) { // artinya pengguna meminta tanpa warna, lewat opsi --no-color atau variabel NO_COLOR
        return false; // return false; warna dimatikan; permintaan ini didahulukan di atas aturan lain
    } // akhir if NO_COLOR
    if (hasEnvironmentValue("FORCE_COLOR") && std::strcmp(std::getenv("FORCE_COLOR"), "0") != 0) { // FORCE_COLOR terisi dan isinya bukan "0" artinya warna dipaksa menyala;
        // std::getenv di sini aman dibaca karena && memastikan FORCE_COLOR sudah terbukti ada, dan std::strcmp != 0 berarti isinya berbeda dari "0".
        return true; // return true; warna dipakai walaupun keluaran bukan terminal (misalnya saat dialihkan ke berkas)
    } // akhir if FORCE_COLOR
    return terminalSupportsColor; // return terminalSupportsColor; tanpa permintaan khusus, warna mengikuti kemampuan terminal yang sudah diperiksa pemanggil
} // akhir fungsi decideColor()

#ifdef _WIN32 // _WIN32: dari sini sampai #else adalah versi Windows dari restoreConsoleCodePage() dan initTerminal()

// UINT adalah tipe bilangan bulat tanpa tanda negatif milik Windows; dipakai karena fungsi code page
// Windows (GetConsoleOutputCP dan SetConsoleOutputCP) memakai tipe ini. Nilai 0 dipakai sebagai tanda
// "belum ada code page yang disimpan".
static UINT g_savedOutputCodePage = 0; // g_savedOutputCodePage menyimpan code page keluaran konsol sebelum diubah program, supaya bisa dikembalikan saat program selesai

// Dipanggil otomatis saat program selesai: mengembalikan code page konsol seperti semula,
// supaya jendela Command Prompt atau PowerShell pengguna tidak ikut berubah.
static void restoreConsoleCodePage() { // restoreConsoleCodePage() berfungsi untuk mengembalikan code page keluaran konsol ke nilai yang disimpan tadi
    if (g_savedOutputCodePage != 0) { // g_savedOutputCodePage != 0 artinya memang ada code page lama yang tersimpan, jadi aman untuk dikembalikan
        SetConsoleOutputCP(g_savedOutputCodePage); // SetConsoleOutputCP(g_savedOutputCodePage) berfungsi untuk memasang kembali code page keluaran yang lama
    } // akhir if
} // akhir fungsi restoreConsoleCodePage()

// Versi Windows: menyalakan UTF-8 dan mode warna pada konsol, lalu mencatat hasilnya.
void initTerminal(bool disableColor, bool forceAscii) { // initTerminal(bool disableColor, bool forceAscii) versi Windows; dipanggil sekali dari runApp()
    // HANDLE adalah tipe Windows untuk "pegangan", yaitu tanda pengenal sebuah saluran (di sini keluaran
    // standar) yang harus disebutkan setiap kali memanggil fungsi konsol Windows.
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE); // GetStdHandle(STD_OUTPUT_HANDLE) berfungsi untuk mengambil pegangan ke keluaran standar (layar konsol)
    // DWORD adalah tipe bilangan bulat 32 bit tanpa tanda negatif milik Windows; mode konsol disimpan dalam tipe ini.
    DWORD mode = 0; // mode adalah tempat menyimpan mode konsol yang sedang aktif; isinya diisi GetConsoleMode di baris berikutnya
    // Tanda & di depan nama variabel (seperti &mode) berfungsi untuk mengambil ALAMAT variabel itu di memori.
    // Alamat dikirim supaya fungsi Windows bisa menulis hasilnya langsung ke variabel mode. Ini berbeda dari &
    // pada parameter fungsi (referensi) dan dari && (operator "dan").
    const bool outputIsConsole = output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode) != 0; // outputIsConsole true bila pegangannya sah dan
    // GetConsoleMode berhasil membaca mode; GetConsoleMode gagal (hasil 0) bila keluaran dialihkan ke berkas atau pipa, jadi hasilnya sekaligus menjawab "apakah ini konsol".

    // Konsol Windows bawaan memakai code page lama (misalnya 437 atau 850) sehingga garis kotak
    // UTF-8 tampil sebagai huruf acak. Keluaran dipindah ke UTF-8 (65001); code page masukan
    // sengaja tidak diubah karena program hanya menerima ketikan ASCII.
    g_savedOutputCodePage = GetConsoleOutputCP(); // GetConsoleOutputCP() berfungsi untuk membaca code page keluaran yang sedang dipakai, lalu disimpan untuk dikembalikan nanti
    const bool utf8Ready = SetConsoleOutputCP(CP_UTF8) != 0; // SetConsoleOutputCP(CP_UTF8) mengganti keluaran konsol ke UTF-8 (CP_UTF8 bernilai 65001);
    // hasilnya 0 bila gagal, jadi utf8Ready true berarti penggantian berhasil.
    if (utf8Ready && g_savedOutputCodePage != 0) { // utf8Ready && g_savedOutputCodePage != 0 artinya code page memang berubah dan nilai lamanya diketahui
        // std::atexit(fungsi) berfungsi untuk mendaftarkan sebuah fungsi agar dipanggil otomatis ketika program
        // selesai secara normal (kembali dari main() atau lewat exit()). Nama fungsi ditulis tanpa tanda ()
        // karena yang dikirim adalah fungsinya sendiri untuk dipanggil nanti, bukan hasil memanggilnya sekarang.
        std::atexit(restoreConsoleCodePage); // std::atexit(restoreConsoleCodePage) memastikan code page lama dikembalikan saat program selesai;
        // ini berlaku baik lewat menu Keluar maupun saat masukan habis, karena keduanya selesai dengan kembali dari main().
    } // akhir if utf8Ready

    // Mode "virtual terminal" membuat konsol memahami kode warna ANSI. Konsol lama yang menolak
    // mode ini tetap bisa dipakai, hanya tanpa warna.
    // Tanda | (OR per bit) berfungsi untuk menyalakan satu bendera tambahan di dalam nilai mode tanpa
    // mematikan bendera lain yang sudah menyala.
    const bool vtReady = outputIsConsole && SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0; // vtReady true bila keluaran adalah konsol
    // dan SetConsoleMode berhasil menyalakan mode virtual terminal; bila bukan konsol, SetConsoleMode tidak dipanggil sama sekali karena && berhenti lebih dulu.

    g_colorEnabled = decideColor(disableColor, vtReady); // g_colorEnabled diisi keputusan warna; konsol dianggap mengerti warna hanya bila vtReady
    g_unicodeEnabled = !forceAscii && (utf8Ready || !outputIsConsole); // garis UTF-8 dipakai bila tidak ada opsi --ascii DAN (konsol sudah UTF-8 ATAU keluaran
    // bukan konsol, misalnya berkas, yang menyimpan byte UTF-8 apa adanya tanpa bergantung pada code page).
    g_inputInteractive = _isatty(_fileno(stdin)) != 0; // _fileno(stdin) memberi nomor saluran masukan standar, lalu _isatty memeriksa apakah saluran itu konsol;
    // hasil selain 0 berarti masukan diketik dari keyboard, 0 berarti dialihkan dari berkas atau pipa.
} // akhir fungsi initTerminal() versi Windows

#else // #else: bukan Windows; dari sini sampai #endif adalah versi macOS/Linux/Android dari initTerminal()

// Versi Unix: terminal macOS, Linux, dan Android umumnya sudah memakai UTF-8, jadi cukup memeriksa apakah keluaran dan
// masukan berupa terminal, serta apakah terminalnya mengerti warna.
void initTerminal(bool disableColor, bool forceAscii) { // initTerminal(bool disableColor, bool forceAscii) versi Unix; dipanggil sekali dari runApp()
    const bool outputIsTerminal = isatty(STDOUT_FILENO) != 0; // isatty(STDOUT_FILENO) memeriksa apakah keluaran standar terhubung ke terminal; hasil selain 0 berarti ya
    // TERM=dumb adalah cara terminal sederhana menyatakan dirinya tidak mengerti kode warna.
    const char* terminalType = std::getenv("TERM"); // std::getenv("TERM") membaca jenis terminal (misalnya xterm-256color), atau NULL bila tidak ada
    const bool dumbTerminal = terminalType != NULL && std::strcmp(terminalType, "dumb") == 0; // dumbTerminal true bila TERM ada dan isinya persis "dumb";
    // terminalType != NULL diperiksa dulu supaya std::strcmp tidak pernah menerima penunjuk kosong.

    g_colorEnabled = decideColor(disableColor, outputIsTerminal && !dumbTerminal); // terminal dianggap mengerti warna bila keluarannya terminal dan bukan jenis "dumb"
    g_unicodeEnabled = !forceAscii; // garis UTF-8 selalu dipakai kecuali pengguna memberi opsi --ascii
    g_inputInteractive = isatty(STDIN_FILENO) != 0; // isatty(STDIN_FILENO) != 0 artinya masukan standar adalah terminal, jadi pengguna mengetik langsung
} // akhir fungsi initTerminal() versi Unix

#endif // akhir pemilihan versi initTerminal() sesuai sistem operasi

// Tiga fungsi baca di bawah hanya mengembalikan nilai yang sudah dicatat initTerminal(). Bagian lain
// program memakai fungsi ini, bukan variabelnya langsung, jadi yang mengisi variabel itu hanya initTerminal().
bool colorsEnabled() { // colorsEnabled() berfungsi untuk memberi tahu apakah warna boleh dipakai
    return g_colorEnabled; // return g_colorEnabled; mengembalikan keputusan warna yang tersimpan
} // akhir fungsi colorsEnabled()

bool unicodeEnabled() { // unicodeEnabled() berfungsi untuk memberi tahu apakah garis kotak memakai karakter UTF-8
    return g_unicodeEnabled; // return g_unicodeEnabled; mengembalikan keputusan jenis garis yang tersimpan
} // akhir fungsi unicodeEnabled()

bool inputIsInteractive() { // inputIsInteractive() berfungsi untuk memberi tahu apakah masukan diketik langsung dari keyboard
    return g_inputInteractive; // return g_inputInteractive; mengembalikan hasil pemeriksaan masukan yang tersimpan
} // akhir fungsi inputIsInteractive()

// Lebar layar dalam kolom, dicari bertahap: tanya terminal dulu, lalu variabel COLUMNS, lalu lebar
// bawaan; hasil akhirnya dijepit ke rentang MIN_TERMINAL_WIDTH sampai MAX_TERMINAL_WIDTH.
int terminalWidth() { // terminalWidth() berfungsi untuk mengembalikan lebar tata letak dalam kolom (40 sampai 120)
    // Lebar ditanyakan setiap kali dipanggil, jadi tampilan ikut menyesuaikan bila jendela
    // diubah ukurannya di tengah pemakaian.
    int columns = 0; // columns = 0 artinya lebar belum diketahui; nilai 0 atau kurang dipakai sebagai tanda "coba cara berikutnya"
#ifdef _WIN32 // _WIN32: cara menanyakan lebar jendela di Windows
    CONSOLE_SCREEN_BUFFER_INFO info; // info adalah struct milik Windows yang akan diisi keterangan layar konsol, termasuk posisi tepi jendela yang terlihat
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) { // GetConsoleScreenBufferInfo(...) mengisi info lewat alamatnya (&info);
        // hasil selain 0 berarti berhasil, sedangkan 0 berarti gagal, misalnya karena keluaran dialihkan ke berkas.
        columns = info.srWindow.Right - info.srWindow.Left + 1; // srWindow adalah kotak jendela yang terlihat; Right dan Left adalah nomor kolom tepi kanan dan kiri,
        // keduanya ikut terhitung, jadi lebarnya Right - Left + 1.
    } // akhir if GetConsoleScreenBufferInfo
#else // #else: cara menanyakan lebar jendela di macOS/Linux/Android
    // Kata struct di depan winsize adalah gaya penulisan bahasa C (pustaka ini berasal dari C); di C++ boleh
    // ditulis atau dihilangkan, artinya tetap sama: membuat variabel bertipe struct winsize.
    struct winsize size; // size adalah struct winsize yang akan diisi ukuran jendela terminal (ws_col = banyaknya kolom, ws_row = banyaknya baris)
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) { // ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) meminta terminal mengisi ukuran jendelanya ke size;
        // TIOCGWINSZ adalah kode permintaan "beri tahu ukuran jendela", dan hasil 0 berarti berhasil (gagal bila keluaran bukan terminal).
        columns = size.ws_col; // columns diisi ws_col, yaitu banyaknya kolom jendela terminal
    } // akhir if ioctl
#endif // akhir pemilihan cara menanyakan lebar sesuai sistem operasi
    // Bila keluaran bukan terminal (dialihkan ke berkas atau pipa), dicoba variabel COLUMNS;
    // bila itu pun tidak ada, dipakai lebar bawaan 80 kolom.
    if (columns <= 0) { // columns <= 0 artinya terminal tidak menjawab, jadi lebar dicari dari variabel lingkungan COLUMNS
        const char* fromEnvironment = std::getenv("COLUMNS"); // fromEnvironment adalah isi variabel COLUMNS (misalnya "100"), atau NULL bila tidak ada
        if (fromEnvironment != NULL) { // fromEnvironment != NULL artinya COLUMNS ada, jadi isinya boleh dibaca
            // std::strtol(teks, NULL, 10) berfungsi untuk mengubah teks gaya C berisi angka menjadi long, misalnya "100" menjadi 100;
            // angka 10 artinya teks dibaca sebagai bilangan basis sepuluh, dan NULL artinya sisa teks sesudah angka tidak diperlukan.
            // Bila teksnya bukan angka, hasilnya 0. Dipilih strtol, bukan std::atoi, karena angka yang terlalu besar tetap aman:
            // strtol menjepit hasilnya ke batas long, sedangkan atoi tidak menjamin apa pun untuk angka sebesar itu.
            const long requested = std::strtol(fromEnvironment, NULL, 10); // requested adalah lebar yang diminta lewat COLUMNS, masih dalam long
            if (requested > MAX_TERMINAL_WIDTH) { // requested > MAX_TERMINAL_WIDTH artinya permintaan melebihi batas tata letak (120 kolom)
                columns = MAX_TERMINAL_WIDTH; // columns langsung diisi batas atas, supaya angka sebesar apa pun tidak perlu diubah ke int
            } else if (requested > 0) { // requested > 0 artinya permintaan berupa angka positif yang masih di dalam batas
                columns = static_cast<int>(requested); // static_cast<int> aman karena nilainya paling besar 120; columns diisi lebar yang diminta
            } // akhir if permintaan COLUMNS; nilai 0 atau negatif diabaikan dan ditangani if berikutnya
        } // akhir if fromEnvironment
    } // akhir if pencarian lewat COLUMNS
    if (columns <= 0) { // columns <= 0 masih benar artinya lebar tetap tidak diketahui
        columns = DEFAULT_TERMINAL_WIDTH; // columns diisi lebar bawaan DEFAULT_TERMINAL_WIDTH (80 kolom)
    } // akhir if lebar bawaan
    if (columns < MIN_TERMINAL_WIDTH) { // columns < MIN_TERMINAL_WIDTH artinya jendela lebih sempit dari 40 kolom
        columns = MIN_TERMINAL_WIDTH; // columns dinaikkan menjadi 40; tata letak tidak dibuat lebih sempit dari ini, terminal yang akan melipat barisnya
    } // akhir if batas bawah
    if (columns > MAX_TERMINAL_WIDTH) { // columns > MAX_TERMINAL_WIDTH artinya jendela lebih lebar dari 120 kolom
        columns = MAX_TERMINAL_WIDTH; // columns diturunkan menjadi 120 supaya baris teks tidak terlalu panjang untuk dibaca
    } // akhir if batas atas
    return columns; // return columns; mengembalikan lebar akhir yang pasti berada di antara 40 dan 120
} // akhir fungsi terminalWidth()

// ===============================================================================================================
// BAGIAN 18 - src/ui.cpp - komponen tampilan: gaya teks, kotak, tabel, dan pesan
// ===============================================================================================================
// Berkas ini berisi isi (definisi) semua fungsi yang dideklarasikan di include/estima/ui.h. Semua gambar di layar dan
// di berkas laporan (kotak, tabel, kartu, pesan berwarna) dibuat di sini, jadi bentuk tampilan cukup diatur di satu tempat.

// Karakter penyusun kotak dan grafik untuk satu mode gambar. Setiap anggota bertipe const char* (teks tetap), bukan char,
// karena satu karakter garis UTF-8 terdiri atas tiga byte sehingga harus disimpan sebagai teks pendek.
struct BoxGlyphs { // BoxGlyphs adalah struct berisi tujuh potongan gambar: empat sudut, garis datar, garis tegak, dan isi batang grafik.
    const char* topLeft; // topLeft adalah sudut kiri atas kotak.
    const char* topRight; // topRight adalah sudut kanan atas kotak.
    const char* bottomLeft; // bottomLeft adalah sudut kiri bawah kotak.
    const char* bottomRight; // bottomRight adalah sudut kanan bawah kotak.
    const char* horizontal; // horizontal adalah garis mendatar untuk tepi atas dan bawah kotak serta garis pemisah tabel.
    const char* vertical; // vertical adalah garis tegak untuk tepi kiri dan kanan kotak.
    const char* bar; // bar adalah karakter pengisi batang grafik pada tabel peringkat biaya.
}; // akhir struct BoxGlyphs

// Dua set karakter disimpan dalam array dua elemen: nomor 0 untuk mode ASCII dan nomor 1 untuk
// mode UTF-8. Mode dipilih dengan mengambil elemen array, tanpa if di setiap tempat menggambar.
// Karakter UTF-8 ditulis sebagai kode byte (\x..) supaya berkas sumber tetap ASCII murni dan
// dikompilasi sama persis oleh compiler apa pun: "\xE2\x94\x8C" adalah sudut kiri atas kotak.
// Di dalam tanda kutip, \x yang diikuti dua digit heksadesimal (basis 16) berarti satu byte bernilai itu;
// tiga byte E2 94 8C bersama-sama membentuk satu karakter garis UTF-8.
// Array ini hanya dipakai di berkas ini dan isinya tidak pernah diubah selama program berjalan.
static const BoxGlyphs GLYPH_SETS[2] = { // GLYPH_SETS[2] adalah array dua BoxGlyphs; isinya ditulis di antara { } sesuai urutan anggota struct.
    { "+", "+", "+", "+", "-", "|", "#" }, // elemen 0 (ASCII): empat sudut "+", garis datar "-", garis tegak "|", dan batang "#"; terbaca di mana pun.
    { "\xE2\x94\x8C", "\xE2\x94\x90", "\xE2\x94\x94", "\xE2\x94\x98", "\xE2\x94\x80", "\xE2\x94\x82", "\xE2\x96\x88" } // elemen 1 (UTF-8):
    // sudut kiri atas, kanan atas, kiri bawah, kanan bawah, garis datar, garis tegak (semuanya garis tipis), dan blok penuh untuk batang.
}; // akhir array GLYPH_SETS

// Kode warna ANSI untuk tiap gaya, disusun sejajar dengan urutan enum TextStyle sehingga gaya
// bisa langsung dipakai sebagai nomor elemen. "\033" adalah karakter ESC pembuka kode.
// \033 ditulis dalam bilangan oktal (basis 8): 3 x 8 + 3 = 27, yaitu nomor karakter ESC. Kode ANSI berbentuk ESC, "[",
// angka gaya, lalu "m"; terminal tidak mencetaknya, melainkan membacanya sebagai perintah ganti tampilan.
// Ukuran array ditulis STYLE_COUNT supaya selalu sama dengan banyaknya gaya di enum TextStyle.
static const char* const ANSI_CODES[STYLE_COUNT] = { // ANSI_CODES[STYLE_COUNT] adalah array 8 teks tetap; nomor elemennya adalah nilai TextStyle.
    "", // nomor 0, STYLE_NORMAL: teks kosong karena teks biasa tidak memerlukan kode warna.
    "\033[1;36m", // nomor 1, STYLE_TITLE: 1;36 artinya tebal (1) sekaligus cyan (36).
    "\033[1m", // nomor 2, STYLE_BOLD: 1 artinya tebal tanpa mengganti warna.
    "\033[90m", // nomor 3, STYLE_DIM: 90 artinya abu-abu (hitam terang), supaya garis dan catatan tidak mencolok.
    "\033[1;32m", // nomor 4, STYLE_SUCCESS: tebal dan hijau (32).
    "\033[1;31m", // nomor 5, STYLE_ERROR: tebal dan merah (31).
    "\033[33m", // nomor 6, STYLE_HINT: kuning (33) tanpa tebal.
    "\033[1;33m" // nomor 7, STYLE_HIGHLIGHT: tebal dan kuning.
}; // akhir array ANSI_CODES
static const char* const ANSI_RESET = "\033[0m"; // ANSI_RESET adalah kode 0 yang mengembalikan tampilan ke normal; ditulis setelah teks berwarna
// supaya warnanya tidak terbawa ke teks berikutnya.

// Kotak memakai 2 kolom garis tepi dan 2 kolom spasi dalam; satu kolom di ujung kanan terminal
// dibiarkan kosong supaya baris tidak pernah terlipat sendiri oleh terminal.
// Angka tata letak ditulis sebagai konstanta bernama supaya maksudnya terbaca dan cukup diubah di satu tempat.
// Tipenya std::size_t karena selalu dihitung bersama lebar teks, yang juga bertipe std::size_t.
static const std::size_t BOX_FRAME_WIDTH = 4; // BOX_FRAME_WIDTH = 4 adalah kolom yang dipakai bingkai kotak: garis kiri, spasi, spasi, garis kanan.
static const std::size_t RIGHT_MARGIN = 1; // RIGHT_MARGIN = 1 adalah satu kolom kosong di ujung kanan keluaran.
static const std::size_t MIN_BOX_INNER = 10; // MIN_BOX_INNER = 10 adalah lebar isi kotak paling sempit, dipakai bila lebar keluaran terlalu kecil.
static const std::size_t COLUMN_GAP = 2; // COLUMN_GAP = 2 adalah banyaknya spasi di antara dua kolom tabel.
static const std::size_t MIN_NOTE_WIDTH = 44; // MIN_NOTE_WIDTH = 44 adalah lebar lipatan catatan paling sempit, supaya catatan di bawah tabel
// yang sempit tidak terpecah menjadi banyak baris pendek.
static const std::size_t CARD_TITLE_COLUMNS = 2; // CARD_TITLE_COLUMNS = 2 artinya dua kolom pertama tabel (nomor dan nama) menjadi judul kartu.
static const char* const CARD_INDENT = "   "; // CARD_INDENT adalah tiga spasi di depan setiap baris isi kartu, supaya isinya menjorok di bawah judul.

// Empat konstruktor di bawah mengisi setiap anggota lewat daftar inisialisasi (bagian setelah tanda :), jadi anggota langsung
// mendapat nilai awal yang benar saat struct dibuat. Badan fungsinya { } kosong karena tidak ada pekerjaan lain.
StyledLine::StyledLine(const std::string& lineText, TextStyle lineStyle) : text(lineText), style(lineStyle) { // konstruktor StyledLine:
    // text diisi lineText dan style diisi lineStyle.
} // akhir konstruktor StyledLine

TableColumn::TableColumn(const std::string& headerText, bool rightAligned) : header(headerText), alignRight(rightAligned) { // konstruktor TableColumn:
    // header diisi judul kolom dan alignRight diisi arah perataannya.
} // akhir konstruktor TableColumn

TableRow::TableRow() : cells(), style(STYLE_NORMAL) { // konstruktor TableRow: cells() dimulai kosong dan style diisi STYLE_NORMAL.
} // akhir konstruktor TableRow

Table::Table() : columns(), rows(), hasFooter(false), footer() { // konstruktor Table: kolom dan baris kosong, tanpa baris penutup (hasFooter false).
} // akhir konstruktor Table

// screenStyle() menyusun gaya keluaran layar dari keadaan terminal. Fungsi ini dipanggil setiap kali akan menggambar
// (bukan sekali di awal), jadi bila jendela terminal diubah ukurannya, gambar berikutnya ikut menyesuaikan.
OutputStyle screenStyle() { // screenStyle() mengembalikan OutputStyle untuk layar.
    OutputStyle style; // style adalah variabel lokal yang diisi satu per satu lalu dikembalikan.
    style.width = terminalWidth(); // terminalWidth() memberi lebar jendela terminal, selalu di antara 40 dan 120 kolom.
    style.useColor = colorsEnabled(); // colorsEnabled() bernilai true bila warna boleh dipakai menurut opsi --no-color, NO_COLOR, FORCE_COLOR, dan jenis keluarannya.
    style.useUnicode = unicodeEnabled(); // unicodeEnabled() bernilai false bila pengguna memakai opsi --ascii, atau (di Windows) bila keluarannya konsol yang gagal dipindah ke UTF-8.
    return style; // return style mengembalikan struct yang sudah lengkap kepada pemanggil.
} // akhir fungsi screenStyle()

// plainStyle() menyusun gaya polos: tanpa warna dan dengan garis ASCII, selebar width. Dipakai saveReport() di report.cpp
// (lebar 120) supaya berkas laporan terbaca di penyunting teks apa pun, dan oleh app.cpp untuk pesan saat masukan habis.
OutputStyle plainStyle(int width) { // plainStyle(width) mengembalikan OutputStyle polos selebar width kolom.
    OutputStyle style; // style adalah variabel lokal yang akan dikembalikan.
    style.width = width; // lebar diambil dari parameter, bukan dari terminal.
    style.useColor = false; // false: tanpa kode warna, karena kode ANSI di berkas teks akan tampil sebagai karakter aneh.
    style.useUnicode = false; // false: garis kotak memakai + - | biasa.
    return style; // mengembalikan gaya polos yang sudah lengkap.
} // akhir fungsi plainStyle()

// glyphsFor() memilih set karakter yang cocok dengan gaya keluaran. Hasilnya berupa referensi (tanda & pada tipe hasil)
// ke elemen array GLYPH_SETS, jadi struct tidak disalin; aman karena GLYPH_SETS tetap ada selama program berjalan.
// Hasilnya tidak boleh diubah oleh pemanggil, dan fungsi ini hanya dipakai di berkas ini.
static const BoxGlyphs& glyphsFor(const OutputStyle& style) { // glyphsFor(style) mengembalikan set karakter garis untuk style.
    return GLYPH_SETS[style.useUnicode ? 1 : 0]; // style.useUnicode ? 1 : 0 memilih nomor 1 (UTF-8) bila useUnicode true, nomor 0 (ASCII) bila false.
} // akhir fungsi glyphsFor()

// colorize() membungkus teks dengan kode warna gayanya: kode pembuka + teks + ANSI_RESET. Hasilnya teks baru, jadi bisa
// langsung ditulis ke aliran keluaran atau disambung dengan teks lain.
std::string colorize(const OutputStyle& style, TextStyle textStyle, const std::string& text) { // colorize(style, textStyle, text) memberi warna pada text.
    if (!style.useColor || textStyle == STYLE_NORMAL || text.empty()) { // bila warna mati, gayanya normal, atau teksnya kosong, tidak ada yang perlu diwarnai,
        return text; // jadi text dikembalikan apa adanya; laporan berkas selalu lewat jalan ini karena useColor-nya false.
    } // akhir if
    return std::string(ANSI_CODES[textStyle]) + text + ANSI_RESET; // ANSI_CODES[textStyle] mengambil kode warna dengan nilai enum sebagai nomor elemen;
    // kode itu dijadikan std::string, lalu text dan ANSI_RESET disambung di belakangnya dengan tanda +.
} // akhir fungsi colorize()

// barGlyph() memberikan karakter batang grafik kepada report.cpp (tabel peringkat biaya) tanpa memperlihatkan
// array GLYPH_SETS ke berkas lain.
const char* barGlyph(const OutputStyle& style) { // barGlyph(style) mengembalikan blok penuh pada mode UTF-8 atau # pada mode ASCII.
    return glyphsFor(style).bar; // glyphsFor(style).bar mengambil anggota bar dari set karakter yang terpilih.
} // akhir fungsi barGlyph()

// boxInnerLimit() menghitung lebar isi terbesar sebuah kotak: lebar keluaran dikurangi bingkai (4) dan margin kanan (1).
// Dipakai drawBox() dan drawTableBox() di berkas ini, serta printMenu() di app.cpp untuk menilai apakah keterangan menu muat.
std::size_t boxInnerLimit(const OutputStyle& style) { // boxInnerLimit(style) mengembalikan lebar isi kotak dalam kolom.
    const std::size_t width = static_cast<std::size_t>(style.width); // width diubah dari int menjadi std::size_t supaya sejenis dengan konstanta di bawah.
    if (width < BOX_FRAME_WIDTH + RIGHT_MARGIN + MIN_BOX_INNER) { // width < 4 + 1 + 10 (= 15) artinya keluaran terlalu sempit untuk kotak yang wajar.
        return MIN_BOX_INNER; // maka dipakai lebar isi 10; ini juga mencegah pengurangan di bawah menjadi "negatif" (std::size_t tidak punya nilai negatif).
        // Dalam pemakaian biasa lebar paling sedikit 40 kolom, jadi cabang ini hanya jaga-jaga.
    } // akhir if
    return width - BOX_FRAME_WIDTH - RIGHT_MARGIN; // width - 4 - 1, misalnya lebar 80 memberi isi kotak 75 kolom.
} // akhir fungsi boxInnerLimit()

// Menentukan indentasi baris lanjutan saat sebuah baris kotak harus dilipat. Baris berbentuk
// "Label : nilai" dilanjutkan tepat di bawah nilainya; baris lain mengikuti indentasi awalnya.
static std::size_t hangingIndentOf(const std::string& text, std::size_t maxWidth) { // hangingIndentOf(text, maxWidth) mengembalikan banyaknya spasi baris lanjutan.
    // find() menghasilkan std::string::npos bila " : " tidak ada di dalam text (lihat penjelasan npos di input_parse.cpp).
    const std::size_t colon = text.find(" : "); // text.find(" : ") mencari posisi pertama " : " di dalam text.
    if (colon != std::string::npos) { // colon != npos artinya baris ini berbentuk "Label : nilai".
        const std::size_t valueColumn = displayWidth(text.substr(0, colon + 3)); // text.substr(0, colon + 3) adalah bagian "Label : " (3 = panjang " : ");
        // lebarnya sama dengan kolom tempat nilai dimulai.
        if (valueColumn <= maxWidth / 2) { // nilai dilanjutkan di kolomnya hanya bila label tidak lebih dari separuh lebar,
            // jadi baris lanjutan masih punya paling sedikit separuh baris untuk isinya.
            return valueColumn; // baris lanjutan dimulai tepat di bawah awal nilai.
        } // akhir if valueColumn
    } // akhir if colon
    std::size_t indent = 0; // indent menghitung spasi di awal teks, mulai dari 0.
    while (indent < text.size() && text[indent] == ' ') { // selama belum sampai ujung teks dan karakter ke-indent masih spasi; batas ukuran diperiksa
        // lebih dulu supaya text[indent] tidak pernah membaca di luar teks.
        indent = indent + 1; // indent bertambah satu untuk setiap spasi di depan.
    } // akhir perulangan while
    return indent; // baris lanjutan menjorok sama dengan baris pertamanya.
} // akhir fungsi hangingIndentOf()

// drawBox() menggambar kotak berjudul berisi baris-baris lines ke aliran out dalam tiga langkah: lipat, ukur, lalu gambar.
// Kotak ini dipakai antara lain untuk banner, menu utama, ringkasan komponen, total keseluruhan, dan semua tabel (lewat drawTableBox()).
void drawBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawBox(out, style, title, lines): out tujuan tulisan, title judul kotak,
             const std::vector<StyledLine>& lines) { // lines baris isinya; dikirim sebagai const & supaya vector tidak disalin dan tidak diubah.
    const BoxGlyphs& glyphs = glyphsFor(style); // glyphs adalah set karakter garis yang dipakai sepanjang fungsi ini.
    const std::size_t maxInner = boxInnerLimit(style); // maxInner adalah lebar isi terbesar yang masih muat di keluaran.

    // Langkah 1: lipat setiap baris supaya muat, sambil membawa gaya barisnya ke potongannya.
    std::vector<StyledLine> wrapped; // wrapped menampung baris hasil lipatan; vector karena banyaknya baru diketahui setelah dilipat.
    for (std::size_t i = 0; i < lines.size(); ++i) { // perulangan untuk setiap baris masukan, i dari 0 sampai lines.size() - 1.
        const std::vector<std::string> pieces = wrapText(lines[i].text, maxInner, hangingIndentOf(lines[i].text, maxInner)); // pieces: potongan baris ke-i,
        // masing-masing paling lebar maxInner; baris lanjutan menjorok sesuai hangingIndentOf().
        for (std::size_t k = 0; k < pieces.size(); ++k) { // perulangan untuk setiap potongan.
            wrapped.push_back(StyledLine(pieces[k], lines[i].style)); // potongan ke-k disimpan bersama gaya baris asalnya.
        } // akhir perulangan potongan
    } // akhir perulangan baris

    // Langkah 2: lebar kotak mengikuti baris terpanjang (atau judulnya), jadi kotak tidak
    // lebih lebar dari yang diperlukan.
    const std::string shownTitle = truncateText(title, maxInner - 1); // shownTitle adalah judul yang dipotong (diberi ".." di ujung) bila lebih lebar dari maxInner - 1.
    // Batasnya maxInner - 1 karena isi kotak harus 1 kolom lebih lebar dari judul (lihat Langkah 3).
    std::size_t inner = 0; // inner adalah lebar isi kotak yang sebenarnya, mulai 0 lalu diperbesar.
    for (std::size_t i = 0; i < wrapped.size(); ++i) { // memeriksa setiap baris hasil lipatan.
        if (displayWidth(wrapped[i].text) > inner) { // bila baris ini lebih lebar dari inner sejauh ini,
            inner = displayWidth(wrapped[i].text); // inner diganti lebar baris ini; pola ini mencari nilai terbesar.
        } // akhir if
    } // akhir perulangan for
    if (!shownTitle.empty() && displayWidth(shownTitle) + 1 > inner) { // bila ada judul dan lebar judul + 1 melebihi isi,
        inner = displayWidth(shownTitle) + 1; // kotak dilebarkan mengikuti judul.
    } // akhir if judul

    // Langkah 3: garis atas (dengan judul), isi, lalu garis bawah.
    // Di garis atas, judul memakai 3 kolom tambahan (satu garis datar dan dua spasi), sedangkan baris isi hanya punya
    // 2 spasi dalam; karena itu inner minimal lebar judul + 1 supaya semua baris kotak sama panjang.
    if (shownTitle.empty()) { // tanpa judul: garis atas hanya berisi sudut dan garis datar.
        // Operator << berfungsi untuk mengirim (menulis) nilai di kanannya ke aliran keluaran di kirinya, di sini out.
        // Beberapa << boleh dirangkai dalam satu pernyataan; nilainya ditulis berurutan dari kiri ke kanan.
        out << colorize(style, STYLE_DIM, glyphs.topLeft + repeatText(glyphs.horizontal, inner + 2) + glyphs.topRight) << '\n'; // garis datar sepanjang
        // inner + 2 (isi dan dua spasi dalam) diapit dua sudut atas, berwarna abu-abu, lalu '\n' pindah baris.
    } else { // dengan judul: judul disisipkan di garis atas.
        const std::size_t fill = inner - displayWidth(shownTitle) - 1; // fill adalah banyaknya garis datar sesudah judul supaya garis atas sama panjang dengan isi.
        out << colorize(style, STYLE_DIM, std::string(glyphs.topLeft) + glyphs.horizontal + " ") // sudut kiri atas, satu garis datar, dan spasi;
        // topLeft dibungkus std::string(...) karena dua const char* tidak bisa disambung langsung dengan tanda +.
            << colorize(style, STYLE_TITLE, shownTitle) // judul kotak dengan gaya judul (cyan tebal).
            << colorize(style, STYLE_DIM, " " + repeatText(glyphs.horizontal, fill) + glyphs.topRight) << '\n'; // spasi, garis datar sebanyak fill, sudut kanan atas.
    } // akhir if-else garis atas
    for (std::size_t i = 0; i < wrapped.size(); ++i) { // menulis setiap baris isi.
        out << colorize(style, STYLE_DIM, glyphs.vertical) << ' ' // garis tegak kiri lalu satu spasi dalam.
            << colorize(style, wrapped[i].style, padRight(wrapped[i].text, inner)) // isi baris dengan gayanya, ditambah spasi kanan sampai selebar inner;
            // padRight dikerjakan sebelum colorize karena kode warna tidak tampil tetapi ikut terhitung lebarnya.
            << ' ' << colorize(style, STYLE_DIM, glyphs.vertical) << '\n'; // satu spasi dalam, garis tegak kanan, lalu pindah baris.
    } // akhir perulangan baris isi
    out << colorize(style, STYLE_DIM, glyphs.bottomLeft + repeatText(glyphs.horizontal, inner + 2) + glyphs.bottomRight) << '\n'; // garis bawah,
    // sama panjang dengan garis atas.
} // akhir fungsi drawBox()

// Menyusun satu baris tabel: tiap sel diratakan sesuai kolomnya lalu disambung dengan jarak antarkolom.
static std::string joinTableCells(const Table& table, const std::vector<std::string>& cells, // joinTableCells(table, cells, widths): table memberi arah perataan,
                                  const std::vector<std::size_t>& widths) { // cells isi sel baris ini, dan widths lebar tiap kolom. Hasilnya satu baris teks.
    std::string line; // line adalah baris yang sedang disusun, awalnya kosong.
    for (std::size_t column = 0; column < table.columns.size(); ++column) { // perulangan untuk setiap kolom tabel.
        const std::string cell = column < cells.size() ? cells[column] : std::string(); // cell adalah isi sel kolom ini; bila baris punya sel lebih sedikit,
        // dipakai teks kosong std::string() supaya tidak membaca di luar vector.
        if (column > 0) { // sebelum setiap kolom kecuali yang pertama,
            line += std::string(COLUMN_GAP, ' '); // std::string(COLUMN_GAP, ' ') membuat teks berisi 2 spasi sebagai jarak antarkolom.
        } // akhir if
        line += table.columns[column].alignRight ? padLeft(cell, widths[column]) : padRight(cell, widths[column]); // kolom rata kanan diberi spasi di kiri,
        // kolom rata kiri diberi spasi di kanan, sampai selebar widths[column].
    } // akhir perulangan kolom
    return line; // mengembalikan baris tabel yang sudah rata.
} // akhir fungsi joinTableCells()

// Mengubah tabel menjadi baris-baris teks rata kolom. Lebar tiap kolom mengikuti isi terlebarnya.
// Hasil false berarti tabel lebih lebar dari maxWidth dan tidak ada baris yang ditambahkan.
// Baris hasilnya ditambahkan ke parameter lines (referensi tanpa const), jadi fungsi ini memberi dua hasil sekaligus:
// muat atau tidak lewat return, dan baris-baris tabel lewat lines.
static bool buildTableLines(const Table& table, const OutputStyle& style, std::size_t maxWidth, // buildTableLines(...): style untuk memilih karakter garis,
                            std::vector<StyledLine>& lines) { // maxWidth lebar yang tersedia, lines tempat baris hasil ditambahkan.
    const std::size_t columnCount = table.columns.size(); // columnCount adalah banyaknya kolom tabel.
    std::vector<std::size_t> widths(columnCount, 0); // widths(columnCount, 0) membuat vector berisi columnCount angka 0; elemen ke-n adalah lebar kolom ke-n.
    for (std::size_t column = 0; column < columnCount; ++column) { // menghitung lebar setiap kolom.
        widths[column] = displayWidth(table.columns[column].header); // lebar awal kolom adalah lebar judulnya.
        for (std::size_t row = 0; row < table.rows.size(); ++row) { // lalu isi kolom ini di setiap baris ikut diukur.
            if (column < table.rows[row].cells.size() && displayWidth(table.rows[row].cells[column]) > widths[column]) { // bila selnya ada dan lebih lebar;
                // syarat pertama diperiksa lebih dulu supaya cells[column] tidak dibaca bila sel itu tidak ada.
                widths[column] = displayWidth(table.rows[row].cells[column]); // lebar kolom diperbesar mengikuti sel itu.
            } // akhir if
        } // akhir perulangan baris
        if (table.hasFooter && column < table.footer.cells.size() && displayWidth(table.footer.cells[column]) > widths[column]) { // baris penutup juga diukur.
            widths[column] = displayWidth(table.footer.cells[column]); // lebar kolom mengikuti sel penutup bila sel itu yang terlebar.
        } // akhir if footer
    } // akhir perulangan kolom

    std::size_t naturalWidth = 0; // naturalWidth adalah lebar tabel bila semua kolom ditulis utuh.
    for (std::size_t column = 0; column < columnCount; ++column) { // menjumlahkan lebar semua kolom.
        naturalWidth = naturalWidth + widths[column]; // lebar kolom ke-column ditambahkan.
    } // akhir perulangan for
    if (columnCount > 1) { // jarak antarkolom hanya ada bila kolomnya lebih dari satu; syarat ini juga mencegah columnCount - 1 dihitung saat columnCount 0.
        naturalWidth = naturalWidth + COLUMN_GAP * (columnCount - 1); // n kolom punya n - 1 jarak, masing-masing 2 spasi.
    } // akhir if
    if (naturalWidth > maxWidth) { // tabel tidak muat di dalam kotak,
        return false; // jadi fungsi berhenti tanpa menambah baris; pemanggilnya lalu memakai bentuk kartu.
    } // akhir if

    std::vector<std::string> headers; // headers berisi judul tiap kolom, disusun seperti sel sebuah baris.
    std::vector<std::string> rules; // rules berisi garis datar selebar tiap kolom, untuk garis pemisah.
    for (std::size_t column = 0; column < columnCount; ++column) { // mengisi headers dan rules untuk setiap kolom.
        headers.push_back(table.columns[column].header); // judul kolom ditambahkan ke ujung headers.
        rules.push_back(repeatText(glyphsFor(style).horizontal, widths[column])); // garis datar sepanjang lebar kolom ditambahkan ke rules.
    } // akhir perulangan kolom
    const std::string ruleLine = joinTableCells(table, rules, widths); // ruleLine adalah garis pemisah utuh; disimpan karena dipakai sampai dua kali.

    lines.push_back(StyledLine(joinTableCells(table, headers, widths), STYLE_BOLD)); // baris judul kolom, ditulis tebal.
    lines.push_back(StyledLine(ruleLine, STYLE_DIM)); // garis pemisah di bawah judul kolom, berwarna abu-abu.
    for (std::size_t row = 0; row < table.rows.size(); ++row) { // setiap baris data tabel
        lines.push_back(StyledLine(joinTableCells(table, table.rows[row].cells, widths), table.rows[row].style)); // ditambahkan dengan gayanya sendiri
        // (misalnya STYLE_HIGHLIGHT untuk komponen termahal).
    } // akhir perulangan baris
    if (table.hasFooter) { // bila tabel punya baris penutup,
        lines.push_back(StyledLine(ruleLine, STYLE_DIM)); // garis pemisah lagi sebelum baris penutup.
        lines.push_back(StyledLine(joinTableCells(table, table.footer.cells, widths), STYLE_BOLD)); // baris penutup (misalnya TOTAL di rekap), ditulis tebal.
    } // akhir if
    return true; // tabel muat dan semua barisnya sudah ditambahkan ke lines.
} // akhir fungsi buildTableLines()

// Bentuk pengganti tabel untuk terminal sempit: tiap baris tabel menjadi "kartu". Dua kolom
// pertama (nomor dan nama) menjadi judul kartu, kolom lainnya ditulis "Judul kolom : isi".
// Baris penutup tabel (footer) tidak ikut ditulis dalam bentuk kartu, dan sel yang kosong dilewati.
static void buildCardLines(const Table& table, std::vector<StyledLine>& lines) { // buildCardLines(table, lines) menambahkan baris-baris kartu ke lines.
    std::size_t labelWidth = 0; // labelWidth adalah lebar judul kolom terpanjang (mulai kolom ketiga), supaya tanda : di setiap kartu lurus.
    for (std::size_t column = CARD_TITLE_COLUMNS; column < table.columns.size(); ++column) { // dimulai dari nomor 2 (kolom ketiga) karena dua kolom pertama
        // sudah menjadi judul kartu.
        if (displayWidth(table.columns[column].header) > labelWidth) { // bila judul kolom ini lebih lebar dari labelWidth,
            labelWidth = displayWidth(table.columns[column].header); // labelWidth diganti lebar judul itu.
        } // akhir if
    } // akhir perulangan kolom
    for (std::size_t row = 0; row < table.rows.size(); ++row) { // satu kartu untuk setiap baris tabel.
        const std::vector<std::string>& cells = table.rows[row].cells; // cells adalah nama pendek (referensi) untuk sel baris ini, tanpa menyalinnya.
        if (row > 0) { // sebelum setiap kartu kecuali yang pertama,
            lines.push_back(StyledLine("", STYLE_NORMAL)); // ditambahkan baris kosong sebagai pemisah antarkartu.
        } // akhir if
        std::string title = cells.size() > 0 ? cells[0] : std::string(); // title diawali isi kolom pertama (nomor), atau teks kosong bila baris tanpa sel.
        if (cells.size() > 1) { // bila ada kolom kedua (nama),
            title += ". " + cells[1]; // judul menjadi "nomor. nama", misalnya "1. Gear".
        } // akhir if
        lines.push_back(StyledLine(title, table.rows[row].style == STYLE_NORMAL ? STYLE_BOLD : table.rows[row].style)); // judul kartu ditulis tebal,
        // kecuali baris itu punya gaya khusus (misalnya sorotan), yang tetap dipakai.
        for (std::size_t column = CARD_TITLE_COLUMNS; column < table.columns.size() && column < cells.size(); ++column) { // kolom ketiga dan seterusnya
            // yang selnya ada di baris ini.
            if (cells[column].empty()) { // sel kosong
                continue; // dilewati, jadi kartu tidak memuat baris "Judul :" tanpa isi.
            } // akhir if
            lines.push_back(StyledLine(CARD_INDENT + padRight(table.columns[column].header, labelWidth) + " : " + cells[column], // baris "Judul : isi",
                                       STYLE_NORMAL)); // menjorok 3 spasi dengan judul diratakan selebar labelWidth, bergaya biasa.
        } // akhir perulangan kolom
    } // akhir perulangan baris
} // akhir fungsi buildCardLines()

// drawTableBox() menggambar tabel di dalam kotak. Bentuk tabel biasa dicoba lebih dulu; bila tidak muat di lebar
// keluaran, dipakai bentuk kartu. Catatan ditambahkan di bawahnya, lalu semuanya digambar oleh drawBox().
void drawTableBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawTableBox(out, style, title, table, notes): sama dengan drawBox(),
                  const Table& table, const std::vector<StyledLine>& notes) { // ditambah table (isi tabel) dan notes (catatan di bawah tabel, boleh kosong).
    const std::size_t maxInner = boxInnerLimit(style); // maxInner adalah lebar isi terbesar kotak.
    std::vector<StyledLine> lines; // lines akan berisi semua baris isi kotak: tabel atau kartu, lalu catatan.
    if (!buildTableLines(table, style, maxInner, lines)) { // bila tabel tidak muat (hasilnya false),
        buildCardLines(table, lines); // dipakai baris-baris kartu sebagai gantinya.
    } // akhir if
    if (!notes.empty()) { // bagian catatan hanya dikerjakan bila ada catatan.
        // Catatan dilipat selebar tabelnya (paling sempit MIN_NOTE_WIDTH kolom), supaya kotak
        // tidak melebar hanya karena kalimat catatan yang panjang.
        std::size_t noteWidth = MIN_NOTE_WIDTH; // noteWidth dimulai dari 44 kolom.
        for (std::size_t i = 0; i < lines.size(); ++i) { // mencari baris tabel atau kartu yang paling lebar.
            if (displayWidth(lines[i].text) > noteWidth) { // bila baris ini lebih lebar dari noteWidth,
                noteWidth = displayWidth(lines[i].text); // noteWidth mengikuti baris itu.
            } // akhir if
        } // akhir perulangan for
        if (noteWidth > maxInner) { // tetapi catatan tidak boleh lebih lebar dari isi kotak (misalnya isi kotak di bawah 44 kolom),
            noteWidth = maxInner; // jadi dibatasi maxInner.
        } // akhir if
        lines.push_back(StyledLine("", STYLE_NORMAL)); // baris kosong pemisah antara tabel dan catatan.
        for (std::size_t i = 0; i < notes.size(); ++i) { // setiap catatan
            const std::vector<std::string> pieces = wrapText(notes[i].text, noteWidth, 0); // dilipat selebar noteWidth tanpa indentasi baris lanjutan (0).
            for (std::size_t k = 0; k < pieces.size(); ++k) { // setiap potongan catatan
                lines.push_back(StyledLine(pieces[k], notes[i].style)); // ditambahkan dengan gaya catatannya (di program ini selalu STYLE_DIM).
            } // akhir perulangan potongan
        } // akhir perulangan catatan
    } // akhir if notes
    drawBox(out, style, title, lines); // akhirnya semua baris digambar sebagai satu kotak berjudul.
} // akhir fungsi drawTableBox()

// printWrapped() mencetak teks tanpa kotak, dilipat selebar keluaran dikurangi margin kanan. Dipakai antara lain untuk baris
// keadaan di bawah menu (app.cpp), pesan daftar kosong (report.cpp), dan ketiga fungsi pesan singkat di bawah.
void printWrapped(std::ostream& out, const OutputStyle& style, TextStyle textStyle, // printWrapped(out, style, textStyle, text, hangingIndent): textStyle gaya teks,
                  const std::string& text, std::size_t hangingIndent) { // text isi yang dicetak, hangingIndent banyaknya spasi di depan baris lanjutan.
    const std::size_t width = static_cast<std::size_t>(style.width) - RIGHT_MARGIN; // width adalah lebar keluaran dikurangi satu kolom kosong di kanan;
    // style.width diubah dulu ke std::size_t supaya sejenis dengan RIGHT_MARGIN.
    const std::vector<std::string> pieces = wrapText(text, width, hangingIndent); // pieces adalah potongan-potongan teks hasil lipatan.
    for (std::size_t i = 0; i < pieces.size(); ++i) { // setiap potongan
        out << colorize(style, textStyle, pieces[i]) << '\n'; // diberi warna lalu ditulis sebagai satu baris; tiap baris membawa kode pembuka dan penutup warnanya sendiri.
    } // akhir perulangan for
} // akhir fungsi printWrapped()

// Tiga fungsi pesan singkat di bawah selalu menulis ke layar dengan gaya layar saat ini. Awalan [!], [OK], dan [i]
// membuat jenis pesan tetap bisa dikenali walaupun warna dimatikan. Indentasi baris lanjutan sama dengan panjang
// awalannya (4 untuk "[!] " dan "[i] ", 5 untuk "[OK] "), jadi baris kedua dan seterusnya lurus dengan awal kalimat.
void printError(const std::string& message) { // printError(message) menampilkan pesan kesalahan, misalnya ketikan yang tidak sah.
    // std::cout adalah aliran keluaran standar, yaitu saluran tulisan biasa ke layar; nilai dikirim ke sana dengan operator <<.
    // Pasangannya, std::cerr, adalah saluran khusus pesan galat; app.cpp memakainya untuk opsi yang tidak dikenal dan masukan
    // yang habis, jadi pesan itu tetap terlihat walau keluaran biasa dialihkan ke berkas. Keduanya bertipe std::ostream,
    // sehingga bisa dikirim ke parameter out milik fungsi gambar.
    printWrapped(std::cout, screenStyle(), STYLE_ERROR, "[!] " + message, 4); // "[!] " + message dicetak merah tebal; 4 adalah panjang "[!] ".
} // akhir fungsi printError()

void printSuccess(const std::string& message) { // printSuccess(message) menampilkan pesan berhasil, misalnya komponen tersimpan.
    printWrapped(std::cout, screenStyle(), STYLE_SUCCESS, "[OK] " + message, 5); // "[OK] " + message dicetak hijau tebal; 5 adalah panjang "[OK] ".
} // akhir fungsi printSuccess()

void printInfo(const std::string& message) { // printInfo(message) menampilkan keterangan, misalnya petunjuk atau aksi yang dibatalkan.
    printWrapped(std::cout, screenStyle(), STYLE_DIM, "[i] " + message, 4); // "[i] " + message dicetak abu-abu; 4 adalah panjang "[i] ".
} // akhir fungsi printInfo()

// ===============================================================================================================
// BAGIAN 19 - src/prompt.cpp - tanya jawab dengan pengguna
// ===============================================================================================================
// Berkas ini berisi isi fungsi-fungsi yang dijanjikan include/estima/prompt.h. Polanya sama untuk
// semua fungsi baca: tampilkan pertanyaan, baca satu baris, periksa dengan fungsi dari input_parse.h,
// lalu ulangi bila salah sambil menampilkan pesan yang menjelaskan bentuk jawaban yang benar.

// Label pertanyaan diratakan ke lebar yang sama supaya tanda titik dua tersusun lurus, misalnya
// "Jumlah (unit)" dan "Berat per unit (kg)" sama-sama ditambah spasi sampai 20 kolom. Label yang
// sudah lebih panjang dari 20 kolom (seperti "Simpan komponen ini? [Y/t]") dibiarkan apa adanya oleh padRight.
// static const membuat konstanta ini hanya terlihat di berkas ini, karena hanya dipakai oleh readLine().
static const std::size_t PROMPT_LABEL_WIDTH = 20; // PROMPT_LABEL_WIDTH = 20 adalah lebar kolom label pertanyaan; std::size_t karena padRight meminta lebar bertipe itu

// Menampilkan label, membaca satu baris, lalu membuang spasi di awal dan akhirnya. Semua fungsi baca
// lain memanggil fungsi ini, jadi urusan "masukan habis" dan "cetak ulang jawaban" cukup ditulis di sini.
std::string readLine(const std::string& label) { // readLine(const std::string& label) berfungsi untuk menampilkan pertanyaan label lalu mengembalikan jawaban pengguna
    std::cout << colorize(screenStyle(), STYLE_BOLD, padRight(label, PROMPT_LABEL_WIDTH) + " : "); // label diratakan 20 kolom, ditambah " : ", lalu ditulis tebal
    // (STYLE_BOLD) bila warna aktif. Tidak ada '\n' di ujungnya, jadi kursor menunggu tepat di sebelah kanan tanda titik dua.
    // std::cout.flush() berfungsi untuk memaksa teks yang masih tertahan di penampung keluaran (buffer) langsung
    // dikirim ke layar. Perlu karena pertanyaan tanpa '\n' bisa tertahan, padahal pengguna harus melihatnya dulu sebelum mengetik.
    std::cout.flush(); // std::cout.flush() memastikan pertanyaan sudah tampil sebelum program mulai menunggu ketikan

    // std::getline membaca satu baris utuh sampai Enter. Dipakai, bukan std::cin >>, karena
    // seluruh ketikan perlu diperiksa: cin >> berhenti di spasi atau di karakter pertama yang
    // bukan angka, dan sisa ketikannya akan ikut terbaca oleh pertanyaan berikutnya.
    std::string line; // line adalah tempat menampung satu baris ketikan apa adanya, sebelum dirapikan
    // std::cin adalah aliran masukan standar, yaitu ketikan dari keyboard (atau isi berkas/pipa bila masukan dialihkan).
    // std::getline(std::cin, line) berfungsi untuk membaca semua karakter sampai tombol Enter ke dalam line (tanda Enter-nya
    // sendiri dibuang). Hasilnya adalah aliran std::cin itu sendiri, yang bernilai salah bila pembacaan gagal karena masukan sudah habis.
    if (!std::getline(std::cin, line)) { // !std::getline(std::cin, line) artinya tidak ada lagi yang bisa dibaca (Ctrl+D, Ctrl+Z lalu Enter, atau berkas habis)
        // throw berfungsi untuk "melempar" sebuah benda pengecualian (exception): fungsi berhenti saat itu juga, begitu pula
        // fungsi-fungsi pemanggilnya satu per satu, sampai ditemukan blok try { ... } catch (Tipe) { ... } yang menangkap tipe
        // benda tersebut. try menandai bagian kode yang diawasi, catch berisi apa yang dikerjakan bila benda itu tertangkap.
        // Di program ini try/catch-nya ada di runApp() (src/app.cpp), jadi dari pertanyaan sedalam apa pun program langsung
        // kembali ke sana dan berhenti dengan rapi, tanpa setiap pemanggil harus memeriksa "apakah masukan sudah habis".
        throw InputClosed(); // throw InputClosed(); membuat benda bertipe InputClosed lalu melemparnya; ditangkap oleh catch (const InputClosed&) di runApp()
    } // akhir if masukan habis
    const std::string answer = trimSpaces(line); // answer adalah ketikan yang sudah dibuang spasi, tab, dan tanda akhir baris di awal dan akhirnya

    // Bila masukan bukan dari keyboard (berkas atau pipa), ketikan tidak tampil di layar.
    // Jawaban dicetak ulang supaya catatan sesi tetap terbaca seperti pemakaian biasa.
    if (!inputIsInteractive()) { // !inputIsInteractive() artinya masukan datang dari berkas atau pipa, bukan diketik langsung
        std::cout << answer << '\n'; // answer ditulis ulang ke layar diikuti '\n' (pindah baris), seolah-olah pengguna mengetiknya
    } // akhir if cetak ulang
    return answer; // return answer; mengembalikan jawaban yang sudah dirapikan ke pemanggil
} // akhir fungsi readLine()

// Dipanggil runMenuLoop() di src/app.cpp sesudah setiap aksi menu (selain Keluar), supaya hasil aksi
// sempat dibaca sebelum menu ditampilkan lagi.
void waitForEnter() { // waitForEnter() berfungsi untuk menahan layar sampai pengguna menekan Enter
    std::cout << colorize(screenStyle(), STYLE_HINT, "Tekan Enter untuk kembali ke menu..."); // ajakan ditulis dengan gaya STYLE_HINT (kuning bila warna aktif), tanpa '\n'
    std::cout.flush(); // std::cout.flush() memastikan ajakan itu sudah tampil sebelum program menunggu
    std::string discarded; // discarded adalah tempat membuang ketikan; isinya tidak dipakai karena yang ditunggu hanya tombol Enter
    if (!std::getline(std::cin, discarded)) { // !std::getline(...) artinya masukan sudah habis, jadi tidak ada Enter yang bisa ditunggu
        throw InputClosed(); // throw InputClosed(); melempar tanda "masukan habis" ke runApp(), sama seperti di readLine()
    } // akhir if masukan habis
    if (!inputIsInteractive()) { // !inputIsInteractive() artinya masukan dari berkas/pipa, jadi Enter pengguna tidak tampil di layar
        std::cout << '\n'; // '\n' ditulis sendiri supaya baris berikutnya tidak menempel di belakang ajakan "Tekan Enter..."
    } // akhir if pindah baris
} // akhir fungsi waitForEnter()

// Dipakai untuk pilihan menu utama, pilihan harga acuan, dan nomor komponen di src/app.cpp.
int readChoice(const std::string& label, int minValue, int maxValue, bool hasDefault, int defaultValue) { // readChoice(...) berfungsi untuk membaca nomor
    // pilihan dari minValue sampai maxValue; Enter kosong menghasilkan defaultValue bila hasDefault true.
    // while (true) mengulang pertanyaan sampai jawabannya sah; jalan keluarnya hanya return (atau InputClosed yang dilempar readLine() bila masukan habis).
    while (true) { // while (true) artinya syarat perulangan selalu benar, jadi pertanyaan diulang terus sampai salah satu return di bawah dijalankan
        const std::string text = readLine(label); // text adalah jawaban pengguna yang sudah dirapikan oleh readLine()
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya pengguna hanya menekan Enter dan pertanyaan ini punya jawaban bawaan
            return defaultValue; // return defaultValue; Enter kosong diterima sebagai jawaban bawaan
        } // akhir if jawaban bawaan
        int choice = 0; // choice adalah tempat hasil angka pilihan; diisi parseMenuChoice lewat referensi bila teksnya sah
        if (parseMenuChoice(text, minValue, maxValue, choice)) { // parseMenuChoice(...) true artinya text berupa angka di dalam rentang minValue..maxValue
            return choice; // return choice; pilihan sah dikembalikan ke pemanggil dan perulangan berhenti
        } // akhir if pilihan sah
        printError("Pilihan tidak dikenal. Ketik angka " + formatThousands(minValue) + " sampai " // pesan salah menyebut rentang yang benar;
                   + formatThousands(maxValue) + " lalu tekan Enter."); // formatThousands menulis angkanya dengan titik ribuan, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readChoice()

// Dipakai untuk semua konfirmasi di src/app.cpp, misalnya "Simpan komponen ini?" dan "Keluar dari program?".
bool askYesNo(const std::string& question, bool defaultYes) { // askYesNo(...) berfungsi untuk bertanya ya/tidak; hasil true berarti ya
    // Huruf besar pada [Y/t] atau [y/T] menunjukkan jawaban yang dipakai bila hanya menekan Enter.
    const std::string label = question + (defaultYes ? " [Y/t]" : " [y/T]"); // label adalah pertanyaan ditambah " [Y/t]" bila bawaannya ya, atau " [y/T]" bila tidak
    while (true) { // while (true) mengulang pertanyaan sampai jawabannya dikenali
        const std::string text = readLine(label); // text adalah jawaban pengguna yang sudah dirapikan
        if (text.empty()) { // text.empty() artinya pengguna hanya menekan Enter
            return defaultYes; // return defaultYes; Enter kosong dianggap memilih jawaban bawaan (yang ditulis huruf besar)
        } // akhir if Enter kosong
        const YesNoAnswer answer = parseYesNo(text); // answer adalah hasil pengenalan jawaban: ANSWER_YES, ANSWER_NO, atau ANSWER_UNKNOWN (tidak dikenali)
        if (answer == ANSWER_YES) { // answer == ANSWER_YES artinya pengguna mengetik y, ya, atau yes (huruf besar/kecil sama saja)
            return true; // return true; jawabannya ya
        } // akhir if ya
        if (answer == ANSWER_NO) { // answer == ANSWER_NO artinya pengguna mengetik t, tidak, n, atau no
            return false; // return false; jawabannya tidak
        } // akhir if tidak
        printError("Jawab dengan y (ya) atau t (tidak), lalu tekan Enter."); // jawaban tidak dikenali, jadi pesan ini ditampilkan lalu pertanyaan diulang
    } // akhir perulangan while
} // akhir fungsi askYesNo()

// Tiga pembaca data komponen di bawah punya pola yang sama: Enter kosong memakai nilai lama (bila ada),
// selain itu teks diperiksa oleh pemeriksa dari input_parse.h. Pemeriksa mengembalikan teks kosong bila
// sah (dan mengisi angkanya), atau pesan kesalahan yang langsung ditampilkan sebelum pertanyaan diulang.
int readQuantity(const std::string& label, bool hasDefault, int defaultValue) { // readQuantity(...) berfungsi untuk membaca jumlah unit komponen sampai sah
    while (true) { // while (true) mengulang pertanyaan sampai jumlah yang diketik sah
        const std::string text = readLine(label); // text adalah ketikan jumlah yang sudah dirapikan
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya Enter kosong saat mengubah komponen
            return defaultValue; // return defaultValue; jumlah lama dipertahankan
        } // akhir if nilai lama
        int quantity = 0; // quantity adalah tempat hasil angka jumlah; diisi checkQuantity lewat referensi bila sah
        const std::string problem = checkQuantity(text, quantity); // problem adalah pesan kesalahan dari checkQuantity, atau teks kosong bila jumlahnya sah
        if (problem.empty()) { // problem.empty() artinya tidak ada masalah, jadi quantity sudah berisi angka yang benar
            return quantity; // return quantity; jumlah yang sah dikembalikan
        } // akhir if sah
        printError(problem); // printError(problem) menampilkan alasan jawaban ditolak, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readQuantity()

double readUnitWeight(const std::string& label, bool hasDefault, double defaultValue) { // readUnitWeight(...) berfungsi untuk membaca berat per unit (kg) sampai sah
    while (true) { // while (true) mengulang pertanyaan sampai berat yang diketik sah
        const std::string text = readLine(label); // text adalah ketikan berat yang sudah dirapikan (boleh memakai koma atau titik desimal)
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya Enter kosong saat mengubah komponen
            return defaultValue; // return defaultValue; berat lama dipertahankan
        } // akhir if nilai lama
        double weightKg = 0.0; // weightKg adalah tempat hasil berat dalam kg; double karena berat boleh berdesimal
        const std::string problem = checkUnitWeight(text, weightKg); // problem adalah pesan kesalahan dari checkUnitWeight, atau teks kosong bila beratnya sah
        if (problem.empty()) { // problem.empty() artinya berat sah dan weightKg sudah terisi
            return weightKg; // return weightKg; berat yang sah dikembalikan
        } // akhir if sah
        printError(problem); // printError(problem) menampilkan alasan jawaban ditolak, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readUnitWeight()

double readPricePerKg(const std::string& label, bool hasDefault, double defaultValue) { // readPricePerKg(...) berfungsi untuk membaca harga per kg (rupiah) sampai sah
    while (true) { // while (true) mengulang pertanyaan sampai harga yang diketik sah
        const std::string text = readLine(label); // text adalah ketikan harga yang sudah dirapikan
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya Enter kosong; dipakai untuk nilai lama saat mengubah, atau harga acuan katalog
            return defaultValue; // return defaultValue; harga bawaan (harga lama atau harga acuan) diterima
        } // akhir if nilai bawaan
        double pricePerKg = 0.0; // pricePerKg adalah tempat hasil harga per kg; double supaya sama dengan tipe pricePerKg di struct Component
        const std::string problem = checkPricePerKg(text, pricePerKg); // problem adalah pesan kesalahan dari checkPricePerKg, atau teks kosong bila harganya sah
        if (problem.empty()) { // problem.empty() artinya harga sah dan pricePerKg sudah terisi
            return pricePerKg; // return pricePerKg; harga yang sah dikembalikan
        } // akhir if sah
        printError(problem); // printError(problem) menampilkan alasan jawaban ditolak, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readPricePerKg()

// ===============================================================================================================
// BAGIAN 20 - src/report.cpp - penyusunan laporan dari data komponen
// ===============================================================================================================
// Berkas ini berisi isi fungsi-fungsi yang dideklarasikan di include/estima/report.h. Di sinilah
// keluaran yang diminta soal disusun: total berat dan total biaya tiap komponen, total keseluruhan,
// dan komponen dengan biaya produksi tertinggi, ditambah peringkat biaya dan laporan berkas .txt.
// Fungsi di sini tidak menghitung sendiri; semua angka diambil dari fungsi di calculation.h, lalu
// dibentuk menjadi teks (text_format.h) dan digambar sebagai kotak atau tabel (ui.h).

// Lebar laporan berkas dibuat tetap supaya hasilnya sama di komputer mana pun dan tabel
// selalu tampil utuh ketika dibuka di penyunting teks.
// Keempat konstanta angka di bawah ini (sampai WIDE_LAYOUT_MIN_WIDTH) memakai static karena hanya dibutuhkan di berkas ini.
static const int REPORT_FILE_WIDTH = 120; // REPORT_FILE_WIDTH = 120 adalah lebar laporan berkas dalam kolom, tidak bergantung pada lebar jendela terminal
// Panjang batang grafik peringkat (dalam karakter) untuk terminal lebar dan sempit.
// Batang dibuat lebih pendek di terminal sempit supaya kolom grafik tidak menghabiskan tempat
// yang dibutuhkan kolom nama dan biaya.
static const int WIDE_BAR_CELLS = 24; // WIDE_BAR_CELLS = 24 adalah panjang batang terpanjang (milik biaya tertinggi) bila keluaran lebar
static const int NARROW_BAR_CELLS = 12; // NARROW_BAR_CELLS = 12 adalah panjang batang terpanjang bila keluaran sempit, yaitu separuh dari versi lebar
static const int WIDE_LAYOUT_MIN_WIDTH = 80; // WIDE_LAYOUT_MIN_WIDTH = 80 artinya keluaran dianggap lebar bila lebarnya 80 kolom atau lebih

// Pesan untuk daftar kosong. Di sini TIDAK memakai static, karena teks ini sudah diumumkan extern di
// report.h supaya bisa dipakai juga oleh menu di app.cpp; static akan bertentangan dengan deklarasi extern itu.
const char* const EMPTY_LIST_MESSAGE = // EMPTY_LIST_MESSAGE adalah isi dari teks tetap yang diumumkan extern di report.h; kalimatnya ada di baris berikut
    "Belum ada komponen. Pilih menu 1 untuk menambah komponen atau menu 6 untuk memuat data contoh."; // kalimat ini sekaligus memberi tahu pengguna langkah berikutnya

// Mencetak pemberitahuan daftar kosong dengan awalan [i]. Dipisah menjadi fungsi kecil karena
// dipakai oleh printRecap() dan printRanking() dengan bentuk yang sama persis.
static void printEmptyNotice(std::ostream& out, const OutputStyle& style) { // printEmptyNotice() berfungsi untuk mencetak EMPTY_LIST_MESSAGE ke out dengan gaya redup
    // std::string("[i] ") membungkus teks pertama menjadi std::string, karena dua const char* tidak bisa disambung
    // dengan + (lihat ui.cpp).
    printWrapped(out, style, STYLE_DIM, std::string("[i] ") + EMPTY_LIST_MESSAGE, 4); // angka 4 adalah lebar "[i] ", jadi baris lanjutan menjorok tepat di bawah awal kalimat
} // akhir fungsi printEmptyNotice()

// Tanggal dan jam saat ini dalam bentuk "10-11-2026 21:45" untuk kepala laporan berkas.
// Hasilnya std::string supaya pemanggil bisa langsung menulisnya dengan <<; bila jam komputer
// tidak bisa dibaca, hasilnya "-" sehingga laporan tetap tersimpan.
static std::string currentDateTime() { // currentDateTime() berfungsi untuk membuat teks tanggal dan jam saat laporan dibuat
    // std::time(NULL) berfungsi untuk membaca waktu sekarang dalam bentuk mentah bertipe std::time_t (biasanya jumlah detik sejak 1 Januari 1970).
    // Argumen NULL artinya hasilnya cukup dikembalikan, tidak perlu juga ditulis ke variabel lain.
    const std::time_t now = std::time(NULL); // now adalah waktu sekarang dalam detik; const karena nilainya hanya dibaca
    // std::localtime(&now) berfungsi untuk memecah detik mentah menjadi tanggal, bulan, tahun, jam, dan menit menurut zona waktu komputer.
    // Hasilnya pointer ke struct std::tm milik pustaka standar, atau NULL bila gagal; &now adalah alamat variabel now.
    const std::tm* local = std::localtime(&now); // local menunjuk ke hasil pecahan waktu; isinya hanya dibaca, jadi memakai const std::tm*
    char buffer[32]; // buffer adalah array 32 char tempat teks tanggal ditulis; "dd-mm-yyyy hh:mm" hanya 16 karakter, jadi ruangnya lebih dari cukup
    // std::strftime(tujuan, ukuran, pola, waktu) berfungsi untuk menulis waktu ke array char mengikuti pola: %d tanggal, %m bulan,
    // %Y tahun empat angka, %H jam 00-23, %M menit. Hasilnya banyak karakter yang ditulis, atau 0 bila tidak muat.
    if (local == NULL || std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M", local) == 0) { // bila localtime gagal ATAU strftime gagal, tanggal tidak bisa dibuat;
        // || berhenti di syarat pertama yang benar, jadi strftime tidak pernah dipanggil dengan local bernilai NULL. sizeof(buffer) = 32, batas tulis aman.
        return "-"; // return "-" artinya tanggal diganti tanda strip, supaya kegagalan membaca jam tidak menggagalkan penyimpanan laporan
    } // akhir if pemeriksaan kegagalan
    return buffer; // return buffer mengubah isi array char menjadi std::string (disalin), jadi aman walaupun buffer hilang saat fungsi selesai
} // akhir fungsi currentDateTime()

// Menyusun satu baris daftar komponen termahal, berbentuk "- Nama : RpX (Y% dari total biaya)". Dipisah menjadi
// fungsi kecil karena printRecap() memerlukannya dua kali: untuk mengukur lebar baris, lalu untuk mengisi kotak.
// Parameter nameWidth adalah lebar kolom nama; bila nilainya tidak melebihi lebar nama, nama ditulis apa adanya.
static std::string topComponentLine(const Component& component, double totalCost, std::size_t nameWidth) { // topComponentLine() berfungsi untuk membuat teks satu baris daftar itu
    const double cost = calculateTotalCost(component); // cost adalah total biaya komponen ini, dihitung oleh calculateTotalCost()
    // padRight(component.name, nameWidth) menambah spasi di kanan nama sampai selebar nameWidth, jadi tanda : sejajar.
    return "- " + padRight(component.name, nameWidth) + " : " + formatRupiah(cost) + " (" // bagian pertama: tanda "- ", nama, " : ", dan biaya dalam rupiah;
           + formatPercent(costSharePercent(cost, totalCost)) + " dari total biaya)"; // costSharePercent() menghitung porsi biaya dalam persen
    // dan formatPercent() menulisnya dengan satu desimal, misalnya "66,3%".
} // akhir fungsi topComponentLine()

// [Kriteria soal: keluaran rapi dan informatif] Keluaran (1) sampai (4) yang diminta soal dicetak di sini, dalam tabel
// dan kotak yang kolom serta tanda titik duanya rata (daftar komponen termahal diratakan selama barisnya muat di kotak).
// printRecap() menyusun tiga kotak berturut-turut: tabel rekap per komponen, total keseluruhan,
// dan daftar komponen berbiaya tertinggi. Semua angka diminta dari fungsi di calculation.h, jadi
// rumusnya hanya ada di satu tempat dan hasil di layar sama dengan hasil di berkas laporan.
void printRecap(std::ostream& out, const OutputStyle& style, const ComponentList& list) { // printRecap() berfungsi untuk mencetak rekap lengkap seluruh komponen di list
    if (list.count == 0) { // list.count == 0 artinya belum ada komponen, jadi tidak ada yang bisa direkap
        printEmptyNotice(out, style); // printEmptyNotice() menampilkan pesan daftar kosong beserta saran menu 1 atau 6
        return; // return di fungsi void artinya berhenti di sini tanpa menggambar tabel kosong
    } // akhir if daftar kosong

    // Array topIndexes menampung posisi komponen termahal. Ukurannya MAX_COMPONENTS karena pada
    // keadaan paling ekstrem semua komponen bisa berbiaya sama.
    int topIndexes[MAX_COMPONENTS]; // topIndexes adalah array int berukuran 100 yang diisi oleh findMostExpensive() di baris berikut
    const int topCount = findMostExpensive(list.items, list.count, topIndexes); // topCount adalah banyaknya komponen termahal (bisa lebih dari satu bila biayanya sama)
    const double totalWeight = sumTotalWeight(list.items, list.count); // totalWeight adalah total berat seluruh komponen dalam kg, bertipe double karena ada desimal gram
    const double totalCost = sumTotalCost(list.items, list.count); // totalCost adalah total biaya seluruh komponen dalam rupiah; double karena bisa melewati batas int
    const long totalUnits = sumQuantity(list.items, list.count); // totalUnits adalah jumlah unit seluruh komponen; bertipe long mengikuti hasil sumQuantity()

    // Tabel rekap: satu baris untuk tiap komponen, berisi data masukan dan hasil hitungnya.
    // Kolom disusun dulu (judul dan perataannya), lalu baris diisi lewat perulangan.
    Table table; // table adalah struct Table dari ui.h; konstruktornya membuat kolom dan baris kosong serta hasFooter = false
    table.columns.push_back(TableColumn("No", true)); // kolom "No"; true artinya rata kanan karena isinya angka
    table.columns.push_back(TableColumn("Komponen", false)); // kolom "Komponen"; false artinya rata kiri karena isinya nama (teks)
    table.columns.push_back(TableColumn("Jumlah", true)); // kolom "Jumlah" (unit), rata kanan
    table.columns.push_back(TableColumn("Berat/unit", true)); // kolom "Berat/unit" (kg per unit), rata kanan
    table.columns.push_back(TableColumn("Harga/kg", true)); // kolom "Harga/kg" (rupiah per kg), rata kanan
    table.columns.push_back(TableColumn("Total berat", true)); // kolom "Total berat" = jumlah x berat per unit, keluaran (1) soal
    table.columns.push_back(TableColumn("Total biaya", true)); // kolom "Total biaya" = total berat x harga per kg, keluaran (2) soal
    for (int i = 0; i < list.count; ++i) { // perulangan for dari i = 0 sampai list.count - 1, yaitu setiap elemen array yang terisi
        const Component& component = list.items[i]; // component adalah nama lain (referensi) untuk list.items[i], dipakai supaya data tidak disalin dan baris tetap pendek
        TableRow row; // row adalah satu baris tabel yang masih kosong; gaya awalnya STYLE_NORMAL dari konstruktor TableRow
        row.cells.push_back(formatThousands(i + 1)); // nomor urut ditulis i + 1 karena posisi array mulai dari 0, sedangkan pengguna menghitung dari 1
        row.cells.push_back(component.name); // nama komponen apa adanya
        row.cells.push_back(formatThousands(component.quantity)); // jumlah unit dengan titik ribuan, misalnya 1.000
        row.cells.push_back(formatWeight(component.unitWeightKg)); // berat per unit dengan tiga desimal, misalnya 1,850 kg
        row.cells.push_back(formatRupiah(component.pricePerKg)); // harga per kg dalam bentuk Rp18.500
        row.cells.push_back(formatWeight(calculateTotalWeight(component))); // total berat komponen ini, dihitung oleh calculateTotalWeight()
        row.cells.push_back(formatRupiah(calculateTotalCost(component))); // total biaya komponen ini, dihitung oleh calculateTotalCost()
        // Baris komponen termahal diberi gaya sorot.
        for (int k = 0; k < topCount; ++k) { // perulangan for memeriksa setiap posisi di topIndexes
            if (topIndexes[k] == i) { // topIndexes[k] == i artinya komponen ke-i termasuk yang biayanya tertinggi
                row.style = STYLE_HIGHLIGHT; // STYLE_HIGHLIGHT membuat baris ini tampil kuning tebal di layar (di berkas tanpa warna)
            } // akhir if komponen termahal
        } // akhir perulangan for k
        table.rows.push_back(row); // baris yang sudah lengkap dimasukkan ke ujung daftar baris tabel
    } // akhir perulangan for i
    // Baris penutup (footer) berisi total. Kolom yang tidak punya total diisi teks kosong, supaya
    // banyaknya sel tetap tujuh dan setiap angka berada di bawah kolomnya sendiri.
    table.hasFooter = true; // hasFooter = true artinya tabel ini punya baris total di bawah garis pemisah
    table.footer.cells.push_back(""); // kolom "No" pada baris total dikosongkan
    table.footer.cells.push_back("TOTAL"); // kolom "Komponen" berisi tulisan TOTAL
    table.footer.cells.push_back(formatThousands(static_cast<double>(totalUnits))); // jumlah seluruh unit;
    // static_cast<double> mengubah long menjadi double, tipe parameter yang diminta formatThousands().
    table.footer.cells.push_back(""); // kolom "Berat/unit" dikosongkan karena menjumlahkan berat per unit tidak bermakna
    table.footer.cells.push_back(""); // kolom "Harga/kg" dikosongkan karena alasan yang sama
    table.footer.cells.push_back(formatWeight(totalWeight)); // total berat keseluruhan, keluaran (3) soal
    table.footer.cells.push_back(formatRupiah(totalCost)); // total biaya keseluruhan, keluaran (3) soal
    drawTableBox(out, style, "Rekap Material dan Biaya", table, std::vector<StyledLine>()); // drawTableBox() menggambar tabel di dalam kotak berjudul;
    // std::vector<StyledLine>() adalah vector kosong yang dibuat langsung di tempat, artinya tabel ini tidak punya catatan di bawahnya.

    // Kotak kedua: ringkasan total. Setiap baris berbentuk "Label : nilai" dengan titik dua
    // yang lurus; bila baris harus dilipat, drawBox() melanjutkannya di bawah awal nilainya
    // (selama bagian label tidak lebih dari separuh lebar kotak).
    std::vector<StyledLine> totals; // totals adalah daftar baris isi kotak "Total Keseluruhan"
    totals.push_back(StyledLine("Jenis komponen : " + formatThousands(list.count), STYLE_NORMAL)); // banyaknya jenis komponen, yaitu isi list.count
    totals.push_back(StyledLine("Jumlah unit    : " + formatThousands(static_cast<double>(totalUnits)), STYLE_NORMAL)); // jumlah semua unit dari semua komponen
    totals.push_back(StyledLine("Total berat    : " + formatWeight(totalWeight), STYLE_BOLD)); // total berat ditebalkan (STYLE_BOLD) karena inilah hasil utama
    totals.push_back(StyledLine("Total biaya    : " + formatRupiah(totalCost), STYLE_BOLD)); // total biaya juga ditebalkan; baris ini diperiksa oleh uji sesi asap
    drawBox(out, style, "Total Keseluruhan", totals); // drawBox() menggambar kotak berjudul berisi baris-baris totals

    // Kotak ketiga: komponen dengan biaya produksi tertinggi (keluaran (4) soal), lengkap dengan
    // porsinya terhadap total biaya.
    std::vector<StyledLine> top; // top adalah daftar baris isi kotak "Biaya Produksi Tertinggi"
    if (topCount > 1) { // topCount > 1 artinya ada beberapa komponen yang biaya tertingginya sama persis
        top.push_back(StyledLine("Ada " + formatThousands(topCount) + " komponen dengan biaya tertinggi yang sama:", STYLE_NORMAL)); // kalimat pembuka
        // supaya pengguna tahu bahwa daftar di bawahnya berisi lebih dari satu komponen termahal.
    } // akhir if lebih dari satu komponen termahal
    // Bila beberapa komponen sama-sama termahal, nama yang lebih pendek ditambah spasi sampai selebar nama
    // terpanjang di antara mereka, supaya tanda titik dua di daftar berada pada kolom yang sama. Bila hanya
    // satu komponen, lebarnya sama dengan lebar namanya sendiri, jadi barisnya tidak berubah.
    std::size_t nameWidth = 0; // nameWidth adalah lebar nama terpanjang di antara komponen termahal; mulai dari 0 lalu diperbesar di perulangan berikut
    for (int k = 0; k < topCount; ++k) { // perulangan for mengukur nama setiap komponen termahal
        const std::size_t currentWidth = displayWidth(list.items[topIndexes[k]].name); // currentWidth adalah lebar nama komponen termahal ke-k dalam kolom layar
        if (currentWidth > nameWidth) { // currentWidth > nameWidth artinya nama ini lebih lebar daripada yang terlebar sejauh ini
            nameWidth = currentWidth; // nameWidth diganti dengan lebar nama ini
        } // akhir if nama lebih lebar
    } // akhir perulangan pengukur lebar nama
    // Perataan itu hanya dipakai bila baris yang sudah diratakan masih muat di dalam kotak. Di terminal sempit, satu
    // nama yang panjang akan membuat semua baris ikut terlipat; pada keadaan itu nameWidth dikembalikan ke 0, sehingga
    // padRight() tidak menambah spasi apa pun dan setiap nama ditulis apa adanya.
    for (int k = 0; k < topCount; ++k) { // perulangan for memeriksa lebar baris setiap komponen termahal bila namanya diratakan
        if (displayWidth(topComponentLine(list.items[topIndexes[k]], totalCost, nameWidth)) > boxInnerLimit(style)) { // boxInnerLimit(style) adalah lebar isi kotak
            // terbesar pada lebar keluaran ini; baris yang lebih lebar dari itu pasti dilipat oleh drawBox().
            nameWidth = 0; // nameWidth = 0 artinya perataan dibatalkan untuk semua baris
        } // akhir if baris tidak muat
    } // akhir perulangan pemeriksa lebar baris
    for (int k = 0; k < topCount; ++k) { // perulangan for atas setiap komponen termahal yang ditemukan
        // topIndexes[k] berisi posisi komponen di array, lalu dipakai sebagai nomor elemen list.items.
        top.push_back(StyledLine(topComponentLine(list.items[topIndexes[k]], totalCost, nameWidth), STYLE_HIGHLIGHT)); // STYLE_HIGHLIGHT menyorot baris ini
    } // akhir perulangan for k
    drawBox(out, style, "Biaya Produksi Tertinggi", top); // kotak terakhir rekap
} // akhir fungsi printRecap()

// printRanking() menampilkan semua komponen dari biaya terbesar ke terkecil, dengan porsi tiap
// komponen dan grafik batang. Data di list tidak diurutkan ulang; yang diurutkan adalah array
// nomor posisi order[], jadi urutan isian pengguna tetap utuh untuk menu lain.
void printRanking(std::ostream& out, const OutputStyle& style, const ComponentList& list) { // printRanking() berfungsi untuk mencetak tabel peringkat biaya
    if (list.count == 0) { // list.count == 0 artinya belum ada komponen untuk diperingkat
        printEmptyNotice(out, style); // menampilkan pesan daftar kosong
        return; // berhenti di sini; tanpa pemeriksaan ini order[0] di bawah akan dibaca padahal tidak terisi
    } // akhir if daftar kosong

    // order[] berisi posisi komponen yang sudah diurutkan dari biaya terbesar.
    // Ukurannya MAX_COMPONENTS (100) karena rankByCost() mengisi satu elemen untuk setiap komponen.
    int order[MAX_COMPONENTS]; // order adalah array int penampung nomor posisi hasil pengurutan
    rankByCost(list.items, list.count, order); // rankByCost() mengisi order[] dengan insertion sort berdasarkan total biaya
    const double totalCost = sumTotalCost(list.items, list.count); // totalCost adalah total biaya semua komponen, pembagi untuk kolom "Porsi"
    const double highestCost = calculateTotalCost(list.items[order[0]]); // highestCost adalah biaya komponen peringkat pertama (order[0]), patokan panjang batang
    // ?: memilih 24 bila lebar keluaran 80 kolom atau lebih, dan 12 bila kurang dari itu.
    const int barCells = style.width >= WIDE_LAYOUT_MIN_WIDTH ? WIDE_BAR_CELLS : NARROW_BAR_CELLS; // barCells adalah panjang batang terpanjang untuk keluaran ini

    Table table; // table adalah tabel peringkat yang masih kosong
    table.columns.push_back(TableColumn("No", true)); // kolom "No" berisi nomor peringkat, rata kanan
    table.columns.push_back(TableColumn("Komponen", false)); // kolom "Komponen" berisi nama, rata kiri
    table.columns.push_back(TableColumn("Total biaya", true)); // kolom "Total biaya", rata kanan
    table.columns.push_back(TableColumn("Porsi", true)); // kolom "Porsi" berisi persen biaya terhadap total, rata kanan
    table.columns.push_back(TableColumn("Grafik", false)); // kolom "Grafik" berisi batang, rata kiri supaya semua batang mulai dari titik yang sama

    int rank = 0; // rank adalah nomor peringkat yang sedang berlaku; nilai awal 0 akan langsung diganti di putaran pertama
    double previousCost = -1.0; // previousCost adalah biaya komponen sebelumnya; -1 dipilih karena tidak ada biaya negatif, jadi komponen pertama pasti mendapat peringkat 1
    for (int i = 0; i < list.count; ++i) { // perulangan for menelusuri komponen sesuai urutan peringkat, dari biaya terbesar
        const Component& component = list.items[order[i]]; // order[i] adalah posisi komponen peringkat ke-(i + 1) di array list.items
        const double cost = calculateTotalCost(component); // cost adalah total biaya komponen ini
        // Komponen dengan biaya sama mendapat peringkat yang sama; peringkat berikutnya
        // melompat sesuai banyaknya komponen di atasnya (1, 1, 3, ...).
        if (cost != previousCost) { // cost != previousCost artinya biayanya berbeda dari komponen sebelumnya; aman memakai != karena biaya selalu rupiah bulat
            rank = i + 1; // peringkat baru sama dengan nomor urutnya (i mulai dari 0, peringkat mulai dari 1)
            previousCost = cost; // biaya ini diingat untuk dibandingkan dengan komponen berikutnya
        } // akhir if biaya berbeda
        // Panjang batang sebanding dengan biaya terhadap biaya tertinggi; komponen yang
        // biayanya tidak nol selalu mendapat paling sedikit satu kotak.
        int cells = 0; // cells adalah panjang batang komponen ini dalam karakter; mulai 0
        if (highestCost > 0.0) { // highestCost > 0.0 dicek dulu supaya tidak terjadi pembagian dengan nol bila semua biaya Rp0
            cells = static_cast<int>(std::floor(cost / highestCost * barCells + 0.5)); // cost / highestCost adalah perbandingan 0 sampai 1, dikali barCells;
            // menambah 0,5 lalu std::floor sama dengan membulatkan ke bilangan terdekat, kemudian static_cast<int> mengubahnya ke int.
            if (cells < 1 && cost > 0.0) { // biaya kecil tetapi bukan nol bisa terbulatkan menjadi 0 kotak
                cells = 1; // diberi satu kotak supaya terlihat bahwa komponen itu tetap punya biaya
            } // akhir if batang minimal satu kotak
        } // akhir if highestCost > 0
        TableRow row; // row adalah satu baris tabel peringkat
        row.cells.push_back(formatThousands(rank)); // nomor peringkat (biaya sama berarti nomor sama)
        row.cells.push_back(component.name); // nama komponen
        row.cells.push_back(formatRupiah(cost)); // total biaya komponen
        row.cells.push_back(formatPercent(costSharePercent(cost, totalCost))); // porsi biaya terhadap total, misalnya 66,3%
        row.cells.push_back(repeatText(barGlyph(style), static_cast<std::size_t>(cells))); // batang grafik: karakter blok (UTF-8) atau # (ASCII) diulang cells kali;
        // static_cast<std::size_t> dipakai karena repeatText() meminta jumlah ulangan bertipe std::size_t, sedangkan cells bertipe int.
        if (rank == 1) { // rank == 1 artinya komponen ini berada di peringkat teratas (bisa lebih dari satu bila biayanya sama)
            row.style = STYLE_HIGHLIGHT; // peringkat pertama disorot, sama seperti komponen termahal di rekap
        } // akhir if peringkat pertama
        table.rows.push_back(row); // baris dimasukkan ke tabel
    } // akhir perulangan for i

    // Catatan di bawah tabel menjelaskan arti kolom "Porsi" dan "Grafik", karena keduanya
    // dihitung dengan pembanding yang berbeda (total biaya dan biaya tertinggi).
    std::vector<StyledLine> notes; // notes adalah daftar baris catatan di bawah tabel
    notes.push_back(StyledLine("Porsi = bagian biaya komponen dari total biaya. Grafik membandingkan tiap komponen dengan biaya tertinggi.", // teks catatan;
                               STYLE_DIM)); // STYLE_DIM membuat catatan tampil redup (abu-abu) supaya tidak bersaing dengan isi tabel
    drawTableBox(out, style, "Peringkat Biaya Produksi", table, notes); // menggambar tabel peringkat beserta catatannya di dalam kotak
} // akhir fungsi printRanking()

// printCatalog() menampilkan daftar harga acuan material. Isinya dibaca dari array konstan
// MATERIAL_CATALOG dengan perulangan, jadi baris yang ditambah ke katalog otomatis ikut tampil.
void printCatalog(std::ostream& out, const OutputStyle& style) { // printCatalog() berfungsi untuk mencetak tabel harga acuan material (menu 7)
    Table table; // table adalah tabel katalog yang masih kosong
    table.columns.push_back(TableColumn("No", true)); // kolom "No" berisi nomor pilihan material, rata kanan
    table.columns.push_back(TableColumn("Material", false)); // kolom "Material" berisi nama bahan, rata kiri
    table.columns.push_back(TableColumn("Harga/kg", true)); // kolom "Harga/kg" berisi harga acuan, rata kanan
    table.columns.push_back(TableColumn("Standar", false)); // kolom "Standar" berisi kode standar bahan, misalnya JIS G4051
    table.columns.push_back(TableColumn("Kegunaan umum", false)); // kolom "Kegunaan umum" berisi contoh pemakaian bahan
    for (int i = 0; i < MATERIAL_COUNT; ++i) { // perulangan for atas setiap baris katalog; MATERIAL_COUNT dihitung dari ukuran array
        TableRow row; // row adalah satu baris tabel katalog
        row.cells.push_back(formatThousands(i + 1)); // nomor mulai dari 1, sama dengan nomor yang dipilih saat menambah komponen
        row.cells.push_back(MATERIAL_CATALOG[i].name); // nama material; const char* otomatis diubah menjadi std::string saat dimasukkan
        row.cells.push_back(formatRupiah(MATERIAL_CATALOG[i].pricePerKg)); // harga acuan per kg dalam bentuk rupiah
        row.cells.push_back(MATERIAL_CATALOG[i].standard); // kode standar bahan
        row.cells.push_back(MATERIAL_CATALOG[i].typicalUse); // kegunaan umum bahan
        table.rows.push_back(row); // baris dimasukkan ke tabel
    } // akhir perulangan for

    // Catatan sumber data ditampilkan supaya pengguna tahu harga itu berasal dari periode mana.
    std::vector<StyledLine> notes; // notes adalah daftar baris catatan di bawah tabel katalog
    notes.push_back(StyledLine(std::string("Periode data: ") + CATALOG_PERIOD + ".", STYLE_DIM)); // periode harga; std::string() dipakai agar teks bisa disambung dengan +
    notes.push_back(StyledLine(CATALOG_SOURCE_NOTE, STYLE_DIM)); // keterangan sumber harga dari material_catalog.cpp
    drawTableBox(out, style, "Harga Acuan Material", table, notes); // menggambar tabel katalog beserta catatannya
} // akhir fungsi printCatalog()

// printComponentSummary() menampilkan data satu komponen beserta hasil hitungnya sebelum disimpan.
// Bentuknya kotak "Label : nilai", bukan tabel, karena hanya berisi satu komponen.
void printComponentSummary(std::ostream& out, const OutputStyle& style, const Component& component) { // printComponentSummary() mencetak ringkasan satu komponen
    std::vector<StyledLine> lines; // lines adalah daftar baris isi kotak ringkasan
    lines.push_back(StyledLine("Nama        : " + component.name, STYLE_NORMAL)); // nama komponen
    lines.push_back(StyledLine("Jumlah      : " + formatThousands(component.quantity) + " unit", STYLE_NORMAL)); // jumlah unit
    lines.push_back(StyledLine("Berat/unit  : " + formatWeight(component.unitWeightKg), STYLE_NORMAL)); // berat per unit
    lines.push_back(StyledLine("Harga/kg    : " + formatRupiah(component.pricePerKg), STYLE_NORMAL)); // harga per kg
    lines.push_back(StyledLine("Total berat : " + formatWeight(calculateTotalWeight(component)), STYLE_BOLD)); // total berat hasil hitung, ditebalkan
    lines.push_back(StyledLine("Total biaya : " + formatRupiah(calculateTotalCost(component)), STYLE_BOLD)); // total biaya hasil hitung, ditebalkan
    drawBox(out, style, "Ringkasan Komponen", lines); // menggambar kotak ringkasan
} // akhir fungsi printComponentSummary()

// printComponentPicker() menampilkan daftar pendek untuk memilih komponen yang akan diubah atau
// dihapus. Isinya cukup tiga kolom (nomor, nama, total biaya), karena gunanya hanya untuk memilih.
void printComponentPicker(std::ostream& out, const OutputStyle& style, const ComponentList& list) { // printComponentPicker() berfungsi untuk mencetak daftar pilihan komponen
    Table table; // table adalah tabel pilihan yang masih kosong
    table.columns.push_back(TableColumn("No", true)); // kolom "No" berisi nomor yang diketik pengguna untuk memilih
    table.columns.push_back(TableColumn("Komponen", false)); // kolom "Komponen" berisi nama
    table.columns.push_back(TableColumn("Total biaya", true)); // kolom "Total biaya" membantu pengguna mengenali komponen yang dimaksud
    for (int i = 0; i < list.count; ++i) { // perulangan for atas setiap komponen yang terisi
        TableRow row; // row adalah satu baris tabel pilihan
        row.cells.push_back(formatThousands(i + 1)); // nomor i + 1; pemanggil di app.cpp mengurangi lagi 1 untuk mendapat posisi array
        row.cells.push_back(list.items[i].name); // nama komponen ke-i
        row.cells.push_back(formatRupiah(calculateTotalCost(list.items[i]))); // total biaya komponen ke-i
        table.rows.push_back(row); // baris dimasukkan ke tabel
    } // akhir perulangan for
    drawTableBox(out, style, "Daftar Komponen", table, std::vector<StyledLine>()); // menggambar tabel pilihan tanpa catatan (vector kosong)
} // akhir fungsi printComponentPicker()

// saveReport() menulis laporan ke berkas teks. Isinya disusun oleh printRecap() dan printRanking()
// yang sama dengan tampilan layar, hanya tujuannya berkas dan gayanya polos. Fungsi ini tidak
// mencetak pesan sendiri: hasil bool dan errorMessage diserahkan ke menu yang memanggilnya.
bool saveReport(const ComponentList& list, const std::string& fileName, std::string& errorMessage) { // saveReport() berfungsi untuk menyimpan laporan ke berkas fileName
    // std::ofstream adalah aliran keluaran ke berkas: membuka (atau membuat) berkas, lalu isinya ditulis dengan << seperti std::cout.
    // Bila berkasnya sudah ada, isi lamanya dikosongkan dulu. Pasangannya std::ifstream membuka berkas untuk DIBACA; program ini
    // memakainya di fileExists() pada src/app.cpp untuk mengecek apakah berkas sudah ada. Keduanya tersedia lewat <fstream>.
    // Karena std::ofstream adalah salah satu jenis std::ostream, variabel file bisa dikirim ke parameter std::ostream& milik printRecap().
    std::ofstream file(fileName.c_str()); // file membuka berkas bernama fileName; .c_str() memberi bentuk const char* karena C++98 belum menerima std::string di sini
    if (!file) { // !file bernilai benar bila berkas gagal dibuka, misalnya folder tidak boleh ditulisi
        errorMessage = "Berkas tidak bisa dibuat. Periksa izin tulis di folder ini."; // sebab kegagalan dititipkan ke errorMessage untuk ditampilkan pemanggil
        return false; // false artinya laporan tidak tersimpan
    } // akhir if gagal membuka berkas

    // Laporan berkas memakai gaya polos: tanpa kode warna dan dengan garis ASCII, supaya terbaca
    // di penyunting teks apa pun. Isinya disusun oleh fungsi yang sama dengan tampilan layar.
    const OutputStyle style = plainStyle(REPORT_FILE_WIDTH); // style adalah gaya polos selebar 120 kolom: useColor dan useUnicode bernilai false
    file << APP_NAME << ' ' << APP_VERSION << " - " << APP_TAGLINE << '\n'; // baris kepala laporan: nama, versi, dan slogan program; ' ' adalah satu karakter spasi
    file << "Laporan dibuat : " << currentDateTime() << '\n'; // baris kedua berisi tanggal dan jam laporan dibuat
    file << '\n'; // baris kosong sebagai pemisah
    printRecap(file, style, list); // rekap lengkap ditulis ke berkas, bukan ke layar, karena out yang dikirim adalah file
    file << '\n'; // baris kosong pemisah antara rekap dan peringkat
    printRanking(file, style, list); // peringkat biaya ditulis ke berkas
    file << '\n'; // baris kosong sebelum catatan penutup
    file << "Catatan: harga per kg adalah harga yang dipakai saat laporan dibuat (harga acuan atau harga yang diketik sendiri).\n"; // catatan penutup laporan;
    // harga per kg di laporan adalah harga yang tersimpan di daftar komponen (diambil dari harga acuan atau diketik pengguna), bukan harga pasar terbaru.

    // close() mengirim sisa tulisan yang masih tertahan di memori ke berkas lalu menutup berkasnya. Berkas ditutup di sini
    // (tidak menunggu ditutup otomatis di akhir fungsi) supaya kegagalan menulis maupun kegagalan menutup, misalnya karena
    // ruang penyimpanan penuh, sudah ketahuan saat diperiksa di bawah.
    file.close(); // file.close() menulis semua sisa isi laporan dan menutup berkas sekarang juga
    if (!file) { // !file bernilai benar bila ada penulisan yang gagal sejak berkas dibuka, atau berkasnya gagal ditutup
        errorMessage = "Penulisan berkas gagal. Periksa sisa ruang penyimpanan lalu coba lagi."; // sebab kegagalan untuk pemanggil
        return false; // false artinya laporan tidak tersimpan dengan utuh
    } // akhir if penulisan gagal
    return true; // true artinya laporan tersimpan
} // akhir fungsi saveReport()

// ===============================================================================================================
// BAGIAN 21 - src/app.cpp - alur utama program: menu dan aksi tiap pilihannya
// ===============================================================================================================
// Berkas ini adalah "pengatur lalu lintas" program: menampilkan menu, membaca pilihan, lalu menjalankan aksi yang sesuai.
// Rumus hitungan, pemeriksaan ketikan, dan cara menggambar tabel tidak ditulis di sini; semuanya dipanggil dari modul lain
// lewat header-nya, sehingga isi berkas ini bisa dibaca sebagai urutan langkah program dari awal sampai keluar.

// Nomor menu diberi nama supaya percabangan switch di bawah terbaca maksudnya
// (case MENU_ADD lebih jelas daripada case 1).
// Nilainya ditulis eksplisit (= 0, = 1, ...) karena angka inilah yang diketik pengguna, jadi harus sama persis dengan nomor
// yang tampil di layar. MENU_EXIT bernilai 0 dan MENU_SAVE 8 (yang terbesar), sehingga rentang pilihan yang sah cukup
// ditulis "MENU_EXIT sampai MENU_SAVE".
enum MenuChoice { // enum MenuChoice adalah daftar nama untuk sembilan nomor menu utama.
    MENU_EXIT = 0, // MENU_EXIT = 0 adalah nomor menu Keluar.
    MENU_ADD = 1, // MENU_ADD = 1 adalah nomor menu Tambah komponen.
    MENU_RECAP = 2, // MENU_RECAP = 2 adalah nomor menu Rekap material & biaya.
    MENU_RANKING = 3, // MENU_RANKING = 3 adalah nomor menu Peringkat biaya.
    MENU_EDIT = 4, // MENU_EDIT = 4 adalah nomor menu Ubah komponen.
    MENU_DELETE = 5, // MENU_DELETE = 5 adalah nomor menu Hapus komponen.
    MENU_SAMPLE = 6, // MENU_SAMPLE = 6 adalah nomor menu Muat data contoh.
    MENU_CATALOG = 7, // MENU_CATALOG = 7 adalah nomor menu Harga acuan material.
    MENU_SAVE = 8 // MENU_SAVE = 8 adalah nomor menu Simpan laporan; tanpa koma karena ini anggota terakhir.
}; // akhir enum MenuChoice

// Satu butir menu: nomor, nama, dan keterangan singkatnya.
// Ketiganya dibungkus struct supaya data milik satu butir selalu berjalan bersama, alasan yang sama dengan struct Component.
// Teksnya bertipe const char* (teks tetap) karena isi menu sudah diketahui saat program ditulis dan tidak pernah berubah.
struct MenuItem { // struct MenuItem adalah record untuk satu baris menu utama.
    int number; // number adalah nomor yang diketik pengguna untuk memilih butir ini; diisi nilai enum MenuChoice, yang otomatis bisa disimpan sebagai int.
    const char* label; // label adalah nama menu yang tampil, misalnya "Tambah komponen".
    const char* description; // description adalah keterangan singkat yang ditulis di samping nama bila terminal cukup lebar.
}; // akhir struct MenuItem

// Daftar menu disimpan sebagai array struct. Menu digambar dengan satu perulangan atas array
// ini, jadi menambah atau mengubah butir menu cukup dilakukan di sini.
// Ukuran di dalam [] sengaja dikosongkan: compiler menghitungnya sendiri dari banyaknya isi, yaitu 9 butir.
// Isi array struct ditulis dengan kurung kurawal bersarang: { } paling luar membungkus seluruh array, dan setiap { } di
// dalamnya mengisi satu MenuItem menurut urutan anggotanya (number, label, description).
// Array ini khusus untuk app.cpp (hanya berkas ini yang memakainya) dan isinya tetap selama program berjalan, karena menu tidak berubah.
// Urutan baris di sini adalah urutan tampil di layar, karena itu Keluar berada paling bawah walaupun nomornya 0.
static const MenuItem MAIN_MENU[] = { // MAIN_MENU[] adalah array berisi 9 butir menu utama.
    { MENU_ADD, "Tambah komponen", "Isi nama, jumlah, berat per unit, dan harga per kg" }, // butir [1]: menambah komponen baru (actionAdd).
    { MENU_RECAP, "Rekap material & biaya", "Total berat dan biaya tiap komponen serta keseluruhan" }, // butir [2]: rekap lengkap (actionRecap).
    { MENU_RANKING, "Peringkat biaya", "Urutan komponen dari biaya terbesar, dengan grafik" }, // butir [3]: peringkat biaya (actionRanking).
    { MENU_EDIT, "Ubah komponen", "Memperbaiki data komponen yang sudah diisi" }, // butir [4]: mengubah komponen (actionEdit).
    { MENU_DELETE, "Hapus komponen", "Membuang satu komponen dari daftar" }, // butir [5]: menghapus komponen (actionDelete).
    { MENU_SAMPLE, "Muat data contoh", "Gear, shaft, bracket, housing, dan bushing siap hitung" }, // butir [6]: memuat data contoh (actionLoadSample).
    { MENU_CATALOG, "Harga acuan material", "Daftar harga per kg beserta periode datanya" }, // butir [7]: katalog harga (actionCatalog).
    { MENU_SAVE, "Simpan laporan", "Menulis rekap dan peringkat ke berkas .txt" }, // butir [8]: menyimpan laporan ke berkas (actionSave).
    { MENU_EXIT, "Keluar", "Menutup program" } // butir [0]: keluar dari program (confirmExit); tanpa koma karena elemen terakhir.
}; // akhir array MAIN_MENU
static const std::size_t MAIN_MENU_COUNT = sizeof(MAIN_MENU) / sizeof(MAIN_MENU[0]); // MAIN_MENU_COUNT adalah banyaknya butir menu (9): ukuran seluruh array
// dibagi ukuran satu elemen. Dihitung begini supaya angkanya otomatis benar bila butir menu ditambah atau dibuang.

// Nama berkas laporan bawaan, dipakai bila pengguna hanya menekan Enter saat ditanya nama berkas (menu 8).
// Dijadikan konstanta karena dipakai di dua tempat: petunjuk "Enter = ..." dan pengganti ketikan kosong.
static const char* const DEFAULT_REPORT_FILE = "laporan-estima.txt"; // DEFAULT_REPORT_FILE adalah penunjuk tetap ke teks tetap "laporan-estima.txt".

// Mencetak teks bantuan: dipakai untuk opsi --help dan sebagai penjelasan saat opsi yang diketik salah.
// Parameternya std::ostream& supaya teks yang sama bisa dikirim ke std::cout (bantuan biasa) maupun ke std::cerr
// (opsi salah) tanpa ditulis dua kali.
static void printUsage(std::ostream& out) { // printUsage() berfungsi untuk menulis cara pemakaian program ke aliran keluaran out.
    // Perintah out << ... di bawah adalah SATU perintah yang dipecah ke beberapa baris supaya mudah dibaca; perintah itu baru berakhir di titik koma.
    out << APP_NAME << ' ' << APP_VERSION << " - " << APP_TAGLINE << "\n\n" // baris judul "ESTIMA 1.0.0 - Estimasi ..."; \n adalah karakter pindah baris,
        // jadi "\n\n" mengakhiri baris judul lalu menyisakan satu baris kosong.
        << "Pemakaian: estima [opsi]\n\n" // cara memanggil program; [opsi] berarti opsi boleh ditulis, boleh juga tidak.
        << "Opsi:\n" // judul daftar opsi.
        << "  -h, --help       Menampilkan bantuan ini\n" // opsi -h atau --help: runApp() mencetak teks ini lalu selesai dengan kode 0.
        << "  -v, --version    Menampilkan versi program\n" // opsi -v atau --version: runApp() hanya mencetak nama dan versi program.
        << "      --no-color   Mematikan warna (sama dengan variabel lingkungan NO_COLOR)\n" // opsi --no-color: diteruskan ke initTerminal() sebagai disableColor.
        << "      --ascii      Menggambar kotak dengan + - | untuk terminal tanpa UTF-8\n\n" // opsi --ascii: diteruskan ke initTerminal() sebagai forceAscii.
        << "Tanpa opsi, program membuka menu interaktif.\n" // perilaku bawaan bila tidak ada opsi sama sekali.
        << "Dokumentasi: " << APP_REPOSITORY << '\n'; // alamat repositori program; '\n' (kutip tunggal) adalah satu karakter pindah baris.
} // akhir fungsi printUsage()

// Banner pembuka: kotak berjudul "ESTIMA <versi>" berisi nama lengkap program, mata kuliah, penulis, dan institusi.
// Dicetak satu kali oleh runMenuLoop() sebelum menu pertama tampil.
static void printBanner() { // printBanner() berfungsi untuk menggambar kotak identitas program di layar.
    std::vector<StyledLine> lines; // lines adalah daftar baris isi kotak; bentuknya std::vector<StyledLine> karena itulah yang diterima drawBox().
    lines.push_back(StyledLine(APP_TAGLINE, STYLE_BOLD)); // baris 1: nama lengkap program, dicetak tebal (STYLE_BOLD).
    lines.push_back(StyledLine(APP_COURSE, STYLE_NORMAL)); // baris 2: mata kuliah dan kelas, gaya biasa.
    lines.push_back(StyledLine(APP_AUTHOR, STYLE_NORMAL)); // baris 3: nama dan NIM penulis.
    lines.push_back(StyledLine(APP_INSTITUTION, STYLE_NORMAL)); // baris 4: program studi dan universitas.
    // Tanda + pada std::string berarti menyambung teks. std::string(APP_NAME) mengubah teks tetap const char* menjadi std::string lebih dulu,
    // karena dua const char* tidak bisa disambung dengan +; begitu sisi kirinya std::string, sambungan berikutnya ikut menjadi std::string.
    drawBox(std::cout, screenStyle(), std::string(APP_NAME) + " " + APP_VERSION, lines); // drawBox() menggambar kotak ke layar dengan judul "ESTIMA 1.0.0".
} // akhir fungsi printBanner()

// Menggambar menu utama di dalam kotak, lalu baris keadaan data dan petunjuk singkat di bawahnya.
// Parameternya const ComponentList& karena daftar hanya dibaca (untuk baris keadaan), tidak diubah, dan tidak perlu disalin.
static void printMenu(const ComponentList& list) { // printMenu() berfungsi untuk menampilkan menu utama beserta ringkasan data saat ini.
    const OutputStyle style = screenStyle(); // style adalah gaya keluaran layar (lebar terminal, warna, UTF-8), diambil sekali untuk seluruh fungsi ini.
    const std::size_t inner = boxInnerLimit(style); // inner adalah lebar isi terbesar (dalam kolom) yang masih muat di dalam kotak pada terminal ini.

    // Lebar kolom nama menu mengikuti nama terpanjang supaya keterangannya lurus.
    std::size_t labelWidth = 0; // labelWidth adalah lebar nama menu terpanjang; mulai 0 lalu diperbesar oleh perulangan pertama.
    std::size_t longestLine = 0; // longestLine adalah lebar baris menu terpanjang bila keterangan ikut ditulis; dihitung oleh perulangan kedua.
    for (std::size_t i = 0; i < MAIN_MENU_COUNT; ++i) { // perulangan pertama: memeriksa setiap butir menu, nomor elemen 0 sampai MAIN_MENU_COUNT - 1.
        if (displayWidth(MAIN_MENU[i].label) > labelWidth) { // bila nama menu ke-i lebih lebar daripada yang terlebar sejauh ini ...
            labelWidth = displayWidth(MAIN_MENU[i].label); // ... lebarnya menjadi rekor baru.
        } // akhir if
    } // akhir perulangan pertama
    for (std::size_t i = 0; i < MAIN_MENU_COUNT; ++i) { // perulangan kedua: menghitung lebar tiap baris menu bila keterangan ikut ditulis.
        const std::size_t lineWidth = 4 + labelWidth + 2 + displayWidth(MAIN_MENU[i].description); // lineWidth = 4 kolom awalan "[n] " + nama selebar labelWidth
        // + 2 spasi pemisah + lebar keterangan. Angka 4 tepat karena semua nomor menu hanya satu digit (0 sampai 8).
        if (lineWidth > longestLine) { // bila baris ini lebih lebar daripada rekor sebelumnya ...
            longestLine = lineWidth; // ... simpan sebagai baris terpanjang.
        } // akhir if
    } // akhir perulangan kedua
    // Bila semua butir muat satu baris, nama dan keterangan ditulis sebaris. Di terminal sempit
    // (misalnya ponsel) hanya nama menu yang ditulis, supaya menu tetap pendek dan tidak ada
    // baris yang terpotong.
    const bool withDescription = longestLine <= inner; // withDescription bernilai true bila baris terpanjang pun muat di kotak (<= karena lebar yang pas sama masih muat).

    std::vector<StyledLine> lines; // lines adalah baris-baris isi kotak menu.
    for (std::size_t i = 0; i < MAIN_MENU_COUNT; ++i) { // menyusun satu baris teks untuk setiap butir menu, sesuai urutan di array MAIN_MENU.
        const std::string number = "[" + formatThousands(MAIN_MENU[i].number) + "] "; // number adalah awalan seperti "[1] "; formatThousands() menerima double,
        // jadi nomor int diubah otomatis menjadi double lalu ditulis tanpa desimal.
        if (withDescription) { // bila terminal cukup lebar ...
            lines.push_back(StyledLine(number + padRight(MAIN_MENU[i].label, labelWidth) + "  " + MAIN_MENU[i].description, // ... baris lengkap: nomor, nama yang
                                       // diratakan kiri selebar labelWidth (padRight menambah spasi di kanan), dua spasi, lalu keterangan ...
                                       STYLE_NORMAL)); // ... dengan gaya biasa.
        } else { // bila terminal sempit ...
            lines.push_back(StyledLine(number + MAIN_MENU[i].label, STYLE_NORMAL)); // ... hanya nomor dan nama menu.
        } // akhir if-else
    } // akhir perulangan penyusun baris menu
    drawBox(std::cout, style, "Menu Utama", lines); // menggambar kotak berjudul "Menu Utama" berisi baris-baris tadi.

    // Baris keadaan: ringkasan data yang sedang dipegang program.
    std::string status = "Data: belum ada komponen."; // status diisi dulu dengan teks untuk daftar kosong; diganti di bawah bila sudah ada data.
    if (list.count > 0) { // bila daftar sudah berisi paling sedikit satu komponen ...
        status = "Data: " + formatThousands(list.count) + " komponen | " // ... status berisi banyaknya komponen,
            + formatWeight(sumTotalWeight(list.items, list.count)) + " | " // total berat seluruh komponen (array list.items dikirim bersama list.count),
            + formatRupiah(sumTotalCost(list.items, list.count)); // dan total biaya seluruh komponen dalam rupiah.
    } // akhir if
    printWrapped(std::cout, style, STYLE_DIM, status, 6); // status dicetak redup (STYLE_DIM); bila terlalu panjang dilipat, dan baris lanjutannya menjorok
    // 6 spasi, selebar awalan "Data: ".
    printWrapped(std::cout, style, STYLE_NORMAL, "Ketik nomor menu lalu tekan Enter.", 0); // petunjuk singkat cara memilih menu.
} // akhir fungsi printMenu()

// Membaca nama komponen sampai sah dan belum dipakai komponen lain. ignoreIndex adalah posisi
// komponen yang sedang diubah (boleh memakai namanya sendiri), atau -1 saat menambah.
// Dipakai oleh dua aksi: tambah (tanpa nilai bawaan) dan ubah (nilai bawaan = nama lama, jadi Enter kosong berarti tidak berubah).
// Nama dianggap sama tanpa membedakan huruf besar/kecil, karena findComponentByName() membandingkan versi huruf kecilnya.
static std::string readComponentName(const ComponentList& list, int ignoreIndex, bool hasDefault, // readComponentName() menghasilkan nama yang sudah sah;
                                     // list hanya dibaca untuk mencari nama kembar, hasDefault menandakan ada nilai bawaan atau tidak ...
                                     const std::string& defaultName) { // ... dan defaultName adalah nama yang dipakai bila pengguna hanya menekan Enter.
    while (true) { // while (true) mengulang pertanyaan terus-menerus; jalan keluarnya hanya return di bawah, yaitu saat nama sudah sah.
        const std::string name = collapseSpaces(readLine("Nama komponen")); // membaca satu baris ketikan, lalu deret spasi di tengahnya dirapatkan menjadi satu spasi.
        if (name.empty() && hasDefault) { // ketikan kosong (hanya Enter) DAN memang ada nilai bawaan ...
            return defaultName; // ... maka nama lama dipertahankan.
        } // akhir if
        const std::string problem = checkComponentName(name); // problem berisi pesan kesalahan dari pemeriksa nama, atau teks kosong bila namanya sah.
        if (!problem.empty()) { // bila problem tidak kosong, artinya nama tidak sah ...
            printError(problem); // ... tampilkan pesannya dengan awalan [!] ...
            continue; // ... lalu continue melompat ke awal perulangan untuk bertanya lagi.
        } // akhir if
        const int existing = findComponentByName(list, name); // existing adalah posisi komponen yang namanya sama, atau -1 bila belum ada.
        if (existing >= 0 && existing != ignoreIndex) { // nama dianggap kembar bila ditemukan (>= 0) DAN bukan milik komponen yang sedang diubah sendiri.
            printError("Nama \"" + name + "\" sudah dipakai komponen no. " + formatThousands(existing + 1) // \" adalah tanda kutip di dalam teks; existing + 1
                       // karena nomor di layar mulai 1, sedangkan posisi di array mulai 0 ...
                       + ". Beri nama lain, misalnya dengan menambah kode ukuran."); // ... dan pesannya ditutup dengan saran nama lain.
            continue; // kembali ke awal perulangan dan bertanya lagi.
        } // akhir if
        return name; // lolos semua pemeriksaan: nama yang sah dikembalikan ke pemanggil.
    } // akhir perulangan while
} // akhir fungsi readComponentName()

// Menampilkan harga acuan lalu membaca harga per kg: pengguna bisa memilih salah satu harga
// acuan (dan masih boleh menggantinya) atau langsung mengetik harga sendiri.
// Hasilnya double karena semua harga di program ini disimpan sebagai double (lihat Component::pricePerKg).
static double readPriceWithReference() { // readPriceWithReference() menghasilkan harga per kg (rupiah) yang sudah sah.
    Table table; // table adalah tabel harga acuan yang akan digambar; konstruktor Table() membuatnya kosong.
    table.columns.push_back(TableColumn("No", true)); // kolom 1 "No", rata kanan (true) karena berisi angka.
    table.columns.push_back(TableColumn("Material", false)); // kolom 2 "Material", rata kiri (false) karena berisi teks.
    table.columns.push_back(TableColumn("Harga/kg", true)); // kolom 3 "Harga/kg", rata kanan karena berisi angka rupiah.
    for (int i = 0; i < MATERIAL_COUNT; ++i) { // satu baris tabel untuk setiap material di katalog; i bertipe int karena MATERIAL_COUNT juga int.
        TableRow row; // row adalah satu baris tabel yang masih kosong.
        row.cells.push_back(formatThousands(i + 1)); // sel nomor: i + 1, karena nomor pilihan di layar mulai 1 sedangkan posisi array mulai 0.
        row.cells.push_back(MATERIAL_CATALOG[i].name); // sel nama material, diambil dari elemen ke-i katalog.
        row.cells.push_back(formatRupiah(MATERIAL_CATALOG[i].pricePerKg)); // sel harga per kg, ditulis sebagai rupiah (misalnya Rp19.500).
        table.rows.push_back(row); // baris yang sudah lengkap dimasukkan ke tabel.
    } // akhir perulangan for
    TableRow manualRow; // manualRow adalah baris tambahan untuk pilihan 0, yaitu mengetik harga sendiri.
    manualRow.cells.push_back("0"); // sel nomor: 0.
    manualRow.cells.push_back("Ketik harga sendiri"); // sel keterangan pilihan 0.
    manualRow.cells.push_back("-"); // tanda - karena pilihan ini tidak punya harga acuan.
    table.rows.push_back(manualRow); // pilihan 0 ditaruh paling bawah, sesudah semua material.

    std::vector<StyledLine> notes; // notes adalah catatan kecil yang dicetak di bawah tabel.
    notes.push_back(StyledLine(std::string("Harga acuan ") + CATALOG_PERIOD + "; keterangannya ada di menu 7.", STYLE_DIM)); // catatan periode harga, dicetak redup.
    std::cout << '\n'; // satu baris kosong sebagai jarak sebelum tabel.
    drawTableBox(std::cout, screenStyle(), "Harga Acuan per kg", table, notes); // menggambar tabel di dalam kotak; di terminal sempit tabel otomatis berubah menjadi "kartu".

    const int choice = readChoice("Pilih harga [0-" + formatThousands(MATERIAL_COUNT) + "]", 0, MATERIAL_COUNT, false, 0); // choice adalah nomor 0 sampai
    // MATERIAL_COUNT; false artinya tanpa nilai bawaan, jadi Enter kosong ditolak dan pertanyaannya diulang.
    if (choice == 0) { // pilihan 0: pengguna ingin mengetik harga sendiri ...
        return readPricePerKg("Harga per kg (Rp)", false, 0.0); // ... harga dibaca tanpa nilai bawaan (0.0 hanya pengisi parameter dan tidak dipakai).
    } // akhir if
    const MaterialReference& material = MATERIAL_CATALOG[choice - 1]; // material adalah referensi (nama lain) untuk baris katalog yang dipilih, jadi tidak
    // disalin; choice - 1 karena nomor di layar mulai 1 sedangkan posisi array mulai 0.
    printInfo("Enter = pakai " + formatRupiah(material.pricePerKg) + " (" + material.name + "), atau ketik harga lain."); // memberi tahu arti Enter kosong.
    return readPricePerKg("Harga per kg (Rp)", true, material.pricePerKg); // harga acuan menjadi nilai bawaan; harga lain yang diketik tetap diperiksa batasnya.
} // akhir fungsi readPriceWithReference()

// true bila biaya komponen masih di dalam batas yang dijamin tepat; bila tidak, pesan dicetak.
// Pemeriksaan ini perlu walaupun setiap ketikan sudah sah, karena gabungan nilai terbesar (1.000.000 unit x 10.000 kg x
// Rp100.000.000) jauh melewati MAX_COMPONENT_COST, batas yang menjaga hitungan tetap tepat (lihat component.h).
static bool costWithinLimit(const Component& component) { // costWithinLimit() memeriksa satu komponen; dikirim sebagai const & supaya tidak disalin.
    if (calculateTotalCost(component) <= MAX_COMPONENT_COST) { // bila total biayanya tidak melebihi batas Rp999.999.999.999 ...
        return true; // ... komponen boleh disimpan.
    } // akhir if
    printError("Total biaya komponen ini melebihi " + formatRupiah(MAX_COMPONENT_COST) // selain itu, pengguna diberi tahu batasnya ...
               + ", batas yang bisa dihitung program. Periksa lagi jumlah, berat, dan harganya."); // ... dan apa yang perlu diperiksa.
    return false; // false: komponen ini ditolak.
} // akhir fungsi costWithinLimit()

// Aksi menu 1 (Tambah komponen): menampilkan aturan pengisian, lalu mengisi komponen satu per satu sampai pengguna
// menjawab tidak pada pertanyaan "Tambah komponen lagi?". Setiap komponen diisi dulu ke variabel draft dan baru masuk
// daftar setelah ringkasannya disetujui. Parameternya ComponentList& tanpa const karena aksi ini memang mengubah daftar.
static void actionAdd(ComponentList& list) { // actionAdd() berfungsi untuk menambah komponen baru ke daftar.
    const OutputStyle style = screenStyle(); // gaya layar diambil sekali dan dipakai untuk semua tampilan di aksi ini.
    std::vector<StyledLine> guide; // guide adalah baris-baris petunjuk pengisian yang ditampilkan di dalam kotak.
    guide.push_back(StyledLine("Nama   : huruf, angka, spasi, dan tanda - _ . , / ( ) + # & (paling banyak " // petunjuk nama: karakter yang boleh dipakai ...
                               + formatThousands(MAX_NAME_LENGTH) + " karakter)", STYLE_NORMAL)); // ... dan panjang maksimal MAX_NAME_LENGTH (30), diambil
                               // dari konstanta supaya angkanya selalu sama dengan aturan pemeriksaannya.
    guide.push_back(StyledLine("Jumlah : bilangan bulat " + formatThousands(MIN_QUANTITY) + " sampai " // petunjuk jumlah: dari MIN_QUANTITY (1) ...
                               + formatThousands(MAX_QUANTITY) + " unit", STYLE_NORMAL)); // ... sampai MAX_QUANTITY (1.000.000) unit.
    guide.push_back(StyledLine("Berat  : kilogram per unit, desimal dengan koma atau titik (contoh 1,85)", STYLE_NORMAL)); // petunjuk berat per unit.
    guide.push_back(StyledLine("Harga  : rupiah bulat per kg, dipilih dari harga acuan atau diketik sendiri", STYLE_NORMAL)); // petunjuk harga per kg.
    std::cout << '\n'; // satu baris kosong sebelum kotak.
    drawBox(std::cout, style, "Tambah Komponen", guide); // menggambar kotak petunjuk berjudul "Tambah Komponen".

    // do ... while berfungsi untuk mengulang isi blok do { } selama syarat di dalam while (...) bernilai benar. Bedanya dengan
    // while biasa: syaratnya baru diperiksa SESUDAH isi blok dijalankan, jadi isi blok pasti dijalankan paling sedikit satu kali.
    // do-while dipakai karena pengisian pasti dilakukan paling sedikit satu kali; pertanyaan
    // "tambah lagi?" baru diajukan setelah satu komponen selesai.
    bool again = false; // again menyimpan jawaban "Tambah komponen lagi?"; diisi di akhir setiap putaran dan diperiksa oleh while.
    do { // awal blok do: satu putaran berarti mengisi satu komponen.
        if (list.count >= MAX_COMPONENTS) { // bila daftar sudah berisi MAX_COMPONENTS (100) komponen, array tidak punya tempat kosong lagi ...
            printError("Daftar sudah penuh (" + formatThousands(MAX_COMPONENTS) // ... pengguna diberi tahu batasnya ...
                       + " komponen). Hapus dulu komponen yang tidak dipakai lewat menu 5."); // ... dan cara mengosongkan tempat.
            break; // break keluar dari perulangan do-while saat itu juga, tanpa bertanya "tambah lagi?".
        } // akhir if
        std::cout << '\n'; // baris kosong sebagai jarak antarkomponen.
        printWrapped(std::cout, style, STYLE_TITLE, "Komponen ke-" + formatThousands(list.count + 1), 0); // judul kecil, misalnya "Komponen ke-3";
        // list.count + 1 adalah nomor yang akan didapat komponen ini bila disimpan.

        Component draft; // draft adalah komponen sementara; konstruktor Component() mengisinya dengan nama kosong dan angka 0.
        draft.name = readComponentName(list, -1, false, ""); // nama: -1 karena tidak ada komponen yang sedang diubah, false karena tidak ada nilai bawaan.
        draft.quantity = readQuantity("Jumlah (unit)", false, 0); // jumlah unit (int), ditanyakan ulang sampai berada di rentang yang sah.
        draft.unitWeightKg = readUnitWeight("Berat per unit (kg)", false, 0.0); // berat per unit dalam kg; double karena boleh desimal, misalnya 1,85.
        draft.pricePerKg = readPriceWithReference(); // harga per kg: dipilih dari tabel harga acuan atau diketik sendiri.

        if (costWithinLimit(draft)) { // hanya bila total biayanya masih di dalam batas, komponen ditawarkan untuk disimpan.
            std::cout << '\n'; // baris kosong sebelum ringkasan.
            printComponentSummary(std::cout, style, draft); // ringkasan: data masukan beserta total berat dan total biaya komponen ini.
            if (askYesNo("Simpan komponen ini?", true)) { // true: jawaban bawaan (hanya Enter) adalah ya, ditandai [Y/t].
                addComponent(list, draft); // draft disalin ke tempat kosong pertama di array; tempatnya pasti ada karena di awal putaran daftar sudah dicek belum penuh.
                printSuccess("Komponen \"" + draft.name + "\" disimpan sebagai no. " + formatThousands(list.count) + "."); // list.count sudah bertambah satu,
                // jadi nilainya sama dengan nomor komponen yang baru disimpan.
            } else { // bila pengguna menjawab tidak ...
                printInfo("Komponen tidak disimpan."); // ... draft dibuang begitu saja dan daftar tidak berubah.
            } // akhir if-else
        } // akhir if costWithinLimit
        again = askYesNo("Tambah komponen lagi?", false); // false: jawaban bawaan (hanya Enter) adalah tidak, ditandai [y/T].
    } while (again); // while (again) diperiksa SESUDAH putaran: bila again true, kembali ke awal do; bila false, perulangan selesai.
} // akhir fungsi actionAdd()

// Aksi menu 2 (Rekap material & biaya): isinya disusun printRecap() di report.cpp, fungsi yang sama dengan yang
// menulis laporan berkas; di sini tujuannya layar (std::cout) dengan gaya terminal.
static void actionRecap(const ComponentList& list) { // actionRecap() hanya membaca daftar, jadi parameternya const ComponentList&.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printRecap(std::cout, screenStyle(), list); // rekap: tabel tiap komponen, total keseluruhan, dan komponen termahal; daftar kosong ditangani printRecap() sendiri.
} // akhir fungsi actionRecap()

// Aksi menu 3 (Peringkat biaya): urutan komponen dari biaya terbesar beserta porsi dan grafik batangnya, disusun printRanking().
static void actionRanking(const ComponentList& list) { // actionRanking() hanya membaca daftar.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printRanking(std::cout, screenStyle(), list); // tabel peringkat biaya; daftar kosong ditangani printRanking() sendiri.
} // akhir fungsi actionRanking()

// Meminta nomor komponen dari daftar. Hasilnya posisi (mulai 0), atau -1 bila dibatalkan.
// Dipakai bersama oleh aksi ubah dan hapus, supaya cara memilih komponennya sama.
static int pickComponent(const ComponentList& list) { // pickComponent() berfungsi untuk menampilkan daftar komponen lalu membaca nomor pilihan.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printComponentPicker(std::cout, screenStyle(), list); // tabel berisi nomor, nama, dan total biaya setiap komponen.
    const int number = readChoice("Nomor komponen [0 = batal]", 0, list.count, false, 0); // number adalah 0 (batal) sampai list.count; nomor lain ditolak readChoice().
    return number - 1; // nomor di layar mulai 1 sedangkan posisi array mulai 0, jadi dikurangi 1; pilihan 0 (batal) otomatis menjadi -1.
} // akhir fungsi pickComponent()

// Aksi menu 4 (Ubah komponen): memilih satu komponen, lalu setiap datanya ditanyakan ulang dengan nilai lama sebagai
// nilai bawaan (Enter = tidak berubah). Parameternya ComponentList& tanpa const karena komponen di daftar bisa ditimpa.
static void actionEdit(ComponentList& list) { // actionEdit() berfungsi untuk memperbaiki data satu komponen yang sudah ada.
    if (list.count == 0) { // bila daftar masih kosong, tidak ada yang bisa diubah ...
        printInfo(EMPTY_LIST_MESSAGE); // ... tampilkan pesan daftar kosong (teksnya dari report.cpp, dipakai bersama) ...
        return; // ... lalu return mengakhiri aksi lebih awal; fungsi void boleh return tanpa nilai.
    } // akhir if
    const int index = pickComponent(list); // index adalah posisi komponen yang dipilih (mulai 0), atau -1 bila batal.
    if (index < 0) { // -1 berarti pengguna memilih 0 (batal) ...
        printInfo("Tidak ada yang diubah."); // ... beri tahu bahwa tidak ada perubahan ...
        return; // ... lalu selesai.
    } // akhir if

    // Perubahan dikerjakan pada salinan (draft). Data asli baru ditimpa setelah pengguna
    // menyetujui ringkasannya, jadi pembatalan di tengah jalan tidak merusak apa pun.
    Component draft = list.items[index]; // draft adalah SALINAN komponen yang dipilih (bukan referensi), jadi mengubah draft belum mengubah isi daftar.
    printInfo("Tekan Enter tanpa mengetik untuk mempertahankan nilai lama."); // petunjuk cara mempertahankan nilai lama.
    printInfo("Nama sekarang: " + draft.name); // nilai lama ditampilkan sebelum pertanyaannya, supaya pengguna tahu apa yang dipertahankan bila menekan Enter.
    draft.name = readComponentName(list, index, true, draft.name); // index dikirim sebagai ignoreIndex supaya nama milik komponen ini sendiri tidak dianggap kembar.
    printInfo("Jumlah sekarang: " + formatThousands(draft.quantity) + " unit"); // jumlah lama.
    draft.quantity = readQuantity("Jumlah (unit)", true, draft.quantity); // true dan draft.quantity: Enter kosong mempertahankan jumlah lama.
    printInfo("Berat per unit sekarang: " + formatWeight(draft.unitWeightKg)); // berat lama, misalnya "1,850 kg".
    draft.unitWeightKg = readUnitWeight("Berat per unit (kg)", true, draft.unitWeightKg); // Enter kosong mempertahankan berat lama.
    printInfo("Harga per kg sekarang: " + formatRupiah(draft.pricePerKg)); // harga lama.
    draft.pricePerKg = readPricePerKg("Harga per kg (Rp)", true, draft.pricePerKg); // harga diketik langsung (tanpa tabel harga acuan); Enter mempertahankan harga lama.

    if (!costWithinLimit(draft)) { // bila biaya hasil perubahan melewati batas, pesannya sudah dicetak costWithinLimit() ...
        return; // ... dan aksi berhenti; isi daftar tetap seperti semula karena yang diubah baru draft.
    } // akhir if
    std::cout << '\n'; // baris kosong sebelum ringkasan.
    printComponentSummary(std::cout, screenStyle(), draft); // ringkasan komponen setelah diubah.
    if (askYesNo("Simpan perubahan?", true)) { // [Y/t]: hanya Enter berarti simpan.
        list.items[index] = draft; // baru di sini data asli di array ditimpa dengan salinan yang sudah diubah.
        printSuccess("Komponen no. " + formatThousands(index + 1) + " diperbarui."); // index + 1 adalah nomor komponen di layar.
    } else { // bila pengguna menjawab tidak ...
        printInfo("Perubahan dibatalkan."); // ... draft dibuang dan data asli tidak tersentuh.
    } // akhir if-else
} // akhir fungsi actionEdit()

// Aksi menu 5 (Hapus komponen): memilih satu komponen lalu menghapusnya setelah dikonfirmasi. Jawaban bawaannya
// tidak ([y/T]), jadi menekan Enter saja tidak akan menghapus apa pun.
static void actionDelete(ComponentList& list) { // actionDelete() berfungsi untuk membuang satu komponen dari daftar; ComponentList& karena daftar diubah.
    if (list.count == 0) { // bila daftar masih kosong, tidak ada yang bisa dihapus ...
        printInfo(EMPTY_LIST_MESSAGE); // ... tampilkan pesan daftar kosong ...
        return; // ... lalu selesai.
    } // akhir if
    const int index = pickComponent(list); // index adalah posisi komponen yang dipilih (mulai 0), atau -1 bila batal.
    if (index < 0) { // pengguna memilih 0 (batal) ...
        printInfo("Tidak ada yang dihapus."); // ... beri tahu ...
        return; // ... lalu selesai.
    } // akhir if
    const std::string name = list.items[index].name; // nama disalin dulu ke variabel name, karena sesudah removeComponent() posisi index sudah berisi komponen
    // lain (atau kosong), sedangkan namanya masih dibutuhkan untuk pesan berhasil.
    printInfo("Komponen yang dipilih: " + name); // menampilkan nama komponen supaya pengguna yakin pilihannya benar.
    if (askYesNo("Hapus komponen ini?", false)) { // [y/T]: hanya Enter berarti tidak jadi menghapus.
        removeComponent(list, index); // menghapus elemen index lalu menggeser elemen sesudahnya satu langkah ke kiri; list.count berkurang satu.
        printSuccess("Komponen \"" + name + "\" dihapus. Sisa " + formatThousands(list.count) + " komponen."); // pesan berhasil beserta sisa isi daftar.
    } else { // bila pengguna menjawab tidak ...
        printInfo("Tidak ada yang dihapus."); // ... daftar dibiarkan.
    } // akhir if-else
} // akhir fungsi actionDelete()

// Aksi menu 6 (Muat data contoh): mengganti isi daftar dengan komponen contoh dari material_catalog.cpp, lalu langsung
// menampilkan rekapnya. Bila daftar sudah berisi, pengguna ditanya dulu karena isinya akan terhapus.
static void actionLoadSample(ComponentList& list) { // actionLoadSample() mengubah daftar, jadi parameternya ComponentList& tanpa const.
    if (list.count > 0) { // bila daftar sudah berisi data ...
        printInfo("Daftar saat ini berisi " + formatThousands(list.count) + " komponen dan akan diganti."); // ... peringatkan bahwa data itu akan diganti ...
        if (!askYesNo("Ganti dengan data contoh?", false)) { // ... dan minta persetujuan; [y/T] berarti Enter saja tidak mengganti apa pun.
            printInfo("Data tidak diubah."); // pengguna menolak: daftar dibiarkan ...
            return; // ... dan aksi selesai.
        } // akhir if
    } // akhir if
    const int loaded = loadSampleComponents(list); // daftar dikosongkan lalu diisi data contoh; loaded adalah banyaknya komponen yang dimuat.
    printSuccess(formatThousands(loaded) + " komponen contoh dimuat dengan harga acuan " + CATALOG_PERIOD + "."); // harga contoh diambil dari katalog,
    // jadi periode harganya disebut.
    std::cout << '\n'; // baris kosong sebelum rekap.
    printRecap(std::cout, screenStyle(), list); // rekap data contoh langsung ditampilkan.
} // akhir fungsi actionLoadSample()

// Aksi menu 7 (Harga acuan material): menampilkan seluruh katalog harga beserta standar, kegunaan, periode, dan sumbernya.
// Tidak butuh parameter daftar karena katalog tidak bergantung pada komponen yang diisi pengguna.
static void actionCatalog() { // actionCatalog() berfungsi untuk menampilkan tabel harga acuan material.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printCatalog(std::cout, screenStyle()); // tabel katalog dicetak ke layar.
} // akhir fungsi actionCatalog()

// true bila berkas dengan nama itu sudah ada (bisa dibuka untuk dibaca).
// Caranya dengan mencoba membuka berkas itu untuk dibaca; berkas yang ada tetapi tidak boleh dibaca juga menghasilkan false.
static bool fileExists(const std::string& fileName) { // fileExists() dipakai actionSave() sebelum menimpa berkas.
    std::ifstream probe(fileName.c_str()); // probe adalah aliran berkas masukan yang langsung mencoba membuka berkas itu. c_str() memberikan isi std::string
    // sebagai teks gaya C (const char*), karena pada C++98 std::ifstream hanya bisa dibuka dengan nama berkas berbentuk itu.
    return probe.good(); // good() bernilai true bila berkas berhasil dibuka; berkasnya otomatis ditutup lagi saat probe hilang di akhir fungsi.
} // akhir fungsi fileExists()

// Aksi menu 8 (Simpan laporan): menanyakan nama berkas (Enter = laporan-estima.txt), meminta izin bila berkas itu sudah
// ada, lalu menulis rekap dan peringkat ke berkas teks lewat saveReport().
static void actionSave(const ComponentList& list) { // actionSave() hanya membaca daftar, jadi parameternya const ComponentList&.
    if (list.count == 0) { // bila daftar kosong, tidak ada yang bisa dilaporkan ...
        printInfo(EMPTY_LIST_MESSAGE); // ... tampilkan pesan daftar kosong ...
        return; // ... lalu selesai.
    } // akhir if
    printInfo("Laporan ditulis sebagai berkas teks di folder tempat program dijalankan."); // nama berkas tidak boleh memuat folder, jadi berkas
    // selalu dibuat di folder kerja saat program dijalankan.
    printInfo(std::string("Enter = ") + DEFAULT_REPORT_FILE); // memberi tahu nama berkas bawaan; std::string(...) supaya + bisa menyambung teks.

    std::string fileName; // fileName akan diisi checkReportFileName() dengan nama berkas yang sah dan sudah berakhiran .txt.
    while (true) { // bertanya terus sampai nama berkasnya sah; jalan keluarnya break di bawah.
        const std::string text = readLine("Nama berkas"); // text adalah ketikan pengguna, sudah dirapikan spasi di awal dan akhirnya.
        const std::string problem = checkReportFileName(text.empty() ? DEFAULT_REPORT_FILE : text, fileName); // ketikan kosong diganti nama bawaan
        // lewat operator ?:, lalu diperiksa; problem kosong berarti nama sah dan fileName sudah terisi.
        if (problem.empty()) { // bila nama berkas sah ...
            break; // ... break keluar dari while (true).
        } // akhir if
        printError(problem); // nama tidak sah: tampilkan sebabnya, lalu perulangan bertanya lagi.
    } // akhir perulangan while
    if (fileExists(fileName)) { // bila berkas dengan nama itu sudah ada ...
        printInfo("Berkas " + fileName + " sudah ada."); // ... beri tahu pengguna ...
        if (!askYesNo("Timpa berkas itu?", false)) { // ... dan minta izin; [y/T] berarti Enter saja tidak menimpa.
            printInfo("Laporan tidak disimpan."); // pengguna menolak: berkas lama dibiarkan ...
            return; // ... dan aksi selesai.
        } // akhir if
    } // akhir if

    std::string errorMessage; // errorMessage diisi saveReport() dengan sebab kegagalan bila penulisan gagal.
    if (saveReport(list, fileName, errorMessage)) { // saveReport() menghasilkan true bila berkas berhasil ditulis seluruhnya.
        printSuccess("Laporan disimpan ke " + fileName + "."); // pesan berhasil beserta nama berkasnya.
    } else { // bila gagal ...
        printError(errorMessage); // ... tampilkan sebabnya, misalnya folder tidak boleh ditulisi.
    } // akhir if-else
} // akhir fungsi actionSave()

// true bila pengguna memang ingin keluar. Bila masih ada data, ditanyakan dulu karena data
// hanya hidup selama program berjalan.
static bool confirmExit(const ComponentList& list) { // confirmExit() dipanggil saat pengguna memilih menu 0 (Keluar).
    if (list.count == 0) { // daftar kosong: tidak ada data yang bisa hilang ...
        return true; // ... jadi langsung boleh keluar tanpa bertanya.
    } // akhir if
    printInfo("Data komponen hanya tersimpan selama program berjalan. Simpan laporan lewat menu 8 bila perlu."); // pengingat sebelum keluar.
    return askYesNo("Keluar dari program?", false); // [y/T]: hanya Enter berarti tidak jadi keluar.
} // akhir fungsi confirmExit()

// [Kriteria soal: perulangan dan percabangan] while (running) mengulang menu dan switch (choice) memilih aksinya. Perulangan
// lain juga ada di banyak bagian, misalnya for di calculation.cpp (menjumlahkan semua komponen), while (true) di prompt.cpp
// (bertanya ulang sampai jawabannya sah), dan do-while di actionAdd() (menambah komponen berikutnya).
// Perulangan menu utama: gambar menu, baca pilihan, jalankan aksinya, lalu tunggu Enter. Diulang sampai pengguna
// memastikan ingin keluar.
static void runMenuLoop() { // runMenuLoop() berfungsi untuk melayani menu utama dari awal sampai pengguna keluar.
    // Seluruh data program hidup di variabel lokal ini dan dikirim ke tiap aksi lewat
    // parameter. Tidak ada variabel global untuk data, jadi jelas fungsi mana yang boleh
    // mengubahnya (ComponentList&) dan mana yang hanya membaca (const ComponentList&).
    ComponentList list; // list adalah satu-satunya tempat data komponen; konstruktor ComponentList() membuatnya kosong (count = 0).
    printBanner(); // banner identitas dicetak sekali, sebelum menu pertama.

    bool running = true; // running adalah penanda perulangan menu; menjadi false hanya bila pengguna memastikan keluar.
    while (running) { // selama running masih true, menu ditampilkan lagi.
        std::cout << '\n'; // baris kosong sebelum menu.
        printMenu(list); // menu utama beserta baris keadaan data.
        const int choice = readChoice("Pilihan Anda [" + formatThousands(MENU_EXIT) + "-" + formatThousands(MENU_SAVE) + "]", // label "Pilihan Anda [0-8]",
                                      // disusun dari konstanta enum supaya ikut berubah bila nomor menu berubah ...
                                      MENU_EXIT, MENU_SAVE, false, 0); // ... rentang sah MENU_EXIT (0) sampai MENU_SAVE (8), tanpa nilai bawaan.

        // switch berfungsi untuk memilih satu dari banyak cabang berdasarkan nilai sebuah bilangan bulat. Setiap "case NILAI:" adalah
        // titik masuk untuk nilai itu, dan break mengakhiri switch; tanpa break, program akan terus menjalankan perintah milik case
        // di bawahnya. "default:" dijalankan bila nilainya tidak cocok dengan case mana pun.
        // switch memilih satu aksi berdasarkan nomor menu. Dipakai, bukan if-else bertingkat,
        // karena yang diperiksa satu nilai bulat dengan banyak kemungkinan yang sudah pasti.
        switch (choice) { // switch (choice) memilih cabang sesuai nomor menu yang diketik.
            case MENU_ADD: // case MENU_ADD: pilihan 1, Tambah komponen.
                actionAdd(list); // list dikirim sebagai referensi, jadi komponen baru langsung masuk ke daftar milik fungsi ini.
                break; // break: selesai, keluar dari switch.
            case MENU_RECAP: // pilihan 2, Rekap material & biaya.
                actionRecap(list); // menampilkan rekap.
                break; // keluar dari switch.
            case MENU_RANKING: // pilihan 3, Peringkat biaya.
                actionRanking(list); // menampilkan peringkat biaya.
                break; // keluar dari switch.
            case MENU_EDIT: // pilihan 4, Ubah komponen.
                actionEdit(list); // mengubah satu komponen.
                break; // keluar dari switch.
            case MENU_DELETE: // pilihan 5, Hapus komponen.
                actionDelete(list); // menghapus satu komponen.
                break; // keluar dari switch.
            case MENU_SAMPLE: // pilihan 6, Muat data contoh.
                actionLoadSample(list); // mengganti isi daftar dengan data contoh.
                break; // keluar dari switch.
            case MENU_CATALOG: // pilihan 7, Harga acuan material.
                actionCatalog(); // menampilkan katalog; tanpa list karena tidak memakai data komponen.
                break; // keluar dari switch.
            case MENU_SAVE: // pilihan 8, Simpan laporan.
                actionSave(list); // menyimpan laporan ke berkas teks.
                break; // keluar dari switch.
            case MENU_EXIT: // pilihan 0, Keluar.
                running = !confirmExit(list); // bila confirmExit() true (jadi keluar), tanda ! membaliknya sehingga running false dan perulangan berhenti.
                break; // keluar dari switch.
            default: // default: nilai lain; karena readChoice() hanya menerima 0 sampai 8, cabang ini tidak pernah tercapai.
                break; // tidak melakukan apa-apa.
        } // akhir switch
        if (running && choice != MENU_EXIT) { // menunggu Enter hanya sesudah aksi biasa; sesudah memilih Keluar lalu batal, menu langsung tampil lagi.
            waitForEnter(); // menahan hasil aksi di layar sampai pengguna menekan Enter.
        } // akhir if
    } // akhir perulangan while (running)
    printSuccess("Program selesai. Terima kasih."); // pesan penutup; hanya tercapai bila keluar lewat menu Keluar.
} // akhir fungsi runMenuLoop()

// Definisi runApp() yang dideklarasikan di app.h dan dipanggil dari main(). Langkahnya: (1) membaca opsi baris perintah,
// (2) menyiapkan terminal, (3) menjalankan perulangan menu di dalam try, supaya habisnya masukan ditangani di satu tempat.
// Fungsi ini tidak static karena dipanggil oleh main() di main.cpp, yang pada versi modul adalah unit kompilasi lain.
int runApp(int argc, char* argv[]) { // argc adalah banyaknya kata pada baris perintah (termasuk nama program), argv adalah array teks kata-kata itu.
    bool disableColor = false; // disableColor menjadi true bila ada opsi --no-color.
    bool forceAscii = false; // forceAscii menjadi true bila ada opsi --ascii.
    for (int i = 1; i < argc; ++i) { // i mulai 1, bukan 0, karena argv[0] berisi nama program itu sendiri, bukan opsi.
        const std::string option = argv[i]; // teks gaya C argv[i] disalin ke std::string supaya bisa dibandingkan dengan == terhadap teks seperti "-h".
        if (option == "-h" || option == "--help") { // opsi bantuan, bentuk pendek atau panjang ...
            printUsage(std::cout); // ... cetak bantuan ke keluaran biasa ...
            return EXIT_CODE_OK; // ... lalu selesai dengan kode 0 tanpa membuka menu.
        } else if (option == "-v" || option == "--version") { // else if hanya diperiksa bila syarat sebelumnya salah; ini opsi versi ...
            std::cout << APP_NAME << ' ' << APP_VERSION << '\n'; // ... cetak "ESTIMA 1.0.0" ...
            return EXIT_CODE_OK; // ... lalu selesai dengan kode 0.
        } else if (option == "--no-color") { // opsi tanpa warna ...
            disableColor = true; // ... dicatat dulu; baru dipakai initTerminal() di bawah, sesudah semua opsi terbaca.
        } else if (option == "--ascii") { // opsi garis ASCII ...
            forceAscii = true; // ... dicatat, nanti dipakai initTerminal().
        } else { // selain empat opsi di atas berarti opsi tidak dikenal ...
            std::cerr << "Opsi tidak dikenal: " << option << "\n\n"; // ... pesannya ditulis ke std::cerr (saluran galat) supaya terpisah dari keluaran biasa ...
            printUsage(std::cerr); // ... disusul cara pemakaian yang benar, juga ke std::cerr ...
            return EXIT_CODE_BAD_USAGE; // ... lalu program berhenti dengan kode 2.
        } // akhir rangkaian if-else if
    } // akhir perulangan for pembaca opsi

    initTerminal(disableColor, forceAscii); // menyiapkan terminal satu kali: memutuskan warna, garis UTF-8 atau ASCII, dan jenis masukan.
    try { // blok try dijalankan sambil berjaga: bila di dalamnya (sedalam apa pun) ada throw InputClosed(), program melompat ke catch di bawah.
        runMenuLoop(); // seluruh pemakaian program berlangsung di sini sampai pengguna memilih Keluar.
    } catch (const InputClosed&) { // menangkap InputClosed yang dilempar readLine() atau waitForEnter() di prompt.cpp ketika masukan habis.
        // Masukan habis sebelum pengguna memilih Keluar. Program berhenti dengan rapi dan
        // memberi tahu sebabnya lewat saluran galat (std::cerr).
        std::cout << '\n'; // pindah baris dulu, karena teks pertanyaan terakhir sudah tercetak tanpa pindah baris dan belum sempat dijawab.
        std::cout.flush(); // flush() memaksa isi std::cout yang masih tertahan di penampung memori langsung dikirim, supaya tampil sebelum pesan std::cerr di bawah.
        printWrapped(std::cerr, plainStyle(terminalWidth()), STYLE_NORMAL, // pesan dicetak ke std::cerr dengan gaya polos (tanpa warna) selebar terminal ...
                     "Input berakhir sebelum program ditutup lewat menu Keluar. Program dihentikan.", 0); // ... isi pesannya, tanpa indentasi baris lanjutan.
        return EXIT_CODE_INPUT_CLOSED; // kode keluar 1: masukan habis sebelum pengguna memilih Keluar.
    } // akhir blok try-catch
    return EXIT_CODE_OK; // runMenuLoop() selesai normal (lewat menu Keluar): kode keluar 0.
} // akhir fungsi runApp()

// ===============================================================================================================
// BAGIAN 22 - src/main.cpp - titik masuk program
// ===============================================================================================================
// Berkas ini paling pendek di seluruh program: isinya hanya fungsi main(), tempat program mulai berjalan.

// main() sengaja hanya meneruskan ke runApp(): seluruh alur ada di modul app sehingga bisa
// dipakai ulang dan diuji tanpa bergantung pada berkas ini.
// Contohnya, uji unit ditautkan dengan semua berkas src/*.cpp KECUALI main.cpp, karena program uji punya main() sendiri.
//
// main(int argc, char* argv[]) adalah fungsi yang pertama kali dijalankan sistem operasi saat program dibuka; setiap program C++
// wajib punya tepat satu fungsi main. argc (argument count) adalah banyaknya kata pada baris perintah, termasuk nama program
// itu sendiri; argv (argument vector) adalah array teks berisi kata-kata itu, misalnya perintah "estima --ascii" menghasilkan
// argc = 2, argv[0] = nama program, dan argv[1] = "--ascii".
// Nilai yang dikembalikan main() dengan return menjadi kode keluar program, yaitu angka yang diterima sistem operasi (atau
// skrip dan uji otomatis yang menjalankan program) untuk mengetahui bagaimana program berakhir: 0 berarti selesai dengan baik,
// angka lain berarti ada masalah. Di ESTIMA angkanya adalah EXIT_CODE_OK (0), EXIT_CODE_INPUT_CLOSED (1), atau EXIT_CODE_BAD_USAGE (2).
int main(int argc, char* argv[]) { // main(int argc, char* argv[]) adalah titik masuk program; hasil int-nya menjadi kode keluar.
    return runApp(argc, argv); // runApp(argc, argv) menjalankan seluruh program dengan opsi baris perintah yang sama, lalu kode keluarnya langsung diteruskan
    // ke sistem operasi lewat return.
} // akhir fungsi main()
