// ===== platform.h - penyesuaian compiler yang harus dibaca paling awal =====
// Isi platform.h harus dibaca compiler paling awal, sebelum header pustaka standar. Pada versi modul,
// semua berkas .cpp di folder src/ dibuka dengan #include "estima/platform.h"; pada berkas tunggal, isinya
// ditaruh sebelum blok #include pustaka standar. Alasannya: pengaturan di bawah hanya berpengaruh bila
// sudah ada SEBELUM header pustaka standar dibaca compiler.
#ifndef ESTIMA_PLATFORM_H // ESTIMA_PLATFORM_H belum ada artinya header ini baru pertama kali dibaca, jadi isinya diproses.
// Penjaga header (include guard) adalah tiga baris #ifndef NAMA_H, #define NAMA_H, dan #endif yang mengapit
// seluruh isi header. Gunanya supaya isi header hanya dibaca satu kali per berkas .cpp, walaupun header itu
// di-#include berkali-kali (langsung maupun lewat header lain). Tanpa penjaga ini, struct dan konstanta di
// dalamnya akan terdefinisi dua kali dan compiler menolak programnya.
// #ifndef ("if not defined") berfungsi untuk bertanya ke compiler apakah sebuah nama BELUM didefinisikan;
// bila belum, baris sampai #endif pasangannya ikut dibaca, bila sudah, semuanya dilewati. Baris #define di
// bawah lalu mendaftarkan nama itu, sehingga pertanyaan yang sama dijawab "sudah" pada pembacaan berikutnya.
#define ESTIMA_PLATFORM_H // ESTIMA_PLATFORM_H didefinisikan sebagai penanda "sudah dibaca", jadi #ifndef di atas bernilai salah
// pada pembacaan berikutnya dan isi header tidak diproses dua kali.

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

#endif // ESTIMA_PLATFORM_H - akhir penjaga header yang dibuka oleh #ifndef ESTIMA_PLATFORM_H di atas
