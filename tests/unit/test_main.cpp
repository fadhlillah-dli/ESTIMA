// ===== test_main.cpp - titik masuk biner uji unit =====
// Pemakaian: estima_unit_tests [folder-keluaran]
// Biner ini menjalankan semua suite, mencetak ringkasan satu baris, dan keluar dengan kode 0
// hanya bila tidak ada pemeriksaan yang gagal. Rincian kegagalan dicetak ke std::cerr.
#include "estima/platform.h"

#include <exception>
#include <iostream>
#include <string>

#include "test_framework.h"
#include "test_suites.h"

// Folder bawaan untuk berkas yang ditulis uji laporan, relatif terhadap folder kerja saat biner
// dijalankan. Letaknya di bawah build/ supaya tidak pernah ikut tersimpan di repositori.
static const char* const DEFAULT_OUTPUT_DIRECTORY = "build/uji-unit";

static void runAllSuites(const std::string& outputDirectory) {
    runComponentTests();
    runCalculationTests();
    runMaterialCatalogTests();
    runTextFormatTests();
    runInputParseTests();
    runUiTests();
    runPromptTests();
    runReportTests(outputDirectory);
}

// Pengecualian yang lolos dari sebuah suite dicatat sebagai satu kegagalan. Dengan begitu
// ringkasan tetap tercetak dan kode keluarnya tetap 1, bukan program berhenti mendadak.
static void recordUnexpectedException(const std::string& description) {
    testCounters().failures = testCounters().failures + 1;
    std::cerr << "GAGAL: uji berhenti karena pengecualian yang tidak ditangani: " << description << '\n';
}

int main(int argc, char* argv[]) {
    std::string outputDirectory = DEFAULT_OUTPUT_DIRECTORY;
    if (argc > 1 && argv[1][0] != '\0') {
        outputDirectory = argv[1];
    }

    try {
        runAllSuites(outputDirectory);
    } catch (const std::exception& error) {
        recordUnexpectedException(error.what());
    } catch (...) {
        recordUnexpectedException("jenisnya tidak dikenal");
    }

    const TestCounters& counters = testCounters();
    std::cout << counters.checks << " pemeriksaan, " << counters.failures << " gagal" << std::endl;
    return counters.failures == 0 ? 0 : 1;
}
