# Makefile - jalan pintas build lewat terminal bagi yang tidak memasang CMake.
#
#   make             membangun program: build/estima
#   make run         membangun lalu menjalankan program
#   make single      membangun build/estima-single dari berkas tunggal di amalgamation/
#   make test        uji unit, lalu uji ujung-ke-ujung bila Python 3.8 ke atas tersedia
#   make amalgamate  menyusun ulang berkas tunggal dari include/ dan src/ (butuh Python)
#   make clean       menghapus biner yang dibuat Makefile ini
#
# Setelan bisa diganti dari baris perintah, misalnya:
#   make CXX=clang++ CXXSTD=c++98 CXXFLAGS="-O2 -Wall -Wextra -pedantic -Werror"
# Windows: jalankan mingw32-make dari terminal MSYS2; hasilnya berakhiran .exe.
# Rangkaian uji lengkap (termasuk uji tata letak dan pemeriksaan berkas tunggal) dijalankan
# lewat CMake; lihat CMakeLists.txt.

CXX       ?= g++
CXXSTD    ?= c++17
CXXFLAGS  ?= -Wall -Wextra -pedantic
PYTHON    ?= python3
BUILD_DIR ?= build

# Di Windows nama program berakhiran .exe. Akhiran itu ikut ditulis pada nama target supaya make
# mengenali berkas hasilnya dan tidak mengompilasi ulang tanpa perlu.
ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

# Urutan hasil wildcard berbeda antarversi make; sort membuat baris perintah kompilasi selalu sama.
SOURCES := $(sort $(wildcard src/*.cpp))
HEADERS := $(sort $(wildcard include/estima/*.h))
# Biner uji unit punya main() sendiri, jadi ditautkan dengan semua sumber kecuali main.cpp.
CORE_SOURCES := $(filter-out src/main.cpp,$(SOURCES))
TEST_SOURCES := $(sort $(wildcard tests/unit/*.cpp))
TEST_HEADERS := $(sort $(wildcard tests/unit/*.h))
SINGLE_SOURCE := amalgamation/UTS_2531070011.cpp

APP        := $(BUILD_DIR)/estima$(EXE)
APP_SINGLE := $(BUILD_DIR)/estima-single$(EXE)
UNIT_TESTS := $(BUILD_DIR)/estima-unit-tests$(EXE)

.PHONY: all run single test amalgamate clean

all: $(APP)

$(APP): $(SOURCES) $(HEADERS) | $(BUILD_DIR)
	$(CXX) -std=$(CXXSTD) $(CXXFLAGS) -Iinclude $(SOURCES) -o $@

run: $(APP)
	$(APP)

single: $(APP_SINGLE)

# Tanpa -Iinclude: berkas tunggal harus bisa dikompilasi sendirian, persis seperti saat dinilai.
# Berkas itu tidak disusun ulang otomatis di sini supaya target ini tidak menuntut Python.
$(APP_SINGLE): $(SINGLE_SOURCE) | $(BUILD_DIR)
	$(CXX) -std=$(CXXSTD) $(CXXFLAGS) $(SINGLE_SOURCE) -o $@

$(UNIT_TESTS): $(TEST_SOURCES) $(TEST_HEADERS) $(CORE_SOURCES) $(HEADERS) | $(BUILD_DIR)
	$(CXX) -std=$(CXXSTD) $(CXXFLAGS) -Iinclude $(TEST_SOURCES) $(CORE_SOURCES) -o $@

# Python dicoba dengan benar-benar menjalankannya, bukan hanya dicari namanya: di Windows nama
# python3 bisa berupa pintasan ke Microsoft Store yang ada di PATH tetapi tidak bisa dipakai.
test: $(APP) $(UNIT_TESTS)
	$(UNIT_TESTS)
	@if $(PYTHON) -c "import sys; sys.exit(sys.version_info < (3, 8))" >/dev/null 2>&1; then \
		echo "$(PYTHON) tests/e2e/run_e2e.py --exe $(APP)"; \
		$(PYTHON) tests/e2e/run_e2e.py --exe $(APP); \
	else \
		echo "Uji ujung-ke-ujung dilewati: $(PYTHON) bukan Python 3.8 ke atas yang bisa dijalankan."; \
	fi

amalgamate:
	$(PYTHON) scripts/amalgamate.py

# Hanya biner buatan Makefile ini yang dihapus; folder build milik CMake tidak disentuh.
clean:
	rm -f $(APP) $(APP_SINGLE) $(UNIT_TESTS)

$(BUILD_DIR):
	mkdir -p $@
