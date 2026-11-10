// ===== app.h - alur utama program =====
// Header ini adalah "pintu depan" modul app: isinya hanya kode keluar program dan satu fungsi runApp().
// Berkas main.cpp cukup meng-include header ini lalu memanggil runApp(); seluruh menu dan aksinya ada di app.cpp.
#ifndef ESTIMA_APP_H // ESTIMA_APP_H adalah nama penjaga header; bila belum terdefinisi, isi header di bawah dibaca compiler.
#define ESTIMA_APP_H // ESTIMA_APP_H didefinisikan sekarang, jadi bila header ini ter-include lagi isinya dilewati (tidak dobel).

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

#endif // ESTIMA_APP_H - akhir penjaga header app.h
