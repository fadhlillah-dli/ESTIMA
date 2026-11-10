// ===== test_text_format.cpp - uji pemformatan angka, perataan teks, dan pelipatan baris =====
// Karakter UTF-8 ditulis sebagai kode byte supaya berkas ini tetap ASCII murni:
//   "\xE2\x94\x80" garis datar kotak (3 byte, 1 kolom)    "\xC3\xA9" huruf e beraksen (2 byte, 1 kolom)
//   "\xF0\x9F\x98\x80" karakter 4 byte (1 kolom)
#include "estima/platform.h"

#include "estima/text_format.h"

#include <cstddef>
#include <string>
#include <vector>

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

static const char* const BOX_H = "\xE2\x94\x80";

static void testFormatThousands() {
    CHECK_EQ(formatThousands(0.0), "0");
    CHECK_EQ(formatThousands(7.0), "7");
    CHECK_EQ(formatThousands(100.0), "100");
    CHECK_EQ(formatThousands(999.0), "999");
    CHECK_EQ(formatThousands(1000.0), "1.000");
    CHECK_EQ(formatThousands(12345.0), "12.345");
    CHECK_EQ(formatThousands(100000.0), "100.000");
    CHECK_EQ(formatThousands(1000000.0), "1.000.000");
    CHECK_EQ(formatThousands(1234567.0), "1.234.567");
    CHECK_EQ(formatThousands(999999999999.0), "999.999.999.999");
    CHECK_EQ(formatThousands(99999999999900.0), "99.999.999.999.900");
}

static void testFormatRupiah() {
    CHECK_EQ(formatRupiah(0.0), "Rp0");
    CHECK_EQ(formatRupiah(1.0), "Rp1");
    CHECK_EQ(formatRupiah(500.0), "Rp500");
    CHECK_EQ(formatRupiah(18500.0), "Rp18.500");
    CHECK_EQ(formatRupiah(821400.0), "Rp821.400");
    CHECK_EQ(formatRupiah(2434200.0), "Rp2.434.200");
    CHECK_EQ(formatRupiah(100000000.0), "Rp100.000.000");
    CHECK_EQ(formatRupiah(999999999999.0), "Rp999.999.999.999");
    // Pecahan dibulatkan ke rupiah terdekat, setengah ke atas.
    CHECK_EQ(formatRupiah(1499.4), "Rp1.499");
    CHECK_EQ(formatRupiah(1499.5), "Rp1.500");
}

static void testFormatWeight() {
    CHECK_EQ(formatWeight(0.0), "0,000 kg");
    CHECK_EQ(formatWeight(0.001), "0,001 kg");
    CHECK_EQ(formatWeight(0.125), "0,125 kg");
    CHECK_EQ(formatWeight(1.85), "1,850 kg");
    CHECK_EQ(formatWeight(4.2), "4,200 kg");
    CHECK_EQ(formatWeight(44.4), "44,400 kg");
    CHECK_EQ(formatWeight(94.8), "94,800 kg");
    CHECK_EQ(formatWeight(999.999), "999,999 kg");
    CHECK_EQ(formatWeight(1000.0), "1.000,000 kg");
    CHECK_EQ(formatWeight(1234.5), "1.234,500 kg");
    CHECK_EQ(formatWeight(10000.0), "10.000,000 kg");
    CHECK_EQ(formatWeight(10000000000.0), "10.000.000.000,000 kg");
}

static void testFormatPercent() {
    CHECK_EQ(formatPercent(0.0), "0,0%");
    CHECK_EQ(formatPercent(0.04), "0,0%");
    CHECK_EQ(formatPercent(0.05), "0,1%");
    CHECK_EQ(formatPercent(9.04), "9,0%");
    CHECK_EQ(formatPercent(9.05), "9,1%");
    CHECK_EQ(formatPercent(33.333), "33,3%");
    CHECK_EQ(formatPercent(37.5), "37,5%");
    CHECK_EQ(formatPercent(66.256), "66,3%");
    CHECK_EQ(formatPercent(99.95), "100,0%");
    CHECK_EQ(formatPercent(100.0), "100,0%");
}

static void testDisplayWidth() {
    CHECK_EQ(displayWidth(""), 0u);
    CHECK_EQ(displayWidth("abc"), 3u);
    CHECK_EQ(displayWidth("a b"), 3u);
    CHECK_EQ(displayWidth("\xE2\x94\x80"), 1u);
    CHECK_EQ(displayWidth("\xE2\x94\x8C\xE2\x94\x80"), 2u);
    CHECK_EQ(displayWidth("a\xE2\x96\x88" "b"), 3u);
    CHECK_EQ(displayWidth("\xC3\xA9"), 1u);
    CHECK_EQ(displayWidth("caf\xC3\xA9"), 4u);
    CHECK_EQ(displayWidth("\xF0\x9F\x98\x80"), 1u);
    CHECK_EQ(displayWidth(repeatText(BOX_H, 40)), 40u);
}

static void testPadding() {
    CHECK_EQ(padRight("ab", 5), "ab   ");
    CHECK_EQ(padLeft("ab", 5), "   ab");
    CHECK_EQ(padRight("ab", 2), "ab");
    CHECK_EQ(padLeft("ab", 2), "ab");
    // Teks yang sudah lebih lebar tidak dipotong.
    CHECK_EQ(padRight("abcdef", 3), "abcdef");
    CHECK_EQ(padLeft("abcdef", 3), "abcdef");
    CHECK_EQ(padRight("", 3), "   ");
    CHECK_EQ(padLeft("", 3), "   ");
    CHECK_EQ(padRight("", 0), "");
    CHECK_EQ(padLeft("ab", 0), "ab");
    // Lebar dihitung dalam kolom, bukan byte.
    CHECK_EQ(padRight("\xE2\x94\x80", 3), "\xE2\x94\x80  ");
    CHECK_EQ(padLeft("\xE2\x94\x80", 3), "  \xE2\x94\x80");
    CHECK_EQ(padRight("caf\xC3\xA9", 6), "caf\xC3\xA9  ");
}

static void testRepeatText() {
    CHECK_EQ(repeatText("ab", 3), "ababab");
    CHECK_EQ(repeatText("-", 5), "-----");
    CHECK_EQ(repeatText("ab", 1), "ab");
    CHECK_EQ(repeatText("ab", 0), "");
    CHECK_EQ(repeatText("", 5), "");
    CHECK_EQ(repeatText("\xE2\x94\x80", 3), "\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80");
}

static void testTruncateText() {
    CHECK_EQ(truncateText("hello", 5), "hello");
    CHECK_EQ(truncateText("hello", 10), "hello");
    CHECK_EQ(truncateText("", 0), "");
    CHECK_EQ(truncateText("abc", 3), "abc");
    CHECK_EQ(truncateText("abcd", 3), "a..");
    CHECK_EQ(truncateText("hello world", 8), "hello ..");
    CHECK_EQ(truncateText("hello world", 5), "hel..");
    // Lebar sempit: hanya dipotong, tanpa tanda "..".
    CHECK_EQ(truncateText("hello", 2), "he");
    CHECK_EQ(truncateText("hello", 1), "h");
    CHECK_EQ(truncateText("hello", 0), "");
    // Karakter UTF-8 tidak dipotong di tengah byte-nya.
    const std::string wide = repeatText(BOX_H, 6);
    CHECK_EQ(truncateText(wide, 5), repeatText(BOX_H, 3) + "..");
    CHECK_EQ(truncateText(wide, 6), wide);
    CHECK_EQ(truncateText(wide, 2), repeatText(BOX_H, 2));
    CHECK_EQ(truncateText("caf\xC3\xA9 bar", 4), "ca..");
    CHECK_EQ(truncateText("caf\xC3\xA9 bar", 5), "caf..");
    CHECK_EQ(truncateText("caf\xC3\xA9 bar", 6), "caf\xC3\xA9..");
}

static void testToLowerAscii() {
    CHECK_EQ(toLowerAscii("ABC xyz 123-_"), "abc xyz 123-_");
    CHECK_EQ(toLowerAscii(""), "");
    CHECK_EQ(toLowerAscii("Base Plate"), "base plate");
    // Hanya A-Z yang diubah; byte UTF-8 dan simbol dibiarkan.
    CHECK_EQ(toLowerAscii("\xC3\x89" "A"), "\xC3\x89" "a");
    CHECK_EQ(toLowerAscii("@[`{"), "@[`{");
}

// ----- wrapText -----

static std::string joinWithBars(const std::vector<std::string>& lines) {
    std::string joined;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) {
            joined += '|';
        }
        joined += lines[i];
    }
    return joined;
}

static std::string withoutSpaces(const std::string& text) {
    std::string result;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] != ' ') {
            result += text[i];
        }
    }
    return result;
}

static void testWrapKeepsTextThatFits() {
    std::vector<std::string> lines = wrapText("hello world", 20, 0);
    CHECK_EQ(lines.size(), 1u);
    CHECK_EQ(joinWithBars(lines), "hello world");

    // Tepat selebar baris.
    CHECK_EQ(joinWithBars(wrapText("hello world", 11, 0)), "hello world");

    // Spasi perata di dalam teks yang muat dipertahankan, begitu juga spasi di ujungnya.
    CHECK_EQ(joinWithBars(wrapText("Label   : value", 15, 4)), "Label   : value");
    CHECK_EQ(joinWithBars(wrapText("a   b", 10, 0)), "a   b");
    CHECK_EQ(joinWithBars(wrapText("  indented", 10, 0)), "  indented");
    CHECK_EQ(joinWithBars(wrapText("trailing  ", 10, 0)), "trailing  ");
}

static void testWrapSimple() {
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 7, 0)), "aaa bbb|ccc ddd");
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 8, 0)), "aaa bbb|ccc ddd");
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 3, 0)), "aaa|bbb|ccc|ddd");
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 100, 0)), "aaa bbb ccc ddd");
    // Spasi ganda di titik lipat tidak menghasilkan spasi di ujung baris.
    CHECK_EQ(joinWithBars(wrapText("aaa    bbb", 5, 0)), "aaa|bbb");
}

static void testWrapHangingIndent() {
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 8, 2)), "aaa bbb|  ccc|  ddd");
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 9, 2)), "aaa bbb|  ccc ddd");
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc ddd", 7, 4)), "aaa bbb|    ccc|    ddd");
    // Indentasi yang tidak menyisakan satu kolom pun untuk isi diabaikan.
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc", 4, 4)), "aaa|bbb|ccc");
    CHECK_EQ(joinWithBars(wrapText("aaa bbb ccc", 4, 9)), "aaa|bbb|ccc");
}

static void testWrapKeepsLabelAlignment() {
    // Spasi perata sesudah label tetap utuh di baris pertama; baris lanjutan menjorok ke kolom nilai.
    CHECK_EQ(joinWithBars(wrapText("Nama    : aaa bbb ccc", 16, 7)), "Nama    : aaa|       bbb ccc");
    CHECK_EQ(joinWithBars(wrapText("Total biaya : Rp821.400 dan seterusnya", 30, 14)),
             "Total biaya : Rp821.400 dan|              seterusnya");
}

static void testWrapLeadingSpaces() {
    // Spasi di awal teks menjadi indentasi baris pertama saja.
    CHECK_EQ(joinWithBars(wrapText("  aa bb cc", 6, 0)), "  aa|bb cc");
    CHECK_EQ(joinWithBars(wrapText("  aa bb cc", 6, 2)), "  aa|  bb|  cc");
}

static void testWrapHardSplit() {
    CHECK_EQ(joinWithBars(wrapText("abcdefghij", 4, 0)), "abcd|efgh|ij");
    CHECK_EQ(joinWithBars(wrapText("abcdefghij", 5, 0)), "abcde|fghij");
    CHECK_EQ(joinWithBars(wrapText("ab cdefghij", 5, 0)), "ab|cdefg|hij");
    CHECK_EQ(joinWithBars(wrapText("abcdefghij", 5, 2)), "abcde|  fgh|  ij");
    CHECK_EQ(joinWithBars(wrapText("lihat https://contoh.example/jalan/panjang", 20, 0)),
             "lihat|https://contoh.examp|le/jalan/panjang");
}

static void testWrapDoesNotSplitUtf8() {
    const std::string wide = repeatText(BOX_H, 6);
    const std::vector<std::string> lines = wrapText(wide, 4, 0);
    CHECK_EQ(lines.size(), 2u);
    CHECK_EQ(lines[0], repeatText(BOX_H, 4));
    CHECK_EQ(lines[1], repeatText(BOX_H, 2));

    // Kata campuran: huruf beraksen tidak terbelah di titik lipat mana pun.
    const std::string text = "caf\xC3\xA9 caf\xC3\xA9 caf\xC3\xA9";
    for (std::size_t width = 1; width <= 16; ++width) {
        const std::vector<std::string> pieces = wrapText(text, width, 0);
        std::string rejoined;
        for (std::size_t i = 0; i < pieces.size(); ++i) {
            CHECK_AT_MOST(displayWidth(pieces[i]), width);
            // Byte lanjutan tidak boleh berada di awal baris: itu tanda karakter terbelah.
            CHECK(pieces[i].empty() || (static_cast<unsigned char>(pieces[i][0]) & 0xC0) != 0x80);
            rejoined += pieces[i];
        }
        CHECK_EQ(withoutSpaces(rejoined), withoutSpaces(text));
    }
}

static void testWrapNeverReturnsEmptyVector() {
    CHECK_EQ(wrapText("", 10, 0).size(), 1u);
    CHECK_EQ(wrapText("", 10, 0)[0], "");
    CHECK_EQ(wrapText("", 0, 0).size(), 1u);
    // Teks yang hanya terdiri dari spasi dan lebih lebar dari baris.
    const std::vector<std::string> blank = wrapText("          ", 4, 0);
    CHECK_EQ(blank.size(), 1u);
    CHECK_AT_MOST(displayWidth(blank[0]), 4u);
    // Lebar 0 diperlakukan sebagai 1.
    CHECK_EQ(joinWithBars(wrapText("ab", 0, 0)), "a|b");
}

static void testWrapEveryLineFits() {
    const std::string text =
        "Nama    : Poros penggerak utama dengan alur pasak dan ulir  https://contoh.example/jalan/yang/sangat/panjang sekali";
    for (std::size_t width = 1; width <= 60; ++width) {
        for (std::size_t indent = 0; indent <= 12; indent += 3) {
            const std::vector<std::string> lines = wrapText(text, width, indent);
            CHECK(!lines.empty());
            std::string rejoined;
            for (std::size_t i = 0; i < lines.size(); ++i) {
                CHECK_AT_MOST(displayWidth(lines[i]), width);
                rejoined += lines[i];
            }
            // Tidak ada huruf yang hilang atau tertukar; hanya spasi yang boleh berubah.
            CHECK_EQ(withoutSpaces(rejoined), withoutSpaces(text));
        }
    }
}

void runTextFormatTests() {
    testFormatThousands();
    testFormatRupiah();
    testFormatWeight();
    testFormatPercent();
    testDisplayWidth();
    testPadding();
    testRepeatText();
    testTruncateText();
    testToLowerAscii();
    testWrapKeepsTextThatFits();
    testWrapSimple();
    testWrapHangingIndent();
    testWrapKeepsLabelAlignment();
    testWrapLeadingSpaces();
    testWrapHardSplit();
    testWrapDoesNotSplitUtf8();
    testWrapNeverReturnsEmptyVector();
    testWrapEveryLineFits();
}
