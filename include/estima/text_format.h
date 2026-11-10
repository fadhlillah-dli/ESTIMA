// ===== text_format.h - pembentukan teks: angka, rupiah, berat, perataan, dan pelipatan baris =====
// Header ini berisi daftar fungsi "tukang tulis" program: fungsi yang mengubah angka menjadi teks yang enak dibaca orang
// Indonesia (titik ribuan, koma desimal, awalan Rp) dan fungsi yang mengatur lebar teks supaya tabel dan kotak tetap lurus.
// Semuanya hanya menyusun teks lalu mengembalikannya; tidak ada yang mencetak ke layar. Jadi hasilnya bisa dipakai untuk layar
// maupun untuk berkas laporan, dan bisa diuji otomatis dengan membandingkan teks hasilnya.
#ifndef ESTIMA_TEXT_FORMAT_H // ESTIMA_TEXT_FORMAT_H adalah nama penjaga header ini; baris sampai #endif hanya dibaca sekali walau header di-#include berkali-kali.
#define ESTIMA_TEXT_FORMAT_H // #define ESTIMA_TEXT_FORMAT_H menandai bahwa header ini sudah dibaca, jadi #include berikutnya dilewati.

#include <cstddef> // <cstddef> digunakan untuk tipe std::size_t, yaitu tipe ukuran dan lebar yang dipakai banyak fungsi di bawah.
#include <string> // <string> digunakan untuk tipe std::string, yaitu teks yang menjadi masukan dan hasil hampir semua fungsi di sini.
#include <vector> // <vector> digunakan untuk tipe std::vector, yaitu daftar yang panjangnya bisa bertambah; dipakai sebagai hasil wrapText().

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

#endif // ESTIMA_TEXT_FORMAT_H (akhir penjaga header text_format.h)
