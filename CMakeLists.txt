# CMakeLists.txt - resep build ESTIMA untuk CMake 3.16 ke atas.
#
# Cara pakai dari folder repositori (nama folder build bebas, asal bukan folder sumber):
#   cmake -S . -B build/default
#   cmake --build build/default
#   cd build/default && ctest --output-on-failure
# Dengan CMake 3.21 ke atas, tiga langkah itu bisa disingkat lewat CMakePresets.json:
#   cmake --preset default && cmake --build --preset default && ctest --preset default
# Di Visual Studio dan Xcode, tambahkan --config Release saat build dan -C Release saat ctest.
#
# Opsi, ditulis pada langkah pertama (contoh: -DESTIMA_CXX_STANDARD=98):
#   ESTIMA_CXX_STANDARD        standar C++ yang dipakai: 98, 11, 14, 17, 20, atau 23 (bawaan 17)
#   ESTIMA_WARNINGS_AS_ERRORS  ON membuat peringatan compiler menggagalkan build (bawaan OFF)
#
# Target: estima (program), estima_single (program yang sama, dikompilasi dari berkas tunggal di
# folder amalgamation/), dan estima_unit_tests (uji unit).

cmake_minimum_required(VERSION 3.16)

# Folder build wajib terpisah dari folder sumber. Kalau CMake dijalankan langsung di sini
# (cmake .), berkas buatannya bercampur dengan sumber dan Makefile tulisan tangan milik
# repositori ikut tertimpa. Pemeriksaan ini diletakkan paling awal supaya CMake berhenti sebelum
# menulis berkas build. Yang sempat terbentuk hanya CMakeCache.txt dan folder CMakeFiles (CMake
# selalu membuat keduanya); pesan di bawah meminta keduanya dihapus, dan .gitignore mengabaikannya.
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_CURRENT_BINARY_DIR)
    message(FATAL_ERROR
        "ESTIMA tidak dibangun di dalam folder sumber. Pakai folder build terpisah:\n"
        "  cmake -S . -B build/default\n"
        "Hapus dulu CMakeCache.txt dan folder CMakeFiles yang baru saja terbentuk di folder ini.")
endif()

# Nomor versi hanya ditulis di satu tempat, yaitu APP_VERSION di include/estima/app_info.h.
# CMake membacanya dari sana, bukan mengetiknya ulang, supaya versi proyek tidak pernah berbeda
# dari versi yang dicetak program lewat --version.
set(ESTIMA_APP_INFO_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/include/estima/app_info.h")
file(STRINGS "${ESTIMA_APP_INFO_HEADER}" ESTIMA_VERSION_LINE REGEX "APP_VERSION[ \t]*=")
if(NOT "${ESTIMA_VERSION_LINE}" MATCHES "APP_VERSION[ \t]*=[ \t]*\"([0-9]+\\.[0-9]+\\.[0-9]+)\"")
    message(FATAL_ERROR
        "Nomor versi tidak terbaca dari ${ESTIMA_APP_INFO_HEADER}.\n"
        "Baris yang dicari berbentuk: const char* const APP_VERSION = \"1.0.0\"; (tiga angka dipisah titik).")
endif()
set(ESTIMA_VERSION "${CMAKE_MATCH_1}")
# Bila berkas itu berubah, CMake mengulang konfigurasi dengan sendirinya pada build berikutnya,
# jadi nomor versi yang sudah terbaca tidak tertinggal.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${ESTIMA_APP_INFO_HEADER}")

# Hanya C++ yang dinyalakan, supaya CMake tidak ikut mencari compiler C yang tidak dipakai.
project(estima VERSION "${ESTIMA_VERSION}" LANGUAGES CXX)

# ----- Opsi -----

# Sumber ditulis dengan C++98 supaya bisa dikompilasi compiler lama maupun baru. Opsi ini dipakai
# untuk membuktikannya: sumber yang sama dibangun pada tiap standar tanpa mengubah berkas ini.
set(ESTIMA_CXX_STANDARD "17" CACHE STRING "Standar C++ yang dipakai: 98, 11, 14, 17, 20, atau 23")
set(ESTIMA_SUPPORTED_STANDARDS 98 11 14 17 20 23)
set_property(CACHE ESTIMA_CXX_STANDARD PROPERTY STRINGS ${ESTIMA_SUPPORTED_STANDARDS})
if(NOT ESTIMA_CXX_STANDARD IN_LIST ESTIMA_SUPPORTED_STANDARDS)
    message(FATAL_ERROR
        "ESTIMA_CXX_STANDARD=${ESTIMA_CXX_STANDARD} tidak dikenal. Pilihannya: 98, 11, 14, 17, 20, atau 23.")
endif()
# CMake baru mengenal C++23 sejak versi 3.20. Tanpa pemeriksaan ini, CMake yang lebih lama
# berhenti dengan pesan yang tidak menyebut jalan keluarnya.
if(ESTIMA_CXX_STANDARD EQUAL 23 AND CMAKE_VERSION VERSION_LESS 3.20)
    message(FATAL_ERROR
        "ESTIMA_CXX_STANDARD=23 membutuhkan CMake 3.20 ke atas (yang terpasang ${CMAKE_VERSION}). "
        "Pilih standar lain atau perbarui CMake.")
endif()

option(ESTIMA_WARNINGS_AS_ERRORS "Peringatan compiler menggagalkan build" OFF)

# ----- Jenis build bawaan -----

# Generator satu-konfigurasi (Makefile, Ninja) mengompilasi tanpa optimasi bila jenis build tidak
# disebut. Release dijadikan bawaan karena kebanyakan orang hanya ingin membangun lalu memakai
# programnya. Generator multi-konfigurasi (Visual Studio, Xcode) memilih jenis build saat build,
# jadi tidak perlu diatur di sini.
get_property(ESTIMA_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT ESTIMA_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Jenis build: Debug, Release, RelWithDebInfo, atau MinSizeRel" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# ----- Setelan kompilasi -----

# Tiap keluarga compiler punya ejaan opsinya sendiri, jadi opsi peringatan dipilih menurut
# compiler yang dipakai. Variabel MSVC bernilai benar untuk Visual C++ dan compiler lain yang
# meniru baris perintahnya (clang-cl). Compiler di luar dua keluarga ini tidak diberi opsi apa pun
# daripada diberi opsi yang belum tentu dikenalnya.
if(MSVC)
    # /permissive- menyuruh Visual C++ menolak kode yang menyimpang dari standar, sepadan dengan
    # -pedantic pada GCC dan Clang.
    set(ESTIMA_WARNING_OPTIONS /W4 /permissive-)
    set(ESTIMA_WARNING_AS_ERROR_OPTION /WX)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
    set(ESTIMA_WARNING_OPTIONS -Wall -Wextra -pedantic)
    set(ESTIMA_WARNING_AS_ERROR_OPTION -Werror)
else()
    set(ESTIMA_WARNING_OPTIONS "")
    set(ESTIMA_WARNING_AS_ERROR_OPTION "")
endif()
if(ESTIMA_WARNINGS_AS_ERRORS)
    list(APPEND ESTIMA_WARNING_OPTIONS ${ESTIMA_WARNING_AS_ERROR_OPTION})
endif()

# Memasang standar C++ dan opsi peringatan pada satu target. Setelan dipasang per target, bukan
# lewat variabel global CMAKE_CXX_FLAGS, supaya jelas target mana saja yang memakainya.
# CXX_EXTENSIONS OFF meminta -std=c++17, bukan -std=gnu++17, sehingga ekstensi khusus GNU tidak
# terpakai tanpa sengaja; CXX_STANDARD_REQUIRED ON membuat CMake berhenti bila compiler tidak
# mengenal standar yang diminta, alih-alih diam-diam turun ke standar yang lebih lama.
function(estima_apply_build_settings target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD ${ESTIMA_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)
    target_compile_options(${target} PRIVATE ${ESTIMA_WARNING_OPTIONS})
endfunction()

# ----- Target -----

# Semua sumber selain main.cpp dikompilasi satu kali menjadi pustaka statis yang dipakai bersama
# oleh program dan biner uji unit. Dengan begitu uji memeriksa kode objek yang sama persis dengan
# yang masuk ke program. Daftarnya ditulis satu per satu: berkas baru yang lupa didaftarkan
# langsung ketahuan karena penautan gagal.
add_library(estima_core STATIC
    src/app.cpp
    src/calculation.cpp
    src/component.cpp
    src/input_parse.cpp
    src/material_catalog.cpp
    src/prompt.cpp
    src/report.cpp
    src/terminal.cpp
    src/text_format.cpp
    src/ui.cpp)
# PUBLIC: folder header ikut diwariskan ke setiap target yang menautkan pustaka ini.
target_include_directories(estima_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
estima_apply_build_settings(estima_core)

add_executable(estima src/main.cpp)
target_link_libraries(estima PRIVATE estima_core)
estima_apply_build_settings(estima)

# Berkas tunggal adalah bentuk yang dikumpulkan sebagai jawaban UTS, jadi ia dibangun sendirian:
# tanpa pustaka di atas dan tanpa folder include. Kalau target ini berhasil dibangun, berkas itu
# terbukti bisa dikompilasi tanpa bantuan berkas lain.
set(ESTIMA_SINGLE_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/amalgamation/UTS_2531070011.cpp")
if(EXISTS "${ESTIMA_SINGLE_SOURCE}")
    add_executable(estima_single "${ESTIMA_SINGLE_SOURCE}")
    estima_apply_build_settings(estima_single)
else()
    message(STATUS "ESTIMA: target estima_single dilewati karena amalgamation/UTS_2531070011.cpp tidak ada")
endif()

# Berbeda dari daftar sumber program, berkas uji diambil dari isi folder: berkas uji yang lupa
# didaftarkan tidak menggagalkan apa pun, ujinya hanya diam-diam tidak pernah jalan.
# CONFIGURE_DEPENDS membuat isi folder diperiksa ulang pada setiap build, jadi berkas uji baru
# langsung ikut tanpa menjalankan ulang cmake.
file(GLOB ESTIMA_UNIT_TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/unit/*.cpp")
if(ESTIMA_UNIT_TEST_SOURCES)
    add_executable(estima_unit_tests ${ESTIMA_UNIT_TEST_SOURCES})
    target_link_libraries(estima_unit_tests PRIVATE estima_core)
    estima_apply_build_settings(estima_unit_tests)
else()
    message(STATUS "ESTIMA: target estima_unit_tests dilewati karena tests/unit tidak berisi berkas .cpp")
endif()

# ----- Uji (dijalankan dengan ctest) -----

enable_testing()

# Uji ujung-ke-ujung dan pemeriksaan berkas tunggal ditulis sebagai skrip Python yang hanya
# memakai pustaka standar. Python tidak diwajibkan: tanpa Python program tetap bisa dibangun dan
# uji unit tetap jalan.
find_package(Python3 3.8 COMPONENTS Interpreter QUIET)

# Mendaftarkan satu uji berbasis skrip: estima_add_python_test(<nama> <skrip> [argumen...]).
# Uji yang tidak bisa dijalankan dilewati dengan keterangan, bukan menggagalkan konfigurasi.
# Opsi -B mencegah Python menulis folder __pycache__ ke dalam folder sumber.
function(estima_add_python_test name script)
    if(NOT Python3_Interpreter_FOUND)
        message(STATUS "ESTIMA: uji ${name} dilewati karena Python 3.8 ke atas tidak ditemukan")
    elseif(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${script}")
        message(STATUS "ESTIMA: uji ${name} dilewati karena ${script} tidak ada")
    else()
        add_test(NAME ${name}
            COMMAND "${Python3_EXECUTABLE}" -B "${CMAKE_CURRENT_SOURCE_DIR}/${script}" ${ARGN})
    endif()
endfunction()

# Biner hasil kompilasi silang (misalnya .exe yang dibuat di Linux) tidak bisa dijalankan di
# mesin yang membangunnya, jadi uji yang menjalankan biner hanya didaftarkan pada build biasa.
if(CMAKE_CROSSCOMPILING)
    message(STATUS "ESTIMA: kompilasi silang, uji yang menjalankan biner tidak didaftarkan")
else()
    if(TARGET estima_unit_tests)
        # Uji laporan menulis berkas sementara; foldernya disebut eksplisit di dalam folder build
        # supaya tidak bergantung pada folder kerja tempat ctest dijalankan.
        add_test(NAME unit COMMAND estima_unit_tests "${CMAKE_CURRENT_BINARY_DIR}/uji-unit")
    endif()
    estima_add_python_test(e2e tests/e2e/run_e2e.py --exe $<TARGET_FILE:estima>)
    # Skenario yang sama dijalankan pada biner dari berkas tunggal, karena berkas itulah yang
    # dikumpulkan dan dikompilasi penilai.
    if(TARGET estima_single)
        estima_add_python_test(e2e_single tests/e2e/run_e2e.py --exe $<TARGET_FILE:estima_single>)
    endif()
    estima_add_python_test(layout tests/e2e/check_layout.py --exe $<TARGET_FILE:estima>)
endif()

# Gagal bila amalgamation/UTS_2531070011.cpp tertinggal dari isi include/ dan src/.
estima_add_python_test(amalgamation_fresh scripts/amalgamate.py --check)

# ----- Pemasangan -----

# cmake --install <folder build> --prefix <tujuan> menyalin program ke <tujuan>/bin.
# GNUInstallDirs menyediakan nama folder baku (CMAKE_INSTALL_BINDIR) untuk tiap sistem.
include(GNUInstallDirs)
install(TARGETS estima RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")

# ----- Ringkasan -----

if(ESTIMA_IS_MULTI_CONFIG)
    set(ESTIMA_BUILD_TYPE_SUMMARY "jenis build dipilih saat build")
else()
    set(ESTIMA_BUILD_TYPE_SUMMARY "${CMAKE_BUILD_TYPE}")
endif()
message(STATUS "ESTIMA ${PROJECT_VERSION}: C++${ESTIMA_CXX_STANDARD}, ${ESTIMA_BUILD_TYPE_SUMMARY}, "
    "peringatan sebagai galat ${ESTIMA_WARNINGS_AS_ERRORS}")
