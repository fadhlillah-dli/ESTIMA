// ===== material_catalog.cpp - harga acuan material dan data contoh =====
// Berkas ini berisi isi katalog harga acuan yang diperkenalkan di include/estima/material_catalog.h, tabel data contoh,
// dan dua fungsi: pencari material berdasarkan nama dan pemuat data contoh ke daftar komponen.
#include "estima/platform.h" // "estima/platform.h" selalu di-#include paling awal agar penyesuaian compiler berlaku sebelum header lain.

#include "estima/material_catalog.h" // "estima/material_catalog.h" berisi struct MaterialReference dan deklarasi extern yang didefinisikan di berkas ini.

// <cstring> adalah header pustaka standar untuk mengolah teks gaya C (deretan char yang diakhiri karakter nol), seperti nilai const char*.
#include <cstring> // <cstring> digunakan untuk std::strcmp, pembanding dua teks gaya C yang dipakai findMaterialByName().

// Harga acuan per kilogram untuk bahan setengah jadi (batang bulat, pelat, atau batang cor) yang
// biasa dibeli bengkel mesin di Indonesia. Tiap angka adalah nilai tengah (median) beberapa harga
// dari daftar harga distributor dan toko daring yang dibuka pada November 2026, lalu dibulatkan;
// sumber, tanggal, dan cara pembulatan tiap angka dicatat di docs/sumber-data.md.
// Harga pasar berubah, jadi angka ini hanya titik awal: pengguna selalu bisa mengetik harga
// sendiri saat mengisi komponen.
// Array struct diisi langsung dengan daftar di antara { }: setiap { ... } di dalamnya adalah satu elemen MaterialReference,
// dan nilainya mengisi anggota struct sesuai urutan deklarasi (name, standard, typicalUse, pricePerKg).
// Ukuran array tidak ditulis di dalam [] karena compiler menghitungnya sendiri dari banyaknya baris (di sini 11).
// Deklarasi extern di material_catalog.h sudah dibaca compiler lebih dulu, jadi definisi const di bawah ini berlaku untuk
// seluruh program; tanpa extern, variabel const di luar fungsi hanya berlaku di unit kompilasi tempat ia ditulis.
const MaterialReference MATERIAL_CATALOG[] = { // MATERIAL_CATALOG[] adalah array konstan berisi 11 material; nomor pilihan di layar = posisi + 1.
    { "Baja SS400", "JIS G3101", "bracket, rangka, base plate", 14000.0 }, // Baja SS400: baja konstruksi umum, Rp14.000 per kg (pilihan nomor 1).
    { "Baja karbon S45C", "JIS G4051", "poros, roda gigi, pin", 19500.0 }, // Baja karbon S45C: baja karbon untuk komponen mesin, Rp19.500 per kg.
    { "Baja paduan SCM440", "JIS G4053", "poros dan gear beban berat", 27000.0 }, // Baja paduan SCM440: baja paduan krom-molibdenum, Rp27.000 per kg.
    { "Stainless SUS304", "JIS G4303", "komponen tahan karat", 53000.0 }, // Stainless SUS304: baja tahan karat yang paling umum, Rp53.000 per kg.
    { "Stainless SUS316L", "JIS G4303", "tahan bahan kimia, air laut", 100000.0 }, // Stainless SUS316L: baja tahan karat untuk lingkungan lebih keras, Rp100.000 per kg.
    { "Besi cor FC250", "JIS G5501", "housing, bed mesin, puli", 38000.0 }, // Besi cor FC250: besi cor kelabu untuk benda cor, Rp38.000 per kg.
    { "Aluminium 6061-T6", "JIS H4040", "bracket ringan, cover, jig", 135000.0 }, // Aluminium 6061-T6: paduan aluminium untuk bagian ringan, Rp135.000 per kg.
    { "Kuningan C3604", "JIS H3250", "bushing, fitting, mur", 250000.0 }, // Kuningan C3604: kuningan yang mudah dibubut, Rp250.000 per kg.
    { "Perunggu BC6", "JIS H5120", "bantalan luncur, bushing", 325000.0 }, // Perunggu BC6: perunggu cor untuk bantalan, Rp325.000 per kg.
    { "Nylon MC (PA6)", "ASTM D5989", "roda gigi ringan, roller", 120000.0 }, // Nylon MC (PA6): plastik teknik nilon cor, Rp120.000 per kg.
    { "POM (polyacetal)", "ASTM D6100", "gear presisi, bagian geser", 100000.0 } // POM (polyacetal): plastik teknik yang licin dan kaku, Rp100.000 per kg;
    // baris terakhir tidak diberi koma di belakangnya.
}; // akhir array MATERIAL_CATALOG

// Banyaknya baris katalog dihitung dari ukuran array, sehingga menambah atau menghapus baris
// di atas tidak menuntut angka lain ikut diubah.
// sizeof berfungsi untuk mengetahui ukuran sebuah variabel atau tipe dalam byte. sizeof(MATERIAL_CATALOG) adalah ukuran seluruh array,
// sedangkan sizeof(MATERIAL_CATALOG[0]) adalah ukuran satu elemennya; hasil baginya sama dengan banyaknya elemen.
// Hasil sizeof bertipe std::size_t (bilangan tanpa tanda), jadi diubah ke int dengan static_cast supaya cocok dengan tipe MATERIAL_COUNT.
const int MATERIAL_COUNT = static_cast<int>(sizeof(MATERIAL_CATALOG) / sizeof(MATERIAL_CATALOG[0])); // MATERIAL_COUNT = ukuran array / ukuran satu elemen,
// yaitu 11 untuk katalog di atas.

// Periode data dan keterangan sumber ditulis sebagai teks tetap (const char* const): penunjuknya tidak bisa diarahkan ke teks lain
// dan hurufnya tidak bisa diubah.
const char* const CATALOG_PERIOD = "November 2026"; // CATALOG_PERIOD = "November 2026" adalah bulan pengambilan data harga katalog.
const char* const CATALOG_SOURCE_NOTE = // CATALOG_SOURCE_NOTE adalah keterangan sumber harga yang dicetak di bawah tabel menu 7.
    // Dua teks dalam tanda kutip yang ditulis berurutan tanpa tanda apa pun di antaranya otomatis digabung compiler menjadi satu teks;
    // cara ini dipakai supaya teks panjang bisa dipecah ke beberapa baris kode.
    "Nilai tengah harga dari daftar harga distributor dan toko daring di Indonesia, dibulatkan; " // bagian pertama keterangan; spasi di ujungnya
    // menjadi pemisah dengan bagian kedua setelah digabung.
    "rinciannya ada di docs/sumber-data.md pada repositori program."; // bagian kedua keterangan, menunjuk berkas yang memuat rincian sumber harga.

// Data contoh: lima komponen mesin yang beratnya dihitung dari ukuran dan massa jenis bahannya
// (lihat docs/sumber-data.md). Bahan dirujuk lewat NAMA di katalog, bukan lewat harga, supaya
// harga contoh selalu mengikuti katalog di atas.
// Struct ini ditulis di berkas .cpp (bukan di header) karena hanya dipakai di berkas ini.
struct SampleComponent { // SampleComponent adalah struct untuk satu baris data contoh.
    const char* name; // name adalah nama komponen contoh, misalnya "Gear"; teks tetap karena tidak pernah berubah.
    int quantity; // quantity adalah jumlah unit komponen contoh.
    double unitWeightKg; // unitWeightKg adalah berat per unit dalam kg, dengan paling banyak tiga angka desimal.
    const char* materialName; // materialName adalah nama bahan yang harus sama persis dengan salah satu name di MATERIAL_CATALOG.
}; // akhir struct SampleComponent

// SAMPLE_COMPONENTS memakai static karena tabel ini hanya dibaca oleh loadSampleComponents() di bawah; berkas lain tidak perlu melihatnya.
static const SampleComponent SAMPLE_COMPONENTS[] = { // SAMPLE_COMPONENTS[] adalah array konstan berisi 5 komponen contoh (nama, jumlah, berat/unit, bahan).
    { "Gear", 24, 2.497, "Baja karbon S45C" }, // Gear: 24 unit, 2,497 kg per unit, bahan Baja karbon S45C.
    { "Shaft", 12, 4.932, "Baja paduan SCM440" }, // Shaft: 12 unit, 4,932 kg per unit, bahan Baja paduan SCM440.
    { "Bracket", 40, 0.965, "Baja SS400" }, // Bracket: 40 unit, 0,965 kg per unit, bahan Baja SS400.
    { "Housing", 8, 6.800, "Besi cor FC250" }, // Housing: 8 unit, 6,800 kg per unit, bahan Besi cor FC250.
    { "Bushing", 30, 0.194, "Perunggu BC6" } // Bushing: 30 unit, 0,194 kg per unit, bahan Perunggu BC6 (baris terakhir, tanpa koma).
}; // akhir array SAMPLE_COMPONENTS

// SAMPLE_COUNT dihitung dengan cara sizeof yang sama seperti MATERIAL_COUNT, jadi ikut berubah bila baris contoh ditambah atau dikurangi.
static const int SAMPLE_COUNT = static_cast<int>(sizeof(SAMPLE_COMPONENTS) / sizeof(SAMPLE_COMPONENTS[0])); // SAMPLE_COUNT adalah banyaknya baris data contoh
// (5); static karena hanya dipakai di berkas ini.

// Pencarian berurutan (linear search) dari baris pertama katalog: barisnya sedikit dan tidak diurutkan menurut nama,
// jadi memeriksa satu per satu sudah cukup cepat dan paling mudah diikuti.
int findMaterialByName(const char* name) { // findMaterialByName(name) berfungsi untuk mencari posisi material bernama name di katalog.
    for (int i = 0; i < MATERIAL_COUNT; ++i) { // for i = 0 sampai MATERIAL_COUNT - 1 artinya setiap baris katalog diperiksa dari depan.
        // std::strcmp(a, b) berfungsi untuk membandingkan dua teks gaya C huruf demi huruf; hasilnya 0 bila keduanya sama persis
        // (huruf besar dan kecil dibedakan). Dipakai karena name dan nama di katalog berupa const char*, dan tanda == pada dua
        // const char* hanya membandingkan alamat memorinya, bukan isi teksnya.
        if (std::strcmp(MATERIAL_CATALOG[i].name, name) == 0) { // std::strcmp(...) == 0 artinya nama di baris ke-i sama persis dengan name.
            return i; // return i; langsung mengembalikan posisi yang ditemukan dan menghentikan pencarian.
        } // akhir if nama cocok
    } // akhir perulangan for
    return -1; // return -1; artinya seluruh katalog sudah diperiksa dan nama itu tidak ada.
} // akhir fungsi findMaterialByName()

// Isi daftar lama dibuang dulu, lalu setiap baris data contoh diubah menjadi Component dengan harga dari katalog.
// Bila daftar sudah berisi, pertanyaan "Ganti dengan data contoh?" diajukan oleh pemanggilnya (menu 6 di app.cpp) sebelum fungsi ini dipanggil.
int loadSampleComponents(ComponentList& list) { // loadSampleComponents(list) berfungsi untuk mengganti isi list dengan data contoh dan mengembalikan banyaknya.
    clearComponents(list); // clearComponents(list) berfungsi untuk mengosongkan daftar lebih dulu, supaya data contoh tidak tercampur data lama.
    for (int i = 0; i < SAMPLE_COUNT; ++i) { // for i = 0 sampai SAMPLE_COUNT - 1 artinya setiap baris data contoh diproses satu per satu.
        const int materialIndex = findMaterialByName(SAMPLE_COMPONENTS[i].materialName); // materialIndex adalah posisi bahan komponen contoh ke-i di katalog,
        // atau -1 bila namanya tidak ditemukan.
        if (materialIndex < 0) { // materialIndex < 0 artinya bahan tidak ada di katalog, sehingga harganya tidak bisa diambil.
            // Hanya terjadi bila nama bahan di tabel contoh salah ketik; uji otomatis menjaganya.
            // continue berfungsi untuk melompati sisa isi perulangan pada putaran ini dan langsung lanjut ke putaran berikutnya.
            continue; // continue; artinya komponen contoh ini dilewati (tidak dimasukkan ke daftar) dan perulangan lanjut ke baris contoh berikutnya.
        } // akhir if bahan tidak ditemukan
        Component component; // component adalah komponen baru; konstruktornya mengisi nilai awal (nama kosong, angka 0) sebelum diisi di bawah.
        component.name = SAMPLE_COMPONENTS[i].name; // component.name diisi nama contoh; teks const char* disalin menjadi std::string.
        component.quantity = SAMPLE_COMPONENTS[i].quantity; // component.quantity diisi jumlah unit dari baris contoh.
        component.unitWeightKg = SAMPLE_COMPONENTS[i].unitWeightKg; // component.unitWeightKg diisi berat per unit dari baris contoh.
        component.pricePerKg = MATERIAL_CATALOG[materialIndex].pricePerKg; // component.pricePerKg diambil dari katalog lewat materialIndex,
        // jadi harga data contoh selalu sama dengan harga acuan yang sedang berlaku.
        addComponent(list, component); // addComponent(list, component) berfungsi untuk menambahkan komponen ke ujung daftar; daftar baru saja
        // dikosongkan dan contohnya hanya 5, jadi batas 100 komponen tidak mungkin terlampaui dan hasil true/false-nya tidak perlu diperiksa.
    } // akhir perulangan for
    return list.count; // return list.count; mengembalikan banyaknya komponen yang benar-benar dimuat (5 bila semua nama bahan cocok).
} // akhir fungsi loadSampleComponents()
