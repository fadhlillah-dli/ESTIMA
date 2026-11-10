// ===== material_catalog.h - harga acuan material dan data contoh =====
// Header ini memperkenalkan katalog harga acuan material (dipakai menu Tambah komponen dan menu 7 Harga acuan material)
// serta fungsi pemuat data contoh (menu 6). Isinya ditulis di src/material_catalog.cpp.
#ifndef ESTIMA_MATERIAL_CATALOG_H // ESTIMA_MATERIAL_CATALOG_H adalah nama penjaga header ini; bila belum dikenal, isi di bawah ikut dikompilasi.
#define ESTIMA_MATERIAL_CATALOG_H // ESTIMA_MATERIAL_CATALOG_H didefinisikan sekarang supaya isi header ini tidak terbaca dua kali.

#include "estima/component.h" // "estima/component.h" dibutuhkan untuk tipe ComponentList yang dipakai loadSampleComponents().

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

#endif // ESTIMA_MATERIAL_CATALOG_H (akhir penjaga header material_catalog.h)
