// ===== app.cpp - alur utama program: menu dan aksi tiap pilihannya =====
// Berkas ini adalah "pengatur lalu lintas" program: menampilkan menu, membaca pilihan, lalu menjalankan aksi yang sesuai.
// Rumus hitungan, pemeriksaan ketikan, dan cara menggambar tabel tidak ditulis di sini; semuanya dipanggil dari modul lain
// lewat header-nya, sehingga isi berkas ini bisa dibaca sebagai urutan langkah program dari awal sampai keluar.
#include "estima/platform.h" // platform.h di-include PERTAMA, sebelum header pustaka standar, karena berisi penyesuaian compiler (Visual C++) yang harus berlaku lebih dulu.

#include "estima/app.h" // app.h adalah header pasangan berkas ini: berisi kode keluar EXIT_CODE_... dan deklarasi runApp() yang didefinisikan di bagian bawah berkas ini.

#include <cstddef> // <cstddef> digunakan untuk std::size_t, tipe yang dipakai untuk banyaknya butir menu dan lebar teks.
#include <fstream> // <fstream> digunakan untuk std::ifstream, yang dipakai fileExists() untuk memeriksa apakah berkas laporan sudah ada.
#include <iostream> // <iostream> digunakan untuk std::cout (keluaran layar biasa) dan std::cerr (keluaran pesan galat).
#include <ostream> // <ostream> digunakan untuk tipe std::ostream, yaitu tipe parameter printUsage() supaya bantuan bisa dicetak ke std::cout maupun std::cerr.
#include <string> // <string> digunakan untuk std::string, tipe teks yang panjangnya bisa berubah.
#include <vector> // <vector> digunakan untuk std::vector, yang di sini menampung baris-baris isi kotak (std::vector<StyledLine>).

#include "estima/app_info.h" // app_info.h berisi teks identitas (APP_NAME, APP_VERSION, APP_TAGLINE, dan lain-lain) untuk banner, --help, dan --version.
#include "estima/calculation.h" // calculation.h berisi calculateTotalCost() untuk memeriksa batas biaya, serta sumTotalWeight() dan sumTotalCost() untuk baris keadaan.
#include "estima/component.h" // component.h berisi struct Component, struct ComponentList, batas-batas data, dan addComponent(), removeComponent(), findComponentByName().
#include "estima/input_parse.h" // input_parse.h berisi collapseSpaces(), checkComponentName(), dan checkReportFileName() untuk memeriksa ketikan.
#include "estima/material_catalog.h" // material_catalog.h berisi katalog harga acuan (MATERIAL_CATALOG, MATERIAL_COUNT, CATALOG_PERIOD) dan loadSampleComponents().
#include "estima/prompt.h" // prompt.h berisi fungsi tanya jawab (readLine, readChoice, askYesNo, readQuantity, dan lain-lain) serta struct InputClosed.
#include "estima/report.h" // report.h berisi pencetak rekap, peringkat, katalog, ringkasan, daftar pilihan komponen, saveReport(), dan EMPTY_LIST_MESSAGE.
#include "estima/terminal.h" // terminal.h berisi initTerminal() untuk menyiapkan terminal dan terminalWidth() untuk lebar jendela.
#include "estima/text_format.h" // text_format.h berisi pembentuk teks: formatThousands, formatRupiah, formatWeight, displayWidth, dan padRight.
#include "estima/ui.h" // ui.h berisi OutputStyle, StyledLine, Table, gaya STYLE_..., drawBox(), drawTableBox(), printWrapped(), dan printError/Success/Info().

// Nomor menu diberi nama supaya percabangan switch di bawah terbaca maksudnya
// (case MENU_ADD lebih jelas daripada case 1).
// Nilainya ditulis eksplisit (= 0, = 1, ...) karena angka inilah yang diketik pengguna, jadi harus sama persis dengan nomor
// yang tampil di layar. MENU_EXIT bernilai 0 dan MENU_SAVE 8 (yang terbesar), sehingga rentang pilihan yang sah cukup
// ditulis "MENU_EXIT sampai MENU_SAVE".
enum MenuChoice { // enum MenuChoice adalah daftar nama untuk sembilan nomor menu utama.
    MENU_EXIT = 0, // MENU_EXIT = 0 adalah nomor menu Keluar.
    MENU_ADD = 1, // MENU_ADD = 1 adalah nomor menu Tambah komponen.
    MENU_RECAP = 2, // MENU_RECAP = 2 adalah nomor menu Rekap material & biaya.
    MENU_RANKING = 3, // MENU_RANKING = 3 adalah nomor menu Peringkat biaya.
    MENU_EDIT = 4, // MENU_EDIT = 4 adalah nomor menu Ubah komponen.
    MENU_DELETE = 5, // MENU_DELETE = 5 adalah nomor menu Hapus komponen.
    MENU_SAMPLE = 6, // MENU_SAMPLE = 6 adalah nomor menu Muat data contoh.
    MENU_CATALOG = 7, // MENU_CATALOG = 7 adalah nomor menu Harga acuan material.
    MENU_SAVE = 8 // MENU_SAVE = 8 adalah nomor menu Simpan laporan; tanpa koma karena ini anggota terakhir.
}; // akhir enum MenuChoice

// Satu butir menu: nomor, nama, dan keterangan singkatnya.
// Ketiganya dibungkus struct supaya data milik satu butir selalu berjalan bersama, alasan yang sama dengan struct Component.
// Teksnya bertipe const char* (teks tetap) karena isi menu sudah diketahui saat program ditulis dan tidak pernah berubah.
struct MenuItem { // struct MenuItem adalah record untuk satu baris menu utama.
    int number; // number adalah nomor yang diketik pengguna untuk memilih butir ini; diisi nilai enum MenuChoice, yang otomatis bisa disimpan sebagai int.
    const char* label; // label adalah nama menu yang tampil, misalnya "Tambah komponen".
    const char* description; // description adalah keterangan singkat yang ditulis di samping nama bila terminal cukup lebar.
}; // akhir struct MenuItem

// Daftar menu disimpan sebagai array struct. Menu digambar dengan satu perulangan atas array
// ini, jadi menambah atau mengubah butir menu cukup dilakukan di sini.
// Ukuran di dalam [] sengaja dikosongkan: compiler menghitungnya sendiri dari banyaknya isi, yaitu 9 butir.
// Isi array struct ditulis dengan kurung kurawal bersarang: { } paling luar membungkus seluruh array, dan setiap { } di
// dalamnya mengisi satu MenuItem menurut urutan anggotanya (number, label, description).
// Array ini khusus untuk app.cpp (hanya berkas ini yang memakainya) dan isinya tetap selama program berjalan, karena menu tidak berubah.
// Urutan baris di sini adalah urutan tampil di layar, karena itu Keluar berada paling bawah walaupun nomornya 0.
static const MenuItem MAIN_MENU[] = { // MAIN_MENU[] adalah array berisi 9 butir menu utama.
    { MENU_ADD, "Tambah komponen", "Isi nama, jumlah, berat per unit, dan harga per kg" }, // butir [1]: menambah komponen baru (actionAdd).
    { MENU_RECAP, "Rekap material & biaya", "Total berat dan biaya tiap komponen serta keseluruhan" }, // butir [2]: rekap lengkap (actionRecap).
    { MENU_RANKING, "Peringkat biaya", "Urutan komponen dari biaya terbesar, dengan grafik" }, // butir [3]: peringkat biaya (actionRanking).
    { MENU_EDIT, "Ubah komponen", "Memperbaiki data komponen yang sudah diisi" }, // butir [4]: mengubah komponen (actionEdit).
    { MENU_DELETE, "Hapus komponen", "Membuang satu komponen dari daftar" }, // butir [5]: menghapus komponen (actionDelete).
    { MENU_SAMPLE, "Muat data contoh", "Gear, shaft, bracket, housing, dan bushing siap hitung" }, // butir [6]: memuat data contoh (actionLoadSample).
    { MENU_CATALOG, "Harga acuan material", "Daftar harga per kg beserta periode datanya" }, // butir [7]: katalog harga (actionCatalog).
    { MENU_SAVE, "Simpan laporan", "Menulis rekap dan peringkat ke berkas .txt" }, // butir [8]: menyimpan laporan ke berkas (actionSave).
    { MENU_EXIT, "Keluar", "Menutup program" } // butir [0]: keluar dari program (confirmExit); tanpa koma karena elemen terakhir.
}; // akhir array MAIN_MENU
static const std::size_t MAIN_MENU_COUNT = sizeof(MAIN_MENU) / sizeof(MAIN_MENU[0]); // MAIN_MENU_COUNT adalah banyaknya butir menu (9): ukuran seluruh array
// dibagi ukuran satu elemen. Dihitung begini supaya angkanya otomatis benar bila butir menu ditambah atau dibuang.

// Nama berkas laporan bawaan, dipakai bila pengguna hanya menekan Enter saat ditanya nama berkas (menu 8).
// Dijadikan konstanta karena dipakai di dua tempat: petunjuk "Enter = ..." dan pengganti ketikan kosong.
static const char* const DEFAULT_REPORT_FILE = "laporan-estima.txt"; // DEFAULT_REPORT_FILE adalah penunjuk tetap ke teks tetap "laporan-estima.txt".

// Mencetak teks bantuan: dipakai untuk opsi --help dan sebagai penjelasan saat opsi yang diketik salah.
// Parameternya std::ostream& supaya teks yang sama bisa dikirim ke std::cout (bantuan biasa) maupun ke std::cerr
// (opsi salah) tanpa ditulis dua kali.
static void printUsage(std::ostream& out) { // printUsage() berfungsi untuk menulis cara pemakaian program ke aliran keluaran out.
    // Perintah out << ... di bawah adalah SATU perintah yang dipecah ke beberapa baris supaya mudah dibaca; perintah itu baru berakhir di titik koma.
    out << APP_NAME << ' ' << APP_VERSION << " - " << APP_TAGLINE << "\n\n" // baris judul "ESTIMA 1.0.0 - Estimasi ..."; \n adalah karakter pindah baris,
        // jadi "\n\n" mengakhiri baris judul lalu menyisakan satu baris kosong.
        << "Pemakaian: estima [opsi]\n\n" // cara memanggil program; [opsi] berarti opsi boleh ditulis, boleh juga tidak.
        << "Opsi:\n" // judul daftar opsi.
        << "  -h, --help       Menampilkan bantuan ini\n" // opsi -h atau --help: runApp() mencetak teks ini lalu selesai dengan kode 0.
        << "  -v, --version    Menampilkan versi program\n" // opsi -v atau --version: runApp() hanya mencetak nama dan versi program.
        << "      --no-color   Mematikan warna (sama dengan variabel lingkungan NO_COLOR)\n" // opsi --no-color: diteruskan ke initTerminal() sebagai disableColor.
        << "      --ascii      Menggambar kotak dengan + - | untuk terminal tanpa UTF-8\n\n" // opsi --ascii: diteruskan ke initTerminal() sebagai forceAscii.
        << "Tanpa opsi, program membuka menu interaktif.\n" // perilaku bawaan bila tidak ada opsi sama sekali.
        << "Dokumentasi: " << APP_REPOSITORY << '\n'; // alamat repositori program; '\n' (kutip tunggal) adalah satu karakter pindah baris.
} // akhir fungsi printUsage()

// Banner pembuka: kotak berjudul "ESTIMA <versi>" berisi nama lengkap program, mata kuliah, penulis, dan institusi.
// Dicetak satu kali oleh runMenuLoop() sebelum menu pertama tampil.
static void printBanner() { // printBanner() berfungsi untuk menggambar kotak identitas program di layar.
    std::vector<StyledLine> lines; // lines adalah daftar baris isi kotak; bentuknya std::vector<StyledLine> karena itulah yang diterima drawBox().
    lines.push_back(StyledLine(APP_TAGLINE, STYLE_BOLD)); // baris 1: nama lengkap program, dicetak tebal (STYLE_BOLD).
    lines.push_back(StyledLine(APP_COURSE, STYLE_NORMAL)); // baris 2: mata kuliah dan kelas, gaya biasa.
    lines.push_back(StyledLine(APP_AUTHOR, STYLE_NORMAL)); // baris 3: nama dan NIM penulis.
    lines.push_back(StyledLine(APP_INSTITUTION, STYLE_NORMAL)); // baris 4: program studi dan universitas.
    // Tanda + pada std::string berarti menyambung teks. std::string(APP_NAME) mengubah teks tetap const char* menjadi std::string lebih dulu,
    // karena dua const char* tidak bisa disambung dengan +; begitu sisi kirinya std::string, sambungan berikutnya ikut menjadi std::string.
    drawBox(std::cout, screenStyle(), std::string(APP_NAME) + " " + APP_VERSION, lines); // drawBox() menggambar kotak ke layar dengan judul "ESTIMA 1.0.0".
} // akhir fungsi printBanner()

// Menggambar menu utama di dalam kotak, lalu baris keadaan data dan petunjuk singkat di bawahnya.
// Parameternya const ComponentList& karena daftar hanya dibaca (untuk baris keadaan), tidak diubah, dan tidak perlu disalin.
static void printMenu(const ComponentList& list) { // printMenu() berfungsi untuk menampilkan menu utama beserta ringkasan data saat ini.
    const OutputStyle style = screenStyle(); // style adalah gaya keluaran layar (lebar terminal, warna, UTF-8), diambil sekali untuk seluruh fungsi ini.
    const std::size_t inner = boxInnerLimit(style); // inner adalah lebar isi terbesar (dalam kolom) yang masih muat di dalam kotak pada terminal ini.

    // Lebar kolom nama menu mengikuti nama terpanjang supaya keterangannya lurus.
    std::size_t labelWidth = 0; // labelWidth adalah lebar nama menu terpanjang; mulai 0 lalu diperbesar oleh perulangan pertama.
    std::size_t longestLine = 0; // longestLine adalah lebar baris menu terpanjang bila keterangan ikut ditulis; dihitung oleh perulangan kedua.
    for (std::size_t i = 0; i < MAIN_MENU_COUNT; ++i) { // perulangan pertama: memeriksa setiap butir menu, nomor elemen 0 sampai MAIN_MENU_COUNT - 1.
        if (displayWidth(MAIN_MENU[i].label) > labelWidth) { // bila nama menu ke-i lebih lebar daripada yang terlebar sejauh ini ...
            labelWidth = displayWidth(MAIN_MENU[i].label); // ... lebarnya menjadi rekor baru.
        } // akhir if
    } // akhir perulangan pertama
    for (std::size_t i = 0; i < MAIN_MENU_COUNT; ++i) { // perulangan kedua: menghitung lebar tiap baris menu bila keterangan ikut ditulis.
        const std::size_t lineWidth = 4 + labelWidth + 2 + displayWidth(MAIN_MENU[i].description); // lineWidth = 4 kolom awalan "[n] " + nama selebar labelWidth
        // + 2 spasi pemisah + lebar keterangan. Angka 4 tepat karena semua nomor menu hanya satu digit (0 sampai 8).
        if (lineWidth > longestLine) { // bila baris ini lebih lebar daripada rekor sebelumnya ...
            longestLine = lineWidth; // ... simpan sebagai baris terpanjang.
        } // akhir if
    } // akhir perulangan kedua
    // Bila semua butir muat satu baris, nama dan keterangan ditulis sebaris. Di terminal sempit
    // (misalnya ponsel) hanya nama menu yang ditulis, supaya menu tetap pendek dan tidak ada
    // baris yang terpotong.
    const bool withDescription = longestLine <= inner; // withDescription bernilai true bila baris terpanjang pun muat di kotak (<= karena lebar yang pas sama masih muat).

    std::vector<StyledLine> lines; // lines adalah baris-baris isi kotak menu.
    for (std::size_t i = 0; i < MAIN_MENU_COUNT; ++i) { // menyusun satu baris teks untuk setiap butir menu, sesuai urutan di array MAIN_MENU.
        const std::string number = "[" + formatThousands(MAIN_MENU[i].number) + "] "; // number adalah awalan seperti "[1] "; formatThousands() menerima double,
        // jadi nomor int diubah otomatis menjadi double lalu ditulis tanpa desimal.
        if (withDescription) { // bila terminal cukup lebar ...
            lines.push_back(StyledLine(number + padRight(MAIN_MENU[i].label, labelWidth) + "  " + MAIN_MENU[i].description, // ... baris lengkap: nomor, nama yang
                                       // diratakan kiri selebar labelWidth (padRight menambah spasi di kanan), dua spasi, lalu keterangan ...
                                       STYLE_NORMAL)); // ... dengan gaya biasa.
        } else { // bila terminal sempit ...
            lines.push_back(StyledLine(number + MAIN_MENU[i].label, STYLE_NORMAL)); // ... hanya nomor dan nama menu.
        } // akhir if-else
    } // akhir perulangan penyusun baris menu
    drawBox(std::cout, style, "Menu Utama", lines); // menggambar kotak berjudul "Menu Utama" berisi baris-baris tadi.

    // Baris keadaan: ringkasan data yang sedang dipegang program.
    std::string status = "Data: belum ada komponen."; // status diisi dulu dengan teks untuk daftar kosong; diganti di bawah bila sudah ada data.
    if (list.count > 0) { // bila daftar sudah berisi paling sedikit satu komponen ...
        status = "Data: " + formatThousands(list.count) + " komponen | " // ... status berisi banyaknya komponen,
            + formatWeight(sumTotalWeight(list.items, list.count)) + " | " // total berat seluruh komponen (array list.items dikirim bersama list.count),
            + formatRupiah(sumTotalCost(list.items, list.count)); // dan total biaya seluruh komponen dalam rupiah.
    } // akhir if
    printWrapped(std::cout, style, STYLE_DIM, status, 6); // status dicetak redup (STYLE_DIM); bila terlalu panjang dilipat, dan baris lanjutannya menjorok
    // 6 spasi, selebar awalan "Data: ".
    printWrapped(std::cout, style, STYLE_NORMAL, "Ketik nomor menu lalu tekan Enter.", 0); // petunjuk singkat cara memilih menu.
} // akhir fungsi printMenu()

// Membaca nama komponen sampai sah dan belum dipakai komponen lain. ignoreIndex adalah posisi
// komponen yang sedang diubah (boleh memakai namanya sendiri), atau -1 saat menambah.
// Dipakai oleh dua aksi: tambah (tanpa nilai bawaan) dan ubah (nilai bawaan = nama lama, jadi Enter kosong berarti tidak berubah).
// Nama dianggap sama tanpa membedakan huruf besar/kecil, karena findComponentByName() membandingkan versi huruf kecilnya.
static std::string readComponentName(const ComponentList& list, int ignoreIndex, bool hasDefault, // readComponentName() menghasilkan nama yang sudah sah;
                                     // list hanya dibaca untuk mencari nama kembar, hasDefault menandakan ada nilai bawaan atau tidak ...
                                     const std::string& defaultName) { // ... dan defaultName adalah nama yang dipakai bila pengguna hanya menekan Enter.
    while (true) { // while (true) mengulang pertanyaan terus-menerus; jalan keluarnya hanya return di bawah, yaitu saat nama sudah sah.
        const std::string name = collapseSpaces(readLine("Nama komponen")); // membaca satu baris ketikan, lalu deret spasi di tengahnya dirapatkan menjadi satu spasi.
        if (name.empty() && hasDefault) { // ketikan kosong (hanya Enter) DAN memang ada nilai bawaan ...
            return defaultName; // ... maka nama lama dipertahankan.
        } // akhir if
        const std::string problem = checkComponentName(name); // problem berisi pesan kesalahan dari pemeriksa nama, atau teks kosong bila namanya sah.
        if (!problem.empty()) { // bila problem tidak kosong, artinya nama tidak sah ...
            printError(problem); // ... tampilkan pesannya dengan awalan [!] ...
            continue; // ... lalu continue melompat ke awal perulangan untuk bertanya lagi.
        } // akhir if
        const int existing = findComponentByName(list, name); // existing adalah posisi komponen yang namanya sama, atau -1 bila belum ada.
        if (existing >= 0 && existing != ignoreIndex) { // nama dianggap kembar bila ditemukan (>= 0) DAN bukan milik komponen yang sedang diubah sendiri.
            printError("Nama \"" + name + "\" sudah dipakai komponen no. " + formatThousands(existing + 1) // \" adalah tanda kutip di dalam teks; existing + 1
                       // karena nomor di layar mulai 1, sedangkan posisi di array mulai 0 ...
                       + ". Beri nama lain, misalnya dengan menambah kode ukuran."); // ... dan pesannya ditutup dengan saran nama lain.
            continue; // kembali ke awal perulangan dan bertanya lagi.
        } // akhir if
        return name; // lolos semua pemeriksaan: nama yang sah dikembalikan ke pemanggil.
    } // akhir perulangan while
} // akhir fungsi readComponentName()

// Menampilkan harga acuan lalu membaca harga per kg: pengguna bisa memilih salah satu harga
// acuan (dan masih boleh menggantinya) atau langsung mengetik harga sendiri.
// Hasilnya double karena semua harga di program ini disimpan sebagai double (lihat Component::pricePerKg).
static double readPriceWithReference() { // readPriceWithReference() menghasilkan harga per kg (rupiah) yang sudah sah.
    Table table; // table adalah tabel harga acuan yang akan digambar; konstruktor Table() membuatnya kosong.
    table.columns.push_back(TableColumn("No", true)); // kolom 1 "No", rata kanan (true) karena berisi angka.
    table.columns.push_back(TableColumn("Material", false)); // kolom 2 "Material", rata kiri (false) karena berisi teks.
    table.columns.push_back(TableColumn("Harga/kg", true)); // kolom 3 "Harga/kg", rata kanan karena berisi angka rupiah.
    for (int i = 0; i < MATERIAL_COUNT; ++i) { // satu baris tabel untuk setiap material di katalog; i bertipe int karena MATERIAL_COUNT juga int.
        TableRow row; // row adalah satu baris tabel yang masih kosong.
        row.cells.push_back(formatThousands(i + 1)); // sel nomor: i + 1, karena nomor pilihan di layar mulai 1 sedangkan posisi array mulai 0.
        row.cells.push_back(MATERIAL_CATALOG[i].name); // sel nama material, diambil dari elemen ke-i katalog.
        row.cells.push_back(formatRupiah(MATERIAL_CATALOG[i].pricePerKg)); // sel harga per kg, ditulis sebagai rupiah (misalnya Rp19.500).
        table.rows.push_back(row); // baris yang sudah lengkap dimasukkan ke tabel.
    } // akhir perulangan for
    TableRow manualRow; // manualRow adalah baris tambahan untuk pilihan 0, yaitu mengetik harga sendiri.
    manualRow.cells.push_back("0"); // sel nomor: 0.
    manualRow.cells.push_back("Ketik harga sendiri"); // sel keterangan pilihan 0.
    manualRow.cells.push_back("-"); // tanda - karena pilihan ini tidak punya harga acuan.
    table.rows.push_back(manualRow); // pilihan 0 ditaruh paling bawah, sesudah semua material.

    std::vector<StyledLine> notes; // notes adalah catatan kecil yang dicetak di bawah tabel.
    notes.push_back(StyledLine(std::string("Harga acuan ") + CATALOG_PERIOD + "; keterangannya ada di menu 7.", STYLE_DIM)); // catatan periode harga, dicetak redup.
    std::cout << '\n'; // satu baris kosong sebagai jarak sebelum tabel.
    drawTableBox(std::cout, screenStyle(), "Harga Acuan per kg", table, notes); // menggambar tabel di dalam kotak; di terminal sempit tabel otomatis berubah menjadi "kartu".

    const int choice = readChoice("Pilih harga [0-" + formatThousands(MATERIAL_COUNT) + "]", 0, MATERIAL_COUNT, false, 0); // choice adalah nomor 0 sampai
    // MATERIAL_COUNT; false artinya tanpa nilai bawaan, jadi Enter kosong ditolak dan pertanyaannya diulang.
    if (choice == 0) { // pilihan 0: pengguna ingin mengetik harga sendiri ...
        return readPricePerKg("Harga per kg (Rp)", false, 0.0); // ... harga dibaca tanpa nilai bawaan (0.0 hanya pengisi parameter dan tidak dipakai).
    } // akhir if
    const MaterialReference& material = MATERIAL_CATALOG[choice - 1]; // material adalah referensi (nama lain) untuk baris katalog yang dipilih, jadi tidak
    // disalin; choice - 1 karena nomor di layar mulai 1 sedangkan posisi array mulai 0.
    printInfo("Enter = pakai " + formatRupiah(material.pricePerKg) + " (" + material.name + "), atau ketik harga lain."); // memberi tahu arti Enter kosong.
    return readPricePerKg("Harga per kg (Rp)", true, material.pricePerKg); // harga acuan menjadi nilai bawaan; harga lain yang diketik tetap diperiksa batasnya.
} // akhir fungsi readPriceWithReference()

// true bila biaya komponen masih di dalam batas yang dijamin tepat; bila tidak, pesan dicetak.
// Pemeriksaan ini perlu walaupun setiap ketikan sudah sah, karena gabungan nilai terbesar (1.000.000 unit x 10.000 kg x
// Rp100.000.000) jauh melewati MAX_COMPONENT_COST, batas yang menjaga hitungan tetap tepat (lihat component.h).
static bool costWithinLimit(const Component& component) { // costWithinLimit() memeriksa satu komponen; dikirim sebagai const & supaya tidak disalin.
    if (calculateTotalCost(component) <= MAX_COMPONENT_COST) { // bila total biayanya tidak melebihi batas Rp999.999.999.999 ...
        return true; // ... komponen boleh disimpan.
    } // akhir if
    printError("Total biaya komponen ini melebihi " + formatRupiah(MAX_COMPONENT_COST) // selain itu, pengguna diberi tahu batasnya ...
               + ", batas yang bisa dihitung program. Periksa lagi jumlah, berat, dan harganya."); // ... dan apa yang perlu diperiksa.
    return false; // false: komponen ini ditolak.
} // akhir fungsi costWithinLimit()

// Aksi menu 1 (Tambah komponen): menampilkan aturan pengisian, lalu mengisi komponen satu per satu sampai pengguna
// menjawab tidak pada pertanyaan "Tambah komponen lagi?". Setiap komponen diisi dulu ke variabel draft dan baru masuk
// daftar setelah ringkasannya disetujui. Parameternya ComponentList& tanpa const karena aksi ini memang mengubah daftar.
static void actionAdd(ComponentList& list) { // actionAdd() berfungsi untuk menambah komponen baru ke daftar.
    const OutputStyle style = screenStyle(); // gaya layar diambil sekali dan dipakai untuk semua tampilan di aksi ini.
    std::vector<StyledLine> guide; // guide adalah baris-baris petunjuk pengisian yang ditampilkan di dalam kotak.
    guide.push_back(StyledLine("Nama   : huruf, angka, spasi, dan tanda - _ . , / ( ) + # & (paling banyak " // petunjuk nama: karakter yang boleh dipakai ...
                               + formatThousands(MAX_NAME_LENGTH) + " karakter)", STYLE_NORMAL)); // ... dan panjang maksimal MAX_NAME_LENGTH (30), diambil
                               // dari konstanta supaya angkanya selalu sama dengan aturan pemeriksaannya.
    guide.push_back(StyledLine("Jumlah : bilangan bulat " + formatThousands(MIN_QUANTITY) + " sampai " // petunjuk jumlah: dari MIN_QUANTITY (1) ...
                               + formatThousands(MAX_QUANTITY) + " unit", STYLE_NORMAL)); // ... sampai MAX_QUANTITY (1.000.000) unit.
    guide.push_back(StyledLine("Berat  : kilogram per unit, desimal dengan koma atau titik (contoh 1,85)", STYLE_NORMAL)); // petunjuk berat per unit.
    guide.push_back(StyledLine("Harga  : rupiah bulat per kg, dipilih dari harga acuan atau diketik sendiri", STYLE_NORMAL)); // petunjuk harga per kg.
    std::cout << '\n'; // satu baris kosong sebelum kotak.
    drawBox(std::cout, style, "Tambah Komponen", guide); // menggambar kotak petunjuk berjudul "Tambah Komponen".

    // do ... while berfungsi untuk mengulang isi blok do { } selama syarat di dalam while (...) bernilai benar. Bedanya dengan
    // while biasa: syaratnya baru diperiksa SESUDAH isi blok dijalankan, jadi isi blok pasti dijalankan paling sedikit satu kali.
    // do-while dipakai karena pengisian pasti dilakukan paling sedikit satu kali; pertanyaan
    // "tambah lagi?" baru diajukan setelah satu komponen selesai.
    bool again = false; // again menyimpan jawaban "Tambah komponen lagi?"; diisi di akhir setiap putaran dan diperiksa oleh while.
    do { // awal blok do: satu putaran berarti mengisi satu komponen.
        if (list.count >= MAX_COMPONENTS) { // bila daftar sudah berisi MAX_COMPONENTS (100) komponen, array tidak punya tempat kosong lagi ...
            printError("Daftar sudah penuh (" + formatThousands(MAX_COMPONENTS) // ... pengguna diberi tahu batasnya ...
                       + " komponen). Hapus dulu komponen yang tidak dipakai lewat menu 5."); // ... dan cara mengosongkan tempat.
            break; // break keluar dari perulangan do-while saat itu juga, tanpa bertanya "tambah lagi?".
        } // akhir if
        std::cout << '\n'; // baris kosong sebagai jarak antarkomponen.
        printWrapped(std::cout, style, STYLE_TITLE, "Komponen ke-" + formatThousands(list.count + 1), 0); // judul kecil, misalnya "Komponen ke-3";
        // list.count + 1 adalah nomor yang akan didapat komponen ini bila disimpan.

        Component draft; // draft adalah komponen sementara; konstruktor Component() mengisinya dengan nama kosong dan angka 0.
        draft.name = readComponentName(list, -1, false, ""); // nama: -1 karena tidak ada komponen yang sedang diubah, false karena tidak ada nilai bawaan.
        draft.quantity = readQuantity("Jumlah (unit)", false, 0); // jumlah unit (int), ditanyakan ulang sampai berada di rentang yang sah.
        draft.unitWeightKg = readUnitWeight("Berat per unit (kg)", false, 0.0); // berat per unit dalam kg; double karena boleh desimal, misalnya 1,85.
        draft.pricePerKg = readPriceWithReference(); // harga per kg: dipilih dari tabel harga acuan atau diketik sendiri.

        if (costWithinLimit(draft)) { // hanya bila total biayanya masih di dalam batas, komponen ditawarkan untuk disimpan.
            std::cout << '\n'; // baris kosong sebelum ringkasan.
            printComponentSummary(std::cout, style, draft); // ringkasan: data masukan beserta total berat dan total biaya komponen ini.
            if (askYesNo("Simpan komponen ini?", true)) { // true: jawaban bawaan (hanya Enter) adalah ya, ditandai [Y/t].
                addComponent(list, draft); // draft disalin ke tempat kosong pertama di array; tempatnya pasti ada karena di awal putaran daftar sudah dicek belum penuh.
                printSuccess("Komponen \"" + draft.name + "\" disimpan sebagai no. " + formatThousands(list.count) + "."); // list.count sudah bertambah satu,
                // jadi nilainya sama dengan nomor komponen yang baru disimpan.
            } else { // bila pengguna menjawab tidak ...
                printInfo("Komponen tidak disimpan."); // ... draft dibuang begitu saja dan daftar tidak berubah.
            } // akhir if-else
        } // akhir if costWithinLimit
        again = askYesNo("Tambah komponen lagi?", false); // false: jawaban bawaan (hanya Enter) adalah tidak, ditandai [y/T].
    } while (again); // while (again) diperiksa SESUDAH putaran: bila again true, kembali ke awal do; bila false, perulangan selesai.
} // akhir fungsi actionAdd()

// Aksi menu 2 (Rekap material & biaya): isinya disusun printRecap() di report.cpp, fungsi yang sama dengan yang
// menulis laporan berkas; di sini tujuannya layar (std::cout) dengan gaya terminal.
static void actionRecap(const ComponentList& list) { // actionRecap() hanya membaca daftar, jadi parameternya const ComponentList&.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printRecap(std::cout, screenStyle(), list); // rekap: tabel tiap komponen, total keseluruhan, dan komponen termahal; daftar kosong ditangani printRecap() sendiri.
} // akhir fungsi actionRecap()

// Aksi menu 3 (Peringkat biaya): urutan komponen dari biaya terbesar beserta porsi dan grafik batangnya, disusun printRanking().
static void actionRanking(const ComponentList& list) { // actionRanking() hanya membaca daftar.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printRanking(std::cout, screenStyle(), list); // tabel peringkat biaya; daftar kosong ditangani printRanking() sendiri.
} // akhir fungsi actionRanking()

// Meminta nomor komponen dari daftar. Hasilnya posisi (mulai 0), atau -1 bila dibatalkan.
// Dipakai bersama oleh aksi ubah dan hapus, supaya cara memilih komponennya sama.
static int pickComponent(const ComponentList& list) { // pickComponent() berfungsi untuk menampilkan daftar komponen lalu membaca nomor pilihan.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printComponentPicker(std::cout, screenStyle(), list); // tabel berisi nomor, nama, dan total biaya setiap komponen.
    const int number = readChoice("Nomor komponen [0 = batal]", 0, list.count, false, 0); // number adalah 0 (batal) sampai list.count; nomor lain ditolak readChoice().
    return number - 1; // nomor di layar mulai 1 sedangkan posisi array mulai 0, jadi dikurangi 1; pilihan 0 (batal) otomatis menjadi -1.
} // akhir fungsi pickComponent()

// Aksi menu 4 (Ubah komponen): memilih satu komponen, lalu setiap datanya ditanyakan ulang dengan nilai lama sebagai
// nilai bawaan (Enter = tidak berubah). Parameternya ComponentList& tanpa const karena komponen di daftar bisa ditimpa.
static void actionEdit(ComponentList& list) { // actionEdit() berfungsi untuk memperbaiki data satu komponen yang sudah ada.
    if (list.count == 0) { // bila daftar masih kosong, tidak ada yang bisa diubah ...
        printInfo(EMPTY_LIST_MESSAGE); // ... tampilkan pesan daftar kosong (teksnya dari report.cpp, dipakai bersama) ...
        return; // ... lalu return mengakhiri aksi lebih awal; fungsi void boleh return tanpa nilai.
    } // akhir if
    const int index = pickComponent(list); // index adalah posisi komponen yang dipilih (mulai 0), atau -1 bila batal.
    if (index < 0) { // -1 berarti pengguna memilih 0 (batal) ...
        printInfo("Tidak ada yang diubah."); // ... beri tahu bahwa tidak ada perubahan ...
        return; // ... lalu selesai.
    } // akhir if

    // Perubahan dikerjakan pada salinan (draft). Data asli baru ditimpa setelah pengguna
    // menyetujui ringkasannya, jadi pembatalan di tengah jalan tidak merusak apa pun.
    Component draft = list.items[index]; // draft adalah SALINAN komponen yang dipilih (bukan referensi), jadi mengubah draft belum mengubah isi daftar.
    printInfo("Tekan Enter tanpa mengetik untuk mempertahankan nilai lama."); // petunjuk cara mempertahankan nilai lama.
    printInfo("Nama sekarang: " + draft.name); // nilai lama ditampilkan sebelum pertanyaannya, supaya pengguna tahu apa yang dipertahankan bila menekan Enter.
    draft.name = readComponentName(list, index, true, draft.name); // index dikirim sebagai ignoreIndex supaya nama milik komponen ini sendiri tidak dianggap kembar.
    printInfo("Jumlah sekarang: " + formatThousands(draft.quantity) + " unit"); // jumlah lama.
    draft.quantity = readQuantity("Jumlah (unit)", true, draft.quantity); // true dan draft.quantity: Enter kosong mempertahankan jumlah lama.
    printInfo("Berat per unit sekarang: " + formatWeight(draft.unitWeightKg)); // berat lama, misalnya "1,850 kg".
    draft.unitWeightKg = readUnitWeight("Berat per unit (kg)", true, draft.unitWeightKg); // Enter kosong mempertahankan berat lama.
    printInfo("Harga per kg sekarang: " + formatRupiah(draft.pricePerKg)); // harga lama.
    draft.pricePerKg = readPricePerKg("Harga per kg (Rp)", true, draft.pricePerKg); // harga diketik langsung (tanpa tabel harga acuan); Enter mempertahankan harga lama.

    if (!costWithinLimit(draft)) { // bila biaya hasil perubahan melewati batas, pesannya sudah dicetak costWithinLimit() ...
        return; // ... dan aksi berhenti; isi daftar tetap seperti semula karena yang diubah baru draft.
    } // akhir if
    std::cout << '\n'; // baris kosong sebelum ringkasan.
    printComponentSummary(std::cout, screenStyle(), draft); // ringkasan komponen setelah diubah.
    if (askYesNo("Simpan perubahan?", true)) { // [Y/t]: hanya Enter berarti simpan.
        list.items[index] = draft; // baru di sini data asli di array ditimpa dengan salinan yang sudah diubah.
        printSuccess("Komponen no. " + formatThousands(index + 1) + " diperbarui."); // index + 1 adalah nomor komponen di layar.
    } else { // bila pengguna menjawab tidak ...
        printInfo("Perubahan dibatalkan."); // ... draft dibuang dan data asli tidak tersentuh.
    } // akhir if-else
} // akhir fungsi actionEdit()

// Aksi menu 5 (Hapus komponen): memilih satu komponen lalu menghapusnya setelah dikonfirmasi. Jawaban bawaannya
// tidak ([y/T]), jadi menekan Enter saja tidak akan menghapus apa pun.
static void actionDelete(ComponentList& list) { // actionDelete() berfungsi untuk membuang satu komponen dari daftar; ComponentList& karena daftar diubah.
    if (list.count == 0) { // bila daftar masih kosong, tidak ada yang bisa dihapus ...
        printInfo(EMPTY_LIST_MESSAGE); // ... tampilkan pesan daftar kosong ...
        return; // ... lalu selesai.
    } // akhir if
    const int index = pickComponent(list); // index adalah posisi komponen yang dipilih (mulai 0), atau -1 bila batal.
    if (index < 0) { // pengguna memilih 0 (batal) ...
        printInfo("Tidak ada yang dihapus."); // ... beri tahu ...
        return; // ... lalu selesai.
    } // akhir if
    const std::string name = list.items[index].name; // nama disalin dulu ke variabel name, karena sesudah removeComponent() posisi index sudah berisi komponen
    // lain (atau kosong), sedangkan namanya masih dibutuhkan untuk pesan berhasil.
    printInfo("Komponen yang dipilih: " + name); // menampilkan nama komponen supaya pengguna yakin pilihannya benar.
    if (askYesNo("Hapus komponen ini?", false)) { // [y/T]: hanya Enter berarti tidak jadi menghapus.
        removeComponent(list, index); // menghapus elemen index lalu menggeser elemen sesudahnya satu langkah ke kiri; list.count berkurang satu.
        printSuccess("Komponen \"" + name + "\" dihapus. Sisa " + formatThousands(list.count) + " komponen."); // pesan berhasil beserta sisa isi daftar.
    } else { // bila pengguna menjawab tidak ...
        printInfo("Tidak ada yang dihapus."); // ... daftar dibiarkan.
    } // akhir if-else
} // akhir fungsi actionDelete()

// Aksi menu 6 (Muat data contoh): mengganti isi daftar dengan komponen contoh dari material_catalog.cpp, lalu langsung
// menampilkan rekapnya. Bila daftar sudah berisi, pengguna ditanya dulu karena isinya akan terhapus.
static void actionLoadSample(ComponentList& list) { // actionLoadSample() mengubah daftar, jadi parameternya ComponentList& tanpa const.
    if (list.count > 0) { // bila daftar sudah berisi data ...
        printInfo("Daftar saat ini berisi " + formatThousands(list.count) + " komponen dan akan diganti."); // ... peringatkan bahwa data itu akan diganti ...
        if (!askYesNo("Ganti dengan data contoh?", false)) { // ... dan minta persetujuan; [y/T] berarti Enter saja tidak mengganti apa pun.
            printInfo("Data tidak diubah."); // pengguna menolak: daftar dibiarkan ...
            return; // ... dan aksi selesai.
        } // akhir if
    } // akhir if
    const int loaded = loadSampleComponents(list); // daftar dikosongkan lalu diisi data contoh; loaded adalah banyaknya komponen yang dimuat.
    printSuccess(formatThousands(loaded) + " komponen contoh dimuat dengan harga acuan " + CATALOG_PERIOD + "."); // harga contoh diambil dari katalog,
    // jadi periode harganya disebut.
    std::cout << '\n'; // baris kosong sebelum rekap.
    printRecap(std::cout, screenStyle(), list); // rekap data contoh langsung ditampilkan.
} // akhir fungsi actionLoadSample()

// Aksi menu 7 (Harga acuan material): menampilkan seluruh katalog harga beserta standar, kegunaan, periode, dan sumbernya.
// Tidak butuh parameter daftar karena katalog tidak bergantung pada komponen yang diisi pengguna.
static void actionCatalog() { // actionCatalog() berfungsi untuk menampilkan tabel harga acuan material.
    std::cout << '\n'; // baris kosong sebagai jarak.
    printCatalog(std::cout, screenStyle()); // tabel katalog dicetak ke layar.
} // akhir fungsi actionCatalog()

// true bila berkas dengan nama itu sudah ada (bisa dibuka untuk dibaca).
// Caranya dengan mencoba membuka berkas itu untuk dibaca; berkas yang ada tetapi tidak boleh dibaca juga menghasilkan false.
static bool fileExists(const std::string& fileName) { // fileExists() dipakai actionSave() sebelum menimpa berkas.
    std::ifstream probe(fileName.c_str()); // probe adalah aliran berkas masukan yang langsung mencoba membuka berkas itu. c_str() memberikan isi std::string
    // sebagai teks gaya C (const char*), karena pada C++98 std::ifstream hanya bisa dibuka dengan nama berkas berbentuk itu.
    return probe.good(); // good() bernilai true bila berkas berhasil dibuka; berkasnya otomatis ditutup lagi saat probe hilang di akhir fungsi.
} // akhir fungsi fileExists()

// Aksi menu 8 (Simpan laporan): menanyakan nama berkas (Enter = laporan-estima.txt), meminta izin bila berkas itu sudah
// ada, lalu menulis rekap dan peringkat ke berkas teks lewat saveReport().
static void actionSave(const ComponentList& list) { // actionSave() hanya membaca daftar, jadi parameternya const ComponentList&.
    if (list.count == 0) { // bila daftar kosong, tidak ada yang bisa dilaporkan ...
        printInfo(EMPTY_LIST_MESSAGE); // ... tampilkan pesan daftar kosong ...
        return; // ... lalu selesai.
    } // akhir if
    printInfo("Laporan ditulis sebagai berkas teks di folder tempat program dijalankan."); // nama berkas tidak boleh memuat folder, jadi berkas
    // selalu dibuat di folder kerja saat program dijalankan.
    printInfo(std::string("Enter = ") + DEFAULT_REPORT_FILE); // memberi tahu nama berkas bawaan; std::string(...) supaya + bisa menyambung teks.

    std::string fileName; // fileName akan diisi checkReportFileName() dengan nama berkas yang sah dan sudah berakhiran .txt.
    while (true) { // bertanya terus sampai nama berkasnya sah; jalan keluarnya break di bawah.
        const std::string text = readLine("Nama berkas"); // text adalah ketikan pengguna, sudah dirapikan spasi di awal dan akhirnya.
        const std::string problem = checkReportFileName(text.empty() ? DEFAULT_REPORT_FILE : text, fileName); // ketikan kosong diganti nama bawaan
        // lewat operator ?:, lalu diperiksa; problem kosong berarti nama sah dan fileName sudah terisi.
        if (problem.empty()) { // bila nama berkas sah ...
            break; // ... break keluar dari while (true).
        } // akhir if
        printError(problem); // nama tidak sah: tampilkan sebabnya, lalu perulangan bertanya lagi.
    } // akhir perulangan while
    if (fileExists(fileName)) { // bila berkas dengan nama itu sudah ada ...
        printInfo("Berkas " + fileName + " sudah ada."); // ... beri tahu pengguna ...
        if (!askYesNo("Timpa berkas itu?", false)) { // ... dan minta izin; [y/T] berarti Enter saja tidak menimpa.
            printInfo("Laporan tidak disimpan."); // pengguna menolak: berkas lama dibiarkan ...
            return; // ... dan aksi selesai.
        } // akhir if
    } // akhir if

    std::string errorMessage; // errorMessage diisi saveReport() dengan sebab kegagalan bila penulisan gagal.
    if (saveReport(list, fileName, errorMessage)) { // saveReport() menghasilkan true bila berkas berhasil ditulis seluruhnya.
        printSuccess("Laporan disimpan ke " + fileName + "."); // pesan berhasil beserta nama berkasnya.
    } else { // bila gagal ...
        printError(errorMessage); // ... tampilkan sebabnya, misalnya folder tidak boleh ditulisi.
    } // akhir if-else
} // akhir fungsi actionSave()

// true bila pengguna memang ingin keluar. Bila masih ada data, ditanyakan dulu karena data
// hanya hidup selama program berjalan.
static bool confirmExit(const ComponentList& list) { // confirmExit() dipanggil saat pengguna memilih menu 0 (Keluar).
    if (list.count == 0) { // daftar kosong: tidak ada data yang bisa hilang ...
        return true; // ... jadi langsung boleh keluar tanpa bertanya.
    } // akhir if
    printInfo("Data komponen hanya tersimpan selama program berjalan. Simpan laporan lewat menu 8 bila perlu."); // pengingat sebelum keluar.
    return askYesNo("Keluar dari program?", false); // [y/T]: hanya Enter berarti tidak jadi keluar.
} // akhir fungsi confirmExit()

// [Kriteria soal: perulangan dan percabangan] while (running) mengulang menu dan switch (choice) memilih aksinya. Perulangan
// lain juga ada di banyak bagian, misalnya for di calculation.cpp (menjumlahkan semua komponen), while (true) di prompt.cpp
// (bertanya ulang sampai jawabannya sah), dan do-while di actionAdd() (menambah komponen berikutnya).
// Perulangan menu utama: gambar menu, baca pilihan, jalankan aksinya, lalu tunggu Enter. Diulang sampai pengguna
// memastikan ingin keluar.
static void runMenuLoop() { // runMenuLoop() berfungsi untuk melayani menu utama dari awal sampai pengguna keluar.
    // Seluruh data program hidup di variabel lokal ini dan dikirim ke tiap aksi lewat
    // parameter. Tidak ada variabel global untuk data, jadi jelas fungsi mana yang boleh
    // mengubahnya (ComponentList&) dan mana yang hanya membaca (const ComponentList&).
    ComponentList list; // list adalah satu-satunya tempat data komponen; konstruktor ComponentList() membuatnya kosong (count = 0).
    printBanner(); // banner identitas dicetak sekali, sebelum menu pertama.

    bool running = true; // running adalah penanda perulangan menu; menjadi false hanya bila pengguna memastikan keluar.
    while (running) { // selama running masih true, menu ditampilkan lagi.
        std::cout << '\n'; // baris kosong sebelum menu.
        printMenu(list); // menu utama beserta baris keadaan data.
        const int choice = readChoice("Pilihan Anda [" + formatThousands(MENU_EXIT) + "-" + formatThousands(MENU_SAVE) + "]", // label "Pilihan Anda [0-8]",
                                      // disusun dari konstanta enum supaya ikut berubah bila nomor menu berubah ...
                                      MENU_EXIT, MENU_SAVE, false, 0); // ... rentang sah MENU_EXIT (0) sampai MENU_SAVE (8), tanpa nilai bawaan.

        // switch berfungsi untuk memilih satu dari banyak cabang berdasarkan nilai sebuah bilangan bulat. Setiap "case NILAI:" adalah
        // titik masuk untuk nilai itu, dan break mengakhiri switch; tanpa break, program akan terus menjalankan perintah milik case
        // di bawahnya. "default:" dijalankan bila nilainya tidak cocok dengan case mana pun.
        // switch memilih satu aksi berdasarkan nomor menu. Dipakai, bukan if-else bertingkat,
        // karena yang diperiksa satu nilai bulat dengan banyak kemungkinan yang sudah pasti.
        switch (choice) { // switch (choice) memilih cabang sesuai nomor menu yang diketik.
            case MENU_ADD: // case MENU_ADD: pilihan 1, Tambah komponen.
                actionAdd(list); // list dikirim sebagai referensi, jadi komponen baru langsung masuk ke daftar milik fungsi ini.
                break; // break: selesai, keluar dari switch.
            case MENU_RECAP: // pilihan 2, Rekap material & biaya.
                actionRecap(list); // menampilkan rekap.
                break; // keluar dari switch.
            case MENU_RANKING: // pilihan 3, Peringkat biaya.
                actionRanking(list); // menampilkan peringkat biaya.
                break; // keluar dari switch.
            case MENU_EDIT: // pilihan 4, Ubah komponen.
                actionEdit(list); // mengubah satu komponen.
                break; // keluar dari switch.
            case MENU_DELETE: // pilihan 5, Hapus komponen.
                actionDelete(list); // menghapus satu komponen.
                break; // keluar dari switch.
            case MENU_SAMPLE: // pilihan 6, Muat data contoh.
                actionLoadSample(list); // mengganti isi daftar dengan data contoh.
                break; // keluar dari switch.
            case MENU_CATALOG: // pilihan 7, Harga acuan material.
                actionCatalog(); // menampilkan katalog; tanpa list karena tidak memakai data komponen.
                break; // keluar dari switch.
            case MENU_SAVE: // pilihan 8, Simpan laporan.
                actionSave(list); // menyimpan laporan ke berkas teks.
                break; // keluar dari switch.
            case MENU_EXIT: // pilihan 0, Keluar.
                running = !confirmExit(list); // bila confirmExit() true (jadi keluar), tanda ! membaliknya sehingga running false dan perulangan berhenti.
                break; // keluar dari switch.
            default: // default: nilai lain; karena readChoice() hanya menerima 0 sampai 8, cabang ini tidak pernah tercapai.
                break; // tidak melakukan apa-apa.
        } // akhir switch
        if (running && choice != MENU_EXIT) { // menunggu Enter hanya sesudah aksi biasa; sesudah memilih Keluar lalu batal, menu langsung tampil lagi.
            waitForEnter(); // menahan hasil aksi di layar sampai pengguna menekan Enter.
        } // akhir if
    } // akhir perulangan while (running)
    printSuccess("Program selesai. Terima kasih."); // pesan penutup; hanya tercapai bila keluar lewat menu Keluar.
} // akhir fungsi runMenuLoop()

// Definisi runApp() yang dideklarasikan di app.h dan dipanggil dari main(). Langkahnya: (1) membaca opsi baris perintah,
// (2) menyiapkan terminal, (3) menjalankan perulangan menu di dalam try, supaya habisnya masukan ditangani di satu tempat.
// Fungsi ini tidak static karena dipanggil oleh main() di main.cpp, yang pada versi modul adalah unit kompilasi lain.
int runApp(int argc, char* argv[]) { // argc adalah banyaknya kata pada baris perintah (termasuk nama program), argv adalah array teks kata-kata itu.
    bool disableColor = false; // disableColor menjadi true bila ada opsi --no-color.
    bool forceAscii = false; // forceAscii menjadi true bila ada opsi --ascii.
    for (int i = 1; i < argc; ++i) { // i mulai 1, bukan 0, karena argv[0] berisi nama program itu sendiri, bukan opsi.
        const std::string option = argv[i]; // teks gaya C argv[i] disalin ke std::string supaya bisa dibandingkan dengan == terhadap teks seperti "-h".
        if (option == "-h" || option == "--help") { // opsi bantuan, bentuk pendek atau panjang ...
            printUsage(std::cout); // ... cetak bantuan ke keluaran biasa ...
            return EXIT_CODE_OK; // ... lalu selesai dengan kode 0 tanpa membuka menu.
        } else if (option == "-v" || option == "--version") { // else if hanya diperiksa bila syarat sebelumnya salah; ini opsi versi ...
            std::cout << APP_NAME << ' ' << APP_VERSION << '\n'; // ... cetak "ESTIMA 1.0.0" ...
            return EXIT_CODE_OK; // ... lalu selesai dengan kode 0.
        } else if (option == "--no-color") { // opsi tanpa warna ...
            disableColor = true; // ... dicatat dulu; baru dipakai initTerminal() di bawah, sesudah semua opsi terbaca.
        } else if (option == "--ascii") { // opsi garis ASCII ...
            forceAscii = true; // ... dicatat, nanti dipakai initTerminal().
        } else { // selain empat opsi di atas berarti opsi tidak dikenal ...
            std::cerr << "Opsi tidak dikenal: " << option << "\n\n"; // ... pesannya ditulis ke std::cerr (saluran galat) supaya terpisah dari keluaran biasa ...
            printUsage(std::cerr); // ... disusul cara pemakaian yang benar, juga ke std::cerr ...
            return EXIT_CODE_BAD_USAGE; // ... lalu program berhenti dengan kode 2.
        } // akhir rangkaian if-else if
    } // akhir perulangan for pembaca opsi

    initTerminal(disableColor, forceAscii); // menyiapkan terminal satu kali: memutuskan warna, garis UTF-8 atau ASCII, dan jenis masukan.
    try { // blok try dijalankan sambil berjaga: bila di dalamnya (sedalam apa pun) ada throw InputClosed(), program melompat ke catch di bawah.
        runMenuLoop(); // seluruh pemakaian program berlangsung di sini sampai pengguna memilih Keluar.
    } catch (const InputClosed&) { // menangkap InputClosed yang dilempar readLine() atau waitForEnter() di prompt.cpp ketika masukan habis.
        // Masukan habis sebelum pengguna memilih Keluar. Program berhenti dengan rapi dan
        // memberi tahu sebabnya lewat saluran galat (std::cerr).
        std::cout << '\n'; // pindah baris dulu, karena teks pertanyaan terakhir sudah tercetak tanpa pindah baris dan belum sempat dijawab.
        std::cout.flush(); // flush() memaksa isi std::cout yang masih tertahan di penampung memori langsung dikirim, supaya tampil sebelum pesan std::cerr di bawah.
        printWrapped(std::cerr, plainStyle(terminalWidth()), STYLE_NORMAL, // pesan dicetak ke std::cerr dengan gaya polos (tanpa warna) selebar terminal ...
                     "Input berakhir sebelum program ditutup lewat menu Keluar. Program dihentikan.", 0); // ... isi pesannya, tanpa indentasi baris lanjutan.
        return EXIT_CODE_INPUT_CLOSED; // kode keluar 1: masukan habis sebelum pengguna memilih Keluar.
    } // akhir blok try-catch
    return EXIT_CODE_OK; // runMenuLoop() selesai normal (lewat menu Keluar): kode keluar 0.
} // akhir fungsi runApp()
