// ===== prompt.cpp - tanya jawab dengan pengguna =====
// Berkas ini berisi isi fungsi-fungsi yang dijanjikan include/estima/prompt.h. Polanya sama untuk
// semua fungsi baca: tampilkan pertanyaan, baca satu baris, periksa dengan fungsi dari input_parse.h,
// lalu ulangi bila salah sambil menampilkan pesan yang menjelaskan bentuk jawaban yang benar.
#include "estima/platform.h" // platform.h wajib di-include paling awal karena ia menyetel penyesuaian compiler sebelum header lain dibaca

#include "estima/prompt.h" // prompt.h adalah header milik berkas ini sendiri; berisi struct InputClosed dan deklarasi fungsi-fungsi baca di bawah

#include <cstddef> // <cstddef> digunakan untuk std::size_t, tipe bilangan bulat tanpa tanda negatif untuk ukuran, dipakai oleh PROMPT_LABEL_WIDTH
#include <iostream> // <iostream> digunakan untuk std::cout (menulis ke layar) dan std::cin (sumber ketikan yang dibaca per baris dengan std::getline)
#include <string> // <string> digunakan untuk std::string, tipe teks untuk label, ketikan, dan pesan, serta std::getline yang membaca satu baris utuh

#include "estima/input_parse.h" // input_parse.h digunakan untuk trimSpaces, parseMenuChoice, parseYesNo, dan pemeriksa checkQuantity/checkUnitWeight/checkPricePerKg
#include "estima/terminal.h" // terminal.h digunakan untuk inputIsInteractive(), yaitu apakah masukan diketik dari keyboard atau datang dari berkas/pipa
#include "estima/text_format.h" // text_format.h digunakan untuk padRight (meratakan label) dan formatThousands (menulis angka di pesan kesalahan)
#include "estima/ui.h" // ui.h digunakan untuk colorize dan screenStyle (memberi gaya pada teks pertanyaan) serta printError (menampilkan pesan salah)

// Label pertanyaan diratakan ke lebar yang sama supaya tanda titik dua tersusun lurus, misalnya
// "Jumlah (unit)" dan "Berat per unit (kg)" sama-sama ditambah spasi sampai 20 kolom. Label yang
// sudah lebih panjang dari 20 kolom (seperti "Simpan komponen ini? [Y/t]") dibiarkan apa adanya oleh padRight.
// static const membuat konstanta ini hanya terlihat di berkas ini, karena hanya dipakai oleh readLine().
static const std::size_t PROMPT_LABEL_WIDTH = 20; // PROMPT_LABEL_WIDTH = 20 adalah lebar kolom label pertanyaan; std::size_t karena padRight meminta lebar bertipe itu

// Menampilkan label, membaca satu baris, lalu membuang spasi di awal dan akhirnya. Semua fungsi baca
// lain memanggil fungsi ini, jadi urusan "masukan habis" dan "cetak ulang jawaban" cukup ditulis di sini.
std::string readLine(const std::string& label) { // readLine(const std::string& label) berfungsi untuk menampilkan pertanyaan label lalu mengembalikan jawaban pengguna
    std::cout << colorize(screenStyle(), STYLE_BOLD, padRight(label, PROMPT_LABEL_WIDTH) + " : "); // label diratakan 20 kolom, ditambah " : ", lalu ditulis tebal
    // (STYLE_BOLD) bila warna aktif. Tidak ada '\n' di ujungnya, jadi kursor menunggu tepat di sebelah kanan tanda titik dua.
    // std::cout.flush() berfungsi untuk memaksa teks yang masih tertahan di penampung keluaran (buffer) langsung
    // dikirim ke layar. Perlu karena pertanyaan tanpa '\n' bisa tertahan, padahal pengguna harus melihatnya dulu sebelum mengetik.
    std::cout.flush(); // std::cout.flush() memastikan pertanyaan sudah tampil sebelum program mulai menunggu ketikan

    // std::getline membaca satu baris utuh sampai Enter. Dipakai, bukan std::cin >>, karena
    // seluruh ketikan perlu diperiksa: cin >> berhenti di spasi atau di karakter pertama yang
    // bukan angka, dan sisa ketikannya akan ikut terbaca oleh pertanyaan berikutnya.
    std::string line; // line adalah tempat menampung satu baris ketikan apa adanya, sebelum dirapikan
    // std::cin adalah aliran masukan standar, yaitu ketikan dari keyboard (atau isi berkas/pipa bila masukan dialihkan).
    // std::getline(std::cin, line) berfungsi untuk membaca semua karakter sampai tombol Enter ke dalam line (tanda Enter-nya
    // sendiri dibuang). Hasilnya adalah aliran std::cin itu sendiri, yang bernilai salah bila pembacaan gagal karena masukan sudah habis.
    if (!std::getline(std::cin, line)) { // !std::getline(std::cin, line) artinya tidak ada lagi yang bisa dibaca (Ctrl+D, Ctrl+Z lalu Enter, atau berkas habis)
        // throw berfungsi untuk "melempar" sebuah benda pengecualian (exception): fungsi berhenti saat itu juga, begitu pula
        // fungsi-fungsi pemanggilnya satu per satu, sampai ditemukan blok try { ... } catch (Tipe) { ... } yang menangkap tipe
        // benda tersebut. try menandai bagian kode yang diawasi, catch berisi apa yang dikerjakan bila benda itu tertangkap.
        // Di program ini try/catch-nya ada di runApp() (src/app.cpp), jadi dari pertanyaan sedalam apa pun program langsung
        // kembali ke sana dan berhenti dengan rapi, tanpa setiap pemanggil harus memeriksa "apakah masukan sudah habis".
        throw InputClosed(); // throw InputClosed(); membuat benda bertipe InputClosed lalu melemparnya; ditangkap oleh catch (const InputClosed&) di runApp()
    } // akhir if masukan habis
    const std::string answer = trimSpaces(line); // answer adalah ketikan yang sudah dibuang spasi, tab, dan tanda akhir baris di awal dan akhirnya

    // Bila masukan bukan dari keyboard (berkas atau pipa), ketikan tidak tampil di layar.
    // Jawaban dicetak ulang supaya catatan sesi tetap terbaca seperti pemakaian biasa.
    if (!inputIsInteractive()) { // !inputIsInteractive() artinya masukan datang dari berkas atau pipa, bukan diketik langsung
        std::cout << answer << '\n'; // answer ditulis ulang ke layar diikuti '\n' (pindah baris), seolah-olah pengguna mengetiknya
    } // akhir if cetak ulang
    return answer; // return answer; mengembalikan jawaban yang sudah dirapikan ke pemanggil
} // akhir fungsi readLine()

// Dipanggil runMenuLoop() di src/app.cpp sesudah setiap aksi menu (selain Keluar), supaya hasil aksi
// sempat dibaca sebelum menu ditampilkan lagi.
void waitForEnter() { // waitForEnter() berfungsi untuk menahan layar sampai pengguna menekan Enter
    std::cout << colorize(screenStyle(), STYLE_HINT, "Tekan Enter untuk kembali ke menu..."); // ajakan ditulis dengan gaya STYLE_HINT (kuning bila warna aktif), tanpa '\n'
    std::cout.flush(); // std::cout.flush() memastikan ajakan itu sudah tampil sebelum program menunggu
    std::string discarded; // discarded adalah tempat membuang ketikan; isinya tidak dipakai karena yang ditunggu hanya tombol Enter
    if (!std::getline(std::cin, discarded)) { // !std::getline(...) artinya masukan sudah habis, jadi tidak ada Enter yang bisa ditunggu
        throw InputClosed(); // throw InputClosed(); melempar tanda "masukan habis" ke runApp(), sama seperti di readLine()
    } // akhir if masukan habis
    if (!inputIsInteractive()) { // !inputIsInteractive() artinya masukan dari berkas/pipa, jadi Enter pengguna tidak tampil di layar
        std::cout << '\n'; // '\n' ditulis sendiri supaya baris berikutnya tidak menempel di belakang ajakan "Tekan Enter..."
    } // akhir if pindah baris
} // akhir fungsi waitForEnter()

// Dipakai untuk pilihan menu utama, pilihan harga acuan, dan nomor komponen di src/app.cpp.
int readChoice(const std::string& label, int minValue, int maxValue, bool hasDefault, int defaultValue) { // readChoice(...) berfungsi untuk membaca nomor
    // pilihan dari minValue sampai maxValue; Enter kosong menghasilkan defaultValue bila hasDefault true.
    // while (true) mengulang pertanyaan sampai jawabannya sah; jalan keluarnya hanya return (atau InputClosed yang dilempar readLine() bila masukan habis).
    while (true) { // while (true) artinya syarat perulangan selalu benar, jadi pertanyaan diulang terus sampai salah satu return di bawah dijalankan
        const std::string text = readLine(label); // text adalah jawaban pengguna yang sudah dirapikan oleh readLine()
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya pengguna hanya menekan Enter dan pertanyaan ini punya jawaban bawaan
            return defaultValue; // return defaultValue; Enter kosong diterima sebagai jawaban bawaan
        } // akhir if jawaban bawaan
        int choice = 0; // choice adalah tempat hasil angka pilihan; diisi parseMenuChoice lewat referensi bila teksnya sah
        if (parseMenuChoice(text, minValue, maxValue, choice)) { // parseMenuChoice(...) true artinya text berupa angka di dalam rentang minValue..maxValue
            return choice; // return choice; pilihan sah dikembalikan ke pemanggil dan perulangan berhenti
        } // akhir if pilihan sah
        printError("Pilihan tidak dikenal. Ketik angka " + formatThousands(minValue) + " sampai " // pesan salah menyebut rentang yang benar;
                   + formatThousands(maxValue) + " lalu tekan Enter."); // formatThousands menulis angkanya dengan titik ribuan, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readChoice()

// Dipakai untuk semua konfirmasi di src/app.cpp, misalnya "Simpan komponen ini?" dan "Keluar dari program?".
bool askYesNo(const std::string& question, bool defaultYes) { // askYesNo(...) berfungsi untuk bertanya ya/tidak; hasil true berarti ya
    // Huruf besar pada [Y/t] atau [y/T] menunjukkan jawaban yang dipakai bila hanya menekan Enter.
    const std::string label = question + (defaultYes ? " [Y/t]" : " [y/T]"); // label adalah pertanyaan ditambah " [Y/t]" bila bawaannya ya, atau " [y/T]" bila tidak
    while (true) { // while (true) mengulang pertanyaan sampai jawabannya dikenali
        const std::string text = readLine(label); // text adalah jawaban pengguna yang sudah dirapikan
        if (text.empty()) { // text.empty() artinya pengguna hanya menekan Enter
            return defaultYes; // return defaultYes; Enter kosong dianggap memilih jawaban bawaan (yang ditulis huruf besar)
        } // akhir if Enter kosong
        const YesNoAnswer answer = parseYesNo(text); // answer adalah hasil pengenalan jawaban: ANSWER_YES, ANSWER_NO, atau ANSWER_UNKNOWN (tidak dikenali)
        if (answer == ANSWER_YES) { // answer == ANSWER_YES artinya pengguna mengetik y, ya, atau yes (huruf besar/kecil sama saja)
            return true; // return true; jawabannya ya
        } // akhir if ya
        if (answer == ANSWER_NO) { // answer == ANSWER_NO artinya pengguna mengetik t, tidak, n, atau no
            return false; // return false; jawabannya tidak
        } // akhir if tidak
        printError("Jawab dengan y (ya) atau t (tidak), lalu tekan Enter."); // jawaban tidak dikenali, jadi pesan ini ditampilkan lalu pertanyaan diulang
    } // akhir perulangan while
} // akhir fungsi askYesNo()

// Tiga pembaca data komponen di bawah punya pola yang sama: Enter kosong memakai nilai lama (bila ada),
// selain itu teks diperiksa oleh pemeriksa dari input_parse.h. Pemeriksa mengembalikan teks kosong bila
// sah (dan mengisi angkanya), atau pesan kesalahan yang langsung ditampilkan sebelum pertanyaan diulang.
int readQuantity(const std::string& label, bool hasDefault, int defaultValue) { // readQuantity(...) berfungsi untuk membaca jumlah unit komponen sampai sah
    while (true) { // while (true) mengulang pertanyaan sampai jumlah yang diketik sah
        const std::string text = readLine(label); // text adalah ketikan jumlah yang sudah dirapikan
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya Enter kosong saat mengubah komponen
            return defaultValue; // return defaultValue; jumlah lama dipertahankan
        } // akhir if nilai lama
        int quantity = 0; // quantity adalah tempat hasil angka jumlah; diisi checkQuantity lewat referensi bila sah
        const std::string problem = checkQuantity(text, quantity); // problem adalah pesan kesalahan dari checkQuantity, atau teks kosong bila jumlahnya sah
        if (problem.empty()) { // problem.empty() artinya tidak ada masalah, jadi quantity sudah berisi angka yang benar
            return quantity; // return quantity; jumlah yang sah dikembalikan
        } // akhir if sah
        printError(problem); // printError(problem) menampilkan alasan jawaban ditolak, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readQuantity()

double readUnitWeight(const std::string& label, bool hasDefault, double defaultValue) { // readUnitWeight(...) berfungsi untuk membaca berat per unit (kg) sampai sah
    while (true) { // while (true) mengulang pertanyaan sampai berat yang diketik sah
        const std::string text = readLine(label); // text adalah ketikan berat yang sudah dirapikan (boleh memakai koma atau titik desimal)
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya Enter kosong saat mengubah komponen
            return defaultValue; // return defaultValue; berat lama dipertahankan
        } // akhir if nilai lama
        double weightKg = 0.0; // weightKg adalah tempat hasil berat dalam kg; double karena berat boleh berdesimal
        const std::string problem = checkUnitWeight(text, weightKg); // problem adalah pesan kesalahan dari checkUnitWeight, atau teks kosong bila beratnya sah
        if (problem.empty()) { // problem.empty() artinya berat sah dan weightKg sudah terisi
            return weightKg; // return weightKg; berat yang sah dikembalikan
        } // akhir if sah
        printError(problem); // printError(problem) menampilkan alasan jawaban ditolak, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readUnitWeight()

double readPricePerKg(const std::string& label, bool hasDefault, double defaultValue) { // readPricePerKg(...) berfungsi untuk membaca harga per kg (rupiah) sampai sah
    while (true) { // while (true) mengulang pertanyaan sampai harga yang diketik sah
        const std::string text = readLine(label); // text adalah ketikan harga yang sudah dirapikan
        if (text.empty() && hasDefault) { // text.empty() && hasDefault artinya Enter kosong; dipakai untuk nilai lama saat mengubah, atau harga acuan katalog
            return defaultValue; // return defaultValue; harga bawaan (harga lama atau harga acuan) diterima
        } // akhir if nilai bawaan
        double pricePerKg = 0.0; // pricePerKg adalah tempat hasil harga per kg; double supaya sama dengan tipe pricePerKg di struct Component
        const std::string problem = checkPricePerKg(text, pricePerKg); // problem adalah pesan kesalahan dari checkPricePerKg, atau teks kosong bila harganya sah
        if (problem.empty()) { // problem.empty() artinya harga sah dan pricePerKg sudah terisi
            return pricePerKg; // return pricePerKg; harga yang sah dikembalikan
        } // akhir if sah
        printError(problem); // printError(problem) menampilkan alasan jawaban ditolak, lalu perulangan bertanya lagi
    } // akhir perulangan while
} // akhir fungsi readPricePerKg()
