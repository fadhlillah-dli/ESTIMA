// ===== report.h - penyusunan laporan dari data komponen =====
// Header ini berisi daftar fungsi yang menyusun laporan dari data komponen: rekap, peringkat biaya,
// daftar harga acuan, ringkasan satu komponen, daftar pilihan komponen, dan penyimpanan ke berkas.
// Fungsi cetak menerima std::ostream& (aliran keluaran apa pun), sehingga laporan yang sama bisa
// dikirim ke layar (std::cout) atau ke berkas (std::ofstream) tanpa menulis kodenya dua kali.
// Isi setiap fungsi ditulis di src/report.cpp; yang memanggilnya adalah menu di src/app.cpp.
#ifndef ESTIMA_REPORT_H // ESTIMA_REPORT_H adalah penjaga header: bila nama ini belum ada, isi header dibaca; bila sudah, seluruh isi dilewati
#define ESTIMA_REPORT_H // ESTIMA_REPORT_H didefinisikan di sini, artinya header ini sudah dibaca sekali dan tidak akan dibaca dua kali

#include <ostream> // <ostream> digunakan untuk tipe std::ostream, yaitu aliran keluaran umum yang menjadi tipe parameter out di bawah
#include <string> // <string> digunakan untuk tipe std::string pada parameter fileName dan errorMessage milik saveReport()

#include "estima/component.h" // "estima/component.h" digunakan untuk struct Component dan ComponentList, yaitu data yang dilaporkan
#include "estima/ui.h" // "estima/ui.h" digunakan untuk struct OutputStyle, yaitu keterangan lebar, warna, dan jenis garis tujuan keluaran

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

#endif // ESTIMA_REPORT_H (akhir penjaga header yang dibuka oleh #ifndef di atas)
