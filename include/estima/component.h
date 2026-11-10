// ===== component.h - data satu komponen mesin dan daftar penyimpannya =====
// Header ini adalah dasar seluruh program: bentuk data satu komponen (struct Component), wadah
// penyimpannya (struct ComponentList berisi array), batas-batas data, dan empat operasi dasar daftar.
// Hampir semua bagian lain (perhitungan, katalog, laporan, menu) meng-#include header ini.
#ifndef ESTIMA_COMPONENT_H // ESTIMA_COMPONENT_H adalah nama penjaga header ini; isi di bawah hanya diproses bila nama itu belum ada.
#define ESTIMA_COMPONENT_H // ESTIMA_COMPONENT_H didefinisikan supaya isi header ini tidak terbaca dua kali dalam satu berkas .cpp.

// #include berfungsi untuk menyalin isi sebuah header ke berkas ini sebelum dikompilasi, supaya nama-nama
// yang dideklarasikan di header itu bisa dipakai. Bentuk #include <...> (kurung sudut) dipakai untuk header
// pustaka standar C++ yang ikut terpasang bersama compiler, sedangkan #include "..." (tanda kutip) dipakai
// untuk header milik proyek ini sendiri, misalnya "estima/text_format.h" yang dicari di folder include/ pada versi
// modul. Di berkas tunggal tidak ada #include "...", karena isi header proyek sudah disalin langsung sebagai bagian
// Persiapan dan Bagian 1-11.
#include <string> // <string> digunakan untuk tipe std::string, yaitu tipe teks yang dipakai untuk nama komponen.

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

#endif // ESTIMA_COMPONENT_H - akhir penjaga header ESTIMA_COMPONENT_H
