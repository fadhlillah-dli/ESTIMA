// ===== input_parse.cpp - pemeriksaan dan pengubahan teks ketikan menjadi data =====
// Berkas ini berisi isi (definisi) fungsi-fungsi yang dideklarasikan di input_parse.h. Semua
// fungsinya bekerja pada teks yang sudah dibaca prompt.cpp, jadi di sini
// tidak ada std::cin sama sekali.
#include "estima/platform.h" // "estima/platform.h" wajib di-include paling awal, sebelum header pustaka standar, karena berisi penyesuaian compiler

#include "estima/input_parse.h" // "estima/input_parse.h" adalah header milik berkas ini sendiri; di-include supaya compiler mencocokkan definisi di sini dengan deklarasinya

#include <cstddef> // <cstddef> digunakan untuk tipe std::size_t, yaitu tipe hasil size() dan posisi karakter di dalam std::string
#include <string> // <string> digunakan untuk tipe std::string beserta fungsi anggotanya (size, substr, find, compare, dan lain-lain)

#include "estima/component.h" // "estima/component.h" digunakan untuk konstanta batas data: MAX_NAME_LENGTH, MIN_/MAX_QUANTITY, MIN_/MAX_UNIT_WEIGHT_KG, MIN_/MAX_PRICE_PER_KG
#include "estima/text_format.h" // "estima/text_format.h" digunakan untuk formatThousands, formatWeight, formatRupiah (menyusun angka di pesan) dan toLowerAscii

// Angka dengan lebih dari sembilan digit pasti di luar semua rentang program ini (batas terbesar,
// harga Rp100.000.000, hanya sembilan digit). Batas ini diperiksa sebelum teks diubah menjadi
// bilangan supaya hasilnya tidak pernah melampaui daya tampung int (sekitar 2,1 miliar, sepuluh
// digit). Tiga konstanta pertama di blok ini bertipe std::size_t karena dibandingkan dengan size() milik
// std::string, yang juga std::size_t, sehingga perbandingannya tidak mencampur tipe bertanda dan
// tak bertanda. REPORT_EXTENSION di bawahnya hanya teks akhiran, jadi bukan std::size_t.
static const std::size_t MAX_SAFE_DIGITS = 9; // MAX_SAFE_DIGITS = 9 adalah batas banyak digit yang boleh diubah menjadi bilangan; 999.999.999 masih muat di int
static const std::size_t MAX_WEIGHT_DECIMALS = 3; // MAX_WEIGHT_DECIMALS = 3 artinya berat paling banyak 3 angka di belakang koma, yaitu ketelitian gram (0,001 kg)
static const std::size_t MAX_FILE_NAME_LENGTH = 60; // MAX_FILE_NAME_LENGTH = 60 adalah panjang maksimal nama berkas yang diketik (belum termasuk .txt tambahan)
static const char* const REPORT_EXTENSION = ".txt"; // REPORT_EXTENSION = ".txt" adalah akhiran yang ditambahkan ke nama berkas laporan bila belum ada

// Nama perangkat yang dicadangkan Windows: berkas bernama CON.txt atau NUL.txt tidak bisa dibuat,
// karena Windows menganggap nama itu sebagai perangkat (misalnya CON adalah layar/keyboard).
// Disimpan dalam array teks supaya pemeriksaannya cukup satu perulangan, bukan 22 perbandingan
// yang ditulis satu per satu. Semua ditulis huruf kecil karena nama ketikan pengguna juga diubah
// ke huruf kecil dulu sebelum dibandingkan (Windows tidak membedakan CON dan con).
static const char* const RESERVED_FILE_NAMES[] = { // RESERVED_FILE_NAMES[] adalah array berisi 22 teks tetap; ukurannya tidak ditulis karena compiler menghitungnya dari isi
    "con", "prn", "aux", "nul", // "con", "prn", "aux", "nul" adalah perangkat konsol, printer, port tambahan, dan perangkat kosong
    "com1", "com2", "com3", "com4", "com5", "com6", "com7", "com8", "com9", // "com1" sampai "com9" adalah nama port serial
    "lpt1", "lpt2", "lpt3", "lpt4", "lpt5", "lpt6", "lpt7", "lpt8", "lpt9" // "lpt1" sampai "lpt9" adalah nama port printer paralel; elemen terakhir tanpa koma
}; // akhir array RESERVED_FILE_NAMES
static const std::size_t RESERVED_FILE_NAME_COUNT = sizeof(RESERVED_FILE_NAMES) / sizeof(RESERVED_FILE_NAMES[0]); // RESERVED_FILE_NAME_COUNT adalah banyaknya elemen array
// di atas (22), dihitung otomatis dari ukuran seluruh array dibagi ukuran satu elemen, jadi tetap benar bila daftarnya ditambah.

// Empat fungsi kecil berikut memeriksa jenis satu karakter. Ditulis sendiri dengan perbandingan
// langsung, sehingga hanya karakter ASCII biasa yang dianggap huruf atau angka, apa pun pengaturan
// bahasa komputernya. Keempatnya hanya fungsi bantu di berkas ini dan tidak dideklarasikan di
// input_parse.h.
static bool isBlankChar(char character) { // isBlankChar(character) berfungsi untuk menentukan apakah character termasuk karakter kosong
    return character == ' ' || character == '\t' || character == '\r' || character == '\n'; // ' ' spasi, '\t' tab, '\r' dan '\n' tanda akhir baris;
    // '\r' ikut dihitung karena teks bergaya Windows berakhiran \r\n, sehingga \r bisa tertinggal di ujung ketikan.
} // akhir fungsi isBlankChar()

static bool isAsciiDigit(char character) { // isAsciiDigit(character) berfungsi untuk menentukan apakah character adalah angka 0-9
    return character >= '0' && character <= '9'; // '0' sampai '9' punya kode berurutan di tabel ASCII (48 sampai 57), jadi cukup diperiksa rentangnya
} // akhir fungsi isAsciiDigit()

static bool isAsciiLetter(char character) { // isAsciiLetter(character) berfungsi untuk menentukan apakah character adalah huruf A-Z atau a-z (tanpa aksen)
    return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'); // huruf besar dan huruf kecil diperiksa sebagai dua rentang terpisah,
    // karena di tabel ASCII keduanya tidak bersambung (di antaranya ada tanda seperti [ dan _).
} // akhir fungsi isAsciiLetter()

// isOneOf() dipakai untuk tanda baca yang diizinkan: daftarnya cukup ditulis sebagai satu teks
// (misalnya "-_.") daripada menulis banyak perbandingan character == '-' || character == '_' ...
static bool isOneOf(char character, const std::string& allowed) { // isOneOf(character, allowed) menghasilkan true bila character ada di dalam teks allowed
    // std::string::npos adalah nilai khusus yang dikembalikan find() bila yang dicari tidak ditemukan.
    return allowed.find(character) != std::string::npos; // allowed.find(character) != std::string::npos artinya character ditemukan di suatu posisi di allowed
} // akhir fungsi isOneOf()

// Membuang angka 0 di depan ("007" menjadi "7") tetapi menyisakan satu digit ("000" menjadi "0").
// Angka 0 di depan tidak mengubah nilai, jadi dibuang dulu sebelum digit dihitung; dengan begitu
// "0000000024" (sepuluh digit) tetap dianggap 24 dan tidak ditolak oleh batas MAX_SAFE_DIGITS.
static std::string stripLeadingZeros(const std::string& digits) { // stripLeadingZeros(digits) mengembalikan teks digits tanpa angka 0 di depannya
    std::size_t start = 0; // start adalah posisi digit pertama yang akan disimpan; mulai dari posisi 0 (karakter paling kiri)
    while (start + 1 < digits.size() && digits[start] == '0') { // start + 1 < digits.size() menjaga digit terakhir tidak ikut dibuang;
        // digits[start] == '0' artinya digit di posisi start adalah nol.
        start = start + 1; // start = start + 1 berfungsi untuk bergeser satu digit ke kanan, melewati angka 0 tadi
    } // akhir perulangan while
    return digits.substr(start); // digits.substr(start) mengambil potongan teks mulai posisi start sampai ujung
} // akhir fungsi stripLeadingZeros()

// Mengubah deret digit menjadi bilangan: tiap digit menggeser nilai lama satu tempat (x 10)
// lalu ditambahkan. Contoh "185": 0 x 10 + 1 = 1, lalu 1 x 10 + 8 = 18, lalu 18 x 10 + 5 = 185.
// Pemanggil sudah memastikan teksnya hanya digit dan tidak terlalu panjang (paling banyak 9 digit).
// Hasilnya double karena berat dan harga memang disimpan sebagai double (weightKg, pricePerKg) dan
// dibandingkan dengan batas bertipe double; bilangan bulat 9 digit masih disimpan double dengan tepat.
static double digitsToNumber(const std::string& digits) { // digitsToNumber(digits) mengembalikan nilai bilangan dari teks yang seluruhnya digit
    double value = 0.0; // value = 0.0 adalah hasil sementara; mulai dari nol sebelum ada digit yang dibaca
    for (std::size_t i = 0; i < digits.size(); ++i) { // perulangan for membaca digit satu per satu dari kiri (i = 0) sampai digit terakhir
        // digits[i] - '0' adalah cara mengubah karakter angka menjadi nilainya: kode '7' (55) dikurangi kode '0' (48) hasilnya 7.
        value = value * 10.0 + static_cast<double>(digits[i] - '0'); // value * 10.0 menggeser nilai lama satu tempat ke kiri, lalu nilai digit baru ditambahkan
    } // akhir perulangan for
    return value; // return value mengembalikan bilangan yang sudah tersusun lengkap
} // akhir fungsi digitsToNumber()

// trimSpaces() mencari posisi karakter bukan-kosong pertama dari kiri (begin) dan dari kanan (end),
// lalu mengambil potongan di antaranya. Cara ini hanya sekali menyalin teks.
std::string trimSpaces(const std::string& text) { // trimSpaces(text) mengembalikan text tanpa karakter kosong di awal dan akhirnya
    std::size_t begin = 0; // begin adalah posisi awal potongan; mulai dari karakter pertama (posisi 0)
    std::size_t end = text.size(); // end adalah posisi SESUDAH karakter terakhir potongan; mulai dari panjang teks (tepat di luar ujung)
    while (begin < end && isBlankChar(text[begin])) { // selama karakter di posisi begin kosong (dan begin belum bertemu end), lewati karakter itu
        begin = begin + 1; // begin = begin + 1 berfungsi untuk maju satu karakter ke kanan
    } // akhir perulangan while dari kiri
    while (end > begin && isBlankChar(text[end - 1])) { // text[end - 1] adalah karakter terakhir potongan; bila kosong, potongan dipendekkan dari kanan
        end = end - 1; // end = end - 1 berfungsi untuk mundur satu karakter ke kiri
    } // akhir perulangan while dari kanan
    return text.substr(begin, end - begin); // text.substr(begin, end - begin) mengambil end - begin karakter mulai posisi begin; teks yang seluruhnya spasi menjadi kosong
} // akhir fungsi trimSpaces()

// collapseSpaces() menyusun teks baru karakter demi karakter. Deret karakter kosong tidak langsung
// ditulis, tetapi hanya dicatat (pendingSpace); satu spasi baru ditulis ketika muncul karakter
// lain sesudahnya. Dengan begitu spasi di awal, spasi ganda, dan spasi di akhir hilang sekaligus.
std::string collapseSpaces(const std::string& text) { // collapseSpaces(text) mengembalikan text yang sudah dirapikan spasinya
    std::string result; // result adalah teks hasil; std::string yang baru dibuat isinya kosong
    bool pendingSpace = false; // pendingSpace bernilai true bila ada spasi yang masih "ditahan" dan perlu ditulis sebelum karakter berikutnya
    for (std::size_t i = 0; i < text.size(); ++i) { // perulangan for memeriksa setiap karakter text dari kiri ke kanan
        if (isBlankChar(text[i])) { // isBlankChar(text[i]) artinya karakter ini spasi, tab, atau tanda akhir baris
            // Spasi baru ditulis nanti, hanya bila masih ada karakter lain sesudahnya.
            pendingSpace = !result.empty(); // !result.empty() bernilai false selama result masih kosong, jadi spasi di awal teks tidak pernah ditahan
        } else { // else: karakter ini bukan karakter kosong
            if (pendingSpace) { // pendingSpace artinya sebelumnya ada spasi yang ditahan
                result += ' '; // result += ' ' menulis tepat satu spasi, berapa pun panjang deret spasi aslinya
            } // akhir if pendingSpace
            result += text[i]; // result += text[i] menyalin karakter ini ke hasil
            pendingSpace = false; // pendingSpace = false karena spasi yang ditahan sudah ditulis
        } // akhir if-else karakter kosong
    } // akhir perulangan for
    return result; // return result; spasi yang masih ditahan di ujung tidak pernah ditulis, jadi spasi di akhir teks ikut hilang
} // akhir fungsi collapseSpaces()

bool isAllDigits(const std::string& text) { // isAllDigits(text) menghasilkan true hanya bila text tidak kosong dan semua karakternya angka 0-9
    if (text.empty()) { // text.empty() artinya teks tidak berisi karakter apa pun
        return false; // return false karena teks kosong tidak bisa dianggap bilangan
    } // akhir if teks kosong
    for (std::size_t i = 0; i < text.size(); ++i) { // perulangan for memeriksa setiap karakter text
        if (!isAsciiDigit(text[i])) { // !isAsciiDigit(text[i]) artinya karakter ini bukan angka
            return false; // return false langsung: satu karakter bukan angka sudah cukup untuk menolak, sisanya tidak perlu diperiksa
        } // akhir if bukan angka
    } // akhir perulangan for
    return true; // return true karena semua karakter sudah lolos pemeriksaan
} // akhir fungsi isAllDigits()

// Urutan pemeriksaan nama: kosong, karakter pertama, seluruh karakter, lalu panjangnya.
// Panjang sengaja diperiksa paling akhir: sesudah semua karakter dipastikan huruf, angka, spasi, atau tanda yang diizinkan
// (semuanya satu byte), name.size() pasti sama dengan banyaknya karakter, sehingga angka di pesan "terlalu panjang" selalu tepat.
// Pemanggil (readComponentName() di app.cpp) sudah merapikan spasinya dengan collapseSpaces().
std::string checkComponentName(const std::string& name) { // checkComponentName(name) mengembalikan "" bila nama sah, atau pesan kesalahan bila tidak
    if (name.empty()) { // name.empty() artinya pengguna belum mengetik nama
        return "Nama komponen belum diisi."; // pesan untuk nama kosong
    } // akhir if nama kosong
    if (!isAsciiLetter(name[0]) && !isAsciiDigit(name[0])) { // name[0] adalah karakter pertama; aman diambil karena nama sudah pasti tidak kosong
        return "Nama harus diawali huruf atau angka."; // pesan bila nama diawali tanda baca atau spasi
    } // akhir if karakter pertama
    for (std::size_t i = 0; i < name.size(); ++i) { // perulangan for memeriksa setiap karakter nama
        const char character = name[i]; // character adalah salinan karakter ke-i; disimpan supaya kondisi di bawah lebih pendek dibaca
        if (!isAsciiLetter(character) && !isAsciiDigit(character) && !isOneOf(character, " -_.,/()+#&")) { // karakter ditolak bila bukan huruf, bukan angka,
            // dan tidak ada di daftar " -_.,/()+#&" (spasi dan sepuluh tanda baca yang diizinkan).
            return "Nama hanya boleh berisi huruf A-Z, angka, spasi, dan tanda - _ . , / ( ) + # &"; // pesan yang sekaligus menyebut semua karakter yang boleh
        } // akhir if karakter terlarang
    } // akhir perulangan for
    if (name.size() > static_cast<std::size_t>(MAX_NAME_LENGTH)) { // MAX_NAME_LENGTH (30, bertipe int) diubah ke std::size_t supaya sama tipenya dengan name.size()
        return "Nama terlalu panjang (" + formatThousands(static_cast<double>(name.size())) + " karakter); paling banyak " // panjang nama ikut disebut di pesan;
        // static_cast<double> diperlukan karena formatThousands() menerima double.
            + formatThousands(MAX_NAME_LENGTH) + " karakter."; // formatThousands(MAX_NAME_LENGTH) menulis batasnya (30) dari konstanta, bukan angka yang diketik ulang
    } // akhir if nama terlalu panjang
    return ""; // return "" (teks kosong) artinya nama sah
} // akhir fungsi checkComponentName()

// Jumlah diperiksa dua tahap: bentuknya dulu (hanya digit), baru nilainya (rentang 1-1.000.000),
// supaya pesan kesalahannya tepat sasaran.
std::string checkQuantity(const std::string& text, int& quantity) { // checkQuantity(text, quantity) mengembalikan "" dan mengisi quantity bila sah, atau pesan bila tidak
    if (text.empty()) { // text.empty() artinya pengguna hanya menekan Enter
        return "Jumlah belum diisi."; // pesan untuk jumlah kosong
    } // akhir if kosong
    if (!isAllDigits(text)) { // !isAllDigits(text) artinya ada karakter selain angka (titik, koma, minus, huruf, spasi)
        return "Jumlah harus bilangan bulat tanpa titik, koma, spasi, atau huruf. Contoh: 24"; // pesan bentuk yang salah beserta contoh yang benar
    } // akhir if bukan angka
    const std::string rangeMessage = "Jumlah harus antara " + formatThousands(MIN_QUANTITY) + " dan " // rangeMessage adalah pesan rentang; disusun sekali karena
        // dipakai di dua tempat di bawah (terlalu banyak digit, atau nilainya di luar rentang).
        + formatThousands(MAX_QUANTITY) + " unit."; // angka batas diambil dari MIN_QUANTITY dan MAX_QUANTITY di component.h
    const std::string digits = stripLeadingZeros(text); // digits adalah ketikan tanpa angka 0 di depan, supaya panjangnya mencerminkan besar angkanya
    if (digits.size() > MAX_SAFE_DIGITS) { // digits.size() > MAX_SAFE_DIGITS artinya angkanya lebih dari 9 digit, pasti melebihi 1.000.000
        return rangeMessage; // ditolak sebelum diubah menjadi bilangan
    } // akhir if terlalu banyak digit
    const double value = digitsToNumber(digits); // value adalah nilai bilangan dari ketikan
    if (value < MIN_QUANTITY || value > MAX_QUANTITY) { // value < MIN_QUANTITY || value > MAX_QUANTITY artinya di bawah 1 (yaitu 0) atau di atas 1.000.000
        return rangeMessage; // pesan rentang
    } // akhir if di luar rentang
    quantity = static_cast<int>(value); // static_cast<int>(value) mengubah value ke int; aman karena value pasti bulat dan di dalam rentang
    return ""; // "" artinya sah, dan quantity sudah terisi
} // akhir fungsi checkQuantity()

// Berat boleh ditulis dengan koma atau titik desimal ("1,85" atau "1.85"), karena orang Indonesia
// biasa memakai koma sedangkan kalkulator dan komputer memakai titik.
std::string checkUnitWeight(const std::string& text, double& weightKg) { // checkUnitWeight(...) mengembalikan "" dan mengisi weightKg bila sah, atau pesan bila tidak
    if (text.empty()) { // text.empty() artinya berat belum diketik
        return "Berat per unit belum diisi."; // pesan untuk berat kosong
    } // akhir if kosong
    const std::string formatMessage = // formatMessage adalah pesan untuk bentuk ketikan yang salah; disimpan karena dipakai di dua tempat
        "Berat ditulis dengan angka; desimal boleh memakai koma atau titik. Contoh: 1,85"; // isi pesan beserta contoh yang benar

    // Teks dipecah di tanda desimal pertama: bagian utuh di kiri, bagian pecahan di kanan.
    std::string wholePart = text; // wholePart adalah bagian utuh (kilogram); mula-mula seluruh teks, untuk ketikan tanpa tanda desimal
    std::string fractionPart; // fractionPart adalah bagian pecahan (di belakang koma); mula-mula kosong
    // find_first_of(".,") adalah fungsi std::string yang mencari posisi pertama dari SALAH SATU karakter di dalam ".,".
    const std::size_t separator = text.find_first_of(".,"); // separator adalah posisi titik atau koma pertama, atau npos bila tidak ada
    if (separator != std::string::npos) { // separator != std::string::npos artinya ketikan memang memakai tanda desimal
        wholePart = text.substr(0, separator); // text.substr(0, separator) mengambil semua karakter sebelum tanda desimal
        fractionPart = text.substr(separator + 1); // text.substr(separator + 1) mengambil semua karakter sesudah tanda desimal
        if (!isAllDigits(fractionPart)) { // pecahan harus angka semua; ini juga menolak "1," (pecahan kosong) dan "1,2,3" (tanda desimal kedua)
            return formatMessage; // pesan bentuk yang salah
        } // akhir if pecahan bukan angka
    } // akhir if ada tanda desimal
    if (!isAllDigits(wholePart)) { // bagian utuh juga harus angka semua; ini menolak huruf, minus, dan ",5" (bagian utuh kosong)
        return formatMessage; // pesan bentuk yang salah
    } // akhir if bagian utuh bukan angka
    if (fractionPart.size() > MAX_WEIGHT_DECIMALS) { // fractionPart.size() > MAX_WEIGHT_DECIMALS artinya lebih dari 3 angka di belakang koma
        return "Berat paling teliti sampai gram, jadi paling banyak " + formatThousands(static_cast<double>(MAX_WEIGHT_DECIMALS)) // pesan ketelitian;
        // angka 3 ditulis dari konstanta MAX_WEIGHT_DECIMALS (static_cast<double> karena formatThousands() menerima double), jadi pesan
        // dan aturan tidak bisa berbeda.
            + " angka di belakang koma. Contoh: 0,125"; // sambungan pesan beserta contoh berat yang sah (125 gram)
    } // akhir if terlalu banyak desimal

    const std::string rangeMessage = "Berat per unit harus antara " + formatWeight(MIN_UNIT_WEIGHT_KG) + " dan " // rangeMessage adalah pesan rentang berat;
        // formatWeight() menulis batasnya dengan tiga desimal: "0,001 kg" dan "10.000,000 kg".
        + formatWeight(MAX_UNIT_WEIGHT_KG) + "."; // batas atas diambil dari MAX_UNIT_WEIGHT_KG di component.h
    wholePart = stripLeadingZeros(wholePart); // angka 0 di depan bagian utuh dibuang supaya panjangnya mencerminkan besar angkanya
    if (wholePart.size() > MAX_SAFE_DIGITS) { // wholePart.size() > MAX_SAFE_DIGITS artinya bagian utuhnya lebih dari 9 digit, pasti di atas 10.000 kg
        return rangeMessage; // pesan rentang
    } // akhir if terlalu banyak digit
    // Bagian pecahan dilengkapi sampai tiga digit ("85" menjadi "850"), sehingga isinya gram.
    // Nilai disusun dalam gram (bilangan bulat) lalu dibagi 1000 sekali saja, supaya hasilnya
    // adalah nilai kilogram yang paling dekat dengan ketikan pengguna.
    while (fractionPart.size() < MAX_WEIGHT_DECIMALS) { // selama pecahan kurang dari 3 digit, tambahkan 0 di kanannya (nilainya tidak berubah: 0,85 = 0,850)
        fractionPart += '0'; // fractionPart += '0' menambah satu angka 0 di ujung kanan
    } // akhir perulangan while
    const double value = (digitsToNumber(wholePart) * 1000.0 + digitsToNumber(fractionPart)) / 1000.0; // value adalah berat dalam kg; contoh "1,85":
    // (1 x 1000 + 850) / 1000 = 1850 gram / 1000 = 1,85 kg.
    if (value < MIN_UNIT_WEIGHT_KG || value > MAX_UNIT_WEIGHT_KG) { // value di luar 0,001 sampai 10.000 kg (misalnya "0" atau "0,000") ditolak
        return rangeMessage; // pesan rentang
    } // akhir if di luar rentang
    weightKg = value; // weightKg = value; parameter keluaran baru diisi setelah semua pemeriksaan lolos
    return ""; // "" artinya sah
} // akhir fungsi checkUnitWeight()

// Harga diperiksa dengan pola yang sama seperti jumlah: bentuk dulu, baru rentang. Harga hanya
// menerima rupiah bulat, jadi "18.500" atau "Rp18500" ditolak dengan pesan yang menjelaskan sebabnya.
std::string checkPricePerKg(const std::string& text, double& pricePerKg) { // checkPricePerKg(...) mengembalikan "" dan mengisi pricePerKg bila sah, atau pesan bila tidak
    if (text.empty()) { // text.empty() artinya harga belum diketik
        return "Harga per kg belum diisi."; // pesan untuk harga kosong
    } // akhir if kosong
    if (!isAllDigits(text)) { // !isAllDigits(text) artinya ada karakter selain angka, misalnya "Rp", titik ribuan, atau koma
        return "Harga ditulis dalam rupiah bulat tanpa Rp, titik, koma, atau spasi. Contoh: 18500"; // pesan bentuk yang salah beserta contoh
    } // akhir if bukan angka
    const std::string rangeMessage = "Harga per kg harus antara " + formatRupiah(MIN_PRICE_PER_KG) + " dan " // rangeMessage adalah pesan rentang harga;
        // formatRupiah() menulis batasnya sebagai "Rp1" dan "Rp100.000.000".
        + formatRupiah(MAX_PRICE_PER_KG) + "."; // batas atas diambil dari MAX_PRICE_PER_KG di component.h
    const std::string digits = stripLeadingZeros(text); // digits adalah ketikan tanpa angka 0 di depan
    if (digits.size() > MAX_SAFE_DIGITS) { // lebih dari 9 digit pasti di atas Rp100.000.000
        return rangeMessage; // pesan rentang
    } // akhir if terlalu banyak digit
    const double value = digitsToNumber(digits); // value adalah nilai harga dari ketikan
    if (value < MIN_PRICE_PER_KG || value > MAX_PRICE_PER_KG) { // value di bawah Rp1 (yaitu 0) atau di atas Rp100.000.000 ditolak
        return rangeMessage; // pesan rentang
    } // akhir if di luar rentang
    pricePerKg = value; // pricePerKg = value; parameter keluaran diisi setelah semua pemeriksaan lolos
    return ""; // "" artinya sah
} // akhir fungsi checkPricePerKg()

// Nama berkas laporan dibatasi ketat (huruf, angka, - _ .) supaya berkas pasti bisa dibuat di
// Windows, macOS, dan Linux, dan supaya laporan tidak tertulis ke folder lain lewat tanda / atau \.
std::string checkReportFileName(const std::string& text, std::string& fileName) { // checkReportFileName(text, fileName) mengembalikan "" dan mengisi fileName bila sah
    if (text.empty()) { // text.empty() artinya nama berkas kosong (app.cpp sudah mengganti Enter kosong dengan nama bawaan, jadi ini jaga-jaga)
        return "Nama berkas belum diisi."; // pesan untuk nama kosong
    } // akhir if kosong
    if (text.size() > MAX_FILE_NAME_LENGTH) { // text.size() > MAX_FILE_NAME_LENGTH artinya nama lebih dari 60 karakter
        return "Nama berkas terlalu panjang; paling banyak " + formatThousands(static_cast<double>(MAX_FILE_NAME_LENGTH)) + " karakter."; // angka 60 di pesan
        // ditulis dari konstanta MAX_FILE_NAME_LENGTH, sehingga pesan selalu sama dengan batas yang diperiksa di atas.
    } // akhir if terlalu panjang
    for (std::size_t i = 0; i < text.size(); ++i) { // perulangan for memeriksa setiap karakter nama berkas
        const char character = text[i]; // character adalah karakter ke-i yang sedang diperiksa
        if (!isAsciiLetter(character) && !isAsciiDigit(character) && !isOneOf(character, "-_.")) { // karakter ditolak bila bukan huruf, bukan angka, dan bukan - _ .
            // Dua teks "..." yang ditulis berurutan digabung compiler menjadi satu (lihat material_catalog.cpp); di sini cara itu
            // dipakai supaya pesan yang panjang bisa dipecah ke dua baris kode.
            return "Nama berkas hanya boleh berisi huruf, angka, tanda hubung (-), garis bawah (_), dan titik, " // bagian pertama pesan
                   "tanpa spasi dan tanpa nama folder."; // bagian kedua pesan, disambung langsung ke bagian pertama
        } // akhir if karakter terlarang
    } // akhir perulangan for
    if (text[0] == '.' || text[text.size() - 1] == '.') { // text[0] karakter pertama, text[text.size() - 1] karakter terakhir; keduanya tidak boleh titik
        return "Nama berkas tidak boleh diawali atau diakhiri titik."; // nama berawalan titik menjadi berkas tersembunyi di macOS/Linux; berakhiran titik dibuang Windows
    } // akhir if titik di ujung

    // Bagian sebelum titik pertama dibandingkan dengan nama perangkat Windows.
    const std::string baseName = toLowerAscii(text.substr(0, text.find('.'))); // baseName adalah nama sebelum titik pertama, dalam huruf kecil; bila tidak ada titik,
    // find() menghasilkan npos dan substr(0, npos) mengambil seluruh teks.
    for (std::size_t i = 0; i < RESERVED_FILE_NAME_COUNT; ++i) { // perulangan for mencocokkan baseName dengan setiap elemen RESERVED_FILE_NAMES
        if (baseName == RESERVED_FILE_NAMES[i]) { // baseName == RESERVED_FILE_NAMES[i] membandingkan isi teks (std::string bisa dibandingkan langsung dengan teks "...")
            return "Nama itu dicadangkan Windows untuk perangkat. Pilih nama lain, misalnya laporan-estima."; // pesan untuk nama perangkat seperti con atau lpt1
        } // akhir if nama dicadangkan
    } // akhir perulangan for

    // Akhiran .txt ditambahkan bila belum ada, supaya berkasnya langsung dikenali penyunting teks.
    const std::string extension = REPORT_EXTENSION; // extension adalah salinan ".txt" sebagai std::string, supaya bisa memakai size() dan dibandingkan
    const std::string lowered = toLowerAscii(text); // lowered adalah nama dalam huruf kecil, supaya akhiran ".TXT" atau ".Txt" juga dikenali
    const bool hasExtension = lowered.size() > extension.size() // hasExtension bernilai true bila nama lebih panjang dari ".txt" (masih ada nama di depannya)
        && lowered.compare(lowered.size() - extension.size(), extension.size(), extension) == 0; // dan 4 karakter terakhirnya sama dengan ".txt";
        // compare(posisi, panjang, teks) menghasilkan 0 bila potongan itu sama persis dengan teks pembanding.
    fileName = hasExtension ? text : text + extension; // bila sudah berakhiran .txt, nama dipakai apa adanya (huruf aslinya); bila belum, ".txt" ditambahkan
    return ""; // "" artinya sah, dan fileName sudah terisi
} // akhir fungsi checkReportFileName()

// parseMenuChoice() memakai langkah yang sama dengan checkQuantity(), tetapi hanya menjawab sah
// atau tidak; batas rentangnya diterima dari pemanggil karena jumlah pilihan tiap menu berbeda-beda.
bool parseMenuChoice(const std::string& text, int minValue, int maxValue, int& choice) { // parseMenuChoice(...) menghasilkan true dan mengisi choice bila nomornya sah
    if (!isAllDigits(text)) { // !isAllDigits(text) artinya bukan angka; teks kosong juga tertolak di sini
        return false; // false artinya pilihan tidak sah
    } // akhir if bukan angka
    const std::string digits = stripLeadingZeros(text); // digits adalah ketikan tanpa angka 0 di depan ("03" menjadi "3")
    if (digits.size() > MAX_SAFE_DIGITS) { // lebih dari 9 digit pasti di luar rentang menu mana pun
        return false; // ditolak sebelum diubah menjadi bilangan
    } // akhir if terlalu banyak digit
    const double value = digitsToNumber(digits); // value adalah nomor yang diketik
    if (value < minValue || value > maxValue) { // value < minValue || value > maxValue artinya nomor di luar daftar pilihan
        return false; // nomor di luar rentang tidak sah
    } // akhir if di luar rentang
    choice = static_cast<int>(value); // choice diisi nomor pilihan sebagai int
    return true; // true artinya pilihan sah
} // akhir fungsi parseMenuChoice()

// parseYesNo() menerima beberapa bentuk jawaban yang lazim (bahasa Indonesia dan Inggris, huruf
// besar atau kecil), karena yang dicari adalah maksud pengguna, bukan ketikan yang persis sama.
YesNoAnswer parseYesNo(const std::string& text) { // parseYesNo(text) mengembalikan ANSWER_YES, ANSWER_NO, atau ANSWER_UNKNOWN
    const std::string answer = toLowerAscii(text); // answer adalah ketikan dalam huruf kecil, jadi "Y", "YA", dan "Yes" ikut dikenali
    if (answer == "y" || answer == "ya" || answer == "yes") { // tiga bentuk jawaban ya
        return ANSWER_YES; // pengguna menjawab ya
    } // akhir if jawaban ya
    if (answer == "t" || answer == "tidak" || answer == "n" || answer == "no") { // empat bentuk jawaban tidak
        return ANSWER_NO; // pengguna menjawab tidak
    } // akhir if jawaban tidak
    return ANSWER_UNKNOWN; // selain itu (termasuk teks kosong) jawabannya tidak dikenal; askYesNo() menangani Enter kosong lebih dulu
} // akhir fungsi parseYesNo()
