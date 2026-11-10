// ===== test_component.cpp - uji daftar komponen: tambah, hapus, kosongkan, dan cari =====
#include "estima/platform.h"

#include "estima/component.h"

#include <sstream>
#include <string>

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

// Nama bernomor ("Komponen 1", "Komponen 2", ...) untuk mengisi daftar sampai penuh.
static std::string numberedName(int number) {
    std::ostringstream stream;
    stream << "Komponen " << number;
    return stream.str();
}

// true bila elemen sudah kembali kosong seperti komponen yang baru dibuat.
static bool isBlank(const Component& component) {
    return component.name.empty() && component.quantity == 0
        && component.unitWeightKg == 0.0 && component.pricePerKg == 0.0;
}

// Lima komponen A sampai E dengan jumlah 1 sampai 5. Tiap komponen diberi angka yang berbeda
// supaya terlihat bila penghapusan hanya menggeser sebagian datanya.
static void fillFive(ComponentList& list) {
    addComponent(list, makeComponent("A", 1, 0.1, 100.0));
    addComponent(list, makeComponent("B", 2, 0.2, 200.0));
    addComponent(list, makeComponent("C", 3, 0.3, 300.0));
    addComponent(list, makeComponent("D", 4, 0.4, 400.0));
    addComponent(list, makeComponent("E", 5, 0.5, 500.0));
}

static void fillToCapacity(ComponentList& list) {
    for (int number = 1; number <= 100; ++number) {
        addComponent(list, makeComponent(numberedName(number), number, 1.0, 1000.0));
    }
}

static void testLimitsMatchSpecification() {
    // Batas data dari soal dan dokumentasi ditulis ulang di sini sebagai angka. Bila ada
    // konstanta yang berubah tanpa sengaja, uji inilah yang lebih dulu memberi tahu.
    CHECK_EQ(MAX_COMPONENTS, 100);
    CHECK_EQ(MAX_NAME_LENGTH, 30);
    CHECK_EQ(MIN_QUANTITY, 1);
    CHECK_EQ(MAX_QUANTITY, 1000000);
    CHECK_EQ(MIN_UNIT_WEIGHT_KG, 0.001);
    CHECK_EQ(MAX_UNIT_WEIGHT_KG, 10000.0);
    CHECK_EQ(MIN_PRICE_PER_KG, 1.0);
    CHECK_EQ(MAX_PRICE_PER_KG, 100000000.0);
    CHECK_EQ(MAX_COMPONENT_COST, 999999999999.0);
}

static void testNewObjectsStartEmpty() {
    const Component component;
    CHECK_EQ(component.name, "");
    CHECK_EQ(component.quantity, 0);
    CHECK_EQ(component.unitWeightKg, 0.0);
    CHECK_EQ(component.pricePerKg, 0.0);

    const ComponentList list;
    CHECK_EQ(list.count, 0);
    CHECK(isBlank(list.items[0]));
    CHECK(isBlank(list.items[99]));
}

static void testAddStoresEveryField() {
    ComponentList list;
    CHECK(addComponent(list, makeComponent("Gear", 24, 1.85, 18500.0)));
    CHECK_EQ(list.count, 1);
    CHECK_EQ(list.items[0].name, "Gear");
    CHECK_EQ(list.items[0].quantity, 24);
    CHECK_EQ(list.items[0].unitWeightKg, 1.85);
    CHECK_EQ(list.items[0].pricePerKg, 18500.0);

    // Komponen berikutnya menempati posisi sesudahnya; yang lama tidak tersentuh.
    CHECK(addComponent(list, makeComponent("Shaft", 12, 4.2, 32000.0)));
    CHECK_EQ(list.count, 2);
    CHECK_EQ(joinComponentNames(list), "Gear,Shaft");
    CHECK_EQ(list.items[0].quantity, 24);
    CHECK_EQ(list.items[1].quantity, 12);
}

static void testAddUntilFullThenRefuse() {
    ComponentList list;
    int accepted = 0;
    for (int number = 1; number <= 100; ++number) {
        if (addComponent(list, makeComponent(numberedName(number), number, 1.0, 1000.0))) {
            accepted = accepted + 1;
        }
    }
    CHECK_EQ(accepted, 100);
    CHECK_EQ(list.count, 100);
    CHECK_EQ(list.items[0].name, "Komponen 1");
    CHECK_EQ(list.items[99].name, "Komponen 100");

    // Komponen ke-101 ditolak dan isi daftar tidak berubah sedikit pun.
    CHECK(!addComponent(list, makeComponent("Kelebihan", 7, 7.0, 7000.0)));
    CHECK_EQ(list.count, 100);
    CHECK_EQ(list.items[99].name, "Komponen 100");
    CHECK_EQ(list.items[99].quantity, 100);
    CHECK_EQ(findComponentByName(list, "Kelebihan"), -1);
}

static void testRemoveFirst() {
    ComponentList list;
    fillFive(list);
    CHECK(removeComponent(list, 0));
    CHECK_EQ(list.count, 4);
    CHECK_EQ(joinComponentNames(list), "B,C,D,E");
    // Yang bergeser seluruh data komponen, bukan namanya saja.
    CHECK_EQ(list.items[0].quantity, 2);
    CHECK_EQ(list.items[0].unitWeightKg, 0.2);
    CHECK_EQ(list.items[0].pricePerKg, 200.0);
    CHECK_EQ(list.items[3].quantity, 5);
    // Elemen bekas ekor dikosongkan, tidak menyisakan salinan E.
    CHECK(isBlank(list.items[4]));
}

static void testRemoveMiddle() {
    ComponentList list;
    fillFive(list);
    CHECK(removeComponent(list, 2));
    CHECK_EQ(list.count, 4);
    CHECK_EQ(joinComponentNames(list), "A,B,D,E");
    CHECK_EQ(list.items[1].quantity, 2);
    CHECK_EQ(list.items[2].quantity, 4);
    CHECK_EQ(list.items[2].pricePerKg, 400.0);
    CHECK_EQ(list.items[3].quantity, 5);
    CHECK(isBlank(list.items[4]));
}

static void testRemoveLast() {
    ComponentList list;
    fillFive(list);
    CHECK(removeComponent(list, 4));
    CHECK_EQ(list.count, 4);
    CHECK_EQ(joinComponentNames(list), "A,B,C,D");
    CHECK_EQ(list.items[3].quantity, 4);
    CHECK(isBlank(list.items[4]));
}

static void testRemoveRepeatedly() {
    // Menghapus depan, tengah, lalu belakang berturut-turut pada daftar yang sama.
    ComponentList list;
    fillFive(list);
    CHECK(removeComponent(list, 0));
    CHECK(removeComponent(list, 1));
    CHECK(removeComponent(list, 2));
    CHECK_EQ(list.count, 2);
    CHECK_EQ(joinComponentNames(list), "B,D");
    CHECK(isBlank(list.items[2]));
    CHECK(isBlank(list.items[3]));
    CHECK(isBlank(list.items[4]));

    // Menghapus satu-satunya isi yang tersisa membuat daftar benar-benar kosong.
    CHECK(removeComponent(list, 1));
    CHECK(removeComponent(list, 0));
    CHECK_EQ(list.count, 0);
    CHECK(isBlank(list.items[0]));
}

static void testRemoveOutOfRange() {
    ComponentList list;
    fillFive(list);
    CHECK(!removeComponent(list, -1));
    CHECK(!removeComponent(list, 5));
    CHECK(!removeComponent(list, 6));
    CHECK(!removeComponent(list, 100));
    CHECK_EQ(list.count, 5);
    CHECK_EQ(joinComponentNames(list), "A,B,C,D,E");

    ComponentList empty;
    CHECK(!removeComponent(empty, 0));
    CHECK_EQ(empty.count, 0);
}

static void testRemoveFromFullList() {
    ComponentList list;
    fillToCapacity(list);
    CHECK(removeComponent(list, 0));
    CHECK_EQ(list.count, 99);
    CHECK_EQ(list.items[0].name, "Komponen 2");
    CHECK_EQ(list.items[98].name, "Komponen 100");
    CHECK(isBlank(list.items[99]));

    // Tempat yang baru dikosongkan langsung bisa diisi lagi.
    CHECK(addComponent(list, makeComponent("Pengganti", 1, 1.0, 1000.0)));
    CHECK_EQ(list.count, 100);
    CHECK_EQ(list.items[99].name, "Pengganti");
}

static void testClear() {
    ComponentList list;
    fillFive(list);
    clearComponents(list);
    CHECK_EQ(list.count, 0);
    CHECK(isBlank(list.items[0]));
    CHECK(isBlank(list.items[4]));

    // Mengosongkan daftar yang sudah kosong tidak berakibat apa-apa.
    clearComponents(list);
    CHECK_EQ(list.count, 0);

    // Daftar yang sudah dikosongkan bisa diisi lagi dari posisi pertama.
    CHECK(addComponent(list, makeComponent("Baru", 1, 1.0, 1000.0)));
    CHECK_EQ(joinComponentNames(list), "Baru");

    ComponentList full;
    fillToCapacity(full);
    clearComponents(full);
    CHECK_EQ(full.count, 0);
    CHECK(isBlank(full.items[0]));
    CHECK(isBlank(full.items[99]));
}

static void testFindByName() {
    ComponentList list;
    addComponent(list, makeComponent("Gear", 24, 1.85, 18500.0));
    addComponent(list, makeComponent("Base Plate", 2, 12.5, 14000.0));
    addComponent(list, makeComponent("Shaft-12", 12, 4.2, 32000.0));

    CHECK_EQ(findComponentByName(list, "Gear"), 0);
    CHECK_EQ(findComponentByName(list, "Base Plate"), 1);
    CHECK_EQ(findComponentByName(list, "Shaft-12"), 2);

    // Huruf besar dan kecil dianggap sama.
    CHECK_EQ(findComponentByName(list, "gear"), 0);
    CHECK_EQ(findComponentByName(list, "GEAR"), 0);
    CHECK_EQ(findComponentByName(list, "bAsE pLaTe"), 1);
    CHECK_EQ(findComponentByName(list, "SHAFT-12"), 2);

    // Yang dicocokkan nama utuh, bukan sebagian, dan spasi ikut dihitung.
    CHECK_EQ(findComponentByName(list, "Gea"), -1);
    CHECK_EQ(findComponentByName(list, "Gears"), -1);
    CHECK_EQ(findComponentByName(list, "Gear "), -1);
    CHECK_EQ(findComponentByName(list, "Base  Plate"), -1);
    CHECK_EQ(findComponentByName(list, "Pulley"), -1);

    // Elemen di luar isi daftar bernama kosong; pencarian tidak boleh sampai ke sana.
    CHECK_EQ(findComponentByName(list, ""), -1);

    // Sesudah dihapus, nama itu tidak ditemukan lagi dan komponen di belakangnya maju satu.
    removeComponent(list, 0);
    CHECK_EQ(findComponentByName(list, "Gear"), -1);
    CHECK_EQ(findComponentByName(list, "shaft-12"), 1);

    const ComponentList empty;
    CHECK_EQ(findComponentByName(empty, "Gear"), -1);
    CHECK_EQ(findComponentByName(empty, ""), -1);
}

static void testFindReturnsFirstMatch() {
    // Program menolak nama kembar saat pengisian, tetapi fungsi pencarinya sendiri harus tetap
    // pasti: yang dikembalikan selalu posisi paling depan.
    ComponentList list;
    addComponent(list, makeComponent("Pin", 1, 1.0, 1000.0));
    addComponent(list, makeComponent("PIN", 2, 2.0, 2000.0));
    CHECK_EQ(findComponentByName(list, "pin"), 0);
}

void runComponentTests() {
    testLimitsMatchSpecification();
    testNewObjectsStartEmpty();
    testAddStoresEveryField();
    testAddUntilFullThenRefuse();
    testRemoveFirst();
    testRemoveMiddle();
    testRemoveLast();
    testRemoveRepeatedly();
    testRemoveOutOfRange();
    testRemoveFromFullList();
    testClear();
    testFindByName();
    testFindReturnsFirstMatch();
}
