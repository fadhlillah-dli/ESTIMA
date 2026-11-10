// ===== prompt.h - tanya jawab dengan pengguna =====
// Setiap fungsi baca di sini mengulang pertanyaannya sampai jawabannya sah, jadi pemanggil
// selalu menerima data yang sudah benar. Pemeriksaan isinya sendiri dikerjakan oleh fungsi di
// input_parse.h; modul ini hanya mengurus bagian "bertanya, membaca ketikan, lalu mengulang bila
// salah". Isi fungsinya ada di src/prompt.cpp dan pemakainya terutama src/app.cpp.
#ifndef ESTIMA_PROMPT_H // ESTIMA_PROMPT_H adalah nama penjaga header; bila belum didefinisikan, isi berkas ini dibaca (mencegah isi header terbaca dua kali)
#define ESTIMA_PROMPT_H // ESTIMA_PROMPT_H didefinisikan sekarang, jadi #include berikutnya untuk berkas ini akan dilewati compiler

#include <string> // <string> digunakan untuk std::string, tipe teks yang dipakai untuk label pertanyaan dan jawaban pada deklarasi di bawah

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

#endif // ESTIMA_PROMPT_H - akhir penjaga header yang dibuka #ifndef di atas
