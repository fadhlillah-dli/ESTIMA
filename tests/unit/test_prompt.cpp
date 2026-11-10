// ===== test_prompt.cpp - uji tanya jawab: pembacaan baris, pilihan, ya/tidak, dan data komponen =====
// std::cin dan std::cout dialihkan ke string stream oleh ConsoleCapture (lihat test_support.h),
// jadi ketikan pengguna ditulis sebagai teks dan layar yang dilihat pengguna bisa diperiksa.
// initTerminal() tidak pernah dipanggil di biner uji: masukan dianggap bukan dari keyboard
// (jawaban dicetak ulang di layar) dan warna mati.
#include "estima/platform.h"

#include "estima/prompt.h"

#include <iostream>
#include <string>

#include "estima/terminal.h"

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

// Awal baris pertanyaan seperti yang tampil di layar: label dirata-kiri selebar 20 kolom lalu " : ".
static std::string promptText(const std::string& label) {
    return label + std::string(20 - label.size(), ' ') + " : ";
}

// Layar dengan lipatan baris pesan dikembalikan menjadi satu baris (baris lanjutan pesan
// kesalahan menjorok 4 spasi), supaya kalimat pesan bisa dicari utuh berapa pun lebar terminal
// yang sedang dipakai penguji.
static std::string unwrapped(const std::string& screen) {
    std::string text = screen;
    const std::string continuation = "\n    ";
    std::size_t position = text.find(continuation);
    while (position != std::string::npos) {
        text.replace(position, continuation.size(), " ");
        position = text.find(continuation, position);
    }
    return text;
}

// Banyaknya pesan kesalahan ("[!] ...") yang tercetak di layar.
static std::size_t errorCount(const std::string& screen) {
    return countOccurrences(screen, "[!] ");
}

static void testTerminalStateAssumptions() {
    CHECK(!inputIsInteractive());
    CHECK(!colorsEnabled());
}

static void testStreamsAreRestored() {
    std::streambuf* const inputBefore = std::cin.rdbuf();
    std::streambuf* const outputBefore = std::cout.rdbuf();
    {
        ConsoleCapture capture("abc\n");
        CHECK(std::cin.rdbuf() != inputBefore);
        CHECK(std::cout.rdbuf() != outputBefore);
    }
    CHECK(std::cin.rdbuf() == inputBefore);
    CHECK(std::cout.rdbuf() == outputBefore);

    // Tetap pulih walau pembacaan berakhir dengan InputClosed.
    try {
        ConsoleCapture capture("");
        readLine("x");
    } catch (const InputClosed&) {
    }
    CHECK(std::cin.rdbuf() == inputBefore);
    CHECK(std::cout.rdbuf() == outputBefore);
    CHECK(std::cin.good());
}

static void testReadLineTrimsAndEchoes() {
    {
        ConsoleCapture capture("  Gear  \n");
        CHECK_EQ(readLine("Nama"), "Gear");
        // Label, jawaban yang sudah dirapikan, lalu baris baru (jawaban dicetak ulang).
        CHECK_EQ(capture.screenText(), promptText("Nama") + "Gear\n");
    }
    {
        ConsoleCapture capture("\t Base   Plate \t\n");
        // Spasi di tengah tidak disentuh; hanya ujung-ujungnya yang dibuang.
        CHECK_EQ(readLine("Nama"), "Base   Plate");
    }
    {
        ConsoleCapture capture("\n");
        CHECK_EQ(readLine("Nama"), "");
        CHECK_EQ(capture.screenText(), promptText("Nama") + "\n");
    }
    {
        // Akhir baris gaya Windows tidak ikut terbaca.
        ConsoleCapture capture("abc\r\n");
        CHECK_EQ(readLine("Nama"), "abc");
    }
    {
        // Baris terakhir tanpa Enter tetap terbaca; pembacaan sesudahnya baru gagal.
        ConsoleCapture capture("abc");
        CHECK_EQ(readLine("Nama"), "abc");
        bool closed = false;
        try {
            readLine("Nama");
        } catch (const InputClosed&) {
            closed = true;
        }
        CHECK(closed);
    }
    {
        // Label yang lebih panjang dari 20 kolom tidak dipotong.
        ConsoleCapture capture("x\n");
        readLine("Label yang panjang sekali melebihi dua puluh");
        CHECK_EQ(capture.screenText(), "Label yang panjang sekali melebihi dua puluh : x\n");
    }
    {
        // Dua baris dibaca berurutan.
        ConsoleCapture capture("satu\ndua\n");
        CHECK_EQ(readLine("A"), "satu");
        CHECK_EQ(readLine("B"), "dua");
    }
}

static void testReadLineThrowsAtEndOfInput() {
    ConsoleCapture capture("");
    bool closed = false;
    try {
        readLine("Nama");
    } catch (const InputClosed&) {
        closed = true;
    }
    CHECK(closed);
    // Label tetap tercetak sebelum masukan ternyata habis.
    CHECK_EQ(capture.screenText(), promptText("Nama"));
}

static void testWaitForEnter() {
    {
        ConsoleCapture capture("\n");
        waitForEnter();
        CHECK_EQ(capture.screenText(), "Tekan Enter untuk kembali ke menu...\n");
    }
    {
        // Ketikan apa pun dibuang sampai Enter; baris berikutnya tidak tersentuh.
        ConsoleCapture capture("sembarang\nberikut\n");
        waitForEnter();
        CHECK_EQ(readLine("A"), "berikut");
    }
    {
        ConsoleCapture capture("");
        bool closed = false;
        try {
            waitForEnter();
        } catch (const InputClosed&) {
            closed = true;
        }
        CHECK(closed);
    }
}

static void testReadChoiceReasksUntilValid() {
    ConsoleCapture capture("9\nabc\n\n-1\n3\n");
    CHECK_EQ(readChoice("Pilih", 0, 8, false, 0), 3);
    const std::string screen = capture.screenText();
    CHECK_EQ(errorCount(screen), 4u);
    CHECK_EQ(countOccurrences(screen, "Pilihan tidak dikenal."), 4u);
    CHECK_EQ(countOccurrences(screen, promptText("Pilih")), 5u);
    // Pesan menyebut rentang yang berlaku.
    CHECK_CONTAINS(unwrapped(screen), "Ketik angka 0 sampai 8 lalu tekan Enter.");
}

static void testReadChoiceBoundaries() {
    {
        ConsoleCapture capture("0\n6\n1\n");
        CHECK_EQ(readChoice("Pilih", 1, 5, false, 0), 1);
        CHECK_EQ(errorCount(capture.screenText()), 2u);
    }
    {
        ConsoleCapture capture("5\n");
        CHECK_EQ(readChoice("Pilih", 1, 5, false, 0), 5);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        ConsoleCapture capture("1000000000000\n2\n");
        CHECK_EQ(readChoice("Pilih", 1, 5, false, 0), 2);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
}

static void testReadChoiceDefault() {
    {
        // Enter kosong memakai bawaan.
        ConsoleCapture capture("\n");
        CHECK_EQ(readChoice("Pilih", 0, 8, true, 2), 2);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        // Jawaban yang diketik menang atas bawaan.
        ConsoleCapture capture("5\n");
        CHECK_EQ(readChoice("Pilih", 0, 8, true, 2), 5);
    }
    {
        // Jawaban salah ditolak, lalu Enter kosong memakai bawaan.
        ConsoleCapture capture("x\n\n");
        CHECK_EQ(readChoice("Pilih", 0, 8, true, 2), 2);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
    {
        // Tanpa bawaan, Enter kosong adalah jawaban salah.
        ConsoleCapture capture("\n4\n");
        CHECK_EQ(readChoice("Pilih", 0, 8, false, 2), 4);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
}

static void testReadChoiceEndOfInput() {
    ConsoleCapture capture("x\n");
    bool closed = false;
    try {
        readChoice("Pilih", 0, 8, false, 0);
    } catch (const InputClosed&) {
        closed = true;
    }
    CHECK(closed);
    CHECK_EQ(errorCount(capture.screenText()), 1u);
}

static void testAskYesNo() {
    {
        ConsoleCapture capture("\n");
        CHECK(askYesNo("Lanjut?", true));
        CHECK_CONTAINS(capture.screenText(), "Lanjut? [Y/t]");
    }
    {
        ConsoleCapture capture("\n");
        CHECK(!askYesNo("Lanjut?", false));
        CHECK_CONTAINS(capture.screenText(), "Lanjut? [y/T]");
    }
    const char* const yes[] = { "y\n", "Y\n", "ya\n", "YA\n", "yes\n" };
    for (std::size_t i = 0; i < sizeof(yes) / sizeof(yes[0]); ++i) {
        ConsoleCapture capture(yes[i]);
        CHECK(askYesNo("Lanjut?", false));
    }
    const char* const no[] = { "t\n", "T\n", "tidak\n", "n\n", "no\n" };
    for (std::size_t i = 0; i < sizeof(no) / sizeof(no[0]); ++i) {
        ConsoleCapture capture(no[i]);
        CHECK(!askYesNo("Lanjut?", true));
    }
}

static void testAskYesNoReasksAfterInvalidAnswer() {
    {
        ConsoleCapture capture("mungkin\n1\ny\n");
        CHECK(askYesNo("Lanjut?", false));
        const std::string screen = capture.screenText();
        CHECK_EQ(errorCount(screen), 2u);
        CHECK_CONTAINS(unwrapped(screen), "Jawab dengan y (ya) atau t (tidak), lalu tekan Enter.");
    }
    {
        // Bawaan tidak berlaku untuk jawaban yang salah, hanya untuk Enter kosong.
        ConsoleCapture capture("x\nt\n");
        CHECK(!askYesNo("Lanjut?", true));
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
    {
        ConsoleCapture capture("x\n");
        bool closed = false;
        try {
            askYesNo("Lanjut?", true);
        } catch (const InputClosed&) {
            closed = true;
        }
        CHECK(closed);
    }
}

static void testReadQuantity() {
    {
        ConsoleCapture capture("24\n");
        CHECK_EQ(readQuantity("Jumlah", false, 0), 24);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        // Lima jawaban salah berturut-turut, masing-masing dengan pesannya sendiri.
        ConsoleCapture capture("0\n-3\n1.5\nabc\n\n24\n");
        CHECK_EQ(readQuantity("Jumlah", false, 0), 24);
        const std::string screen = capture.screenText();
        CHECK_EQ(errorCount(screen), 5u);
        CHECK_CONTAINS(unwrapped(screen), "Jumlah belum diisi.");
        CHECK_CONTAINS(unwrapped(screen), "Jumlah harus bilangan bulat tanpa titik");
    }
    {
        // Batas atas dan sedikit di atasnya.
        ConsoleCapture capture("1000001\n1000000\n");
        CHECK_EQ(readQuantity("Jumlah", false, 0), 1000000);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
        CHECK_CONTAINS(unwrapped(capture.screenText()), "Jumlah harus antara 1 dan 1.000.000 unit.");
    }
    {
        ConsoleCapture capture("\n");
        CHECK_EQ(readQuantity("Jumlah", true, 12), 12);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        ConsoleCapture capture("7\n");
        CHECK_EQ(readQuantity("Jumlah", true, 12), 7);
    }
    {
        ConsoleCapture capture("0\n\n");
        CHECK_EQ(readQuantity("Jumlah", true, 12), 12);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
}

static void testReadUnitWeight() {
    {
        ConsoleCapture capture("1,85\n");
        CHECK_EQ(readUnitWeight("Berat/unit (kg)", false, 0.0), 1.85);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        ConsoleCapture capture("0\n10000,001\n1,2345\nabc\n\n0.25\n");
        CHECK_EQ(readUnitWeight("Berat/unit (kg)", false, 0.0), 0.25);
        const std::string screen = capture.screenText();
        CHECK_EQ(errorCount(screen), 5u);
        CHECK_CONTAINS(unwrapped(screen), "Berat per unit belum diisi.");
    }
    {
        ConsoleCapture capture("\n");
        CHECK_EQ(readUnitWeight("Berat/unit (kg)", true, 4.2), 4.2);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        ConsoleCapture capture("0,5\n");
        CHECK_EQ(readUnitWeight("Berat/unit (kg)", true, 4.2), 0.5);
    }
    {
        ConsoleCapture capture("x\n\n");
        CHECK_EQ(readUnitWeight("Berat/unit (kg)", true, 4.2), 4.2);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
}

static void testReadPricePerKg() {
    {
        ConsoleCapture capture("18500\n");
        CHECK_EQ(readPricePerKg("Harga per kg (Rp)", false, 0.0), 18500.0);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        ConsoleCapture capture("0\n18.500\nRp18500\n100000001\n\n18500\n");
        CHECK_EQ(readPricePerKg("Harga per kg (Rp)", false, 0.0), 18500.0);
        const std::string screen = capture.screenText();
        CHECK_EQ(errorCount(screen), 5u);
        CHECK_CONTAINS(unwrapped(screen), "Harga per kg belum diisi.");
    }
    {
        ConsoleCapture capture("\n");
        CHECK_EQ(readPricePerKg("Harga per kg (Rp)", true, 32000.0), 32000.0);
        CHECK_EQ(errorCount(capture.screenText()), 0u);
    }
    {
        ConsoleCapture capture("45000\n");
        CHECK_EQ(readPricePerKg("Harga per kg (Rp)", true, 32000.0), 45000.0);
    }
    {
        ConsoleCapture capture("-1\n\n");
        CHECK_EQ(readPricePerKg("Harga per kg (Rp)", true, 32000.0), 32000.0);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
}

static void testReadersThrowAtEndOfInput() {
    {
        ConsoleCapture capture("");
        bool closed = false;
        try {
            readQuantity("Jumlah", true, 1);
        } catch (const InputClosed&) {
            closed = true;
        }
        CHECK(closed);
    }
    {
        ConsoleCapture capture("abc\n");
        bool closed = false;
        try {
            readUnitWeight("Berat", false, 0.0);
        } catch (const InputClosed&) {
            closed = true;
        }
        CHECK(closed);
        CHECK_EQ(errorCount(capture.screenText()), 1u);
    }
    {
        ConsoleCapture capture("");
        bool closed = false;
        try {
            readPricePerKg("Harga", true, 1.0);
        } catch (const InputClosed&) {
            closed = true;
        }
        CHECK(closed);
    }
}

void runPromptTests() {
    testTerminalStateAssumptions();
    testStreamsAreRestored();
    testReadLineTrimsAndEchoes();
    testReadLineThrowsAtEndOfInput();
    testWaitForEnter();
    testReadChoiceReasksUntilValid();
    testReadChoiceBoundaries();
    testReadChoiceDefault();
    testReadChoiceEndOfInput();
    testAskYesNo();
    testAskYesNoReasksAfterInvalidAnswer();
    testReadQuantity();
    testReadUnitWeight();
    testReadPricePerKg();
    testReadersThrowAtEndOfInput();
}
