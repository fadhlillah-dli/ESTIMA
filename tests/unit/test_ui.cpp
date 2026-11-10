// ===== test_ui.cpp - uji gaya keluaran, kotak, tabel, dan pesan layar =====
// Keluaran digambar ke std::ostringstream dengan plainStyle(lebar) (tanpa warna, garis ASCII)
// sehingga teks yang dihasilkan bisa dibandingkan huruf demi huruf dengan yang ditulis di sini.
#include "estima/platform.h"

#include "estima/ui.h"

#include <cstddef>
#include <sstream>
#include <string>
#include <vector>

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

// Garis kotak UTF-8: datar, tegak, dan keempat sudutnya.
static const char* const U_H = "\xE2\x94\x80";
static const char* const U_V = "\xE2\x94\x82";
static const char* const U_TL = "\xE2\x94\x8C";
static const char* const U_TR = "\xE2\x94\x90";
static const char* const U_BL = "\xE2\x94\x94";
static const char* const U_BR = "\xE2\x94\x98";

static OutputStyle unicodeStyle(int width) {
    OutputStyle style = plainStyle(width);
    style.useUnicode = true;
    return style;
}

static OutputStyle colorStyle(int width) {
    OutputStyle style = plainStyle(width);
    style.useColor = true;
    return style;
}

static std::string drawnBox(const OutputStyle& style, const std::string& title, const std::vector<StyledLine>& lines) {
    std::ostringstream out;
    drawBox(out, style, title, lines);
    return out.str();
}

static std::string drawnTable(const OutputStyle& style, const std::string& title, const Table& table,
                              const std::vector<StyledLine>& notes) {
    std::ostringstream out;
    drawTableBox(out, style, title, table, notes);
    return out.str();
}

// Tabel kecil tiga kolom yang muat di layar mana pun: No, Nama, Total.
static Table smallTable() {
    Table table;
    table.columns.push_back(TableColumn("No", true));
    table.columns.push_back(TableColumn("Nama", false));
    table.columns.push_back(TableColumn("Total", true));
    TableRow first;
    first.cells.push_back("1");
    first.cells.push_back("Gear");
    first.cells.push_back("100");
    table.rows.push_back(first);
    TableRow second;
    second.cells.push_back("2");
    second.cells.push_back("Shaft");
    second.cells.push_back("2.000");
    table.rows.push_back(second);
    table.hasFooter = true;
    table.footer.cells.push_back("");
    table.footer.cells.push_back("TOTAL");
    table.footer.cells.push_back("2.100");
    return table;
}

// Tabel empat kolom dengan nama komponen sepanjang batas, terlalu lebar untuk 40 kolom.
static Table wideTable() {
    Table table;
    table.columns.push_back(TableColumn("No", true));
    table.columns.push_back(TableColumn("Komponen", false));
    table.columns.push_back(TableColumn("Total biaya", true));
    table.columns.push_back(TableColumn("Porsi", true));
    TableRow first;
    first.cells.push_back("1");
    first.cells.push_back("Shaft-Heavy-Duty-Gear-Assembly");
    first.cells.push_back("Rp1.612.800");
    first.cells.push_back("66,3%");
    table.rows.push_back(first);
    TableRow second;
    second.cells.push_back("2");
    second.cells.push_back("Gear");
    second.cells.push_back("Rp821.400");
    second.cells.push_back("33,7%");
    table.rows.push_back(second);
    return table;
}

static const char* const NOTE_WORDS = "satu dua tiga empat lima enam tujuh delapan sembilan sepuluh sebelas";

static void testPlainStyle() {
    const OutputStyle style = plainStyle(72);
    CHECK_EQ(style.width, 72);
    CHECK(!style.useColor);
    CHECK(!style.useUnicode);
    CHECK_EQ(plainStyle(40).width, 40);
    CHECK_EQ(plainStyle(120).width, 120);
}

static void testColorizeWithColorOff() {
    const OutputStyle style = plainStyle(80);
    for (int textStyle = 0; textStyle < STYLE_COUNT; ++textStyle) {
        CHECK_EQ(colorize(style, static_cast<TextStyle>(textStyle), "teks"), "teks");
    }
    CHECK_EQ(colorize(style, STYLE_ERROR, ""), "");
}

static void testColorizeWithColorOn() {
    const OutputStyle style = colorStyle(80);
    CHECK_EQ(colorize(style, STYLE_TITLE, "x"), "\x1B[1;36mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_BOLD, "x"), "\x1B[1mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_DIM, "x"), "\x1B[90mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_SUCCESS, "x"), "\x1B[1;32mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_ERROR, "x"), "\x1B[1;31mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_HINT, "x"), "\x1B[33mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_HIGHLIGHT, "x"), "\x1B[1;33mx\x1B[0m");
    CHECK_EQ(colorize(style, STYLE_BOLD, "dua kata"), "\x1B[1mdua kata\x1B[0m");
    // Gaya biasa dan teks kosong tidak dibungkus kode apa pun.
    CHECK_EQ(colorize(style, STYLE_NORMAL, "x"), "x");
    CHECK_EQ(colorize(style, STYLE_ERROR, ""), "");
}

static void testBarGlyphAndInnerLimit() {
    CHECK_EQ(std::string(barGlyph(plainStyle(80))), "#");
    CHECK_EQ(std::string(barGlyph(unicodeStyle(80))), "\xE2\x96\x88");

    // Lebar isi = lebar keluaran dikurangi 2 kolom tepi, 2 kolom spasi, dan 1 kolom sisa kanan.
    CHECK_EQ(boxInnerLimit(plainStyle(80)), 75u);
    CHECK_EQ(boxInnerLimit(plainStyle(40)), 35u);
    CHECK_EQ(boxInnerLimit(plainStyle(120)), 115u);
    // Untuk lebar yang sangat sempit isi tetap diberi sepuluh kolom.
    CHECK_EQ(boxInnerLimit(plainStyle(10)), 10u);
    CHECK_EQ(boxInnerLimit(plainStyle(1)), 10u);
}

static void testDrawBoxExactText() {
    std::vector<StyledLine> lines;
    lines.push_back(StyledLine("abc", STYLE_NORMAL));
    lines.push_back(StyledLine("de", STYLE_BOLD));
    CHECK_EQ(drawnBox(plainStyle(80), "Judul", lines),
             "+- Judul +\n"
             "| abc    |\n"
             "| de     |\n"
             "+--------+\n");

    // Judul lebih pendek dari isi: garis atas diisi sampai ujung kanan.
    std::vector<StyledLine> hello;
    hello.push_back(StyledLine("Hello", STYLE_NORMAL));
    CHECK_EQ(drawnBox(plainStyle(80), "T", hello),
             "+- T ---+\n"
             "| Hello |\n"
             "+-------+\n");

    // Tanpa judul.
    std::vector<StyledLine> bare;
    bare.push_back(StyledLine("ab", STYLE_NORMAL));
    CHECK_EQ(drawnBox(plainStyle(80), "", bare),
             "+----+\n"
             "| ab |\n"
             "+----+\n");

    // Tanpa baris isi: hanya garis atas dan bawah.
    CHECK_EQ(drawnBox(plainStyle(80), "T", std::vector<StyledLine>()),
             "+- T +\n"
             "+----+\n");

    // Baris kosong tetap digambar sebagai baris.
    std::vector<StyledLine> gap;
    gap.push_back(StyledLine("a", STYLE_NORMAL));
    gap.push_back(StyledLine("", STYLE_NORMAL));
    gap.push_back(StyledLine("b", STYLE_NORMAL));
    CHECK_EQ(drawnBox(plainStyle(80), "", gap),
             "+---+\n"
             "| a |\n"
             "|   |\n"
             "| b |\n"
             "+---+\n");
}

static void testDrawBoxUnicode() {
    std::vector<StyledLine> hello;
    hello.push_back(StyledLine("Hello", STYLE_NORMAL));
    const std::string expected =
        std::string(U_TL) + U_H + " T " + U_H + U_H + U_H + U_TR + "\n"
        + U_V + " Hello " + U_V + "\n"
        + U_BL + U_H + U_H + U_H + U_H + U_H + U_H + U_H + U_BR + "\n";
    CHECK_EQ(drawnBox(unicodeStyle(80), "T", hello), expected);
}

static void testDrawBoxWithColor() {
    std::vector<StyledLine> lines;
    lines.push_back(StyledLine("abc", STYLE_ERROR));
    const std::string out = drawnBox(colorStyle(80), "Judul", lines);
    CHECK_CONTAINS(out, "\x1B[1;36mJudul\x1B[0m");
    CHECK_CONTAINS(out, "\x1B[1;31mabc   \x1B[0m");
    CHECK_CONTAINS(out, "\x1B[90m");
    // Warna tidak mengubah tata letak: tanpa kode ESC, barisnya sama dengan versi polos.
    std::string stripped;
    bool insideCode = false;
    for (std::size_t i = 0; i < out.size(); ++i) {
        if (out[i] == '\x1B') {
            insideCode = true;
        } else if (insideCode) {
            insideCode = out[i] != 'm';
        } else {
            stripped += out[i];
        }
    }
    CHECK_EQ(stripped, drawnBox(plainStyle(80), "Judul", lines));
}

static void testDrawBoxLinesHaveEqualWidth() {
    std::vector<StyledLine> lines;
    lines.push_back(StyledLine("short", STYLE_NORMAL));
    lines.push_back(StyledLine("a much longer line of text that is going to need wrapping at narrow widths for sure okay", STYLE_BOLD));
    lines.push_back(StyledLine("Label : value that is quite long indeed and will wrap under the value column for sure yes", STYLE_NORMAL));
    lines.push_back(StyledLine("", STYLE_NORMAL));
    lines.push_back(StyledLine("  indented line", STYLE_DIM));
    lines.push_back(StyledLine("caf\xC3\xA9 \xE2\x96\x88\xE2\x96\x88", STYLE_NORMAL));
    lines.push_back(StyledLine("https://contoh.example/jalan/yang/sangat/panjang/sekali/dan/tidak/bisa/dilipat/di/spasi/mana/pun", STYLE_NORMAL));

    const int widths[] = { 40, 60, 80, 120 };
    for (std::size_t w = 0; w < sizeof(widths) / sizeof(widths[0]); ++w) {
        for (int unicode = 0; unicode < 2; ++unicode) {
            OutputStyle style = plainStyle(widths[w]);
            style.useUnicode = unicode != 0;
            const std::string out = drawnBox(style, "Judul Kotak", lines);
            CHECK_EQ(widestLine(out), narrowestLine(out));
            // Satu kolom di ujung kanan terminal tidak dipakai.
            CHECK_AT_MOST(widestLine(out), static_cast<std::size_t>(widths[w] - 1));
        }
    }
}

static void testDrawBoxLongTitle() {
    std::vector<StyledLine> lines;
    lines.push_back(StyledLine("isi", STYLE_NORMAL));
    const std::string title(100, 'J');
    const std::string out = drawnBox(plainStyle(40), title, lines);
    CHECK_EQ(widestLine(out), narrowestLine(out));
    CHECK_AT_MOST(widestLine(out), 39u);
    CHECK_CONTAINS(out, "J..");
}

static void testDrawBoxHangingIndentUnderValue() {
    // Baris "Label : nilai" yang dilipat dilanjutkan tepat di bawah nilainya.
    std::vector<StyledLine> lines;
    lines.push_back(StyledLine("Nama : satu dua tiga empat lima enam tujuh delapan sembilan sepuluh sebelas", STYLE_NORMAL));
    const std::vector<std::string> rows = splitLines(drawnBox(plainStyle(40), "", lines));
    CHECK(rows.size() >= 4);
    if (rows.size() >= 4) {
        CHECK_EQ(rows[1].substr(0, 9), "| Nama : ");
        CHECK_EQ(rows[2].substr(0, 9), "|        ");
        CHECK(rows[2][9] != ' ');
    }
}

static void testTableThatFitsIsATable() {
    CHECK_EQ(drawnTable(plainStyle(80), "Tabel", smallTable(), std::vector<StyledLine>()),
             "+- Tabel ----------+\n"
             "| No  Nama   Total |\n"
             "| --  -----  ----- |\n"
             "|  1  Gear     100 |\n"
             "|  2  Shaft  2.000 |\n"
             "| --  -----  ----- |\n"
             "|     TOTAL  2.100 |\n"
             "+------------------+\n");

    // Tabel yang sama juga muat di 40 kolom dan hasilnya sama.
    CHECK_EQ(drawnTable(plainStyle(40), "Tabel", smallTable(), std::vector<StyledLine>()),
             drawnTable(plainStyle(80), "Tabel", smallTable(), std::vector<StyledLine>()));
}

static void testTableWithoutFooterOrRows() {
    Table table = smallTable();
    table.hasFooter = false;
    CHECK_EQ(drawnTable(plainStyle(80), "Tabel", table, std::vector<StyledLine>()),
             "+- Tabel ----------+\n"
             "| No  Nama   Total |\n"
             "| --  -----  ----- |\n"
             "|  1  Gear     100 |\n"
             "|  2  Shaft  2.000 |\n"
             "+------------------+\n");

    // Tanpa baris dan tanpa total, lebar kolom ditentukan judulnya saja.
    table.rows.clear();
    CHECK_EQ(drawnTable(plainStyle(80), "Tabel", table, std::vector<StyledLine>()),
             "+- Tabel ---------+\n"
             "| No  Nama  Total |\n"
             "| --  ----  ----- |\n"
             "+-----------------+\n");
}

static void testTableUnicodeRule() {
    const std::string out = drawnTable(unicodeStyle(80), "Tabel", smallTable(), std::vector<StyledLine>());
    const std::string rule = std::string(U_H) + U_H + "  " + U_H + U_H + U_H + U_H + U_H + "  " + U_H + U_H + U_H + U_H + U_H;
    CHECK_EQ(countOccurrences(out, rule), 2u);
    CHECK_EQ(widestLine(out), narrowestLine(out));
}

static void testTableShortRowsAreFilled() {
    // Baris dengan sel lebih sedikit dari kolomnya dilengkapi sel kosong, bukan merusak tabel.
    Table table;
    table.columns.push_back(TableColumn("A", false));
    table.columns.push_back(TableColumn("B", false));
    TableRow row;
    row.cells.push_back("x");
    table.rows.push_back(row);
    CHECK_EQ(drawnTable(plainStyle(80), "", table, std::vector<StyledLine>()),
             "+------+\n"
             "| A  B |\n"
             "| -  - |\n"
             "| x    |\n"
             "+------+\n");
}

static void testTableThatDoesNotFitFallsBackToCards() {
    const std::string out = drawnTable(plainStyle(40), "Kartu", wideTable(), std::vector<StyledLine>());
    const std::vector<std::string> rows = splitLines(out);
    CHECK_EQ(rows.size(), 9u);
    if (rows.size() != 9) {
        return;
    }
    const std::size_t inner = 33;
    CHECK_EQ(rows[0], "+- Kartu " + std::string(27, '-') + "+");
    CHECK_EQ(rows[1], boxRow("1. Shaft-Heavy-Duty-Gear-Assembly", inner));
    CHECK_EQ(rows[2], boxRow("   Total biaya : Rp1.612.800", inner));
    CHECK_EQ(rows[3], boxRow("   Porsi       : 66,3%", inner));
    CHECK_EQ(rows[4], boxRow("", inner));
    CHECK_EQ(rows[5], boxRow("2. Gear", inner));
    CHECK_EQ(rows[6], boxRow("   Total biaya : Rp821.400", inner));
    CHECK_EQ(rows[7], boxRow("   Porsi       : 33,7%", inner));
    CHECK_EQ(rows[8], "+" + std::string(35, '-') + "+");
    // Tidak ada baris judul kolom atau garis pemisah seperti pada tabel.
    CHECK_NOT_CONTAINS(out, "No  Komponen");
    CHECK_NOT_CONTAINS(out, "--  ");

    // Pada terminal yang lebar tabel yang sama digambar sebagai tabel.
    const std::string wide = drawnTable(plainStyle(80), "Tabel", wideTable(), std::vector<StyledLine>());
    CHECK_CONTAINS(wide, "No  Komponen");
    CHECK_NOT_CONTAINS(wide, "1. Shaft");
}

static void testNotesWrapToTableWidth() {
    // Tabel 16 kolom lebarnya kurang dari batas catatan, jadi catatan dilipat di 44 kolom.
    std::vector<StyledLine> notes;
    notes.push_back(StyledLine(NOTE_WORDS, STYLE_DIM));
    const std::string out = drawnTable(plainStyle(80), "Tabel", smallTable(), notes);
    CHECK_CONTAINS(out, "| satu dua tiga empat lima enam tujuh delapan |\n");
    CHECK_CONTAINS(out, "| sembilan sepuluh sebelas" + std::string(19, ' ') + " |\n");
    CHECK_EQ(widestLine(out), 47u);
    CHECK_EQ(narrowestLine(out), 47u);
    // Satu baris kosong memisahkan tabel dari catatan.
    CHECK_CONTAINS(out, "| " + std::string(43, ' ') + " |\n");

    // Pada 40 kolom catatan dilipat di 35 kolom, batas isi kotak.
    const std::string narrow = drawnTable(plainStyle(40), "Tabel", smallTable(), notes);
    CHECK_CONTAINS(narrow, "| satu dua tiga empat lima enam tujuh |\n");
    CHECK_CONTAINS(narrow, "| delapan sembilan sepuluh sebelas    |\n");
    CHECK_EQ(widestLine(narrow), 39u);
}

static void testNotesFollowWideTable() {
    // Tabel lebar 60 kolom: catatan dilipat selebar tabel, bukan 44 kolom.
    Table table;
    table.columns.push_back(TableColumn("Isi", false));
    TableRow row;
    row.cells.push_back(std::string(60, 'x'));
    table.rows.push_back(row);

    std::string text = "kata";
    for (int i = 1; i < 20; ++i) {
        text += " kata";
    }
    std::vector<StyledLine> notes;
    notes.push_back(StyledLine(text, STYLE_NORMAL));
    const std::string out = drawnTable(plainStyle(120), "Lebar", table, notes);

    std::string twelve = "kata";
    for (int i = 1; i < 12; ++i) {
        twelve += " kata";
    }
    CHECK_CONTAINS(out, "| " + twelve + "  |\n");
    CHECK_NOT_CONTAINS(out, twelve + " kata");
    CHECK_EQ(widestLine(out), 64u);
    CHECK_EQ(narrowestLine(out), 64u);
}

static void testCardsKeepNotes() {
    std::vector<StyledLine> notes;
    notes.push_back(StyledLine(NOTE_WORDS, STYLE_DIM));
    const std::string out = drawnTable(plainStyle(40), "Kartu", wideTable(), notes);
    CHECK_CONTAINS(out, "1. Shaft-Heavy-Duty-Gear-Assembly");
    CHECK_CONTAINS(out, "satu dua tiga empat lima enam tujuh");
    CHECK_CONTAINS(out, "delapan sembilan sepuluh sebelas");
    CHECK_EQ(widestLine(out), narrowestLine(out));
    CHECK_AT_MOST(widestLine(out), 39u);
}

static void testPrintWrapped() {
    std::ostringstream out;
    printWrapped(out, plainStyle(20), STYLE_NORMAL, "aaaa bbbb cccc dddd eeee", 0);
    CHECK_EQ(out.str(), "aaaa bbbb cccc dddd\neeee\n");

    std::ostringstream indented;
    printWrapped(indented, plainStyle(20), STYLE_NORMAL, "aaaa bbbb cccc dddd eeee", 2);
    CHECK_EQ(indented.str(), "aaaa bbbb cccc dddd\n  eeee\n");

    std::ostringstream shortText;
    printWrapped(shortText, plainStyle(20), STYLE_NORMAL, "pendek", 4);
    CHECK_EQ(shortText.str(), "pendek\n");

    // Dengan warna, setiap baris dibungkus sendiri-sendiri.
    std::ostringstream colored;
    printWrapped(colored, colorStyle(20), STYLE_ERROR, "aaaa bbbb cccc dddd eeee", 0);
    CHECK_EQ(colored.str(), "\x1B[1;31maaaa bbbb cccc dddd\x1B[0m\n\x1B[1;31meeee\x1B[0m\n");
}

static void testScreenMessages() {
    {
        ConsoleCapture capture("");
        printError("Salah");
        CHECK_EQ(capture.screenText(), "[!] Salah\n");
    }
    {
        ConsoleCapture capture("");
        printSuccess("Selesai");
        CHECK_EQ(capture.screenText(), "[OK] Selesai\n");
    }
    {
        ConsoleCapture capture("");
        printInfo("Keterangan");
        CHECK_EQ(capture.screenText(), "[i] Keterangan\n");
    }
    {
        // Pesan panjang dilipat dan baris lanjutannya menjorok di bawah teks pesan.
        ConsoleCapture capture("");
        std::string longMessage = "kata";
        for (int i = 1; i < 40; ++i) {
            longMessage += " kata";
        }
        printError(longMessage);
        const std::vector<std::string> rows = splitLines(capture.screenText());
        CHECK(rows.size() >= 2);
        CHECK_EQ(rows[0].substr(0, 4), "[!] ");
        CHECK_EQ(rows[1].substr(0, 4), "    ");
    }
}

void runUiTests() {
    testPlainStyle();
    testColorizeWithColorOff();
    testColorizeWithColorOn();
    testBarGlyphAndInnerLimit();
    testDrawBoxExactText();
    testDrawBoxUnicode();
    testDrawBoxWithColor();
    testDrawBoxLinesHaveEqualWidth();
    testDrawBoxLongTitle();
    testDrawBoxHangingIndentUnderValue();
    testTableThatFitsIsATable();
    testTableWithoutFooterOrRows();
    testTableUnicodeRule();
    testTableShortRowsAreFilled();
    testTableThatDoesNotFitFallsBackToCards();
    testNotesWrapToTableWidth();
    testNotesFollowWideTable();
    testCardsKeepNotes();
    testPrintWrapped();
    testScreenMessages();
}
