// ===== test_support.h - alat bantu yang dipakai bersama oleh beberapa suite =====
#ifndef ESTIMA_TEST_SUPPORT_H
#define ESTIMA_TEST_SUPPORT_H

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "estima/component.h"

// Membuat satu komponen dalam satu baris, supaya data uji mudah dibaca berdampingan dengan
// angka yang diharapkan.
inline Component makeComponent(const std::string& name, int quantity, double unitWeightKg, double pricePerKg) {
    Component component;
    component.name = name;
    component.quantity = quantity;
    component.unitWeightKg = unitWeightKg;
    component.pricePerKg = pricePerKg;
    return component;
}

// Nama seluruh isi daftar dipisah koma ("Gear,Shaft"), supaya isi dan urutan daftar bisa
// dibandingkan dengan satu pemeriksaan.
inline std::string joinComponentNames(const ComponentList& list) {
    std::string names;
    for (int i = 0; i < list.count; ++i) {
        if (i > 0) {
            names += ',';
        }
        names += list.items[i].name;
    }
    return names;
}

// Deret nomor ditulis dipisah koma ("1,2,0"), supaya urutan hasil peringkat atau daftar indeks
// bisa dibandingkan dengan satu pemeriksaan.
inline std::string joinNumbers(const int numbers[], int count) {
    std::ostringstream stream;
    for (int i = 0; i < count; ++i) {
        if (i > 0) {
            stream << ',';
        }
        stream << numbers[i];
    }
    return stream.str();
}

// Memecah teks keluaran menjadi baris-barisnya, tanpa karakter akhir baris.
inline std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::string current;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\n') {
            lines.push_back(current);
            current.clear();
        } else {
            current += text[i];
        }
    }
    if (!current.empty()) {
        lines.push_back(current);
    }
    return lines;
}

// Baris pertama di dalam text yang memuat part; teks kosong bila tidak ada.
inline std::string lineContaining(const std::string& text, const std::string& part) {
    const std::vector<std::string> lines = splitLines(text);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(part) != std::string::npos) {
            return lines[i];
        }
    }
    return "";
}

// Memecah satu baris menjadi kata-katanya (dipisah satu spasi atau lebih). Berguna untuk
// memeriksa isi sel tabel tanpa bergantung pada banyaknya spasi perata di antaranya.
inline std::vector<std::string> splitWords(const std::string& line) {
    std::vector<std::string> words;
    std::string current;
    for (std::size_t i = 0; i < line.size(); ++i) {
        if (line[i] == ' ') {
            if (!current.empty()) {
                words.push_back(current);
                current.clear();
            }
        } else {
            current += line[i];
        }
    }
    if (!current.empty()) {
        words.push_back(current);
    }
    return words;
}

// Menghitung berapa kali part muncul di dalam text.
inline std::size_t countOccurrences(const std::string& text, const std::string& part) {
    if (part.empty()) {
        return 0;
    }
    std::size_t count = 0;
    std::size_t position = text.find(part);
    while (position != std::string::npos) {
        count = count + 1;
        position = text.find(part, position + part.size());
    }
    return count;
}

// Satu baris isi kotak ASCII yang isinya selebar inner kolom: "| teks    |". Dipakai untuk
// menulis harapan baris kotak tanpa menghitung spasi pengisinya satu per satu.
inline std::string boxRow(const std::string& text, std::size_t inner) {
    return "| " + text + std::string(inner - text.size(), ' ') + " |";
}

// Lebar tampilan sebuah baris dalam kolom. Sengaja dihitung sendiri (byte lanjutan UTF-8, yang
// berpola 10xxxxxx, tidak dihitung) dan tidak memakai displayWidth() milik program, supaya uji
// tata letak tidak bergantung pada fungsi yang ikut diuji.
inline std::size_t countColumns(const std::string& line) {
    std::size_t columns = 0;
    for (std::size_t i = 0; i < line.size(); ++i) {
        if ((static_cast<unsigned char>(line[i]) & 0xC0) != 0x80) {
            columns = columns + 1;
        }
    }
    return columns;
}

// Lebar baris terlebar di dalam teks berbaris banyak (0 bila teksnya kosong).
inline std::size_t widestLine(const std::string& text) {
    const std::vector<std::string> lines = splitLines(text);
    std::size_t widest = 0;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (countColumns(lines[i]) > widest) {
            widest = countColumns(lines[i]);
        }
    }
    return widest;
}

// Lebar baris tersempit di dalam teks berbaris banyak (0 bila teksnya kosong). Bersama
// widestLine() dipakai untuk memastikan semua baris sebuah kotak sama lebar.
inline std::size_t narrowestLine(const std::string& text) {
    const std::vector<std::string> lines = splitLines(text);
    if (lines.empty()) {
        return 0;
    }
    std::size_t narrowest = countColumns(lines[0]);
    for (std::size_t i = 1; i < lines.size(); ++i) {
        if (countColumns(lines[i]) < narrowest) {
            narrowest = countColumns(lines[i]);
        }
    }
    return narrowest;
}

// true bila teks hanya berisi karakter ASCII yang tercetak dan akhir baris. Keluaran gaya polos
// (laporan berkas) harus begitu: tanpa kode warna dan tanpa karakter garis UTF-8.
inline bool isPlainAscii(const std::string& text) {
    for (std::size_t i = 0; i < text.size(); ++i) {
        const unsigned char byte = static_cast<unsigned char>(text[i]);
        if (byte != '\n' && (byte < 0x20 || byte > 0x7E)) {
            return false;
        }
    }
    return true;
}

// Selama objek ini hidup, std::cin membaca dari teks typedInput dan semua yang ditulis ke
// std::cout ditampung. Dengan begitu fungsi yang berbicara langsung dengan keyboard dan layar
// bisa diuji tanpa pengguna. Pengembalian kedua aliran ditaruh di destruktor supaya tetap
// terjadi ketika fungsi yang diuji melempar InputClosed.
class ConsoleCapture {
public:
    explicit ConsoleCapture(const std::string& typedInput)
        : fakeInput(typedInput),
          capturedOutput(),
          originalInput(std::cin.rdbuf(fakeInput.rdbuf())),
          originalOutput(std::cout.rdbuf(capturedOutput.rdbuf())) {
    }

    ~ConsoleCapture() {
        // rdbuf() sekaligus menghapus tanda "masukan habis" yang ditinggalkan ketikan tiruan,
        // jadi std::cin siap dipakai lagi oleh uji berikutnya.
        std::cin.rdbuf(originalInput);
        std::cout.rdbuf(originalOutput);
    }

    // Semua yang sudah ditulis ke std::cout sejak objek ini dibuat.
    std::string screenText() const {
        return capturedOutput.str();
    }

private:
    std::istringstream fakeInput;
    std::ostringstream capturedOutput;
    std::streambuf* originalInput;
    std::streambuf* originalOutput;

    // Objek ini memegang keadaan std::cin dan std::cout, jadi tidak boleh disalin.
    ConsoleCapture(const ConsoleCapture&);
    ConsoleCapture& operator=(const ConsoleCapture&);
};

#endif // ESTIMA_TEST_SUPPORT_H
