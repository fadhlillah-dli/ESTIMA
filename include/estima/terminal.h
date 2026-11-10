// ===== terminal.h - kemampuan terminal: warna, UTF-8, lebar jendela, dan jenis masukan =====
// Modul terminal (terminal.h dan src/terminal.cpp) adalah satu-satunya bagian program yang berbicara langsung
// dengan sistem operasi.
// Windows dan macOS/Linux/Android punya cara berbeda untuk hal yang sama (menyalakan warna,
// memakai UTF-8, menanyakan lebar jendela), jadi perbedaan itu dikurung di modul ini; bagian lain
// program cukup memanggil lima fungsi di bawah tanpa perlu tahu sedang berjalan di sistem apa.
// Isi fungsinya ada di src/terminal.cpp.
#ifndef ESTIMA_TERMINAL_H // ESTIMA_TERMINAL_H adalah nama penjaga header; bila belum didefinisikan, isi berkas ini dibaca (mencegah isi header terbaca dua kali)
#define ESTIMA_TERMINAL_H // ESTIMA_TERMINAL_H didefinisikan sekarang, jadi #include berikutnya untuk berkas ini akan dilewati compiler

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

#endif // ESTIMA_TERMINAL_H - akhir penjaga header yang dibuka #ifndef di atas
