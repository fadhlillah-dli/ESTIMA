// ===== test_material_catalog.cpp - uji katalog harga acuan dan data contoh =====
// Harga katalog sengaja tidak ditulis di sini karena angkanya diperbarui dari waktu ke waktu;
// yang diuji adalah bentuk dan kewajaran datanya serta keterkaitan data contoh dengan katalog.
#include "estima/platform.h"

#include "estima/material_catalog.h"

#include <cmath>
#include <cstddef>
#include <cstring>
#include <string>

#include "estima/calculation.h"
#include "estima/input_parse.h"

#include "test_framework.h"
#include "test_suites.h"
#include "test_support.h"

// Daftar pilihan harga di layar menyediakan 19 kolom untuk nama material.
static const std::size_t MAX_MATERIAL_NAME_WIDTH = 19;

static bool isPrintableAscii(const std::string& text) {
    for (std::size_t i = 0; i < text.size(); ++i) {
        const unsigned char byte = static_cast<unsigned char>(text[i]);
        if (byte < 0x20 || byte > 0x7E) {
            return false;
        }
    }
    return true;
}

static void testCatalogIsNotEmpty() {
    CHECK(MATERIAL_COUNT >= 5);
    CHECK(MATERIAL_COUNT <= 99);
    CHECK(CATALOG_PERIOD != NULL);
    CHECK(CATALOG_SOURCE_NOTE != NULL);
    CHECK(std::strlen(CATALOG_PERIOD) > 0);
    CHECK(std::strlen(CATALOG_SOURCE_NOTE) > 0);
    CHECK(isPrintableAscii(CATALOG_PERIOD));
}

static void testEveryRowIsComplete() {
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        const MaterialReference& row = MATERIAL_CATALOG[i];
        CHECK(row.name != NULL && row.standard != NULL && row.typicalUse != NULL);
        if (row.name == NULL || row.standard == NULL || row.typicalUse == NULL) {
            continue;
        }
        CHECK(std::strlen(row.name) > 0);
        CHECK(std::strlen(row.standard) > 0);
        CHECK(std::strlen(row.typicalUse) > 0);
        CHECK(isPrintableAscii(row.name));
        CHECK(isPrintableAscii(row.standard));
        CHECK(isPrintableAscii(row.typicalUse));
        CHECK_AT_MOST(std::strlen(row.name), MAX_MATERIAL_NAME_WIDTH);
    }
}

static void testPricesAreWithinProgramLimits() {
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        const double price = MATERIAL_CATALOG[i].pricePerKg;
        CHECK(price >= MIN_PRICE_PER_KG);
        CHECK(price <= MAX_PRICE_PER_KG);
        // Harga yang dipilih dari katalog harus lolos pemeriksaan yang sama dengan harga ketikan,
        // yaitu rupiah bulat.
        CHECK_EQ(std::floor(price), price);
    }
}

static void testNamesAreUnique() {
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        for (int k = i + 1; k < MATERIAL_COUNT; ++k) {
            CHECK(std::strcmp(MATERIAL_CATALOG[i].name, MATERIAL_CATALOG[k].name) != 0);
        }
    }
}

static void testFindMaterialByName() {
    // Setiap baris ditemukan di posisinya sendiri.
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        CHECK_EQ(findMaterialByName(MATERIAL_CATALOG[i].name), i);
    }
    CHECK_EQ(findMaterialByName("Material Tidak Ada"), -1);
    CHECK_EQ(findMaterialByName(""), -1);

    // Yang dicocokkan nama persis: huruf besar/kecil dibedakan dan potongan nama tidak cukup.
    const std::string first = MATERIAL_CATALOG[0].name;
    const std::string prefixed = std::string("x") + first;
    CHECK_EQ(findMaterialByName(prefixed.c_str()), -1);
    CHECK_EQ(findMaterialByName((first + " ").c_str()), -1);
    CHECK_EQ(findMaterialByName(first.substr(0, first.size() - 1).c_str()), -1);
    std::string otherCase = first;
    for (std::size_t i = 0; i < otherCase.size(); ++i) {
        if (otherCase[i] >= 'A' && otherCase[i] <= 'Z') {
            otherCase[i] = static_cast<char>(otherCase[i] - 'A' + 'a');
        }
    }
    if (otherCase != first) {
        CHECK_EQ(findMaterialByName(otherCase.c_str()), -1);
    }
}

// true bila ada baris katalog dengan harga sama persis seperti price.
static bool catalogHasPrice(double price) {
    for (int i = 0; i < MATERIAL_COUNT; ++i) {
        if (MATERIAL_CATALOG[i].pricePerKg == price) {
            return true;
        }
    }
    return false;
}

static void testSampleComponentsAreValid() {
    ComponentList list;
    CHECK_EQ(loadSampleComponents(list), 5);
    CHECK_EQ(list.count, 5);

    for (int i = 0; i < list.count; ++i) {
        const Component& component = list.items[i];
        // Nama lolos pemeriksaan nama biasa dan unik tanpa membedakan huruf besar/kecil.
        CHECK_EQ(checkComponentName(component.name), "");
        CHECK_EQ(findComponentByName(list, component.name), i);
        CHECK(component.quantity >= MIN_QUANTITY);
        CHECK(component.quantity <= MAX_QUANTITY);
        CHECK(component.unitWeightKg >= MIN_UNIT_WEIGHT_KG);
        CHECK(component.unitWeightKg <= MAX_UNIT_WEIGHT_KG);
        CHECK(component.pricePerKg >= MIN_PRICE_PER_KG);
        CHECK(component.pricePerKg <= MAX_PRICE_PER_KG);
        // Harga contoh berasal dari salah satu baris katalog, bukan angka lepas.
        CHECK(catalogHasPrice(component.pricePerKg));
        // Berat contoh paling teliti sampai gram, dan biayanya masih di bawah batas program.
        CHECK_EQ(std::floor(component.unitWeightKg * 1000.0 + 0.5) / 1000.0, component.unitWeightKg);
        CHECK(calculateTotalCost(component) <= MAX_COMPONENT_COST);
        CHECK(calculateTotalCost(component) >= 1.0);
    }
    // Elemen sesudah isi daftar tetap kosong.
    CHECK_EQ(list.items[5].name, "");
    CHECK_EQ(list.items[5].quantity, 0);
}

static void testSampleLoadReplacesPreviousContent() {
    ComponentList list;
    addComponent(list, makeComponent("Sisa A", 1, 1.0, 1000.0));
    addComponent(list, makeComponent("Sisa B", 2, 2.0, 2000.0));
    CHECK_EQ(loadSampleComponents(list), 5);
    CHECK_EQ(list.count, 5);
    CHECK_EQ(findComponentByName(list, "Sisa A"), -1);
    CHECK_EQ(findComponentByName(list, "Sisa B"), -1);

    // Memuat dua kali memberi lima komponen, bukan sepuluh.
    CHECK_EQ(loadSampleComponents(list), 5);
    CHECK_EQ(list.count, 5);

    // Daftar yang sudah penuh pun diganti seluruhnya.
    ComponentList full;
    for (int i = 0; i < MAX_COMPONENTS; ++i) {
        addComponent(full, makeComponent("Penuh", 1, 1.0, 1000.0));
    }
    CHECK_EQ(loadSampleComponents(full), 5);
    CHECK_EQ(full.count, 5);
    CHECK_EQ(findComponentByName(full, "Penuh"), -1);
    CHECK_EQ(full.items[5].name, "");
    CHECK_EQ(full.items[99].name, "");
}

void runMaterialCatalogTests() {
    testCatalogIsNotEmpty();
    testEveryRowIsComplete();
    testPricesAreWithinProgramLimits();
    testNamesAreUnique();
    testFindMaterialByName();
    testSampleComponentsAreValid();
    testSampleLoadReplacesPreviousContent();
}
