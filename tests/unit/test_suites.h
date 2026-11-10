// ===== test_suites.h - daftar suite uji =====
// Setiap berkas test_<modul>.cpp mengisi satu fungsi di bawah ini, dan test_main.cpp memanggilnya
// satu per satu. Deklarasinya dikumpulkan di satu header supaya compiler ikut memeriksa bahwa
// nama dan parameter di kedua tempat itu sama.
#ifndef ESTIMA_TEST_SUITES_H
#define ESTIMA_TEST_SUITES_H

#include <string>

void runComponentTests();
void runCalculationTests();
void runMaterialCatalogTests();
void runTextFormatTests();
void runInputParseTests();
void runUiTests();
void runPromptTests();

// Uji laporan menulis berkas sungguhan; outputDirectory adalah folder tempat berkas itu dibuat.
void runReportTests(const std::string& outputDirectory);

#endif // ESTIMA_TEST_SUITES_H
