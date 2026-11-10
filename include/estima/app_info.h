// ===== app_info.h - identitas program =====
// Semua teks identitas dikumpulkan di satu tempat supaya banner, bantuan (--help), nomor versi,
// dan kepala laporan selalu memakai tulisan yang sama dan cukup diubah di sini.
// Header ini di-#include oleh src/app.cpp (banner pembuka, --help, --version) dan src/report.cpp
// (baris pertama laporan berkas .txt).
#ifndef ESTIMA_APP_INFO_H // ESTIMA_APP_INFO_H adalah nama penjaga header ini; isi di bawah hanya diproses bila nama itu belum ada.
#define ESTIMA_APP_INFO_H // ESTIMA_APP_INFO_H didefinisikan supaya pembacaan kedua header ini dalam satu berkas .cpp dilewati.

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

#endif // ESTIMA_APP_INFO_H - akhir penjaga header ESTIMA_APP_INFO_H
