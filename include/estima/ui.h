// ===== ui.h - komponen tampilan: gaya teks, kotak, tabel, dan pesan =====
// Header ini berisi "alat gambar" yang dipakai seluruh tampilan program: gaya teks (warna), kotak berjudul, tabel,
// dan pesan singkat. Di sini hanya ada deklarasi; isi fungsinya ditulis di src/ui.cpp.
// Pemakainya: src/report.cpp (rekap, peringkat, harga acuan, laporan berkas), src/app.cpp (banner, menu, pesan),
// dan src/prompt.cpp (label pertanyaan dan pesan kesalahan). Dengan alat yang sama, semua layar tampil seragam.
#ifndef ESTIMA_UI_H // ESTIMA_UI_H adalah penjaga header: isi berkas ini hanya dibaca bila nama itu belum didefinisikan.
#define ESTIMA_UI_H // #define ESTIMA_UI_H menandai bahwa header ini sudah dibaca, jadi #include berikutnya tidak membacanya dua kali.

#include <cstddef> // <cstddef> digunakan untuk std::size_t, tipe bilangan bulat tanpa tanda negatif untuk lebar dan ukuran teks.
#include <ostream> // <ostream> digunakan untuk std::ostream, yaitu aliran keluaran (layar atau berkas) yang menjadi tujuan gambar.
#include <string> // <string> digunakan untuk std::string, tipe teks untuk judul kotak, isi baris, dan isi sel tabel.
#include <vector> // <vector> digunakan untuk std::vector, daftar yang bisa bertambah panjang; dipakai untuk baris kotak serta kolom dan baris tabel.

// Gaya teks yang dikenal tampilan. Nilainya berurutan mulai 0 karena dipakai sebagai nomor
// elemen pada array kode warna (ANSI_CODES di src/ui.cpp); STYLE_COUNT di ujung otomatis bernilai banyaknya gaya.
// Pada enum, nama yang tidak diberi nilai otomatis bernilai satu lebih besar dari nama sebelumnya (1, 2, 3, ...).
// Dipakai enum, bukan angka biasa, supaya pemanggil menulis nama yang jelas seperti STYLE_ERROR, dan compiler menolak
// angka int sembarang di tempat yang meminta TextStyle.
enum TextStyle { // TextStyle adalah enum berisi semua gaya teks yang bisa dipakai saat mencetak.
    STYLE_NORMAL = 0, // STYLE_NORMAL = 0 adalah teks biasa tanpa kode warna; ditulis = 0 supaya jelas penomoran dimulai dari 0.
    STYLE_TITLE, // STYLE_TITLE (bernilai 1) adalah gaya judul: tebal dan berwarna cyan, misalnya judul kotak.
    STYLE_BOLD, // STYLE_BOLD (2) adalah teks tebal, misalnya judul kolom tabel, baris TOTAL, dan label pertanyaan.
    STYLE_DIM, // STYLE_DIM (3) adalah abu-abu yang tidak mencolok, untuk garis kotak, catatan, dan pesan keterangan [i].
    STYLE_SUCCESS, // STYLE_SUCCESS (4) adalah hijau tebal untuk pesan berhasil [OK].
    STYLE_ERROR, // STYLE_ERROR (5) adalah merah tebal untuk pesan kesalahan [!].
    STYLE_HINT, // STYLE_HINT (6) adalah kuning untuk ajakan "Tekan Enter untuk kembali ke menu...".
    STYLE_HIGHLIGHT, // STYLE_HIGHLIGHT (7) adalah kuning tebal untuk menyorot komponen berbiaya tertinggi di rekap dan peringkat.
    STYLE_COUNT // STYLE_COUNT (8) bukan gaya, melainkan banyaknya gaya di atasnya; dipakai sebagai ukuran array kode warna.
}; // akhir enum TextStyle

// Tujuan keluaran: seberapa lebar, boleh berwarna atau tidak, dan memakai garis UTF-8 atau ASCII.
// Dengan mengirim keterangan ini ke fungsi gambar, kode yang sama bisa menulis ke layar
// (berwarna, selebar jendela) maupun ke berkas laporan (polos, lebar tetap).
// Ketiga keterangan dibungkus dalam satu struct supaya setiap fungsi gambar cukup menerima satu parameter, bukan tiga.
struct OutputStyle { // OutputStyle adalah struct berisi tiga keterangan tentang tujuan keluaran.
    int width; // width adalah lebar keluaran dalam kolom; int sudah cukup karena nilainya kecil (40 sampai 120 kolom).
    bool useColor; // useColor bernilai true bila kode warna ANSI boleh ditulis; selalu false untuk berkas laporan.
    bool useUnicode; // useUnicode bernilai true bila garis kotak memakai karakter UTF-8; false berarti memakai + - | biasa.
}; // akhir struct OutputStyle

// Satu baris isi kotak beserta gayanya. Teks dan gaya disimpan berdampingan dalam satu struct, jadi saat baris
// yang panjang dilipat, setiap potongannya tetap membawa gaya yang sama.
struct StyledLine { // StyledLine adalah struct untuk satu baris isi kotak.
    std::string text; // text adalah isi baris yang akan ditulis.
    TextStyle style; // style adalah gaya (warna) baris itu, salah satu nilai enum TextStyle.

    StyledLine(const std::string& lineText, TextStyle lineStyle); // StyledLine(lineText, lineStyle) adalah konstruktor supaya baris bisa dibuat dan diisi
    // dalam satu ekspresi, misalnya lines.push_back(StyledLine("Total", STYLE_BOLD)).
}; // akhir struct StyledLine

// Keterangan satu kolom tabel: judul dan arah perataan (angka rata kanan, teks rata kiri).
struct TableColumn { // TableColumn adalah struct untuk keterangan satu kolom tabel.
    std::string header; // header adalah judul kolom, misalnya "Komponen" atau "Total biaya".
    bool alignRight; // alignRight bernilai true untuk kolom rata kanan (angka, supaya satuan dan ribuannya lurus), false untuk rata kiri (teks).

    TableColumn(const std::string& headerText, bool rightAligned); // TableColumn(headerText, rightAligned) adalah konstruktor, dipakai seperti
    // table.columns.push_back(TableColumn("No", true)).
}; // akhir struct TableColumn

// Satu baris tabel: isi tiap sel sebagai teks, ditambah gaya barisnya. Sel disimpan sebagai teks yang sudah
// dibentuk (misalnya "Rp821.400"), jadi fungsi gambar tidak perlu tahu arti angkanya.
struct TableRow { // TableRow adalah struct untuk satu baris tabel.
    std::vector<std::string> cells; // cells adalah isi sel dari kolom pertama sampai terakhir; vector karena banyak kolomnya berbeda tiap tabel.
    TextStyle style; // style adalah gaya seluruh baris, misalnya STYLE_HIGHLIGHT untuk komponen termahal.

    TableRow(); // TableRow() adalah konstruktor tanpa parameter yang mengisi style dengan STYLE_NORMAL, supaya baris baru tidak bergaya acak.
}; // akhir struct TableRow

// Tabel umum. Baris dan kolomnya disimpan dalam std::vector karena banyaknya baru diketahui
// saat program berjalan (mengikuti jumlah komponen). Satu struktur ini dipakai oleh rekap,
// peringkat biaya, daftar harga acuan, dan daftar pilihan komponen.
struct Table { // Table adalah struct yang menyatukan kolom, baris, dan baris penutup sebuah tabel.
    std::vector<TableColumn> columns; // columns adalah keterangan setiap kolom, berurutan dari kiri ke kanan.
    std::vector<TableRow> rows; // rows adalah baris-baris data tabel, berurutan dari atas ke bawah.
    bool hasFooter; // hasFooter bernilai true bila tabel punya baris penutup (misalnya baris TOTAL pada rekap).
    TableRow footer; // footer adalah isi baris penutup itu; hanya digambar bila hasFooter true.

    Table(); // Table() adalah konstruktor yang membuat tabel kosong dengan hasFooter bernilai false.
}; // akhir struct Table

// Gaya keluaran untuk layar (mengikuti terminal) dan untuk berkas teks polos selebar width.
OutputStyle screenStyle(); // screenStyle() berfungsi untuk menyusun gaya layar dari keadaan terminal: lebar jendela, warna, dan UTF-8.
OutputStyle plainStyle(int width); // plainStyle(width) berfungsi untuk menyusun gaya polos tanpa warna dengan garis ASCII, dipakai untuk laporan berkas.

// Membungkus teks dengan kode warna sesuai gayanya; teks dikembalikan apa adanya bila warna mati.
std::string colorize(const OutputStyle& style, TextStyle textStyle, const std::string& text); // colorize(style, textStyle, text) mengembalikan text
// yang sudah diberi kode warna textStyle; semua parameter dikirim sebagai const & atau nilai kecil, jadi tidak ada yang diubah.

// Karakter pengisi batang grafik: blok penuh pada mode UTF-8, tanda # pada mode ASCII.
const char* barGlyph(const OutputStyle& style); // barGlyph(style) mengembalikan teks tetap untuk satu kotak batang grafik di tabel peringkat biaya.

// Lebar isi terbesar sebuah kotak pada gaya keluaran ini.
std::size_t boxInnerLimit(const OutputStyle& style); // boxInnerLimit(style) mengembalikan lebar keluaran dikurangi bingkai kotak dan margin kanan.

// Menggambar kotak berjudul. Baris yang terlalu panjang dilipat mengikuti lebar keluaran.
// std::ostream& sebagai parameter berarti fungsi menerima aliran keluaran apa pun lewat referensi: std::cout (layar),
// std::cerr (saluran pesan galat), atau std::ofstream (berkas). Karena itu satu fungsi gambar cukup ditulis sekali dan
// dipakai untuk layar maupun laporan berkas. Tidak diberi const karena menulis ke aliran mengubah keadaan aliran itu.
void drawBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawBox(out, style, title, lines): out tujuan tulisan, style gaya keluaran,
             const std::vector<StyledLine>& lines); // title judul kotak, dan lines baris-baris isinya; void karena fungsi ini hanya menulis, tanpa hasil.

// Menggambar tabel di dalam kotak. Bila tabel tidak muat, setiap baris ditampilkan sebagai
// "kartu" bersusun ke bawah (dua kolom pertama menjadi judul kartu). notes dicetak di bawah tabel.
void drawTableBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawTableBox(out, style, title, table, notes): tiga parameter
                  const Table& table, const std::vector<StyledLine>& notes); // pertama sama dengan drawBox(), ditambah tabelnya dan catatan (boleh kosong).

// Mencetak teks yang dilipat selebar keluaran; baris lanjutan menjorok hangingIndent spasi.
void printWrapped(std::ostream& out, const OutputStyle& style, TextStyle textStyle, // printWrapped(out, style, textStyle, text, hangingIndent) mencetak
                  const std::string& text, std::size_t hangingIndent); // text tanpa kotak dengan gaya textStyle, dilipat bila lebih lebar dari keluaran.

// Pesan singkat ke layar dengan awalan [!] (salah), [OK] (berhasil), dan [i] (keterangan).
// Ketiganya hanya menerima teks pesan karena tujuan (layar) dan gayanya sudah pasti, jadi pemanggilannya pendek.
void printError(const std::string& message); // printError(message) menampilkan pesan kesalahan berwarna merah dengan awalan [!].
void printSuccess(const std::string& message); // printSuccess(message) menampilkan pesan berhasil berwarna hijau dengan awalan [OK].
void printInfo(const std::string& message); // printInfo(message) menampilkan keterangan berwarna abu-abu dengan awalan [i].

#endif // ESTIMA_UI_H (akhir penjaga header ui.h)
