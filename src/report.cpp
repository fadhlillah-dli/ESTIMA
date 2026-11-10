// ===== report.cpp - penyusunan laporan dari data komponen =====
// Berkas ini berisi isi fungsi-fungsi yang dideklarasikan di include/estima/report.h. Di sinilah
// keluaran yang diminta soal disusun: total berat dan total biaya tiap komponen, total keseluruhan,
// dan komponen dengan biaya produksi tertinggi, ditambah peringkat biaya dan laporan berkas .txt.
// Fungsi di sini tidak menghitung sendiri; semua angka diambil dari fungsi di calculation.h, lalu
// dibentuk menjadi teks (text_format.h) dan digambar sebagai kotak atau tabel (ui.h).
#include "estima/platform.h" // "estima/platform.h" wajib paling awal; di Visual C++ header ini mematikan peringatan "tidak aman" untuk std::localtime yang dipakai di bawah

#include "estima/report.h" // "estima/report.h" adalah header milik berkas ini sendiri, supaya compiler memeriksa bahwa isi fungsi di sini cocok dengan deklarasinya

#include <cmath> // <cmath> digunakan untuk std::floor, yaitu pembulatan ke bawah, yang dipakai saat menghitung panjang batang grafik
#include <cstddef> // <cstddef> digunakan untuk std::size_t, tipe bilangan bulat tak negatif untuk lebar nama (nameWidth) dan jumlah ulangan repeatText()
#include <ctime> // <ctime> digunakan untuk membaca jam komputer: std::time, std::localtime, std::strftime, beserta tipe std::time_t dan std::tm
#include <fstream> // <fstream> digunakan untuk std::ofstream, yaitu aliran keluaran yang menulis ke berkas, dipakai oleh saveReport()
#include <ostream> // <ostream> digunakan untuk std::ostream, tipe parameter out pada setiap fungsi cetak
#include <string> // <string> digunakan untuk std::string, teks yang panjangnya bisa berubah
#include <vector> // <vector> digunakan untuk std::vector, yaitu daftar baris isi kotak (StyledLine) dan sel tabel yang banyaknya mengikuti data

#include "estima/app_info.h" // "estima/app_info.h" digunakan untuk APP_NAME, APP_VERSION, dan APP_TAGLINE di kepala laporan berkas
#include "estima/calculation.h" // "estima/calculation.h" digunakan untuk semua rumus: total berat, total biaya, komponen termahal, peringkat, dan porsi biaya
#include "estima/material_catalog.h" // "estima/material_catalog.h" digunakan untuk MATERIAL_CATALOG, MATERIAL_COUNT, CATALOG_PERIOD, dan CATALOG_SOURCE_NOTE
#include "estima/text_format.h" // "estima/text_format.h" digunakan untuk formatThousands, formatRupiah, formatWeight, formatPercent, repeatText,
// serta displayWidth dan padRight yang meratakan tanda titik dua di daftar biaya tertinggi

// Lebar laporan berkas dibuat tetap supaya hasilnya sama di komputer mana pun dan tabel
// selalu tampil utuh ketika dibuka di penyunting teks.
// Keempat konstanta angka di bawah ini (sampai WIDE_LAYOUT_MIN_WIDTH) memakai static karena hanya dibutuhkan di berkas ini.
static const int REPORT_FILE_WIDTH = 120; // REPORT_FILE_WIDTH = 120 adalah lebar laporan berkas dalam kolom, tidak bergantung pada lebar jendela terminal
// Panjang batang grafik peringkat (dalam karakter) untuk terminal lebar dan sempit.
// Batang dibuat lebih pendek di terminal sempit supaya kolom grafik tidak menghabiskan tempat
// yang dibutuhkan kolom nama dan biaya.
static const int WIDE_BAR_CELLS = 24; // WIDE_BAR_CELLS = 24 adalah panjang batang terpanjang (milik biaya tertinggi) bila keluaran lebar
static const int NARROW_BAR_CELLS = 12; // NARROW_BAR_CELLS = 12 adalah panjang batang terpanjang bila keluaran sempit, yaitu separuh dari versi lebar
static const int WIDE_LAYOUT_MIN_WIDTH = 80; // WIDE_LAYOUT_MIN_WIDTH = 80 artinya keluaran dianggap lebar bila lebarnya 80 kolom atau lebih

// Pesan untuk daftar kosong. Di sini TIDAK memakai static, karena teks ini sudah diumumkan extern di
// report.h supaya bisa dipakai juga oleh menu di app.cpp; static akan bertentangan dengan deklarasi extern itu.
const char* const EMPTY_LIST_MESSAGE = // EMPTY_LIST_MESSAGE adalah isi dari teks tetap yang diumumkan extern di report.h; kalimatnya ada di baris berikut
    "Belum ada komponen. Pilih menu 1 untuk menambah komponen atau menu 6 untuk memuat data contoh."; // kalimat ini sekaligus memberi tahu pengguna langkah berikutnya

// Mencetak pemberitahuan daftar kosong dengan awalan [i]. Dipisah menjadi fungsi kecil karena
// dipakai oleh printRecap() dan printRanking() dengan bentuk yang sama persis.
static void printEmptyNotice(std::ostream& out, const OutputStyle& style) { // printEmptyNotice() berfungsi untuk mencetak EMPTY_LIST_MESSAGE ke out dengan gaya redup
    // std::string("[i] ") membungkus teks pertama menjadi std::string, karena dua const char* tidak bisa disambung
    // dengan + (lihat ui.cpp).
    printWrapped(out, style, STYLE_DIM, std::string("[i] ") + EMPTY_LIST_MESSAGE, 4); // angka 4 adalah lebar "[i] ", jadi baris lanjutan menjorok tepat di bawah awal kalimat
} // akhir fungsi printEmptyNotice()

// Tanggal dan jam saat ini dalam bentuk "10-11-2026 21:45" untuk kepala laporan berkas.
// Hasilnya std::string supaya pemanggil bisa langsung menulisnya dengan <<; bila jam komputer
// tidak bisa dibaca, hasilnya "-" sehingga laporan tetap tersimpan.
static std::string currentDateTime() { // currentDateTime() berfungsi untuk membuat teks tanggal dan jam saat laporan dibuat
    // std::time(NULL) berfungsi untuk membaca waktu sekarang dalam bentuk mentah bertipe std::time_t (biasanya jumlah detik sejak 1 Januari 1970).
    // Argumen NULL artinya hasilnya cukup dikembalikan, tidak perlu juga ditulis ke variabel lain.
    const std::time_t now = std::time(NULL); // now adalah waktu sekarang dalam detik; const karena nilainya hanya dibaca
    // std::localtime(&now) berfungsi untuk memecah detik mentah menjadi tanggal, bulan, tahun, jam, dan menit menurut zona waktu komputer.
    // Hasilnya pointer ke struct std::tm milik pustaka standar, atau NULL bila gagal; &now adalah alamat variabel now.
    const std::tm* local = std::localtime(&now); // local menunjuk ke hasil pecahan waktu; isinya hanya dibaca, jadi memakai const std::tm*
    char buffer[32]; // buffer adalah array 32 char tempat teks tanggal ditulis; "dd-mm-yyyy hh:mm" hanya 16 karakter, jadi ruangnya lebih dari cukup
    // std::strftime(tujuan, ukuran, pola, waktu) berfungsi untuk menulis waktu ke array char mengikuti pola: %d tanggal, %m bulan,
    // %Y tahun empat angka, %H jam 00-23, %M menit. Hasilnya banyak karakter yang ditulis, atau 0 bila tidak muat.
    if (local == NULL || std::strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M", local) == 0) { // bila localtime gagal ATAU strftime gagal, tanggal tidak bisa dibuat;
        // || berhenti di syarat pertama yang benar, jadi strftime tidak pernah dipanggil dengan local bernilai NULL. sizeof(buffer) = 32, batas tulis aman.
        return "-"; // return "-" artinya tanggal diganti tanda strip, supaya kegagalan membaca jam tidak menggagalkan penyimpanan laporan
    } // akhir if pemeriksaan kegagalan
    return buffer; // return buffer mengubah isi array char menjadi std::string (disalin), jadi aman walaupun buffer hilang saat fungsi selesai
} // akhir fungsi currentDateTime()

// Menyusun satu baris daftar komponen termahal, berbentuk "- Nama : RpX (Y% dari total biaya)". Dipisah menjadi
// fungsi kecil karena printRecap() memerlukannya dua kali: untuk mengukur lebar baris, lalu untuk mengisi kotak.
// Parameter nameWidth adalah lebar kolom nama; bila nilainya tidak melebihi lebar nama, nama ditulis apa adanya.
static std::string topComponentLine(const Component& component, double totalCost, std::size_t nameWidth) { // topComponentLine() berfungsi untuk membuat teks satu baris daftar itu
    const double cost = calculateTotalCost(component); // cost adalah total biaya komponen ini, dihitung oleh calculateTotalCost()
    // padRight(component.name, nameWidth) menambah spasi di kanan nama sampai selebar nameWidth, jadi tanda : sejajar.
    return "- " + padRight(component.name, nameWidth) + " : " + formatRupiah(cost) + " (" // bagian pertama: tanda "- ", nama, " : ", dan biaya dalam rupiah;
           + formatPercent(costSharePercent(cost, totalCost)) + " dari total biaya)"; // costSharePercent() menghitung porsi biaya dalam persen
    // dan formatPercent() menulisnya dengan satu desimal, misalnya "66,3%".
} // akhir fungsi topComponentLine()

// [Kriteria soal: keluaran rapi dan informatif] Keluaran (1) sampai (4) yang diminta soal dicetak di sini, dalam tabel
// dan kotak yang kolom serta tanda titik duanya rata (daftar komponen termahal diratakan selama barisnya muat di kotak).
// printRecap() menyusun tiga kotak berturut-turut: tabel rekap per komponen, total keseluruhan,
// dan daftar komponen berbiaya tertinggi. Semua angka diminta dari fungsi di calculation.h, jadi
// rumusnya hanya ada di satu tempat dan hasil di layar sama dengan hasil di berkas laporan.
void printRecap(std::ostream& out, const OutputStyle& style, const ComponentList& list) { // printRecap() berfungsi untuk mencetak rekap lengkap seluruh komponen di list
    if (list.count == 0) { // list.count == 0 artinya belum ada komponen, jadi tidak ada yang bisa direkap
        printEmptyNotice(out, style); // printEmptyNotice() menampilkan pesan daftar kosong beserta saran menu 1 atau 6
        return; // return di fungsi void artinya berhenti di sini tanpa menggambar tabel kosong
    } // akhir if daftar kosong

    // Array topIndexes menampung posisi komponen termahal. Ukurannya MAX_COMPONENTS karena pada
    // keadaan paling ekstrem semua komponen bisa berbiaya sama.
    int topIndexes[MAX_COMPONENTS]; // topIndexes adalah array int berukuran 100 yang diisi oleh findMostExpensive() di baris berikut
    const int topCount = findMostExpensive(list.items, list.count, topIndexes); // topCount adalah banyaknya komponen termahal (bisa lebih dari satu bila biayanya sama)
    const double totalWeight = sumTotalWeight(list.items, list.count); // totalWeight adalah total berat seluruh komponen dalam kg, bertipe double karena ada desimal gram
    const double totalCost = sumTotalCost(list.items, list.count); // totalCost adalah total biaya seluruh komponen dalam rupiah; double karena bisa melewati batas int
    const long totalUnits = sumQuantity(list.items, list.count); // totalUnits adalah jumlah unit seluruh komponen; bertipe long mengikuti hasil sumQuantity()

    // Tabel rekap: satu baris untuk tiap komponen, berisi data masukan dan hasil hitungnya.
    // Kolom disusun dulu (judul dan perataannya), lalu baris diisi lewat perulangan.
    Table table; // table adalah struct Table dari ui.h; konstruktornya membuat kolom dan baris kosong serta hasFooter = false
    table.columns.push_back(TableColumn("No", true)); // kolom "No"; true artinya rata kanan karena isinya angka
    table.columns.push_back(TableColumn("Komponen", false)); // kolom "Komponen"; false artinya rata kiri karena isinya nama (teks)
    table.columns.push_back(TableColumn("Jumlah", true)); // kolom "Jumlah" (unit), rata kanan
    table.columns.push_back(TableColumn("Berat/unit", true)); // kolom "Berat/unit" (kg per unit), rata kanan
    table.columns.push_back(TableColumn("Harga/kg", true)); // kolom "Harga/kg" (rupiah per kg), rata kanan
    table.columns.push_back(TableColumn("Total berat", true)); // kolom "Total berat" = jumlah x berat per unit, keluaran (1) soal
    table.columns.push_back(TableColumn("Total biaya", true)); // kolom "Total biaya" = total berat x harga per kg, keluaran (2) soal
    for (int i = 0; i < list.count; ++i) { // perulangan for dari i = 0 sampai list.count - 1, yaitu setiap elemen array yang terisi
        const Component& component = list.items[i]; // component adalah nama lain (referensi) untuk list.items[i], dipakai supaya data tidak disalin dan baris tetap pendek
        TableRow row; // row adalah satu baris tabel yang masih kosong; gaya awalnya STYLE_NORMAL dari konstruktor TableRow
        row.cells.push_back(formatThousands(i + 1)); // nomor urut ditulis i + 1 karena posisi array mulai dari 0, sedangkan pengguna menghitung dari 1
        row.cells.push_back(component.name); // nama komponen apa adanya
        row.cells.push_back(formatThousands(component.quantity)); // jumlah unit dengan titik ribuan, misalnya 1.000
        row.cells.push_back(formatWeight(component.unitWeightKg)); // berat per unit dengan tiga desimal, misalnya 1,850 kg
        row.cells.push_back(formatRupiah(component.pricePerKg)); // harga per kg dalam bentuk Rp18.500
        row.cells.push_back(formatWeight(calculateTotalWeight(component))); // total berat komponen ini, dihitung oleh calculateTotalWeight()
        row.cells.push_back(formatRupiah(calculateTotalCost(component))); // total biaya komponen ini, dihitung oleh calculateTotalCost()
        // Baris komponen termahal diberi gaya sorot.
        for (int k = 0; k < topCount; ++k) { // perulangan for memeriksa setiap posisi di topIndexes
            if (topIndexes[k] == i) { // topIndexes[k] == i artinya komponen ke-i termasuk yang biayanya tertinggi
                row.style = STYLE_HIGHLIGHT; // STYLE_HIGHLIGHT membuat baris ini tampil kuning tebal di layar (di berkas tanpa warna)
            } // akhir if komponen termahal
        } // akhir perulangan for k
        table.rows.push_back(row); // baris yang sudah lengkap dimasukkan ke ujung daftar baris tabel
    } // akhir perulangan for i
    // Baris penutup (footer) berisi total. Kolom yang tidak punya total diisi teks kosong, supaya
    // banyaknya sel tetap tujuh dan setiap angka berada di bawah kolomnya sendiri.
    table.hasFooter = true; // hasFooter = true artinya tabel ini punya baris total di bawah garis pemisah
    table.footer.cells.push_back(""); // kolom "No" pada baris total dikosongkan
    table.footer.cells.push_back("TOTAL"); // kolom "Komponen" berisi tulisan TOTAL
    table.footer.cells.push_back(formatThousands(static_cast<double>(totalUnits))); // jumlah seluruh unit;
    // static_cast<double> mengubah long menjadi double, tipe parameter yang diminta formatThousands().
    table.footer.cells.push_back(""); // kolom "Berat/unit" dikosongkan karena menjumlahkan berat per unit tidak bermakna
    table.footer.cells.push_back(""); // kolom "Harga/kg" dikosongkan karena alasan yang sama
    table.footer.cells.push_back(formatWeight(totalWeight)); // total berat keseluruhan, keluaran (3) soal
    table.footer.cells.push_back(formatRupiah(totalCost)); // total biaya keseluruhan, keluaran (3) soal
    drawTableBox(out, style, "Rekap Material dan Biaya", table, std::vector<StyledLine>()); // drawTableBox() menggambar tabel di dalam kotak berjudul;
    // std::vector<StyledLine>() adalah vector kosong yang dibuat langsung di tempat, artinya tabel ini tidak punya catatan di bawahnya.

    // Kotak kedua: ringkasan total. Setiap baris berbentuk "Label : nilai" dengan titik dua
    // yang lurus; bila baris harus dilipat, drawBox() melanjutkannya di bawah awal nilainya
    // (selama bagian label tidak lebih dari separuh lebar kotak).
    std::vector<StyledLine> totals; // totals adalah daftar baris isi kotak "Total Keseluruhan"
    totals.push_back(StyledLine("Jenis komponen : " + formatThousands(list.count), STYLE_NORMAL)); // banyaknya jenis komponen, yaitu isi list.count
    totals.push_back(StyledLine("Jumlah unit    : " + formatThousands(static_cast<double>(totalUnits)), STYLE_NORMAL)); // jumlah semua unit dari semua komponen
    totals.push_back(StyledLine("Total berat    : " + formatWeight(totalWeight), STYLE_BOLD)); // total berat ditebalkan (STYLE_BOLD) karena inilah hasil utama
    totals.push_back(StyledLine("Total biaya    : " + formatRupiah(totalCost), STYLE_BOLD)); // total biaya juga ditebalkan; baris ini diperiksa oleh uji sesi asap
    drawBox(out, style, "Total Keseluruhan", totals); // drawBox() menggambar kotak berjudul berisi baris-baris totals

    // Kotak ketiga: komponen dengan biaya produksi tertinggi (keluaran (4) soal), lengkap dengan
    // porsinya terhadap total biaya.
    std::vector<StyledLine> top; // top adalah daftar baris isi kotak "Biaya Produksi Tertinggi"
    if (topCount > 1) { // topCount > 1 artinya ada beberapa komponen yang biaya tertingginya sama persis
        top.push_back(StyledLine("Ada " + formatThousands(topCount) + " komponen dengan biaya tertinggi yang sama:", STYLE_NORMAL)); // kalimat pembuka
        // supaya pengguna tahu bahwa daftar di bawahnya berisi lebih dari satu komponen termahal.
    } // akhir if lebih dari satu komponen termahal
    // Bila beberapa komponen sama-sama termahal, nama yang lebih pendek ditambah spasi sampai selebar nama
    // terpanjang di antara mereka, supaya tanda titik dua di daftar berada pada kolom yang sama. Bila hanya
    // satu komponen, lebarnya sama dengan lebar namanya sendiri, jadi barisnya tidak berubah.
    std::size_t nameWidth = 0; // nameWidth adalah lebar nama terpanjang di antara komponen termahal; mulai dari 0 lalu diperbesar di perulangan berikut
    for (int k = 0; k < topCount; ++k) { // perulangan for mengukur nama setiap komponen termahal
        const std::size_t currentWidth = displayWidth(list.items[topIndexes[k]].name); // currentWidth adalah lebar nama komponen termahal ke-k dalam kolom layar
        if (currentWidth > nameWidth) { // currentWidth > nameWidth artinya nama ini lebih lebar daripada yang terlebar sejauh ini
            nameWidth = currentWidth; // nameWidth diganti dengan lebar nama ini
        } // akhir if nama lebih lebar
    } // akhir perulangan pengukur lebar nama
    // Perataan itu hanya dipakai bila baris yang sudah diratakan masih muat di dalam kotak. Di terminal sempit, satu
    // nama yang panjang akan membuat semua baris ikut terlipat; pada keadaan itu nameWidth dikembalikan ke 0, sehingga
    // padRight() tidak menambah spasi apa pun dan setiap nama ditulis apa adanya.
    for (int k = 0; k < topCount; ++k) { // perulangan for memeriksa lebar baris setiap komponen termahal bila namanya diratakan
        if (displayWidth(topComponentLine(list.items[topIndexes[k]], totalCost, nameWidth)) > boxInnerLimit(style)) { // boxInnerLimit(style) adalah lebar isi kotak
            // terbesar pada lebar keluaran ini; baris yang lebih lebar dari itu pasti dilipat oleh drawBox().
            nameWidth = 0; // nameWidth = 0 artinya perataan dibatalkan untuk semua baris
        } // akhir if baris tidak muat
    } // akhir perulangan pemeriksa lebar baris
    for (int k = 0; k < topCount; ++k) { // perulangan for atas setiap komponen termahal yang ditemukan
        // topIndexes[k] berisi posisi komponen di array, lalu dipakai sebagai nomor elemen list.items.
        top.push_back(StyledLine(topComponentLine(list.items[topIndexes[k]], totalCost, nameWidth), STYLE_HIGHLIGHT)); // STYLE_HIGHLIGHT menyorot baris ini
    } // akhir perulangan for k
    drawBox(out, style, "Biaya Produksi Tertinggi", top); // kotak terakhir rekap
} // akhir fungsi printRecap()

// printRanking() menampilkan semua komponen dari biaya terbesar ke terkecil, dengan porsi tiap
// komponen dan grafik batang. Data di list tidak diurutkan ulang; yang diurutkan adalah array
// nomor posisi order[], jadi urutan isian pengguna tetap utuh untuk menu lain.
void printRanking(std::ostream& out, const OutputStyle& style, const ComponentList& list) { // printRanking() berfungsi untuk mencetak tabel peringkat biaya
    if (list.count == 0) { // list.count == 0 artinya belum ada komponen untuk diperingkat
        printEmptyNotice(out, style); // menampilkan pesan daftar kosong
        return; // berhenti di sini; tanpa pemeriksaan ini order[0] di bawah akan dibaca padahal tidak terisi
    } // akhir if daftar kosong

    // order[] berisi posisi komponen yang sudah diurutkan dari biaya terbesar.
    // Ukurannya MAX_COMPONENTS (100) karena rankByCost() mengisi satu elemen untuk setiap komponen.
    int order[MAX_COMPONENTS]; // order adalah array int penampung nomor posisi hasil pengurutan
    rankByCost(list.items, list.count, order); // rankByCost() mengisi order[] dengan insertion sort berdasarkan total biaya
    const double totalCost = sumTotalCost(list.items, list.count); // totalCost adalah total biaya semua komponen, pembagi untuk kolom "Porsi"
    const double highestCost = calculateTotalCost(list.items[order[0]]); // highestCost adalah biaya komponen peringkat pertama (order[0]), patokan panjang batang
    // ?: memilih 24 bila lebar keluaran 80 kolom atau lebih, dan 12 bila kurang dari itu.
    const int barCells = style.width >= WIDE_LAYOUT_MIN_WIDTH ? WIDE_BAR_CELLS : NARROW_BAR_CELLS; // barCells adalah panjang batang terpanjang untuk keluaran ini

    Table table; // table adalah tabel peringkat yang masih kosong
    table.columns.push_back(TableColumn("No", true)); // kolom "No" berisi nomor peringkat, rata kanan
    table.columns.push_back(TableColumn("Komponen", false)); // kolom "Komponen" berisi nama, rata kiri
    table.columns.push_back(TableColumn("Total biaya", true)); // kolom "Total biaya", rata kanan
    table.columns.push_back(TableColumn("Porsi", true)); // kolom "Porsi" berisi persen biaya terhadap total, rata kanan
    table.columns.push_back(TableColumn("Grafik", false)); // kolom "Grafik" berisi batang, rata kiri supaya semua batang mulai dari titik yang sama

    int rank = 0; // rank adalah nomor peringkat yang sedang berlaku; nilai awal 0 akan langsung diganti di putaran pertama
    double previousCost = -1.0; // previousCost adalah biaya komponen sebelumnya; -1 dipilih karena tidak ada biaya negatif, jadi komponen pertama pasti mendapat peringkat 1
    for (int i = 0; i < list.count; ++i) { // perulangan for menelusuri komponen sesuai urutan peringkat, dari biaya terbesar
        const Component& component = list.items[order[i]]; // order[i] adalah posisi komponen peringkat ke-(i + 1) di array list.items
        const double cost = calculateTotalCost(component); // cost adalah total biaya komponen ini
        // Komponen dengan biaya sama mendapat peringkat yang sama; peringkat berikutnya
        // melompat sesuai banyaknya komponen di atasnya (1, 1, 3, ...).
        if (cost != previousCost) { // cost != previousCost artinya biayanya berbeda dari komponen sebelumnya; aman memakai != karena biaya selalu rupiah bulat
            rank = i + 1; // peringkat baru sama dengan nomor urutnya (i mulai dari 0, peringkat mulai dari 1)
            previousCost = cost; // biaya ini diingat untuk dibandingkan dengan komponen berikutnya
        } // akhir if biaya berbeda
        // Panjang batang sebanding dengan biaya terhadap biaya tertinggi; komponen yang
        // biayanya tidak nol selalu mendapat paling sedikit satu kotak.
        int cells = 0; // cells adalah panjang batang komponen ini dalam karakter; mulai 0
        if (highestCost > 0.0) { // highestCost > 0.0 dicek dulu supaya tidak terjadi pembagian dengan nol bila semua biaya Rp0
            cells = static_cast<int>(std::floor(cost / highestCost * barCells + 0.5)); // cost / highestCost adalah perbandingan 0 sampai 1, dikali barCells;
            // menambah 0,5 lalu std::floor sama dengan membulatkan ke bilangan terdekat, kemudian static_cast<int> mengubahnya ke int.
            if (cells < 1 && cost > 0.0) { // biaya kecil tetapi bukan nol bisa terbulatkan menjadi 0 kotak
                cells = 1; // diberi satu kotak supaya terlihat bahwa komponen itu tetap punya biaya
            } // akhir if batang minimal satu kotak
        } // akhir if highestCost > 0
        TableRow row; // row adalah satu baris tabel peringkat
        row.cells.push_back(formatThousands(rank)); // nomor peringkat (biaya sama berarti nomor sama)
        row.cells.push_back(component.name); // nama komponen
        row.cells.push_back(formatRupiah(cost)); // total biaya komponen
        row.cells.push_back(formatPercent(costSharePercent(cost, totalCost))); // porsi biaya terhadap total, misalnya 66,3%
        row.cells.push_back(repeatText(barGlyph(style), static_cast<std::size_t>(cells))); // batang grafik: karakter blok (UTF-8) atau # (ASCII) diulang cells kali;
        // static_cast<std::size_t> dipakai karena repeatText() meminta jumlah ulangan bertipe std::size_t, sedangkan cells bertipe int.
        if (rank == 1) { // rank == 1 artinya komponen ini berada di peringkat teratas (bisa lebih dari satu bila biayanya sama)
            row.style = STYLE_HIGHLIGHT; // peringkat pertama disorot, sama seperti komponen termahal di rekap
        } // akhir if peringkat pertama
        table.rows.push_back(row); // baris dimasukkan ke tabel
    } // akhir perulangan for i

    // Catatan di bawah tabel menjelaskan arti kolom "Porsi" dan "Grafik", karena keduanya
    // dihitung dengan pembanding yang berbeda (total biaya dan biaya tertinggi).
    std::vector<StyledLine> notes; // notes adalah daftar baris catatan di bawah tabel
    notes.push_back(StyledLine("Porsi = bagian biaya komponen dari total biaya. Grafik membandingkan tiap komponen dengan biaya tertinggi.", // teks catatan;
                               STYLE_DIM)); // STYLE_DIM membuat catatan tampil redup (abu-abu) supaya tidak bersaing dengan isi tabel
    drawTableBox(out, style, "Peringkat Biaya Produksi", table, notes); // menggambar tabel peringkat beserta catatannya di dalam kotak
} // akhir fungsi printRanking()

// printCatalog() menampilkan daftar harga acuan material. Isinya dibaca dari array konstan
// MATERIAL_CATALOG dengan perulangan, jadi baris yang ditambah ke katalog otomatis ikut tampil.
void printCatalog(std::ostream& out, const OutputStyle& style) { // printCatalog() berfungsi untuk mencetak tabel harga acuan material (menu 7)
    Table table; // table adalah tabel katalog yang masih kosong
    table.columns.push_back(TableColumn("No", true)); // kolom "No" berisi nomor pilihan material, rata kanan
    table.columns.push_back(TableColumn("Material", false)); // kolom "Material" berisi nama bahan, rata kiri
    table.columns.push_back(TableColumn("Harga/kg", true)); // kolom "Harga/kg" berisi harga acuan, rata kanan
    table.columns.push_back(TableColumn("Standar", false)); // kolom "Standar" berisi kode standar bahan, misalnya JIS G4051
    table.columns.push_back(TableColumn("Kegunaan umum", false)); // kolom "Kegunaan umum" berisi contoh pemakaian bahan
    for (int i = 0; i < MATERIAL_COUNT; ++i) { // perulangan for atas setiap baris katalog; MATERIAL_COUNT dihitung dari ukuran array
        TableRow row; // row adalah satu baris tabel katalog
        row.cells.push_back(formatThousands(i + 1)); // nomor mulai dari 1, sama dengan nomor yang dipilih saat menambah komponen
        row.cells.push_back(MATERIAL_CATALOG[i].name); // nama material; const char* otomatis diubah menjadi std::string saat dimasukkan
        row.cells.push_back(formatRupiah(MATERIAL_CATALOG[i].pricePerKg)); // harga acuan per kg dalam bentuk rupiah
        row.cells.push_back(MATERIAL_CATALOG[i].standard); // kode standar bahan
        row.cells.push_back(MATERIAL_CATALOG[i].typicalUse); // kegunaan umum bahan
        table.rows.push_back(row); // baris dimasukkan ke tabel
    } // akhir perulangan for

    // Catatan sumber data ditampilkan supaya pengguna tahu harga itu berasal dari periode mana.
    std::vector<StyledLine> notes; // notes adalah daftar baris catatan di bawah tabel katalog
    notes.push_back(StyledLine(std::string("Periode data: ") + CATALOG_PERIOD + ".", STYLE_DIM)); // periode harga; std::string() dipakai agar teks bisa disambung dengan +
    notes.push_back(StyledLine(CATALOG_SOURCE_NOTE, STYLE_DIM)); // keterangan sumber harga dari material_catalog.cpp
    drawTableBox(out, style, "Harga Acuan Material", table, notes); // menggambar tabel katalog beserta catatannya
} // akhir fungsi printCatalog()

// printComponentSummary() menampilkan data satu komponen beserta hasil hitungnya sebelum disimpan.
// Bentuknya kotak "Label : nilai", bukan tabel, karena hanya berisi satu komponen.
void printComponentSummary(std::ostream& out, const OutputStyle& style, const Component& component) { // printComponentSummary() mencetak ringkasan satu komponen
    std::vector<StyledLine> lines; // lines adalah daftar baris isi kotak ringkasan
    lines.push_back(StyledLine("Nama        : " + component.name, STYLE_NORMAL)); // nama komponen
    lines.push_back(StyledLine("Jumlah      : " + formatThousands(component.quantity) + " unit", STYLE_NORMAL)); // jumlah unit
    lines.push_back(StyledLine("Berat/unit  : " + formatWeight(component.unitWeightKg), STYLE_NORMAL)); // berat per unit
    lines.push_back(StyledLine("Harga/kg    : " + formatRupiah(component.pricePerKg), STYLE_NORMAL)); // harga per kg
    lines.push_back(StyledLine("Total berat : " + formatWeight(calculateTotalWeight(component)), STYLE_BOLD)); // total berat hasil hitung, ditebalkan
    lines.push_back(StyledLine("Total biaya : " + formatRupiah(calculateTotalCost(component)), STYLE_BOLD)); // total biaya hasil hitung, ditebalkan
    drawBox(out, style, "Ringkasan Komponen", lines); // menggambar kotak ringkasan
} // akhir fungsi printComponentSummary()

// printComponentPicker() menampilkan daftar pendek untuk memilih komponen yang akan diubah atau
// dihapus. Isinya cukup tiga kolom (nomor, nama, total biaya), karena gunanya hanya untuk memilih.
void printComponentPicker(std::ostream& out, const OutputStyle& style, const ComponentList& list) { // printComponentPicker() berfungsi untuk mencetak daftar pilihan komponen
    Table table; // table adalah tabel pilihan yang masih kosong
    table.columns.push_back(TableColumn("No", true)); // kolom "No" berisi nomor yang diketik pengguna untuk memilih
    table.columns.push_back(TableColumn("Komponen", false)); // kolom "Komponen" berisi nama
    table.columns.push_back(TableColumn("Total biaya", true)); // kolom "Total biaya" membantu pengguna mengenali komponen yang dimaksud
    for (int i = 0; i < list.count; ++i) { // perulangan for atas setiap komponen yang terisi
        TableRow row; // row adalah satu baris tabel pilihan
        row.cells.push_back(formatThousands(i + 1)); // nomor i + 1; pemanggil di app.cpp mengurangi lagi 1 untuk mendapat posisi array
        row.cells.push_back(list.items[i].name); // nama komponen ke-i
        row.cells.push_back(formatRupiah(calculateTotalCost(list.items[i]))); // total biaya komponen ke-i
        table.rows.push_back(row); // baris dimasukkan ke tabel
    } // akhir perulangan for
    drawTableBox(out, style, "Daftar Komponen", table, std::vector<StyledLine>()); // menggambar tabel pilihan tanpa catatan (vector kosong)
} // akhir fungsi printComponentPicker()

// saveReport() menulis laporan ke berkas teks. Isinya disusun oleh printRecap() dan printRanking()
// yang sama dengan tampilan layar, hanya tujuannya berkas dan gayanya polos. Fungsi ini tidak
// mencetak pesan sendiri: hasil bool dan errorMessage diserahkan ke menu yang memanggilnya.
bool saveReport(const ComponentList& list, const std::string& fileName, std::string& errorMessage) { // saveReport() berfungsi untuk menyimpan laporan ke berkas fileName
    // std::ofstream adalah aliran keluaran ke berkas: membuka (atau membuat) berkas, lalu isinya ditulis dengan << seperti std::cout.
    // Bila berkasnya sudah ada, isi lamanya dikosongkan dulu. Pasangannya std::ifstream membuka berkas untuk DIBACA; program ini
    // memakainya di fileExists() pada src/app.cpp untuk mengecek apakah berkas sudah ada. Keduanya tersedia lewat <fstream>.
    // Karena std::ofstream adalah salah satu jenis std::ostream, variabel file bisa dikirim ke parameter std::ostream& milik printRecap().
    std::ofstream file(fileName.c_str()); // file membuka berkas bernama fileName; .c_str() memberi bentuk const char* karena C++98 belum menerima std::string di sini
    if (!file) { // !file bernilai benar bila berkas gagal dibuka, misalnya folder tidak boleh ditulisi
        errorMessage = "Berkas tidak bisa dibuat. Periksa izin tulis di folder ini."; // sebab kegagalan dititipkan ke errorMessage untuk ditampilkan pemanggil
        return false; // false artinya laporan tidak tersimpan
    } // akhir if gagal membuka berkas

    // Laporan berkas memakai gaya polos: tanpa kode warna dan dengan garis ASCII, supaya terbaca
    // di penyunting teks apa pun. Isinya disusun oleh fungsi yang sama dengan tampilan layar.
    const OutputStyle style = plainStyle(REPORT_FILE_WIDTH); // style adalah gaya polos selebar 120 kolom: useColor dan useUnicode bernilai false
    file << APP_NAME << ' ' << APP_VERSION << " - " << APP_TAGLINE << '\n'; // baris kepala laporan: nama, versi, dan slogan program; ' ' adalah satu karakter spasi
    file << "Laporan dibuat : " << currentDateTime() << '\n'; // baris kedua berisi tanggal dan jam laporan dibuat
    file << '\n'; // baris kosong sebagai pemisah
    printRecap(file, style, list); // rekap lengkap ditulis ke berkas, bukan ke layar, karena out yang dikirim adalah file
    file << '\n'; // baris kosong pemisah antara rekap dan peringkat
    printRanking(file, style, list); // peringkat biaya ditulis ke berkas
    file << '\n'; // baris kosong sebelum catatan penutup
    file << "Catatan: harga per kg adalah harga yang dipakai saat laporan dibuat (harga acuan atau harga yang diketik sendiri).\n"; // catatan penutup laporan;
    // harga per kg di laporan adalah harga yang tersimpan di daftar komponen (diambil dari harga acuan atau diketik pengguna), bukan harga pasar terbaru.

    // close() mengirim sisa tulisan yang masih tertahan di memori ke berkas lalu menutup berkasnya. Berkas ditutup di sini
    // (tidak menunggu ditutup otomatis di akhir fungsi) supaya kegagalan menulis maupun kegagalan menutup, misalnya karena
    // ruang penyimpanan penuh, sudah ketahuan saat diperiksa di bawah.
    file.close(); // file.close() menulis semua sisa isi laporan dan menutup berkas sekarang juga
    if (!file) { // !file bernilai benar bila ada penulisan yang gagal sejak berkas dibuka, atau berkasnya gagal ditutup
        errorMessage = "Penulisan berkas gagal. Periksa sisa ruang penyimpanan lalu coba lagi."; // sebab kegagalan untuk pemanggil
        return false; // false artinya laporan tidak tersimpan dengan utuh
    } // akhir if penulisan gagal
    return true; // true artinya laporan tersimpan
} // akhir fungsi saveReport()
