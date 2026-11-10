// ===== test_calculation.cpp - uji rumus berat dan biaya, penjumlahan, dan peringkat =====
// Semua angka harapan dihitung dengan tangan (dalam gram dan rupiah bulat) dan ditulis apa adanya;
// tidak ada yang dihitung ulang dengan fungsi yang sedang diuji.
#include "estima/platform.h"

#include "estima/calculation.h"

#include <string>

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

// Komponen satu unit seberat 1 kg: biayanya persis sama dengan harga per kg, sehingga urutan
// dan kesamaan biaya pada uji peringkat bisa dibaca langsung dari angka di kode.
static Component costing(const std::string& name, double cost) {
    return makeComponent(name, 1, 1.0, cost);
}

static void testExamQuestionExamples() {
    // Gear: 24 x 1,85 kg = 44,4 kg. Dihitung langsung dalam double hasilnya 44,400000000000006;
    // program harus memberi 44,4 tepat.
    const Component gear = makeComponent("Gear", 24, 1.85, 18500.0);
    CHECK_EQ(calculateTotalWeight(gear), 44.4);
    CHECK_EQ(calculateTotalCost(gear), 821400.0);

    // Shaft: 12 x 4,2 kg = 50,4 kg; 50,4 x Rp32.000 = Rp1.612.800.
    const Component shaft = makeComponent("Shaft", 12, 4.2, 32000.0);
    CHECK_EQ(calculateTotalWeight(shaft), 50.4);
    CHECK_EQ(calculateTotalCost(shaft), 1612800.0);

    // Jumlah kedua komponen sama dengan angka sesi asap: 94,8 kg dan Rp2.434.200.
    Component items[2];
    items[0] = gear;
    items[1] = shaft;
    CHECK_EQ(sumTotalWeight(items, 2), 94.8);
    CHECK_EQ(sumTotalCost(items, 2), 2434200.0);
    CHECK_EQ(sumQuantity(items, 2), 36L);
}

static void testAwkwardDecimals() {
    // 3 x 0,1 kg: 0,1 + 0,1 + 0,1 dalam double adalah 0,30000000000000004.
    CHECK_EQ(calculateTotalWeight(makeComponent("a", 3, 0.1, 1000.0)), 0.3);
    CHECK_EQ(calculateTotalCost(makeComponent("a", 3, 0.1, 1000.0)), 300.0);
    // 3 x 1,005 kg = 3,015 kg.
    CHECK_EQ(calculateTotalWeight(makeComponent("b", 3, 1.005, 1000.0)), 3.015);
    CHECK_EQ(calculateTotalCost(makeComponent("b", 3, 1.005, 1000.0)), 3015.0);
    // 7 x 0,285 kg = 1,995 kg; 1,995 x Rp1.000 = Rp1.995.
    CHECK_EQ(calculateTotalWeight(makeComponent("c", 7, 0.285, 1000.0)), 1.995);
    CHECK_EQ(calculateTotalCost(makeComponent("c", 7, 0.285, 1000.0)), 1995.0);
    // 1 x 4,35 kg x Rp100 = Rp435 (4,35 sendiri tidak tepat di biner).
    CHECK_EQ(calculateTotalCost(makeComponent("d", 1, 4.35, 100.0)), 435.0);
    // 100 x 0,29 kg = 29 kg; 0,29 x 100 langsung dalam double adalah 28,999999999999996.
    CHECK_EQ(calculateTotalWeight(makeComponent("e", 100, 0.29, 1000.0)), 29.0);
    CHECK_EQ(calculateTotalCost(makeComponent("e", 100, 0.29, 1000.0)), 29000.0);
}

static void testGramPrecision() {
    // Satu gram adalah satuan terkecil yang dikenal program.
    const Component oneGram = makeComponent("g", 1, 0.001, 1000.0);
    CHECK_EQ(calculateTotalWeight(oneGram), 0.001);
    CHECK_EQ(calculateTotalCost(oneGram), 1.0);

    // 1000 x 0,001 kg = tepat 1 kg.
    CHECK_EQ(calculateTotalWeight(makeComponent("g", 1000, 0.001, 1000.0)), 1.0);
    CHECK_EQ(calculateTotalCost(makeComponent("g", 1000, 0.001, 1000.0)), 1000.0);

    // 999 x 0,001 kg = 0,999 kg dan 3 x 0,333 kg = 0,999 kg.
    CHECK_EQ(calculateTotalWeight(makeComponent("g", 999, 0.001, 1000.0)), 0.999);
    CHECK_EQ(calculateTotalWeight(makeComponent("g", 3, 0.333, 1000.0)), 0.999);
}

static void testHalfRupiahRoundsUp() {
    // 1 gram x Rp500/kg = Rp0,50 -> Rp1 (setengah ke atas).
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 500.0)), 1.0);
    // 15 gram x Rp100/kg = Rp1,50 -> Rp2.
    CHECK_EQ(calculateTotalCost(makeComponent("r", 3, 0.005, 100.0)), 2.0);
    // 1 gram x Rp499/kg = Rp0,499 -> Rp0 (di bawah setengah dibulatkan turun).
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 499.0)), 0.0);
    // 1 gram x Rp1/kg = Rp0,001 -> Rp0.
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 1.0)), 0.0);
    // 1 gram x Rp1.499/kg = Rp1,499 -> Rp1; 1 gram x Rp1.500/kg = Rp1,50 -> Rp2.
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 1499.0)), 1.0);
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 1500.0)), 2.0);
    // 21 gram x Rp100/kg = Rp2,10 -> Rp2.
    CHECK_EQ(calculateTotalCost(makeComponent("r", 7, 0.003, 100.0)), 2.0);
    // 2 gram x Rp250/kg = Rp0,50 -> Rp1.
    CHECK_EQ(calculateTotalCost(makeComponent("r", 2, 0.001, 250.0)), 1.0);
    // Dua harga berbeda yang sama-sama dibulatkan ke Rp1: Rp1,00 dan Rp1,40.
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 1000.0)), 1.0);
    CHECK_EQ(calculateTotalCost(makeComponent("r", 1, 0.001, 1400.0)), 1.0);
}

static void testLargeValuesStayExact() {
    // Berat terbesar: 1.000.000 unit x 10.000 kg = 10.000.000.000 kg.
    const Component heaviest = makeComponent("max", 1000000, 10000.0, 1.0);
    CHECK_EQ(calculateTotalWeight(heaviest), 10000000000.0);
    CHECK_EQ(calculateTotalCost(heaviest), 10000000000.0);

    // 1.000.000 unit x 1 kg x Rp999.999 = Rp999.999.000.000.
    CHECK_EQ(calculateTotalCost(makeComponent("big", 1000000, 1.0, 999999.0)), 999999000000.0);

    // 1.000.000 unit x 9,999 kg = 9.999.000 kg; x Rp99.999 = Rp999.890.001.000.
    CHECK_EQ(calculateTotalWeight(makeComponent("big", 1000000, 9.999, 99999.0)), 9999000.0);
    CHECK_EQ(calculateTotalCost(makeComponent("big", 1000000, 9.999, 99999.0)), 999890001000.0);

    // Harga terbesar: 1.000.000 unit x 0,001 kg = 1.000 kg; x Rp100.000.000 = Rp100.000.000.000.
    CHECK_EQ(calculateTotalCost(makeComponent("pricey", 1000000, 0.001, 100000000.0)), 100000000000.0);

    // Tepat di batas biaya komponen, Rp999.999.999.999: 999.999 unit x 1 kg x Rp1.000.001.
    const Component limit = makeComponent("limit", 999999, 1.0, 1000001.0);
    CHECK_EQ(calculateTotalWeight(limit), 999999.0);
    CHECK_EQ(calculateTotalCost(limit), MAX_COMPONENT_COST);
    CHECK_EQ(calculateTotalCost(limit), 999999999999.0);
}

static void testSumsAtTheLimit() {
    // Seratus komponen yang masing-masing tepat di batas biaya: 100 x 999.999.999.999 = 99.999.999.999.900.
    ComponentList list;
    for (int i = 0; i < 100; ++i) {
        addComponent(list, makeComponent("limit", 999999, 1.0, 1000001.0));
    }
    CHECK_EQ(sumTotalCost(list.items, list.count), 99999999999900.0);
    CHECK_EQ(sumTotalWeight(list.items, list.count), 99999900.0);
    CHECK_EQ(sumQuantity(list.items, list.count), 99999900L);

    // Seratus komponen dengan jumlah maksimum: 100 x 1.000.000 = 100.000.000 unit.
    ComponentList many;
    for (int i = 0; i < 100; ++i) {
        addComponent(many, makeComponent("max", 1000000, 0.001, 1.0));
    }
    CHECK_EQ(sumQuantity(many.items, many.count), 100000000L);
}

static void testSums() {
    // Sepuluh komponen 0,1 kg: kalau dijumlah dalam double hasilnya 0,9999999999999999.
    ComponentList tenths;
    for (int i = 0; i < 10; ++i) {
        addComponent(tenths, makeComponent("t", 1, 0.1, 1000.0));
    }
    CHECK_EQ(sumTotalWeight(tenths.items, tenths.count), 1.0);
    CHECK_EQ(sumTotalCost(tenths.items, tenths.count), 1000.0);
    CHECK_EQ(sumQuantity(tenths.items, tenths.count), 10L);

    // Biaya dibulatkan per komponen lebih dulu, baru dijumlah: 3 x Rp0,50 menjadi 3 x Rp1 = Rp3
    // (bukan Rp1,50 yang dibulatkan menjadi Rp2).
    ComponentList halves;
    for (int i = 0; i < 3; ++i) {
        addComponent(halves, makeComponent("h", 1, 0.001, 500.0));
    }
    CHECK_EQ(sumTotalCost(halves.items, halves.count), 3.0);
    // Beratnya tetap dijumlah tepat: 3 gram.
    CHECK_EQ(sumTotalWeight(halves.items, halves.count), 0.003);

    // Hanya count elemen pertama yang dijumlahkan.
    Component items[3];
    items[0] = makeComponent("a", 2, 1.0, 100.0);
    items[1] = makeComponent("b", 3, 2.0, 100.0);
    items[2] = makeComponent("c", 4, 3.0, 100.0);
    CHECK_EQ(sumTotalWeight(items, 1), 2.0);
    CHECK_EQ(sumTotalWeight(items, 2), 8.0);
    CHECK_EQ(sumTotalWeight(items, 3), 20.0);
    CHECK_EQ(sumTotalCost(items, 2), 800.0);
    CHECK_EQ(sumTotalCost(items, 3), 2000.0);
    CHECK_EQ(sumQuantity(items, 1), 2L);
    CHECK_EQ(sumQuantity(items, 3), 9L);

    // Daftar kosong menjumlah menjadi nol.
    CHECK_EQ(sumTotalWeight(items, 0), 0.0);
    CHECK_EQ(sumTotalCost(items, 0), 0.0);
    CHECK_EQ(sumQuantity(items, 0), 0L);
    const ComponentList empty;
    CHECK_EQ(sumTotalWeight(empty.items, empty.count), 0.0);
    CHECK_EQ(sumTotalCost(empty.items, empty.count), 0.0);
    CHECK_EQ(sumQuantity(empty.items, empty.count), 0L);
}

// Memanggil findMostExpensive dengan array hasil yang diisi -1 lebih dulu, supaya terlihat bila
// fungsi menulis lebih dari yang dilaporkan. Mengembalikan indeks yang dilaporkan sebagai teks
// "0,2,3"; sentinelAfter diisi dua elemen tepat sesudah yang dilaporkan.
static std::string mostExpensive(const ComponentList& list, int& found, int sentinelAfter[2]) {
    int indexes[MAX_COMPONENTS + 2];
    for (int i = 0; i < MAX_COMPONENTS + 2; ++i) {
        indexes[i] = -1;
    }
    found = findMostExpensive(list.items, list.count, indexes);
    sentinelAfter[0] = indexes[found];
    sentinelAfter[1] = indexes[found + 1];
    return joinNumbers(indexes, found);
}

static void testMostExpensiveSingleMaximum() {
    ComponentList list;
    addComponent(list, costing("a", 100.0));
    addComponent(list, costing("b", 500.0));
    addComponent(list, costing("c", 300.0));
    int found = 0;
    int after[2];
    CHECK_EQ(mostExpensive(list, found, after), "1");
    CHECK_EQ(found, 1);
    // Elemen sesudah yang dilaporkan tidak disentuh.
    CHECK_EQ(after[0], -1);
    CHECK_EQ(after[1], -1);
}

static void testMostExpensiveAtEitherEnd() {
    int found = 0;
    int after[2];
    ComponentList first;
    addComponent(first, costing("a", 900.0));
    addComponent(first, costing("b", 100.0));
    addComponent(first, costing("c", 200.0));
    CHECK_EQ(mostExpensive(first, found, after), "0");
    CHECK_EQ(found, 1);

    ComponentList last;
    addComponent(last, costing("a", 100.0));
    addComponent(last, costing("b", 200.0));
    addComponent(last, costing("c", 900.0));
    CHECK_EQ(mostExpensive(last, found, after), "2");
    CHECK_EQ(found, 1);
}

static void testMostExpensiveSingleItem() {
    ComponentList list;
    addComponent(list, costing("only", 42.0));
    int found = 0;
    int after[2];
    CHECK_EQ(mostExpensive(list, found, after), "0");
    CHECK_EQ(found, 1);
}

static void testMostExpensiveTies() {
    int found = 0;
    int after[2];
    // Tiga komponen berbiaya tertinggi sama: semua dikembalikan sesuai urutan masukan.
    ComponentList list;
    addComponent(list, costing("a", 500.0));
    addComponent(list, costing("b", 100.0));
    addComponent(list, costing("c", 500.0));
    addComponent(list, costing("d", 500.0));
    addComponent(list, costing("e", 200.0));
    CHECK_EQ(mostExpensive(list, found, after), "0,2,3");
    CHECK_EQ(found, 3);
    CHECK_EQ(after[0], -1);

    // Kembar di kedua ujung daftar.
    ComponentList ends;
    addComponent(ends, costing("a", 700.0));
    addComponent(ends, costing("b", 100.0));
    addComponent(ends, costing("c", 700.0));
    CHECK_EQ(mostExpensive(ends, found, after), "0,2");
    CHECK_EQ(found, 2);

    // Kembar yang lebih murah dari maksimum tidak ikut.
    ComponentList cheapTwins;
    addComponent(cheapTwins, costing("a", 100.0));
    addComponent(cheapTwins, costing("b", 100.0));
    addComponent(cheapTwins, costing("c", 900.0));
    CHECK_EQ(mostExpensive(cheapTwins, found, after), "2");
    CHECK_EQ(found, 1);
}

static void testMostExpensiveTieAfterRounding() {
    // Biaya dibandingkan setelah dibulatkan ke rupiah: 1 gram x Rp1.000/kg dan 1 gram x Rp1.400/kg
    // keduanya Rp1, jadi keduanya termahal.
    ComponentList list;
    addComponent(list, makeComponent("a", 1, 0.001, 1000.0));
    addComponent(list, makeComponent("b", 1, 0.001, 1400.0));
    int found = 0;
    int after[2];
    CHECK_EQ(mostExpensive(list, found, after), "0,1");
    CHECK_EQ(found, 2);
}

static void testMostExpensiveAllEqual() {
    ComponentList list;
    addComponent(list, costing("a", 7.0));
    addComponent(list, costing("b", 7.0));
    addComponent(list, costing("c", 7.0));
    int found = 0;
    int after[2];
    CHECK_EQ(mostExpensive(list, found, after), "0,1,2");
    CHECK_EQ(found, 3);

    // Seratus komponen sama biaya: semuanya dikembalikan, indeks 0 sampai 99.
    ComponentList full;
    for (int i = 0; i < 100; ++i) {
        addComponent(full, costing("x", 5.0));
    }
    int indexes[MAX_COMPONENTS];
    CHECK_EQ(findMostExpensive(full.items, full.count, indexes), 100);
    CHECK_EQ(indexes[0], 0);
    CHECK_EQ(indexes[57], 57);
    CHECK_EQ(indexes[99], 99);
}

static void testMostExpensiveEmpty() {
    const ComponentList list;
    int indexes[3] = { -1, -1, -1 };
    CHECK_EQ(findMostExpensive(list.items, list.count, indexes), 0);
    CHECK_EQ(indexes[0], -1);
    CHECK_EQ(indexes[1], -1);
    CHECK_EQ(indexes[2], -1);
}

// Hasil rankByCost sebagai teks "1,2,0" (nomor urut komponen dari peringkat teratas).
static std::string rankOrder(const ComponentList& list) {
    int order[MAX_COMPONENTS];
    rankByCost(list.items, list.count, order);
    return joinNumbers(order, list.count);
}

static void testRankDescending() {
    ComponentList list;
    addComponent(list, costing("a", 100.0));
    addComponent(list, costing("b", 500.0));
    addComponent(list, costing("c", 300.0));
    CHECK_EQ(rankOrder(list), "1,2,0");

    // Data aslinya tidak dipindah.
    CHECK_EQ(joinComponentNames(list), "a,b,c");
    CHECK_EQ(list.items[0].pricePerKg, 100.0);

    // Sudah terurut menurun tetap sama; terurut menaik menjadi terbalik.
    ComponentList descending;
    addComponent(descending, costing("a", 9.0));
    addComponent(descending, costing("b", 5.0));
    addComponent(descending, costing("c", 1.0));
    CHECK_EQ(rankOrder(descending), "0,1,2");

    ComponentList ascending;
    addComponent(ascending, costing("a", 1.0));
    addComponent(ascending, costing("b", 2.0));
    addComponent(ascending, costing("c", 3.0));
    addComponent(ascending, costing("d", 4.0));
    CHECK_EQ(rankOrder(ascending), "3,2,1,0");
}

static void testRankIsStable() {
    // Biaya sama tetap pada urutan pengisiannya.
    ComponentList list;
    addComponent(list, costing("a", 200.0));
    addComponent(list, costing("b", 300.0));
    addComponent(list, costing("c", 200.0));
    addComponent(list, costing("d", 300.0));
    addComponent(list, costing("e", 100.0));
    CHECK_EQ(rankOrder(list), "1,3,0,2,4");

    ComponentList equal;
    addComponent(equal, costing("a", 5.0));
    addComponent(equal, costing("b", 5.0));
    addComponent(equal, costing("c", 5.0));
    CHECK_EQ(rankOrder(equal), "0,1,2");
}

static void testRankEdgeSizes() {
    ComponentList single;
    addComponent(single, costing("a", 5.0));
    CHECK_EQ(rankOrder(single), "0");

    // Daftar kosong: tidak ada yang ditulis ke order[].
    const ComponentList empty;
    int order[2] = { -1, -1 };
    rankByCost(empty.items, empty.count, order);
    CHECK_EQ(order[0], -1);
    CHECK_EQ(order[1], -1);

    // Seratus komponen dengan biaya 1 sampai 100 menjadi urutan 99, 98, ..., 0.
    ComponentList full;
    for (int i = 1; i <= 100; ++i) {
        addComponent(full, costing("x", static_cast<double>(i)));
    }
    int fullOrder[MAX_COMPONENTS];
    rankByCost(full.items, full.count, fullOrder);
    CHECK_EQ(fullOrder[0], 99);
    CHECK_EQ(fullOrder[1], 98);
    CHECK_EQ(fullOrder[50], 49);
    CHECK_EQ(fullOrder[99], 0);

    // Seratus komponen sama biaya: urutan tetap 0, 1, ..., 99.
    ComponentList same;
    for (int i = 0; i < 100; ++i) {
        addComponent(same, costing("x", 5.0));
    }
    int sameOrder[MAX_COMPONENTS];
    rankByCost(same.items, same.count, sameOrder);
    CHECK_EQ(sameOrder[0], 0);
    CHECK_EQ(sameOrder[63], 63);
    CHECK_EQ(sameOrder[99], 99);
}

static void testRankUsesRoundedCost() {
    // Peringkat memakai biaya yang sudah dibulatkan: a = Rp1 dan b = Rp1 (dari Rp1,00 dan Rp1,40)
    // dianggap sama dan tetap berurutan a, b; c = Rp2 berada di depan keduanya.
    ComponentList list;
    addComponent(list, makeComponent("a", 1, 0.001, 1000.0));
    addComponent(list, makeComponent("b", 1, 0.001, 1400.0));
    addComponent(list, makeComponent("c", 1, 0.001, 1500.0));
    CHECK_EQ(rankOrder(list), "2,0,1");
}

static void testCostSharePercent() {
    CHECK_EQ(costSharePercent(50.0, 200.0), 25.0);
    CHECK_EQ(costSharePercent(100.0, 100.0), 100.0);
    CHECK_EQ(costSharePercent(0.0, 100.0), 0.0);
    CHECK_NEAR(costSharePercent(1.0, 3.0), 33.3333333, 0.0001);
    // Contoh sesi asap: 821.400 dan 1.612.800 dari 2.434.200.
    CHECK_NEAR(costSharePercent(821400.0, 2434200.0), 33.7441, 0.001);
    CHECK_NEAR(costSharePercent(1612800.0, 2434200.0), 66.2558, 0.001);
    // Total nol tidak boleh menghasilkan pembagian dengan nol.
    CHECK_EQ(costSharePercent(0.0, 0.0), 0.0);
    CHECK_EQ(costSharePercent(50.0, 0.0), 0.0);
}

void runCalculationTests() {
    testExamQuestionExamples();
    testAwkwardDecimals();
    testGramPrecision();
    testHalfRupiahRoundsUp();
    testLargeValuesStayExact();
    testSumsAtTheLimit();
    testSums();
    testMostExpensiveSingleMaximum();
    testMostExpensiveAtEitherEnd();
    testMostExpensiveSingleItem();
    testMostExpensiveTies();
    testMostExpensiveTieAfterRounding();
    testMostExpensiveAllEqual();
    testMostExpensiveEmpty();
    testRankDescending();
    testRankIsStable();
    testRankEdgeSizes();
    testRankUsesRoundedCost();
    testCostSharePercent();
}
