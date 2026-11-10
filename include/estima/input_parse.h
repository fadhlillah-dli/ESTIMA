// ===== input_parse.h - pemeriksaan dan pengubahan teks ketikan menjadi data =====
// Fungsi di sini hanya menerima teks dan mengembalikan hasil pemeriksaan; tidak ada satu pun yang
// membaca keyboard. Pembacaan keyboard dikerjakan readLine() di prompt.cpp, lalu teks hasil bacaannya
// diserahkan (lewat prompt.cpp dan app.cpp) ke fungsi-fungsi di sini. Pemisahan ini membuat setiap aturan validasi bisa diuji
// otomatis satu per satu (lihat tests/unit/test_input_parse.cpp) tanpa perlu mengetik apa pun.
#ifndef ESTIMA_INPUT_PARSE_H // ESTIMA_INPUT_PARSE_H belum didefinisikan artinya header ini baru pertama kali dibaca, jadi isinya boleh diproses
#define ESTIMA_INPUT_PARSE_H // ESTIMA_INPUT_PARSE_H didefinisikan sekarang supaya #include berikutnya terhadap header ini dilewati (tidak dobel)

#include <string> // <string> digunakan untuk tipe std::string, karena semua fungsi di sini menerima teks ketikan dan banyak yang mengembalikan teks pesan

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

#endif // ESTIMA_INPUT_PARSE_H - akhir penjaga header yang dibuka oleh #ifndef di atas
