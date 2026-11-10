// ===== test_framework.h - kerangka uji unit kecil buatan sendiri =====
// Kerangka ini sengaja dibuat sendiri dan sekecil mungkin, supaya uji unit bisa dikompilasi oleh
// compiler mana pun yang dipakai program utama (termasuk mode C++98) tanpa memasang pustaka
// tambahan. Isinya hanya pencacah, beberapa fungsi pembanding, dan makro CHECK yang mencatat
// nama berkas serta nomor baris tempat sebuah pemeriksaan ditulis.
//
// Tidak ada pendaftaran uji otomatis: setiap berkas uji menyediakan satu fungsi (lihat
// test_suites.h) yang dipanggil langsung oleh test_main.cpp, jadi daftar dan urutan uji bisa
// dibaca apa adanya di sana.
#ifndef ESTIMA_TEST_FRAMEWORK_H
#define ESTIMA_TEST_FRAMEWORK_H

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

// Banyaknya pemeriksaan yang sudah dijalankan dan berapa di antaranya yang gagal.
struct TestCounters {
    int checks;
    int failures;
};

// Pencacah disimpan sebagai variabel static di dalam fungsi inline. Dengan cara ini semua berkas
// uji yang menyertakan header ini memakai satu pencacah yang sama, tanpa berkas .cpp tambahan.
inline TestCounters& testCounters() {
    static TestCounters counters = { 0, 0 };
    return counters;
}

// ----- Mengubah nilai menjadi teks untuk pesan kegagalan -----

// Versi umum memakai operator <<, sehingga berlaku untuk int, long, std::size_t, dan enum.
template <typename Value>
std::string describeValue(const Value& value) {
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

inline std::string describeValue(bool value) {
    return value ? "true" : "false";
}

// Bilangan pecahan ditulis dengan 17 angka penting: cukup untuk membedakan dua nilai double
// yang berbeda walau hanya di bit terakhirnya, misalnya 44.4 dan 44.400000000000006.
inline std::string describeValue(double value) {
    std::ostringstream stream;
    stream.precision(17);
    stream << value;
    return stream.str();
}

// Teks ditulis di antara tanda kutip dan karakter yang tidak terlihat diganti kodenya (\n,
// \x1B, dan seterusnya), supaya spasi di ujung, kode warna, dan byte UTF-8 tampak jelas.
// Teks berbaris banyak, misalnya gambar kotak, dipecah per baris agar mudah dibandingkan.
inline std::string describeValue(const std::string& value) {
    static const char* const HEX_DIGITS = "0123456789ABCDEF";
    std::string result = "\"";
    for (std::size_t i = 0; i < value.size(); ++i) {
        const unsigned char byte = static_cast<unsigned char>(value[i]);
        if (byte == '\n') {
            result += "\\n";
            if (i + 1 < value.size()) {
                result += "\"\n\"";
            }
        } else if (byte == '\\' || byte == '"') {
            result += '\\';
            result += value[i];
        } else if (byte < 0x20 || byte >= 0x7F) {
            result += "\\x";
            result += HEX_DIGITS[byte / 16];
            result += HEX_DIGITS[byte % 16];
        } else {
            result += value[i];
        }
    }
    result += '"';
    return result;
}

inline std::string describeValue(const char* value) {
    return describeValue(std::string(value));
}

// ----- Pencatatan hasil -----

// Mencatat satu pemeriksaan dan mengembalikan true bila lulus. Bila gagal, lokasi dan isi
// pemeriksaannya dicetak ke std::cerr. Dipakai std::cerr, bukan std::cout, karena sebagian uji
// mengalihkan std::cout untuk menangkap keluaran program; pesan kegagalan tidak boleh ikut
// tertangkap di sana.
inline bool recordCheck(bool passed, const char* expression, const char* file, int line) {
    TestCounters& counters = testCounters();
    counters.checks = counters.checks + 1;
    if (passed) {
        return true;
    }
    counters.failures = counters.failures + 1;
    std::cerr << file << ':' << line << ": GAGAL: " << expression << '\n';
    return false;
}

// Mencetak satu rincian di bawah pesan kegagalan, misalnya nilai yang didapat. Baris kedua dan
// seterusnya dari nilai berbaris banyak dijorokkan sejajar dengan baris pertamanya.
inline void printFailureDetail(const char* label, const std::string& value) {
    std::cerr << "    " << label << " : ";
    for (std::size_t i = 0; i < value.size(); ++i) {
        std::cerr << value[i];
        if (value[i] == '\n') {
            std::cerr << "              ";
        }
    }
    std::cerr << '\n';
}

// ----- Fungsi pemeriksa (dipanggil lewat makro CHECK di bawah) -----

inline void checkTrue(bool condition, const char* expression, const char* file, int line) {
    recordCheck(condition, expression, file, line);
}

// Kesamaan persis. Untuk double pun dipakai ==, karena hitungan program dijanjikan tepat sampai
// gram dan rupiah; selisih sekecil apa pun adalah kesalahan yang memang ingin ditangkap.
template <typename Actual, typename Expected>
void checkEqual(const Actual& actual, const Expected& expected, const char* expression, const char* file, int line) {
    if (!recordCheck(actual == expected, expression, file, line)) {
        printFailureDetail("hasil  ", describeValue(actual));
        printFailureDetail("harapan", describeValue(expected));
    }
}

// Dua teks C dibandingkan isinya. Tanpa fungsi ini, CHECK_EQ pada dua const char* hanya akan
// membandingkan alamat memorinya.
inline void checkEqual(const char* actual, const char* expected, const char* expression, const char* file, int line) {
    checkEqual(std::string(actual), std::string(expected), expression, file, line);
}

// Kesamaan dengan kelonggaran, untuk hasil pembagian yang memang tidak bulat (misalnya persen).
inline void checkNear(double actual, double expected, double tolerance, const char* expression, const char* file, int line) {
    const double difference = actual > expected ? actual - expected : expected - actual;
    if (!recordCheck(difference <= tolerance, expression, file, line)) {
        printFailureDetail("hasil  ", describeValue(actual));
        printFailureDetail("harapan", describeValue(expected) + " (kelonggaran " + describeValue(tolerance) + ")");
    }
}

// Batas atas, misalnya lebar baris terlebar terhadap lebar layar.
inline void checkAtMost(std::size_t actual, std::size_t limit, const char* expression, const char* file, int line) {
    if (!recordCheck(actual <= limit, expression, file, line)) {
        printFailureDetail("hasil  ", describeValue(actual));
        printFailureDetail("batas  ", describeValue(limit));
    }
}

// Ada (shouldContain true) atau tidak adanya (false) sepotong teks di dalam teks yang lebih panjang.
inline void checkContains(const std::string& text, const std::string& part, bool shouldContain,
                          const char* expression, const char* file, int line) {
    const bool found = text.find(part) != std::string::npos;
    if (!recordCheck(found == shouldContain, expression, file, line)) {
        printFailureDetail("dicari ", describeValue(part));
        printFailureDetail("di teks", describeValue(text));
    }
}

// ----- Makro pemeriksaan -----
// Makro diperlukan hanya untuk mengambil teks ekspresi (#condition) serta __FILE__ dan __LINE__
// di tempat pemeriksaan ditulis; pekerjaannya sendiri dilakukan fungsi-fungsi di atas.

#define CHECK(condition) \
    checkTrue((condition), #condition, __FILE__, __LINE__)

#define CHECK_EQ(actual, expected) \
    checkEqual((actual), (expected), #actual " == " #expected, __FILE__, __LINE__)

#define CHECK_NEAR(actual, expected, tolerance) \
    checkNear((actual), (expected), (tolerance), #actual " ~ " #expected, __FILE__, __LINE__)

#define CHECK_AT_MOST(actual, limit) \
    checkAtMost((actual), (limit), #actual " <= " #limit, __FILE__, __LINE__)

#define CHECK_CONTAINS(text, part) \
    checkContains((text), (part), true, #text " memuat " #part, __FILE__, __LINE__)

#define CHECK_NOT_CONTAINS(text, part) \
    checkContains((text), (part), false, #text " tidak memuat " #part, __FILE__, __LINE__)

#endif // ESTIMA_TEST_FRAMEWORK_H
