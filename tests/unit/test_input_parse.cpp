// ===== test_input_parse.cpp - uji pemeriksaan ketikan: nama, jumlah, berat, harga, berkas, menu, ya/tidak =====
// Untuk setiap penolakan diperiksa dua hal: pesan kesalahan tidak kosong, dan parameter keluaran
// tidak berubah (nilai awalnya sengaja diisi angka yang tidak mungkin dihasilkan penerimaan).
#include "estima/platform.h"

#include "estima/input_parse.h"

#include <string>

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

static const int QUANTITY_SENTINEL = 777;
static const double WEIGHT_SENTINEL = 777.5;
static const double PRICE_SENTINEL = 777.0;

static void expectQuantity(const std::string& text, int expected) {
    int quantity = QUANTITY_SENTINEL;
    CHECK_EQ(checkQuantity(text, quantity), "");
    CHECK_EQ(quantity, expected);
}

static void expectQuantityRejected(const std::string& text) {
    int quantity = QUANTITY_SENTINEL;
    CHECK(!checkQuantity(text, quantity).empty());
    CHECK_EQ(quantity, QUANTITY_SENTINEL);
}

static void expectWeight(const std::string& text, double expected) {
    double weight = WEIGHT_SENTINEL;
    CHECK_EQ(checkUnitWeight(text, weight), "");
    CHECK_EQ(weight, expected);
}

static void expectWeightRejected(const std::string& text) {
    double weight = WEIGHT_SENTINEL;
    CHECK(!checkUnitWeight(text, weight).empty());
    CHECK_EQ(weight, WEIGHT_SENTINEL);
}

static void expectPrice(const std::string& text, double expected) {
    double price = PRICE_SENTINEL;
    CHECK_EQ(checkPricePerKg(text, price), "");
    CHECK_EQ(price, expected);
}

static void expectPriceRejected(const std::string& text) {
    double price = PRICE_SENTINEL;
    CHECK(!checkPricePerKg(text, price).empty());
    CHECK_EQ(price, PRICE_SENTINEL);
}

static void expectFileName(const std::string& text, const std::string& expected) {
    std::string fileName = "SENTINEL";
    CHECK_EQ(checkReportFileName(text, fileName), "");
    CHECK_EQ(fileName, expected);
}

static void expectFileNameRejected(const std::string& text) {
    std::string fileName = "SENTINEL";
    CHECK(!checkReportFileName(text, fileName).empty());
    CHECK_EQ(fileName, "SENTINEL");
}

static void testTrimSpaces() {
    CHECK_EQ(trimSpaces("  a b  "), "a b");
    CHECK_EQ(trimSpaces("\t x \r\n"), "x");
    CHECK_EQ(trimSpaces("a"), "a");
    CHECK_EQ(trimSpaces(""), "");
    CHECK_EQ(trimSpaces("   "), "");
    CHECK_EQ(trimSpaces(" \t\r\n"), "");
    CHECK_EQ(trimSpaces("a   b"), "a   b");
    CHECK_EQ(trimSpaces("abc\r"), "abc");
}

static void testCollapseSpaces() {
    CHECK_EQ(collapseSpaces("  a   b \t c  "), "a b c");
    CHECK_EQ(collapseSpaces("a\t\tb"), "a b");
    CHECK_EQ(collapseSpaces("a\r\nb"), "a b");
    CHECK_EQ(collapseSpaces("a"), "a");
    CHECK_EQ(collapseSpaces("a  "), "a");
    CHECK_EQ(collapseSpaces("  a"), "a");
    CHECK_EQ(collapseSpaces("   "), "");
    CHECK_EQ(collapseSpaces(""), "");
    CHECK_EQ(collapseSpaces("a b c"), "a b c");
}

static void testIsAllDigits() {
    CHECK(isAllDigits("0"));
    CHECK(isAllDigits("123"));
    CHECK(isAllDigits("007"));
    CHECK(!isAllDigits(""));
    CHECK(!isAllDigits("12a"));
    CHECK(!isAllDigits("1 2"));
    CHECK(!isAllDigits("-1"));
    CHECK(!isAllDigits("1.5"));
    CHECK(!isAllDigits(" 1"));
}

static void testComponentNameAccepted() {
    CHECK_EQ(checkComponentName("Gear"), "");
    CHECK_EQ(checkComponentName("A"), "");
    CHECK_EQ(checkComponentName("7"), "");
    CHECK_EQ(checkComponentName("Base Plate"), "");
    CHECK_EQ(checkComponentName("Shaft-12 (long)"), "");
    CHECK_EQ(checkComponentName("Pin_2.5, tipe A/B"), "");
    CHECK_EQ(checkComponentName("C#&+"), "");
    // Tepat 30 karakter.
    CHECK_EQ(checkComponentName(std::string(30, 'a')), "");
}

static void testComponentNameRejected() {
    CHECK(!checkComponentName("").empty());
    // 31 karakter, satu di atas batas.
    CHECK(!checkComponentName(std::string(31, 'a')).empty());
    CHECK(!checkComponentName(std::string(200, 'a')).empty());
    // Harus diawali huruf atau angka.
    CHECK(!checkComponentName("-Gear").empty());
    CHECK(!checkComponentName("_Gear").empty());
    CHECK(!checkComponentName("(Gear)").empty());
    CHECK(!checkComponentName(" Gear").empty());
    // Karakter di luar yang diizinkan.
    CHECK(!checkComponentName("Gear!").empty());
    CHECK(!checkComponentName("Gear:2").empty());
    CHECK(!checkComponentName("Gear*").empty());
    CHECK(!checkComponentName("a@b").empty());
    CHECK(!checkComponentName("a;b").empty());
    CHECK(!checkComponentName("a\"b").empty());
    CHECK(!checkComponentName("a\\b").empty());
    CHECK(!checkComponentName("tab\tx").empty());
    CHECK(!checkComponentName("Gear\xC3\xA9").empty());
    CHECK(!checkComponentName("\xC3\xA9" "gear").empty());
}

static void testQuantityAccepted() {
    expectQuantity("24", 24);
    expectQuantity("1", 1);
    expectQuantity("1000000", 1000000);
    expectQuantity("999999", 999999);
    expectQuantity("10", 10);
    // Angka 0 di depan tidak mengubah nilai, juga yang sangat panjang.
    expectQuantity("007", 7);
    expectQuantity("000024", 24);
    expectQuantity("0000000001", 1);
    expectQuantity(std::string(39, '0') + "5", 5);
}

static void testQuantityRejected() {
    expectQuantityRejected("");
    expectQuantityRejected("0");
    expectQuantityRejected("00");
    expectQuantityRejected("1000001");
    expectQuantityRejected("2000000");
    expectQuantityRejected("999999999");
    expectQuantityRejected("1000000000");
    expectQuantityRejected(std::string(40, '9'));
    expectQuantityRejected("1" + std::string(39, '0'));
    expectQuantityRejected("-5");
    expectQuantityRejected("+5");
    expectQuantityRejected("1.5");
    expectQuantityRejected("1,5");
    expectQuantityRejected("2 4");
    expectQuantityRejected("24 ");
    expectQuantityRejected(" 24");
    expectQuantityRejected("abc");
    expectQuantityRejected("24x");
    expectQuantityRejected("1e3");
    expectQuantityRejected("1.000");
}

static void testWeightAccepted() {
    expectWeight("1,85", 1.85);
    expectWeight("1.85", 1.85);
    expectWeight("4,2", 4.2);
    expectWeight("4.200", 4.2);
    expectWeight("24", 24.0);
    expectWeight("0,125", 0.125);
    expectWeight("0.5", 0.5);
    expectWeight("9999,999", 9999.999);
    // Batas bawah dan batas atas.
    expectWeight("0,001", 0.001);
    expectWeight("0.001", 0.001);
    expectWeight("10000", 10000.0);
    expectWeight("10000,000", 10000.0);
    expectWeight("10000.0", 10000.0);
    // Angka 0 di depan.
    expectWeight("007,5", 7.5);
    expectWeight("00,001", 0.001);
    expectWeight(std::string(38, '0') + "1,5", 1.5);
    expectWeight(std::string(40, '0') + ",5", 0.5);
}

static void testWeightRejected() {
    expectWeightRejected("");
    // Di bawah batas bawah dan di atas batas atas.
    expectWeightRejected("0");
    expectWeightRejected("0,000");
    expectWeightRejected("0.0");
    expectWeightRejected("10000,001");
    expectWeightRejected("10001");
    expectWeightRejected("20000");
    // Lebih dari tiga angka desimal, juga bila angka terakhirnya 0.
    expectWeightRejected("0,0009");
    expectWeightRejected("0,0001");
    expectWeightRejected("1,8500");
    expectWeightRejected("1.2345");
    expectWeightRejected("10000,0001");
    expectWeightRejected("1," + std::string(40, '1'));
    // Angka terlalu panjang.
    expectWeightRejected(std::string(40, '9'));
    expectWeightRejected(std::string(10, '9') + ",5");
    // Bentuk yang salah.
    expectWeightRejected("-1");
    expectWeightRejected("-0,5");
    expectWeightRejected("abc");
    expectWeightRejected(",5");
    expectWeightRejected(".5");
    expectWeightRejected("5,");
    expectWeightRejected("5.");
    expectWeightRejected(",");
    expectWeightRejected("1,2,3");
    expectWeightRejected("1.2.3");
    expectWeightRejected("1,2.3");
    expectWeightRejected("1e3");
    expectWeightRejected("1 85");
    expectWeightRejected("1,85 kg");
    expectWeightRejected(" 1,85");
    expectWeightRejected("1,8a");
}

static void testPriceAccepted() {
    expectPrice("18500", 18500.0);
    expectPrice("32000", 32000.0);
    expectPrice("1", 1.0);
    expectPrice("99999999", 99999999.0);
    expectPrice("100000000", 100000000.0);
    expectPrice("000018500", 18500.0);
    expectPrice("0000000001", 1.0);
    expectPrice(std::string(39, '0') + "7", 7.0);
}

static void testPriceRejected() {
    expectPriceRejected("");
    expectPriceRejected("0");
    expectPriceRejected("000");
    expectPriceRejected("100000001");
    expectPriceRejected("999999999");
    expectPriceRejected("1000000000");
    expectPriceRejected(std::string(40, '9'));
    expectPriceRejected("18.500");
    expectPriceRejected("18,500");
    expectPriceRejected("18500,50");
    expectPriceRejected("Rp18500");
    expectPriceRejected("Rp 18500");
    expectPriceRejected("-1");
    expectPriceRejected("1e5");
    expectPriceRejected(" 5");
    expectPriceRejected("5 ");
    expectPriceRejected("1 000");
    expectPriceRejected("abc");
}

static void testReportFileNameAccepted() {
    expectFileName("laporan", "laporan.txt");
    expectFileName("laporan-estima", "laporan-estima.txt");
    expectFileName("laporan_1", "laporan_1.txt");
    expectFileName("Laporan2026", "Laporan2026.txt");
    expectFileName("laporan.txt", "laporan.txt");
    expectFileName("laporan.TXT", "laporan.TXT");
    expectFileName("Laporan.Txt", "Laporan.Txt");
    expectFileName("a.b", "a.b.txt");
    expectFileName("data.csv", "data.csv.txt");
    expectFileName("a.txt.txt", "a.txt.txt");
    expectFileName("txt", "txt.txt");
    // Nama yang hanya mirip nama perangkat Windows boleh.
    expectFileName("console", "console.txt");
    expectFileName("com0", "com0.txt");
    expectFileName("com10", "com10.txt");
    expectFileName("lpt", "lpt.txt");
    expectFileName("auxiliary.txt", "auxiliary.txt");
    // Tepat 60 karakter.
    expectFileName(std::string(60, 'a'), std::string(60, 'a') + ".txt");
}

static void testReportFileNameRejected() {
    expectFileNameRejected("");
    expectFileNameRejected(std::string(61, 'a'));
    // Spasi dan pemisah folder.
    expectFileNameRejected("my report");
    expectFileNameRejected(" laporan");
    expectFileNameRejected("laporan ");
    expectFileNameRejected("a/b");
    expectFileNameRejected("/etc/passwd");
    expectFileNameRejected("a\\b");
    expectFileNameRejected("C:\\laporan");
    expectFileNameRejected("C:laporan");
    expectFileNameRejected("../laporan");
    expectFileNameRejected("..");
    // Karakter lain yang dilarang sistem berkas.
    expectFileNameRejected("a*b");
    expectFileNameRejected("a?b");
    expectFileNameRejected("a<b");
    expectFileNameRejected("a|b");
    expectFileNameRejected("a\"b");
    expectFileNameRejected("lap\xC3\xA9");
    // Titik di awal atau akhir.
    expectFileNameRejected(".txt");
    expectFileNameRejected(".hidden");
    expectFileNameRejected("laporan.");
    expectFileNameRejected("laporan.txt.");
    expectFileNameRejected(".");
    // Nama perangkat Windows dalam huruf apa pun, dengan atau tanpa akhiran.
    expectFileNameRejected("con");
    expectFileNameRejected("CON");
    expectFileNameRejected("Con.txt");
    expectFileNameRejected("prn");
    expectFileNameRejected("aux");
    expectFileNameRejected("AUX.txt");
    expectFileNameRejected("nul");
    expectFileNameRejected("NUL.TXT");
    expectFileNameRejected("com1");
    expectFileNameRejected("COM1");
    expectFileNameRejected("com9");
    expectFileNameRejected("Com5.txt");
    expectFileNameRejected("lpt1");
    expectFileNameRejected("LPT9.log");
    expectFileNameRejected("con.tar.gz");
}

static void testParseMenuChoice() {
    int choice = -5;
    CHECK(parseMenuChoice("3", 0, 8, choice));
    CHECK_EQ(choice, 3);
    CHECK(parseMenuChoice("0", 0, 8, choice));
    CHECK_EQ(choice, 0);
    CHECK(parseMenuChoice("8", 0, 8, choice));
    CHECK_EQ(choice, 8);
    CHECK(parseMenuChoice("03", 0, 8, choice));
    CHECK_EQ(choice, 3);
    CHECK(parseMenuChoice(std::string(30, '0') + "4", 0, 8, choice));
    CHECK_EQ(choice, 4);
    CHECK(parseMenuChoice("1", 1, 5, choice));
    CHECK_EQ(choice, 1);
    CHECK(parseMenuChoice("5", 1, 5, choice));
    CHECK_EQ(choice, 5);
    CHECK(parseMenuChoice("12", 10, 12, choice));
    CHECK_EQ(choice, 12);

    // Penolakan tidak mengubah choice.
    choice = -5;
    CHECK(!parseMenuChoice("9", 0, 8, choice));
    CHECK(!parseMenuChoice("0", 1, 5, choice));
    CHECK(!parseMenuChoice("6", 1, 5, choice));
    CHECK(!parseMenuChoice("9", 10, 12, choice));
    CHECK(!parseMenuChoice("13", 10, 12, choice));
    CHECK(!parseMenuChoice("", 0, 8, choice));
    CHECK(!parseMenuChoice("-1", 0, 8, choice));
    CHECK(!parseMenuChoice("+3", 0, 8, choice));
    CHECK(!parseMenuChoice("abc", 0, 8, choice));
    CHECK(!parseMenuChoice("3a", 0, 8, choice));
    CHECK(!parseMenuChoice("3 ", 0, 8, choice));
    CHECK(!parseMenuChoice(" 3", 0, 8, choice));
    CHECK(!parseMenuChoice("3.0", 0, 8, choice));
    CHECK(!parseMenuChoice("1234567890", 0, 8, choice));
    CHECK(!parseMenuChoice(std::string(40, '9'), 0, 8, choice));
    CHECK_EQ(choice, -5);
}

static void testParseYesNo() {
    const char* const yes[] = { "y", "Y", "ya", "YA", "Ya", "yes", "YES", "Yes" };
    for (std::size_t i = 0; i < sizeof(yes) / sizeof(yes[0]); ++i) {
        CHECK_EQ(parseYesNo(yes[i]), ANSWER_YES);
    }
    const char* const no[] = { "t", "T", "tidak", "TIDAK", "Tidak", "n", "N", "no", "NO" };
    for (std::size_t i = 0; i < sizeof(no) / sizeof(no[0]); ++i) {
        CHECK_EQ(parseYesNo(no[i]), ANSWER_NO);
    }
    const char* const unknown[] = { "", "yy", "x", "ye", "tid", "1", "0", "ok", "yas" };
    for (std::size_t i = 0; i < sizeof(unknown) / sizeof(unknown[0]); ++i) {
        CHECK_EQ(parseYesNo(unknown[i]), ANSWER_UNKNOWN);
    }
}

void runInputParseTests() {
    testTrimSpaces();
    testCollapseSpaces();
    testIsAllDigits();
    testComponentNameAccepted();
    testComponentNameRejected();
    testQuantityAccepted();
    testQuantityRejected();
    testWeightAccepted();
    testWeightRejected();
    testPriceAccepted();
    testPriceRejected();
    testReportFileNameAccepted();
    testReportFileNameRejected();
    testParseMenuChoice();
    testParseYesNo();
}
