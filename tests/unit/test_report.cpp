// ===== test_report.cpp - uji tampilan rekap, peringkat, katalog, ringkasan, dan penyimpanan laporan =====
// Setiap tampilan digambar ke std::ostringstream dengan plainStyle(120) (tabel penuh) dan
// plainStyle(40) (terminal sempit: tabel berubah menjadi kartu). Angka harapan dihitung dengan
// tangan dari data uji di bawah, bukan dengan fungsi hitung program.
#include "estima/platform.h"

#include "estima/report.h"

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "estima/app_info.h"
#include "estima/material_catalog.h"
#include "estima/text_format.h"

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

// Gear dan Shaft dari sesi asap: biaya Rp821.400 dan Rp1.612.800, total 94,800 kg dan Rp2.434.200.
static ComponentList smokeList() {
    ComponentList list;
    addComponent(list, makeComponent("Gear", 24, 1.85, 18500.0));
    addComponent(list, makeComponent("Shaft", 12, 4.2, 32000.0));
    return list;
}

// Dua komponen berbiaya sama Rp1.000 (Alfa: 1 x 1 kg; Beta: 2 x 0,5 kg) dan satu Rp100.
// Total Rp2.100; porsi 1.000 / 2.100 = 47,6% dan 100 / 2.100 = 4,8%.
static ComponentList tieList() {
    ComponentList list;
    addComponent(list, makeComponent("Alfa", 1, 1.0, 1000.0));
    addComponent(list, makeComponent("Beta", 2, 0.5, 1000.0));
    addComponent(list, makeComponent("Gamma", 1, 0.1, 1000.0));
    return list;
}

// Biaya 500, 900, 900, dan 100: peringkatnya Beta 1, Gamma 1, Alfa 3, Delta 4. Total Rp2.400.
static ComponentList rankingList() {
    ComponentList list;
    addComponent(list, makeComponent("Alfa", 1, 1.0, 500.0));
    addComponent(list, makeComponent("Beta", 1, 1.0, 900.0));
    addComponent(list, makeComponent("Gamma", 1, 1.0, 900.0));
    addComponent(list, makeComponent("Delta", 1, 1.0, 100.0));
    return list;
}

static std::string recapText(const OutputStyle& style, const ComponentList& list) {
    std::ostringstream out;
    printRecap(out, style, list);
    return out.str();
}

static std::string rankingText(const OutputStyle& style, const ComponentList& list) {
    std::ostringstream out;
    printRanking(out, style, list);
    return out.str();
}

// Semua baris di dalam text yang memuat part, sesuai urutan tampilnya.
static std::vector<std::string> linesContaining(const std::string& text, const std::string& part) {
    std::vector<std::string> found;
    const std::vector<std::string> lines = splitLines(text);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(part) != std::string::npos) {
            found.push_back(lines[i]);
        }
    }
    return found;
}

// true bila ada satu baris yang memuat first dan second sekaligus.
static bool hasLineWith(const std::string& text, const std::string& first, const std::string& second) {
    const std::vector<std::string> lines = linesContaining(text, first);
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(second) != std::string::npos) {
            return true;
        }
    }
    return false;
}

// Memeriksa bahwa keluaran muat di layar selebar width (satu kolom ujung kanan dibiarkan kosong)
// dan hanya berisi ASCII.
static void checkFits(const std::string& text, int width) {
    CHECK(!text.empty());
    CHECK_AT_MOST(widestLine(text), static_cast<std::size_t>(width - 1));
    CHECK(isPlainAscii(text));
}

// Untuk keluaran yang berupa satu kotak: semua barisnya juga harus sama lebar.
static void checkLayout(const std::string& text, int width) {
    checkFits(text, width);
    CHECK_EQ(widestLine(text), narrowestLine(text));
}

// ----- printRecap -----

static void testRecapWideTable() {
    const std::string out = recapText(plainStyle(120), smokeList());
    CHECK_CONTAINS(out, "Rekap Material dan Biaya");
    CHECK_CONTAINS(out, "Total Keseluruhan");
    CHECK_CONTAINS(out, "Biaya Produksi Tertinggi");
    CHECK(isPlainAscii(out));
    CHECK_AT_MOST(widestLine(out), 119u);

    // Judul kolom, baris tiap komponen, dan baris total sebagai kata-kata (tanpa spasi perata).
    const std::vector<std::string> header = splitWords(lineContaining(out, "Berat/unit"));
    CHECK_EQ(header.size(), 11u);
    if (header.size() == 11) {
        CHECK_EQ(header[1], "No");
        CHECK_EQ(header[2], "Komponen");
        CHECK_EQ(header[3], "Jumlah");
        CHECK_EQ(header[4], "Berat/unit");
        CHECK_EQ(header[5], "Harga/kg");
    }
    const std::vector<std::string> gear = splitWords(lineContaining(out, "Gear"));
    CHECK_EQ(gear.size(), 11u);
    if (gear.size() == 11) {
        CHECK_EQ(gear[1], "1");
        CHECK_EQ(gear[2], "Gear");
        CHECK_EQ(gear[3], "24");
        CHECK_EQ(gear[4], "1,850");
        CHECK_EQ(gear[6], "Rp18.500");
        CHECK_EQ(gear[7], "44,400");
        CHECK_EQ(gear[9], "Rp821.400");
    }
    const std::vector<std::string> shaft = splitWords(lineContaining(out, "Shaft  "));
    CHECK_EQ(shaft.size(), 11u);
    if (shaft.size() == 11) {
        CHECK_EQ(shaft[1], "2");
        CHECK_EQ(shaft[3], "12");
        CHECK_EQ(shaft[4], "4,200");
        CHECK_EQ(shaft[6], "Rp32.000");
        CHECK_EQ(shaft[7], "50,400");
        CHECK_EQ(shaft[9], "Rp1.612.800");
    }
    const std::vector<std::string> total = splitWords(lineContaining(out, "TOTAL"));
    CHECK_EQ(total.size(), 7u);
    if (total.size() == 7) {
        CHECK_EQ(total[1], "TOTAL");
        CHECK_EQ(total[2], "36");
        CHECK_EQ(total[3], "94,800");
        CHECK_EQ(total[5], "Rp2.434.200");
    }

    // Kotak total keseluruhan dan komponen termahal (angka sesi asap).
    CHECK_CONTAINS(out, "Jenis komponen : 2");
    CHECK_CONTAINS(out, "Jumlah unit    : 36");
    CHECK_CONTAINS(out, "Total berat    : 94,800 kg");
    CHECK_CONTAINS(out, "Total biaya    : Rp2.434.200");
    CHECK_CONTAINS(out, "- Shaft : Rp1.612.800 (66,3% dari total biaya)");
    // Hanya satu komponen termahal: tidak ada kalimat tentang kesamaan biaya dan Gear tidak ikut.
    CHECK_NOT_CONTAINS(out, "Ada ");
    CHECK_NOT_CONTAINS(out, "- Gear");
}

static void testRecapFitsEightyColumns() {
    // Tabel rekap selebar 68 kolom, jadi masih muat sebagai tabel di 80 kolom.
    const std::string out = recapText(plainStyle(80), smokeList());
    checkFits(out, 80);
    CHECK_CONTAINS(out, "Berat/unit");
    CHECK_NOT_CONTAINS(out, "1. Gear");
}

static void testRecapNarrowFallsBackToCards() {
    const std::string out = recapText(plainStyle(40), smokeList());
    checkFits(out, 40);
    CHECK_CONTAINS(out, "1. Gear");
    CHECK_CONTAINS(out, "   Jumlah      : 24");
    CHECK_CONTAINS(out, "   Berat/unit  : 1,850 kg");
    CHECK_CONTAINS(out, "   Harga/kg    : Rp18.500");
    CHECK_CONTAINS(out, "   Total berat : 44,400 kg");
    CHECK_CONTAINS(out, "   Total biaya : Rp821.400");
    CHECK_CONTAINS(out, "2. Shaft");
    CHECK_CONTAINS(out, "   Total biaya : Rp1.612.800");
    CHECK_NOT_CONTAINS(out, "Berat/unit  Harga/kg");
    // Kotak total tetap memuat angka pentingnya.
    CHECK_CONTAINS(out, "Total berat    : 94,800 kg");
    CHECK_CONTAINS(out, "Total biaya    : Rp2.434.200");
    CHECK_CONTAINS(out, "Rp1.612.800");
    CHECK_CONTAINS(out, "66,3%");
}

static void testRecapTies() {
    const std::string out = recapText(plainStyle(120), tieList());
    CHECK_CONTAINS(out, "Ada 2 komponen dengan biaya tertinggi yang sama:");
    CHECK_CONTAINS(out, "- Alfa : Rp1.000 (47,6% dari total biaya)");
    CHECK_CONTAINS(out, "- Beta : Rp1.000 (47,6% dari total biaya)");
    CHECK_NOT_CONTAINS(out, "- Gamma");
    // Urutan mengikuti urutan masukan.
    CHECK(out.find("- Alfa") < out.find("- Beta"));
    CHECK_CONTAINS(out, "Total biaya    : Rp2.100");
    CHECK_CONTAINS(out, "Total berat    : 2,100 kg");

    ComponentList three;
    addComponent(three, makeComponent("Satu", 1, 1.0, 100.0));
    addComponent(three, makeComponent("Dua", 1, 1.0, 100.0));
    addComponent(three, makeComponent("Tiga", 1, 1.0, 100.0));
    const std::string all = recapText(plainStyle(120), three);
    CHECK_CONTAINS(all, "Ada 3 komponen dengan biaya tertinggi yang sama:");
    CHECK_CONTAINS(all, "- Satu : Rp100 (33,3% dari total biaya)");
    CHECK_CONTAINS(all, "- Tiga : Rp100 (33,3% dari total biaya)");
}

// Seri dengan nama yang panjangnya berbeda: nama yang pendek ditambah spasi sampai selebar nama
// terpanjang, jadi tanda titik dua kedua baris berada di kolom yang sama. Pin: 100 x 0,5 kg x
// Rp2.000 = Rp100.000; nama 30 karakter: 4 x 5 kg x Rp5.000 = Rp100.000; Baut Rp50.000.
// Total Rp250.000, jadi porsi masing-masing 40,0%.
static void testRecapTieAlignsColons() {
    const std::string longName = "Rumah Bantalan Poros Utama 120";
    CHECK_EQ(longName.size(), static_cast<std::size_t>(30));
    ComponentList list;
    addComponent(list, makeComponent("Pin", 100, 0.5, 2000.0));
    addComponent(list, makeComponent(longName, 4, 5.0, 5000.0));
    addComponent(list, makeComponent("Baut", 100, 0.05, 10000.0));

    const std::string wide = recapText(plainStyle(120), list);
    checkFits(wide, 120);
    CHECK_CONTAINS(wide, "Ada 2 komponen dengan biaya tertinggi yang sama:");
    CHECK_CONTAINS(wide, "- Pin" + std::string(27, ' ') + " : Rp100.000 (40,0% dari total biaya)");
    CHECK_CONTAINS(wide, "- " + longName + " : Rp100.000 (40,0% dari total biaya)");
    CHECK_NOT_CONTAINS(wide, "- Baut");
    const std::vector<std::string> shortLines = linesContaining(wide, "- Pin ");
    const std::vector<std::string> longLines = linesContaining(wide, "- " + longName);
    CHECK_EQ(shortLines.size(), static_cast<std::size_t>(1));
    CHECK_EQ(longLines.size(), static_cast<std::size_t>(1));
    if (shortLines.size() == 1 && longLines.size() == 1) {
        CHECK(shortLines[0].find(" : ") != std::string::npos);
        CHECK_EQ(shortLines[0].find(" : "), longLines[0].find(" : "));
    }

    // Baris yang diratakan selebar 2 + 30 + 3 + 34 = 69 kolom. Isi kotak pada lebar 74 adalah 69 kolom
    // (74 - 5), jadi di sana perataan masih dipakai; pada lebar 73 baris itu tidak muat lagi.
    const std::string justFits = recapText(plainStyle(74), list);
    checkFits(justFits, 74);
    CHECK_CONTAINS(justFits, "- Pin" + std::string(27, ' ') + " : Rp100.000 (40,0% dari total biaya)");
    CHECK_CONTAINS(justFits, "- " + longName + " : Rp100.000 (40,0% dari total biaya)");

    // Bila baris yang diratakan tidak muat di kotak, perataan dibatalkan: nama pendek ditulis apa
    // adanya, jadi barisnya tidak ikut terlipat hanya karena ada satu nama yang panjang.
    const std::string tooNarrow = recapText(plainStyle(73), list);
    checkFits(tooNarrow, 73);
    CHECK_CONTAINS(tooNarrow, "- Pin : Rp100.000 (40,0% dari total biaya)");
    CHECK_NOT_CONTAINS(tooNarrow, "- Pin  ");
    CHECK_CONTAINS(tooNarrow, "- " + longName + " : Rp100.000 (40,0% dari");

    const std::string narrow = recapText(plainStyle(40), list);
    checkFits(narrow, 40);
    CHECK_CONTAINS(narrow, "- Pin : Rp100.000 (40,0% dari");
    CHECK_NOT_CONTAINS(narrow, "- Pin  ");
    CHECK_EQ(linesContaining(narrow, "- " + longName + " :").size(), static_cast<std::size_t>(1));
    CHECK_CONTAINS(narrow, "Rp100.000 (40,0% dari total biaya)");
}

static void testRecapZeroTotal() {
    // 1 gram x Rp1/kg dibulatkan menjadi Rp0, jadi total biaya 0 dan porsi tidak boleh dibagi nol.
    ComponentList list;
    addComponent(list, makeComponent("Nol1", 1, 0.001, 1.0));
    addComponent(list, makeComponent("Nol2", 1, 0.001, 1.0));
    const std::string out = recapText(plainStyle(120), list);
    CHECK_CONTAINS(out, "Total biaya    : Rp0");
    CHECK_CONTAINS(out, "Total berat    : 0,002 kg");
    CHECK_CONTAINS(out, "Ada 2 komponen dengan biaya tertinggi yang sama:");
    CHECK_CONTAINS(out, "- Nol1 : Rp0 (0,0% dari total biaya)");
}

static void testRecapEmptyList() {
    const ComponentList empty;
    CHECK_EQ(recapText(plainStyle(120), empty), std::string("[i] ") + EMPTY_LIST_MESSAGE + "\n");

    const std::string narrow = recapText(plainStyle(40), empty);
    CHECK_CONTAINS(narrow, "[i] Belum ada komponen.");
    CHECK_NOT_CONTAINS(narrow, "TOTAL");
    CHECK_NOT_CONTAINS(narrow, "+-");
    CHECK_AT_MOST(widestLine(narrow), 39u);
}

static void testRecapFullList() {
    // Seratus komponen: tabel panjang, tetap rapi dan semua baris tampil.
    ComponentList list;
    for (int i = 1; i <= 100; ++i) {
        std::ostringstream name;
        name << "Komponen " << i;
        addComponent(list, makeComponent(name.str(), i, 1.0, 1000.0));
    }
    const std::string out = recapText(plainStyle(120), list);
    checkFits(out, 120);
    CHECK_CONTAINS(out, "Komponen 1 ");
    CHECK_CONTAINS(out, "Komponen 100");
    CHECK_CONTAINS(out, "Jenis komponen : 100");
    // Jumlah unit 1 + 2 + ... + 100 = 5.050; berat dan biaya Rp1.000 per kg.
    CHECK_CONTAINS(out, "Jumlah unit    : 5.050");
    CHECK_CONTAINS(out, "Total berat    : 5.050,000 kg");
    CHECK_CONTAINS(out, "Total biaya    : Rp5.050.000");
    CHECK_CONTAINS(out, "- Komponen 100 : Rp100.000 (2,0% dari total biaya)");
}

// ----- printRanking -----

static void testRankingWithTies() {
    const std::string out = rankingText(plainStyle(120), rankingList());
    checkLayout(out, 120);
    CHECK_CONTAINS(out, "Peringkat Biaya Produksi");
    CHECK_CONTAINS(out, "Porsi = bagian biaya komponen dari total biaya.");

    const std::vector<std::string> beta = splitWords(lineContaining(out, "Beta"));
    const std::vector<std::string> gamma = splitWords(lineContaining(out, "Gamma"));
    const std::vector<std::string> alfa = splitWords(lineContaining(out, "Alfa"));
    const std::vector<std::string> delta = splitWords(lineContaining(out, "Delta"));
    CHECK_EQ(beta.size(), 7u);
    CHECK_EQ(gamma.size(), 7u);
    CHECK_EQ(alfa.size(), 7u);
    CHECK_EQ(delta.size(), 7u);
    if (beta.size() != 7 || gamma.size() != 7 || alfa.size() != 7 || delta.size() != 7) {
        return;
    }
    // Peringkat 1, 1, 3, 4: yang biayanya sama berbagi peringkat dan peringkat berikutnya melompat.
    CHECK_EQ(beta[1], "1");
    CHECK_EQ(gamma[1], "1");
    CHECK_EQ(alfa[1], "3");
    CHECK_EQ(delta[1], "4");
    CHECK_EQ(beta[3], "Rp900");
    CHECK_EQ(gamma[3], "Rp900");
    CHECK_EQ(alfa[3], "Rp500");
    CHECK_EQ(delta[3], "Rp100");
    // Porsi dari total Rp2.400: 37,5%, 37,5%, 20,8%, dan 4,2%.
    CHECK_EQ(beta[4], "37,5%");
    CHECK_EQ(gamma[4], "37,5%");
    CHECK_EQ(alfa[4], "20,8%");
    CHECK_EQ(delta[4], "4,2%");
    // Batang selebar 24 kotak untuk biaya tertinggi; 500/900 x 24 = 13,3 dan 100/900 x 24 = 2,7
    // dibulatkan menjadi 13 dan 3.
    CHECK_EQ(beta[5], std::string(24, '#'));
    CHECK_EQ(gamma[5], std::string(24, '#'));
    CHECK_EQ(alfa[5], std::string(13, '#'));
    CHECK_EQ(delta[5], std::string(3, '#'));
    // Biaya yang sama tetap berurutan seperti saat dimasukkan; lalu menurun.
    CHECK(out.find("Beta") < out.find("Gamma"));
    CHECK(out.find("Gamma") < out.find("Alfa"));
    CHECK(out.find("Alfa") < out.find("Delta"));
}

static void testRankingNarrowCards() {
    const std::string out = rankingText(plainStyle(40), rankingList());
    checkLayout(out, 40);
    CHECK_CONTAINS(out, "1. Beta");
    CHECK_CONTAINS(out, "1. Gamma");
    CHECK_CONTAINS(out, "3. Alfa");
    CHECK_CONTAINS(out, "4. Delta");
    CHECK_CONTAINS(out, "   Total biaya : Rp900");
    CHECK_CONTAINS(out, "   Porsi       : 37,5%");
    CHECK_CONTAINS(out, "Porsi = bagian biaya komponen");
    // Terminal sempit memakai batang 12 kotak: 12, 12, 7 (500/900 x 12 = 6,7), dan 1 (100/900 x 12 = 1,3).
    const std::vector<std::string> bars = linesContaining(out, "Grafik  ");
    CHECK_EQ(bars.size(), 4u);
    if (bars.size() == 4) {
        CHECK_EQ(splitWords(bars[0])[3], std::string(12, '#'));
        CHECK_EQ(splitWords(bars[1])[3], std::string(12, '#'));
        CHECK_EQ(splitWords(bars[2])[3], std::string(7, '#'));
        CHECK_EQ(splitWords(bars[3])[3], std::string(1, '#'));
    }
}

static void testRankingSmallCostKeepsOneBarCell() {
    // 1 / 1.000.000 x 24 jauh di bawah setengah kotak, tetapi biaya yang tidak nol selalu diberi satu kotak.
    ComponentList list;
    addComponent(list, makeComponent("Tiny", 1, 1.0, 1.0));
    addComponent(list, makeComponent("Big", 1, 1.0, 1000000.0));
    const std::string out = rankingText(plainStyle(120), list);
    const std::vector<std::string> big = splitWords(lineContaining(out, "Big"));
    const std::vector<std::string> tiny = splitWords(lineContaining(out, "Tiny"));
    CHECK_EQ(big.size(), 7u);
    CHECK_EQ(tiny.size(), 7u);
    if (big.size() == 7 && tiny.size() == 7) {
        CHECK_EQ(big[1], "1");
        CHECK_EQ(big[3], "Rp1.000.000");
        CHECK_EQ(big[4], "100,0%");
        CHECK_EQ(big[5], std::string(24, '#'));
        CHECK_EQ(tiny[1], "2");
        CHECK_EQ(tiny[3], "Rp1");
        CHECK_EQ(tiny[4], "0,0%");
        CHECK_EQ(tiny[5], "#");
    }
}

static void testRankingZeroCost() {
    // Biaya 0 tidak mendapat batang, dan bila semuanya 0 tidak ada pembagian dengan nol.
    ComponentList list;
    addComponent(list, makeComponent("Nol1", 1, 0.001, 1.0));
    addComponent(list, makeComponent("Nol2", 1, 0.001, 1.0));
    const std::string out = rankingText(plainStyle(120), list);
    checkLayout(out, 120);
    const std::vector<std::string> first = splitWords(lineContaining(out, "Nol1"));
    const std::vector<std::string> second = splitWords(lineContaining(out, "Nol2"));
    CHECK_EQ(first.size(), 6u);
    CHECK_EQ(second.size(), 6u);
    if (first.size() == 6 && second.size() == 6) {
        CHECK_EQ(first[1], "1");
        CHECK_EQ(second[1], "1");
        CHECK_EQ(first[3], "Rp0");
        CHECK_EQ(first[4], "0,0%");
        CHECK_EQ(second[4], "0,0%");
    }
    CHECK_NOT_CONTAINS(out, "#");
}

static void testRankingEmptyList() {
    const ComponentList empty;
    CHECK_EQ(rankingText(plainStyle(120), empty), std::string("[i] ") + EMPTY_LIST_MESSAGE + "\n");
    CHECK_CONTAINS(rankingText(plainStyle(40), empty), "[i] Belum ada komponen.");
}

static void testRankingUnicodeBar() {
    OutputStyle style = plainStyle(120);
    style.useUnicode = true;
    const std::string out = rankingText(style, rankingList());
    CHECK_CONTAINS(out, repeatText("\xE2\x96\x88", 24));
    CHECK_NOT_CONTAINS(out, "#");
}

// ----- printCatalog -----

static std::string catalogText(const OutputStyle& style) {
    std::ostringstream out;
    printCatalog(out, style);
    return out.str();
}

static void testCatalogWideTable() {
    const std::string out = catalogText(plainStyle(120));
    checkLayout(out, 120);
    CHECK_CONTAINS(out, "Harga Acuan Material");
    CHECK_CONTAINS(out, "Material");
    CHECK_CONTAINS(out, "Harga/kg");
    CHECK_CONTAINS(out, "Standar");
    CHECK_CONTAINS(out, "Kegunaan umum");
    CHECK_CONTAINS(out, std::string("Periode data: ") + CATALOG_PERIOD + ".");
    // Setiap material tampil satu baris dengan harga, standar, dan kegunaannya.
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        const MaterialReference& row = MATERIAL_CATALOG[i];
        CHECK(hasLineWith(out, row.name, formatRupiah(row.pricePerKg)));
        CHECK(hasLineWith(out, row.name, row.standard));
        CHECK(hasLineWith(out, row.name, row.typicalUse));
    }
}

static void testCatalogNarrowCards() {
    const std::string out = catalogText(plainStyle(40));
    checkLayout(out, 40);
    CHECK_CONTAINS(out, "Harga Acuan Material");
    CHECK_CONTAINS(out, "Periode data: ");
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        const MaterialReference& row = MATERIAL_CATALOG[i];
        std::ostringstream title;
        title << (i + 1) << ". " << row.name;
        CHECK_CONTAINS(out, title.str());
        CHECK(hasLineWith(out, "Harga/kg", formatRupiah(row.pricePerKg)));
    }
    CHECK_EQ(linesContaining(out, "Harga/kg").size(), static_cast<std::size_t>(MATERIAL_COUNT));
}

// ----- printComponentSummary -----

static std::string summaryText(const OutputStyle& style, const Component& component) {
    std::ostringstream out;
    printComponentSummary(out, style, component);
    return out.str();
}

static void testComponentSummaryExactText() {
    const Component gear = makeComponent("Gear", 24, 1.85, 18500.0);
    const std::size_t inner = 23;
    const std::string expected =
        "+- Ringkasan Komponen ----+\n"
        + boxRow("Nama        : Gear", inner) + "\n"
        + boxRow("Jumlah      : 24 unit", inner) + "\n"
        + boxRow("Berat/unit  : 1,850 kg", inner) + "\n"
        + boxRow("Harga/kg    : Rp18.500", inner) + "\n"
        + boxRow("Total berat : 44,400 kg", inner) + "\n"
        + boxRow("Total biaya : Rp821.400", inner) + "\n"
        + "+" + std::string(25, '-') + "+\n";
    CHECK_EQ(summaryText(plainStyle(120), gear), expected);
    // Selebar 23 kolom isi, kotak ini sama di terminal sempit.
    CHECK_EQ(summaryText(plainStyle(40), gear), expected);
}

static void testComponentSummaryLongNameWraps() {
    const Component longName = makeComponent("Shaft-Heavy-Duty-Gear-Assembly", 12, 4.2, 32000.0);
    const std::string out = summaryText(plainStyle(40), longName);
    checkLayout(out, 40);
    // Nilai yang terlalu panjang dipindah ke bawah labelnya dan dipotong di lebar kotak.
    CHECK_CONTAINS(out, "Nama        :");
    CHECK_CONTAINS(out, "Shaft-Heavy-Duty-Gear");
    CHECK_CONTAINS(out, "-Assembly");
    CHECK_CONTAINS(out, "Total berat : 50,400 kg");
    CHECK_CONTAINS(out, "Total biaya : Rp1.612.800");
}

// ----- printComponentPicker -----

static std::string pickerText(const OutputStyle& style, const ComponentList& list) {
    std::ostringstream out;
    printComponentPicker(out, style, list);
    return out.str();
}

static void testComponentPickerTable() {
    ComponentList list = smokeList();
    // 2 x 12,5 kg x Rp14.000 = 25 kg x Rp14.000 = Rp350.000.
    addComponent(list, makeComponent("Base Plate", 2, 12.5, 14000.0));
    const std::string out = pickerText(plainStyle(80), list);
    checkLayout(out, 80);
    CHECK_CONTAINS(out, "Daftar Komponen");
    CHECK_CONTAINS(out, "Total biaya");
    const std::vector<std::string> gear = splitWords(lineContaining(out, "Gear"));
    const std::vector<std::string> shaft = splitWords(lineContaining(out, "Shaft"));
    const std::vector<std::string> plate = splitWords(lineContaining(out, "Base"));
    CHECK_EQ(gear.size(), 5u);
    CHECK_EQ(shaft.size(), 5u);
    CHECK_EQ(plate.size(), 6u);
    if (gear.size() == 5 && shaft.size() == 5 && plate.size() == 6) {
        CHECK_EQ(gear[1], "1");
        CHECK_EQ(gear[3], "Rp821.400");
        CHECK_EQ(shaft[1], "2");
        CHECK_EQ(shaft[3], "Rp1.612.800");
        CHECK_EQ(plate[1], "3");
        CHECK_EQ(plate[3], "Plate");
        CHECK_EQ(plate[4], "Rp350.000");
    }
    // Nama pendek masih muat sebagai tabel di 40 kolom.
    CHECK_CONTAINS(pickerText(plainStyle(40), list), "No  Komponen");
}

static void testComponentPickerNarrowCards() {
    ComponentList list;
    addComponent(list, makeComponent("Shaft-Heavy-Duty-Gear-Assembly", 12, 4.2, 32000.0));
    const std::string out = pickerText(plainStyle(40), list);
    checkLayout(out, 40);
    CHECK_CONTAINS(out, "1. Shaft-Heavy-Duty-Gear-Assembly");
    CHECK_CONTAINS(out, "   Total biaya : Rp1.612.800");
}

static void testComponentPickerEmptyList() {
    // Daftar kosong hanya menampilkan judul kolom; menu sudah memeriksa daftar kosong lebih dulu.
    const ComponentList empty;
    const std::size_t inner = 25;
    CHECK_EQ(pickerText(plainStyle(80), empty),
             "+- Daftar Komponen " + std::string(9, '-') + "+\n"
             + boxRow("No  Komponen  Total biaya", inner) + "\n"
             + boxRow("--  --------  -----------", inner) + "\n"
             + "+" + std::string(27, '-') + "+\n");
}

// ----- saveReport -----

static void makeDirectory(const std::string& path) {
#ifdef _WIN32
    _mkdir(path.c_str());
#else
    mkdir(path.c_str(), 0777);
#endif
}

// Membuat folder bertingkat ("build/uji-unit") dari yang paling atas; folder yang sudah ada
// dibiarkan. Hasil mkdir tidak diperiksa di sini karena bila folder gagal dibuat, penulisan
// berkas sesudahnya gagal dan uji menandainya.
static void ensureDirectory(const std::string& path) {
    for (std::size_t i = 1; i <= path.size(); ++i) {
        if (i == path.size() || path[i] == '/' || path[i] == '\\') {
            makeDirectory(path.substr(0, i));
        }
    }
}

static std::string readFile(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

static bool fileExists(const std::string& fileName) {
    std::ifstream file(fileName.c_str());
    return file.good();
}

static bool isDigitChar(char character) {
    return character >= '0' && character <= '9';
}

// Tanggal dan jam di kepala laporan berbentuk "dd-mm-yyyy hh:mm".
static bool looksLikeDateTime(const std::string& text) {
    if (text.size() != 16) {
        return false;
    }
    for (std::size_t i = 0; i < text.size(); ++i) {
        const bool separator = i == 2 || i == 5 || i == 10 || i == 13;
        if (separator) {
            const char expected = i == 10 ? ' ' : (i == 13 ? ':' : '-');
            if (text[i] != expected) {
                return false;
            }
        } else if (!isDigitChar(text[i])) {
            return false;
        }
    }
    return true;
}

static void testSaveReportWritesFile(const std::string& outputDirectory) {
    ensureDirectory(outputDirectory);
    const std::string path = outputDirectory + "/laporan-uji.txt";
    std::remove(path.c_str());

    const ComponentList list = smokeList();
    std::string errorMessage;
    CHECK(saveReport(list, path, errorMessage));
    CHECK(fileExists(path));

    const std::string content = readFile(path);
    const std::vector<std::string> lines = splitLines(content);
    CHECK(lines.size() > 5);
    if (lines.size() > 5) {
        CHECK_EQ(lines[0], std::string(APP_NAME) + " " + APP_VERSION + " - " + APP_TAGLINE);
        CHECK_EQ(lines[1].substr(0, 17), "Laporan dibuat : ");
        CHECK(looksLikeDateTime(lines[1].substr(17)));
        CHECK_EQ(lines[2], "");
    }
    // Isi laporan adalah rekap dan peringkat selebar 120 kolom, persis seperti tampilan layar.
    CHECK_CONTAINS(content, recapText(plainStyle(120), list));
    CHECK_CONTAINS(content, rankingText(plainStyle(120), list));
    CHECK_CONTAINS(content, "Total berat    : 94,800 kg");
    CHECK_CONTAINS(content, "Total biaya    : Rp2.434.200");
    CHECK_CONTAINS(content, "Catatan: harga per kg adalah harga yang dipakai saat laporan dibuat (harga acuan atau harga yang diketik sendiri).");
    // Berkas laporan polos: tanpa kode warna dan tanpa garis UTF-8.
    CHECK(isPlainAscii(content));
    CHECK_AT_MOST(widestLine(content), 119u);

    std::remove(path.c_str());
    CHECK(!fileExists(path));
}

static void testSaveReportOverwritesAndHandlesEmptyList(const std::string& outputDirectory) {
    ensureDirectory(outputDirectory);
    const std::string path = outputDirectory + "/laporan-ganti.txt";
    std::string errorMessage;
    CHECK(saveReport(smokeList(), path, errorMessage));
    CHECK_CONTAINS(readFile(path), "Gear");

    // Menyimpan ulang ke berkas yang sama menggantikan isi lama seluruhnya.
    const ComponentList empty;
    CHECK(saveReport(empty, path, errorMessage));
    const std::string content = readFile(path);
    CHECK_NOT_CONTAINS(content, "Gear");
    CHECK_NOT_CONTAINS(content, "Shaft");
    CHECK_CONTAINS(content, "Belum ada komponen.");
    CHECK_CONTAINS(content, APP_NAME);
    CHECK(isPlainAscii(content));
    std::remove(path.c_str());
}

static void testSaveReportFailsCleanly(const std::string& outputDirectory) {
    ensureDirectory(outputDirectory);
    // Folder tujuan tidak ada: penyimpanan gagal dengan pesan, bukan berhenti mendadak.
    const std::string missing = outputDirectory + "/folder-yang-tidak-ada/laporan.txt";
    std::string errorMessage;
    CHECK(!saveReport(smokeList(), missing, errorMessage));
    CHECK(!errorMessage.empty());
    CHECK(!fileExists(missing));

    // Nama kosong juga tidak bisa dibuka.
    std::string emptyNameMessage;
    CHECK(!saveReport(smokeList(), "", emptyNameMessage));
    CHECK(!emptyNameMessage.empty());
}

void runReportTests(const std::string& outputDirectory) {
    testRecapWideTable();
    testRecapFitsEightyColumns();
    testRecapNarrowFallsBackToCards();
    testRecapTies();
    testRecapTieAlignsColons();
    testRecapZeroTotal();
    testRecapEmptyList();
    testRecapFullList();
    testRankingWithTies();
    testRankingNarrowCards();
    testRankingSmallCostKeepsOneBarCell();
    testRankingZeroCost();
    testRankingEmptyList();
    testRankingUnicodeBar();
    testCatalogWideTable();
    testCatalogNarrowCards();
    testComponentSummaryExactText();
    testComponentSummaryLongNameWraps();
    testComponentPickerTable();
    testComponentPickerNarrowCards();
    testComponentPickerEmptyList();
    testSaveReportWritesFile(outputDirectory);
    testSaveReportOverwritesAndHandlesEmptyList(outputDirectory);
    testSaveReportFailsCleanly(outputDirectory);
}
