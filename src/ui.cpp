// ===== ui.cpp - komponen tampilan: gaya teks, kotak, tabel, dan pesan =====
// Berkas ini berisi isi (definisi) semua fungsi yang dideklarasikan di include/estima/ui.h. Semua gambar di layar dan
// di berkas laporan (kotak, tabel, kartu, pesan berwarna) dibuat di sini, jadi bentuk tampilan cukup diatur di satu tempat.
#include "estima/platform.h" // platform.h wajib dibaca paling awal, sebelum header pustaka standar, karena berisi penyesuaian untuk Visual C++.

#include "estima/ui.h" // ui.h adalah header milik berkas ini: berisi enum, struct, dan deklarasi fungsi yang isinya ditulis di bawah.

#include <cstddef> // <cstddef> digunakan untuk std::size_t, tipe yang dipakai di semua hitungan lebar di berkas ini.
#include <iostream> // <iostream> digunakan untuk std::cout, saluran keluaran ke layar yang dipakai printError(), printSuccess(), dan printInfo().
#include <ostream> // <ostream> digunakan untuk std::ostream dan operator << yang menulis ke aliran keluaran.
#include <string> // <string> digunakan untuk std::string.
#include <vector> // <vector> digunakan untuk std::vector.

#include "estima/terminal.h" // terminal.h menyediakan terminalWidth(), colorsEnabled(), dan unicodeEnabled() yang dipakai screenStyle().
#include "estima/text_format.h" // text_format.h menyediakan displayWidth(), padRight(), padLeft(), repeatText(), truncateText(), dan wrapText().

// Karakter penyusun kotak dan grafik untuk satu mode gambar. Setiap anggota bertipe const char* (teks tetap), bukan char,
// karena satu karakter garis UTF-8 terdiri atas tiga byte sehingga harus disimpan sebagai teks pendek.
struct BoxGlyphs { // BoxGlyphs adalah struct berisi tujuh potongan gambar: empat sudut, garis datar, garis tegak, dan isi batang grafik.
    const char* topLeft; // topLeft adalah sudut kiri atas kotak.
    const char* topRight; // topRight adalah sudut kanan atas kotak.
    const char* bottomLeft; // bottomLeft adalah sudut kiri bawah kotak.
    const char* bottomRight; // bottomRight adalah sudut kanan bawah kotak.
    const char* horizontal; // horizontal adalah garis mendatar untuk tepi atas dan bawah kotak serta garis pemisah tabel.
    const char* vertical; // vertical adalah garis tegak untuk tepi kiri dan kanan kotak.
    const char* bar; // bar adalah karakter pengisi batang grafik pada tabel peringkat biaya.
}; // akhir struct BoxGlyphs

// Dua set karakter disimpan dalam array dua elemen: nomor 0 untuk mode ASCII dan nomor 1 untuk
// mode UTF-8. Mode dipilih dengan mengambil elemen array, tanpa if di setiap tempat menggambar.
// Karakter UTF-8 ditulis sebagai kode byte (\x..) supaya berkas sumber tetap ASCII murni dan
// dikompilasi sama persis oleh compiler apa pun: "\xE2\x94\x8C" adalah sudut kiri atas kotak.
// Di dalam tanda kutip, \x yang diikuti dua digit heksadesimal (basis 16) berarti satu byte bernilai itu;
// tiga byte E2 94 8C bersama-sama membentuk satu karakter garis UTF-8.
// Array ini hanya dipakai di berkas ini dan isinya tidak pernah diubah selama program berjalan.
static const BoxGlyphs GLYPH_SETS[2] = { // GLYPH_SETS[2] adalah array dua BoxGlyphs; isinya ditulis di antara { } sesuai urutan anggota struct.
    { "+", "+", "+", "+", "-", "|", "#" }, // elemen 0 (ASCII): empat sudut "+", garis datar "-", garis tegak "|", dan batang "#"; terbaca di mana pun.
    { "\xE2\x94\x8C", "\xE2\x94\x90", "\xE2\x94\x94", "\xE2\x94\x98", "\xE2\x94\x80", "\xE2\x94\x82", "\xE2\x96\x88" } // elemen 1 (UTF-8):
    // sudut kiri atas, kanan atas, kiri bawah, kanan bawah, garis datar, garis tegak (semuanya garis tipis), dan blok penuh untuk batang.
}; // akhir array GLYPH_SETS

// Kode warna ANSI untuk tiap gaya, disusun sejajar dengan urutan enum TextStyle sehingga gaya
// bisa langsung dipakai sebagai nomor elemen. "\033" adalah karakter ESC pembuka kode.
// \033 ditulis dalam bilangan oktal (basis 8): 3 x 8 + 3 = 27, yaitu nomor karakter ESC. Kode ANSI berbentuk ESC, "[",
// angka gaya, lalu "m"; terminal tidak mencetaknya, melainkan membacanya sebagai perintah ganti tampilan.
// Ukuran array ditulis STYLE_COUNT supaya selalu sama dengan banyaknya gaya di enum TextStyle.
static const char* const ANSI_CODES[STYLE_COUNT] = { // ANSI_CODES[STYLE_COUNT] adalah array 8 teks tetap; nomor elemennya adalah nilai TextStyle.
    "", // nomor 0, STYLE_NORMAL: teks kosong karena teks biasa tidak memerlukan kode warna.
    "\033[1;36m", // nomor 1, STYLE_TITLE: 1;36 artinya tebal (1) sekaligus cyan (36).
    "\033[1m", // nomor 2, STYLE_BOLD: 1 artinya tebal tanpa mengganti warna.
    "\033[90m", // nomor 3, STYLE_DIM: 90 artinya abu-abu (hitam terang), supaya garis dan catatan tidak mencolok.
    "\033[1;32m", // nomor 4, STYLE_SUCCESS: tebal dan hijau (32).
    "\033[1;31m", // nomor 5, STYLE_ERROR: tebal dan merah (31).
    "\033[33m", // nomor 6, STYLE_HINT: kuning (33) tanpa tebal.
    "\033[1;33m" // nomor 7, STYLE_HIGHLIGHT: tebal dan kuning.
}; // akhir array ANSI_CODES
static const char* const ANSI_RESET = "\033[0m"; // ANSI_RESET adalah kode 0 yang mengembalikan tampilan ke normal; ditulis setelah teks berwarna
// supaya warnanya tidak terbawa ke teks berikutnya.

// Kotak memakai 2 kolom garis tepi dan 2 kolom spasi dalam; satu kolom di ujung kanan terminal
// dibiarkan kosong supaya baris tidak pernah terlipat sendiri oleh terminal.
// Angka tata letak ditulis sebagai konstanta bernama supaya maksudnya terbaca dan cukup diubah di satu tempat.
// Tipenya std::size_t karena selalu dihitung bersama lebar teks, yang juga bertipe std::size_t.
static const std::size_t BOX_FRAME_WIDTH = 4; // BOX_FRAME_WIDTH = 4 adalah kolom yang dipakai bingkai kotak: garis kiri, spasi, spasi, garis kanan.
static const std::size_t RIGHT_MARGIN = 1; // RIGHT_MARGIN = 1 adalah satu kolom kosong di ujung kanan keluaran.
static const std::size_t MIN_BOX_INNER = 10; // MIN_BOX_INNER = 10 adalah lebar isi kotak paling sempit, dipakai bila lebar keluaran terlalu kecil.
static const std::size_t COLUMN_GAP = 2; // COLUMN_GAP = 2 adalah banyaknya spasi di antara dua kolom tabel.
static const std::size_t MIN_NOTE_WIDTH = 44; // MIN_NOTE_WIDTH = 44 adalah lebar lipatan catatan paling sempit, supaya catatan di bawah tabel
// yang sempit tidak terpecah menjadi banyak baris pendek.
static const std::size_t CARD_TITLE_COLUMNS = 2; // CARD_TITLE_COLUMNS = 2 artinya dua kolom pertama tabel (nomor dan nama) menjadi judul kartu.
static const char* const CARD_INDENT = "   "; // CARD_INDENT adalah tiga spasi di depan setiap baris isi kartu, supaya isinya menjorok di bawah judul.

// Empat konstruktor di bawah mengisi setiap anggota lewat daftar inisialisasi (bagian setelah tanda :), jadi anggota langsung
// mendapat nilai awal yang benar saat struct dibuat. Badan fungsinya { } kosong karena tidak ada pekerjaan lain.
StyledLine::StyledLine(const std::string& lineText, TextStyle lineStyle) : text(lineText), style(lineStyle) { // konstruktor StyledLine:
    // text diisi lineText dan style diisi lineStyle.
} // akhir konstruktor StyledLine

TableColumn::TableColumn(const std::string& headerText, bool rightAligned) : header(headerText), alignRight(rightAligned) { // konstruktor TableColumn:
    // header diisi judul kolom dan alignRight diisi arah perataannya.
} // akhir konstruktor TableColumn

TableRow::TableRow() : cells(), style(STYLE_NORMAL) { // konstruktor TableRow: cells() dimulai kosong dan style diisi STYLE_NORMAL.
} // akhir konstruktor TableRow

Table::Table() : columns(), rows(), hasFooter(false), footer() { // konstruktor Table: kolom dan baris kosong, tanpa baris penutup (hasFooter false).
} // akhir konstruktor Table

// screenStyle() menyusun gaya keluaran layar dari keadaan terminal. Fungsi ini dipanggil setiap kali akan menggambar
// (bukan sekali di awal), jadi bila jendela terminal diubah ukurannya, gambar berikutnya ikut menyesuaikan.
OutputStyle screenStyle() { // screenStyle() mengembalikan OutputStyle untuk layar.
    OutputStyle style; // style adalah variabel lokal yang diisi satu per satu lalu dikembalikan.
    style.width = terminalWidth(); // terminalWidth() memberi lebar jendela terminal, selalu di antara 40 dan 120 kolom.
    style.useColor = colorsEnabled(); // colorsEnabled() bernilai true bila warna boleh dipakai menurut opsi --no-color, NO_COLOR, FORCE_COLOR, dan jenis keluarannya.
    style.useUnicode = unicodeEnabled(); // unicodeEnabled() bernilai false bila pengguna memakai opsi --ascii, atau (di Windows) bila keluarannya konsol yang gagal dipindah ke UTF-8.
    return style; // return style mengembalikan struct yang sudah lengkap kepada pemanggil.
} // akhir fungsi screenStyle()

// plainStyle() menyusun gaya polos: tanpa warna dan dengan garis ASCII, selebar width. Dipakai saveReport() di report.cpp
// (lebar 120) supaya berkas laporan terbaca di penyunting teks apa pun, dan oleh app.cpp untuk pesan saat masukan habis.
OutputStyle plainStyle(int width) { // plainStyle(width) mengembalikan OutputStyle polos selebar width kolom.
    OutputStyle style; // style adalah variabel lokal yang akan dikembalikan.
    style.width = width; // lebar diambil dari parameter, bukan dari terminal.
    style.useColor = false; // false: tanpa kode warna, karena kode ANSI di berkas teks akan tampil sebagai karakter aneh.
    style.useUnicode = false; // false: garis kotak memakai + - | biasa.
    return style; // mengembalikan gaya polos yang sudah lengkap.
} // akhir fungsi plainStyle()

// glyphsFor() memilih set karakter yang cocok dengan gaya keluaran. Hasilnya berupa referensi (tanda & pada tipe hasil)
// ke elemen array GLYPH_SETS, jadi struct tidak disalin; aman karena GLYPH_SETS tetap ada selama program berjalan.
// Hasilnya tidak boleh diubah oleh pemanggil, dan fungsi ini hanya dipakai di berkas ini.
static const BoxGlyphs& glyphsFor(const OutputStyle& style) { // glyphsFor(style) mengembalikan set karakter garis untuk style.
    return GLYPH_SETS[style.useUnicode ? 1 : 0]; // style.useUnicode ? 1 : 0 memilih nomor 1 (UTF-8) bila useUnicode true, nomor 0 (ASCII) bila false.
} // akhir fungsi glyphsFor()

// colorize() membungkus teks dengan kode warna gayanya: kode pembuka + teks + ANSI_RESET. Hasilnya teks baru, jadi bisa
// langsung ditulis ke aliran keluaran atau disambung dengan teks lain.
std::string colorize(const OutputStyle& style, TextStyle textStyle, const std::string& text) { // colorize(style, textStyle, text) memberi warna pada text.
    if (!style.useColor || textStyle == STYLE_NORMAL || text.empty()) { // bila warna mati, gayanya normal, atau teksnya kosong, tidak ada yang perlu diwarnai,
        return text; // jadi text dikembalikan apa adanya; laporan berkas selalu lewat jalan ini karena useColor-nya false.
    } // akhir if
    return std::string(ANSI_CODES[textStyle]) + text + ANSI_RESET; // ANSI_CODES[textStyle] mengambil kode warna dengan nilai enum sebagai nomor elemen;
    // kode itu dijadikan std::string, lalu text dan ANSI_RESET disambung di belakangnya dengan tanda +.
} // akhir fungsi colorize()

// barGlyph() memberikan karakter batang grafik kepada report.cpp (tabel peringkat biaya) tanpa memperlihatkan
// array GLYPH_SETS ke berkas lain.
const char* barGlyph(const OutputStyle& style) { // barGlyph(style) mengembalikan blok penuh pada mode UTF-8 atau # pada mode ASCII.
    return glyphsFor(style).bar; // glyphsFor(style).bar mengambil anggota bar dari set karakter yang terpilih.
} // akhir fungsi barGlyph()

// boxInnerLimit() menghitung lebar isi terbesar sebuah kotak: lebar keluaran dikurangi bingkai (4) dan margin kanan (1).
// Dipakai drawBox() dan drawTableBox() di berkas ini, serta printMenu() di app.cpp untuk menilai apakah keterangan menu muat.
std::size_t boxInnerLimit(const OutputStyle& style) { // boxInnerLimit(style) mengembalikan lebar isi kotak dalam kolom.
    const std::size_t width = static_cast<std::size_t>(style.width); // width diubah dari int menjadi std::size_t supaya sejenis dengan konstanta di bawah.
    if (width < BOX_FRAME_WIDTH + RIGHT_MARGIN + MIN_BOX_INNER) { // width < 4 + 1 + 10 (= 15) artinya keluaran terlalu sempit untuk kotak yang wajar.
        return MIN_BOX_INNER; // maka dipakai lebar isi 10; ini juga mencegah pengurangan di bawah menjadi "negatif" (std::size_t tidak punya nilai negatif).
        // Dalam pemakaian biasa lebar paling sedikit 40 kolom, jadi cabang ini hanya jaga-jaga.
    } // akhir if
    return width - BOX_FRAME_WIDTH - RIGHT_MARGIN; // width - 4 - 1, misalnya lebar 80 memberi isi kotak 75 kolom.
} // akhir fungsi boxInnerLimit()

// Menentukan indentasi baris lanjutan saat sebuah baris kotak harus dilipat. Baris berbentuk
// "Label : nilai" dilanjutkan tepat di bawah nilainya; baris lain mengikuti indentasi awalnya.
static std::size_t hangingIndentOf(const std::string& text, std::size_t maxWidth) { // hangingIndentOf(text, maxWidth) mengembalikan banyaknya spasi baris lanjutan.
    // find() menghasilkan std::string::npos bila " : " tidak ada di dalam text (lihat penjelasan npos di input_parse.cpp).
    const std::size_t colon = text.find(" : "); // text.find(" : ") mencari posisi pertama " : " di dalam text.
    if (colon != std::string::npos) { // colon != npos artinya baris ini berbentuk "Label : nilai".
        const std::size_t valueColumn = displayWidth(text.substr(0, colon + 3)); // text.substr(0, colon + 3) adalah bagian "Label : " (3 = panjang " : ");
        // lebarnya sama dengan kolom tempat nilai dimulai.
        if (valueColumn <= maxWidth / 2) { // nilai dilanjutkan di kolomnya hanya bila label tidak lebih dari separuh lebar,
            // jadi baris lanjutan masih punya paling sedikit separuh baris untuk isinya.
            return valueColumn; // baris lanjutan dimulai tepat di bawah awal nilai.
        } // akhir if valueColumn
    } // akhir if colon
    std::size_t indent = 0; // indent menghitung spasi di awal teks, mulai dari 0.
    while (indent < text.size() && text[indent] == ' ') { // selama belum sampai ujung teks dan karakter ke-indent masih spasi; batas ukuran diperiksa
        // lebih dulu supaya text[indent] tidak pernah membaca di luar teks.
        indent = indent + 1; // indent bertambah satu untuk setiap spasi di depan.
    } // akhir perulangan while
    return indent; // baris lanjutan menjorok sama dengan baris pertamanya.
} // akhir fungsi hangingIndentOf()

// drawBox() menggambar kotak berjudul berisi baris-baris lines ke aliran out dalam tiga langkah: lipat, ukur, lalu gambar.
// Kotak ini dipakai antara lain untuk banner, menu utama, ringkasan komponen, total keseluruhan, dan semua tabel (lewat drawTableBox()).
void drawBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawBox(out, style, title, lines): out tujuan tulisan, title judul kotak,
             const std::vector<StyledLine>& lines) { // lines baris isinya; dikirim sebagai const & supaya vector tidak disalin dan tidak diubah.
    const BoxGlyphs& glyphs = glyphsFor(style); // glyphs adalah set karakter garis yang dipakai sepanjang fungsi ini.
    const std::size_t maxInner = boxInnerLimit(style); // maxInner adalah lebar isi terbesar yang masih muat di keluaran.

    // Langkah 1: lipat setiap baris supaya muat, sambil membawa gaya barisnya ke potongannya.
    std::vector<StyledLine> wrapped; // wrapped menampung baris hasil lipatan; vector karena banyaknya baru diketahui setelah dilipat.
    for (std::size_t i = 0; i < lines.size(); ++i) { // perulangan untuk setiap baris masukan, i dari 0 sampai lines.size() - 1.
        const std::vector<std::string> pieces = wrapText(lines[i].text, maxInner, hangingIndentOf(lines[i].text, maxInner)); // pieces: potongan baris ke-i,
        // masing-masing paling lebar maxInner; baris lanjutan menjorok sesuai hangingIndentOf().
        for (std::size_t k = 0; k < pieces.size(); ++k) { // perulangan untuk setiap potongan.
            wrapped.push_back(StyledLine(pieces[k], lines[i].style)); // potongan ke-k disimpan bersama gaya baris asalnya.
        } // akhir perulangan potongan
    } // akhir perulangan baris

    // Langkah 2: lebar kotak mengikuti baris terpanjang (atau judulnya), jadi kotak tidak
    // lebih lebar dari yang diperlukan.
    const std::string shownTitle = truncateText(title, maxInner - 1); // shownTitle adalah judul yang dipotong (diberi ".." di ujung) bila lebih lebar dari maxInner - 1.
    // Batasnya maxInner - 1 karena isi kotak harus 1 kolom lebih lebar dari judul (lihat Langkah 3).
    std::size_t inner = 0; // inner adalah lebar isi kotak yang sebenarnya, mulai 0 lalu diperbesar.
    for (std::size_t i = 0; i < wrapped.size(); ++i) { // memeriksa setiap baris hasil lipatan.
        if (displayWidth(wrapped[i].text) > inner) { // bila baris ini lebih lebar dari inner sejauh ini,
            inner = displayWidth(wrapped[i].text); // inner diganti lebar baris ini; pola ini mencari nilai terbesar.
        } // akhir if
    } // akhir perulangan for
    if (!shownTitle.empty() && displayWidth(shownTitle) + 1 > inner) { // bila ada judul dan lebar judul + 1 melebihi isi,
        inner = displayWidth(shownTitle) + 1; // kotak dilebarkan mengikuti judul.
    } // akhir if judul

    // Langkah 3: garis atas (dengan judul), isi, lalu garis bawah.
    // Di garis atas, judul memakai 3 kolom tambahan (satu garis datar dan dua spasi), sedangkan baris isi hanya punya
    // 2 spasi dalam; karena itu inner minimal lebar judul + 1 supaya semua baris kotak sama panjang.
    if (shownTitle.empty()) { // tanpa judul: garis atas hanya berisi sudut dan garis datar.
        // Operator << berfungsi untuk mengirim (menulis) nilai di kanannya ke aliran keluaran di kirinya, di sini out.
        // Beberapa << boleh dirangkai dalam satu pernyataan; nilainya ditulis berurutan dari kiri ke kanan.
        out << colorize(style, STYLE_DIM, glyphs.topLeft + repeatText(glyphs.horizontal, inner + 2) + glyphs.topRight) << '\n'; // garis datar sepanjang
        // inner + 2 (isi dan dua spasi dalam) diapit dua sudut atas, berwarna abu-abu, lalu '\n' pindah baris.
    } else { // dengan judul: judul disisipkan di garis atas.
        const std::size_t fill = inner - displayWidth(shownTitle) - 1; // fill adalah banyaknya garis datar sesudah judul supaya garis atas sama panjang dengan isi.
        out << colorize(style, STYLE_DIM, std::string(glyphs.topLeft) + glyphs.horizontal + " ") // sudut kiri atas, satu garis datar, dan spasi;
        // topLeft dibungkus std::string(...) karena dua const char* tidak bisa disambung langsung dengan tanda +.
            << colorize(style, STYLE_TITLE, shownTitle) // judul kotak dengan gaya judul (cyan tebal).
            << colorize(style, STYLE_DIM, " " + repeatText(glyphs.horizontal, fill) + glyphs.topRight) << '\n'; // spasi, garis datar sebanyak fill, sudut kanan atas.
    } // akhir if-else garis atas
    for (std::size_t i = 0; i < wrapped.size(); ++i) { // menulis setiap baris isi.
        out << colorize(style, STYLE_DIM, glyphs.vertical) << ' ' // garis tegak kiri lalu satu spasi dalam.
            << colorize(style, wrapped[i].style, padRight(wrapped[i].text, inner)) // isi baris dengan gayanya, ditambah spasi kanan sampai selebar inner;
            // padRight dikerjakan sebelum colorize karena kode warna tidak tampil tetapi ikut terhitung lebarnya.
            << ' ' << colorize(style, STYLE_DIM, glyphs.vertical) << '\n'; // satu spasi dalam, garis tegak kanan, lalu pindah baris.
    } // akhir perulangan baris isi
    out << colorize(style, STYLE_DIM, glyphs.bottomLeft + repeatText(glyphs.horizontal, inner + 2) + glyphs.bottomRight) << '\n'; // garis bawah,
    // sama panjang dengan garis atas.
} // akhir fungsi drawBox()

// Menyusun satu baris tabel: tiap sel diratakan sesuai kolomnya lalu disambung dengan jarak antarkolom.
static std::string joinTableCells(const Table& table, const std::vector<std::string>& cells, // joinTableCells(table, cells, widths): table memberi arah perataan,
                                  const std::vector<std::size_t>& widths) { // cells isi sel baris ini, dan widths lebar tiap kolom. Hasilnya satu baris teks.
    std::string line; // line adalah baris yang sedang disusun, awalnya kosong.
    for (std::size_t column = 0; column < table.columns.size(); ++column) { // perulangan untuk setiap kolom tabel.
        const std::string cell = column < cells.size() ? cells[column] : std::string(); // cell adalah isi sel kolom ini; bila baris punya sel lebih sedikit,
        // dipakai teks kosong std::string() supaya tidak membaca di luar vector.
        if (column > 0) { // sebelum setiap kolom kecuali yang pertama,
            line += std::string(COLUMN_GAP, ' '); // std::string(COLUMN_GAP, ' ') membuat teks berisi 2 spasi sebagai jarak antarkolom.
        } // akhir if
        line += table.columns[column].alignRight ? padLeft(cell, widths[column]) : padRight(cell, widths[column]); // kolom rata kanan diberi spasi di kiri,
        // kolom rata kiri diberi spasi di kanan, sampai selebar widths[column].
    } // akhir perulangan kolom
    return line; // mengembalikan baris tabel yang sudah rata.
} // akhir fungsi joinTableCells()

// Mengubah tabel menjadi baris-baris teks rata kolom. Lebar tiap kolom mengikuti isi terlebarnya.
// Hasil false berarti tabel lebih lebar dari maxWidth dan tidak ada baris yang ditambahkan.
// Baris hasilnya ditambahkan ke parameter lines (referensi tanpa const), jadi fungsi ini memberi dua hasil sekaligus:
// muat atau tidak lewat return, dan baris-baris tabel lewat lines.
static bool buildTableLines(const Table& table, const OutputStyle& style, std::size_t maxWidth, // buildTableLines(...): style untuk memilih karakter garis,
                            std::vector<StyledLine>& lines) { // maxWidth lebar yang tersedia, lines tempat baris hasil ditambahkan.
    const std::size_t columnCount = table.columns.size(); // columnCount adalah banyaknya kolom tabel.
    std::vector<std::size_t> widths(columnCount, 0); // widths(columnCount, 0) membuat vector berisi columnCount angka 0; elemen ke-n adalah lebar kolom ke-n.
    for (std::size_t column = 0; column < columnCount; ++column) { // menghitung lebar setiap kolom.
        widths[column] = displayWidth(table.columns[column].header); // lebar awal kolom adalah lebar judulnya.
        for (std::size_t row = 0; row < table.rows.size(); ++row) { // lalu isi kolom ini di setiap baris ikut diukur.
            if (column < table.rows[row].cells.size() && displayWidth(table.rows[row].cells[column]) > widths[column]) { // bila selnya ada dan lebih lebar;
                // syarat pertama diperiksa lebih dulu supaya cells[column] tidak dibaca bila sel itu tidak ada.
                widths[column] = displayWidth(table.rows[row].cells[column]); // lebar kolom diperbesar mengikuti sel itu.
            } // akhir if
        } // akhir perulangan baris
        if (table.hasFooter && column < table.footer.cells.size() && displayWidth(table.footer.cells[column]) > widths[column]) { // baris penutup juga diukur.
            widths[column] = displayWidth(table.footer.cells[column]); // lebar kolom mengikuti sel penutup bila sel itu yang terlebar.
        } // akhir if footer
    } // akhir perulangan kolom

    std::size_t naturalWidth = 0; // naturalWidth adalah lebar tabel bila semua kolom ditulis utuh.
    for (std::size_t column = 0; column < columnCount; ++column) { // menjumlahkan lebar semua kolom.
        naturalWidth = naturalWidth + widths[column]; // lebar kolom ke-column ditambahkan.
    } // akhir perulangan for
    if (columnCount > 1) { // jarak antarkolom hanya ada bila kolomnya lebih dari satu; syarat ini juga mencegah columnCount - 1 dihitung saat columnCount 0.
        naturalWidth = naturalWidth + COLUMN_GAP * (columnCount - 1); // n kolom punya n - 1 jarak, masing-masing 2 spasi.
    } // akhir if
    if (naturalWidth > maxWidth) { // tabel tidak muat di dalam kotak,
        return false; // jadi fungsi berhenti tanpa menambah baris; pemanggilnya lalu memakai bentuk kartu.
    } // akhir if

    std::vector<std::string> headers; // headers berisi judul tiap kolom, disusun seperti sel sebuah baris.
    std::vector<std::string> rules; // rules berisi garis datar selebar tiap kolom, untuk garis pemisah.
    for (std::size_t column = 0; column < columnCount; ++column) { // mengisi headers dan rules untuk setiap kolom.
        headers.push_back(table.columns[column].header); // judul kolom ditambahkan ke ujung headers.
        rules.push_back(repeatText(glyphsFor(style).horizontal, widths[column])); // garis datar sepanjang lebar kolom ditambahkan ke rules.
    } // akhir perulangan kolom
    const std::string ruleLine = joinTableCells(table, rules, widths); // ruleLine adalah garis pemisah utuh; disimpan karena dipakai sampai dua kali.

    lines.push_back(StyledLine(joinTableCells(table, headers, widths), STYLE_BOLD)); // baris judul kolom, ditulis tebal.
    lines.push_back(StyledLine(ruleLine, STYLE_DIM)); // garis pemisah di bawah judul kolom, berwarna abu-abu.
    for (std::size_t row = 0; row < table.rows.size(); ++row) { // setiap baris data tabel
        lines.push_back(StyledLine(joinTableCells(table, table.rows[row].cells, widths), table.rows[row].style)); // ditambahkan dengan gayanya sendiri
        // (misalnya STYLE_HIGHLIGHT untuk komponen termahal).
    } // akhir perulangan baris
    if (table.hasFooter) { // bila tabel punya baris penutup,
        lines.push_back(StyledLine(ruleLine, STYLE_DIM)); // garis pemisah lagi sebelum baris penutup.
        lines.push_back(StyledLine(joinTableCells(table, table.footer.cells, widths), STYLE_BOLD)); // baris penutup (misalnya TOTAL di rekap), ditulis tebal.
    } // akhir if
    return true; // tabel muat dan semua barisnya sudah ditambahkan ke lines.
} // akhir fungsi buildTableLines()

// Bentuk pengganti tabel untuk terminal sempit: tiap baris tabel menjadi "kartu". Dua kolom
// pertama (nomor dan nama) menjadi judul kartu, kolom lainnya ditulis "Judul kolom : isi".
// Baris penutup tabel (footer) tidak ikut ditulis dalam bentuk kartu, dan sel yang kosong dilewati.
static void buildCardLines(const Table& table, std::vector<StyledLine>& lines) { // buildCardLines(table, lines) menambahkan baris-baris kartu ke lines.
    std::size_t labelWidth = 0; // labelWidth adalah lebar judul kolom terpanjang (mulai kolom ketiga), supaya tanda : di setiap kartu lurus.
    for (std::size_t column = CARD_TITLE_COLUMNS; column < table.columns.size(); ++column) { // dimulai dari nomor 2 (kolom ketiga) karena dua kolom pertama
        // sudah menjadi judul kartu.
        if (displayWidth(table.columns[column].header) > labelWidth) { // bila judul kolom ini lebih lebar dari labelWidth,
            labelWidth = displayWidth(table.columns[column].header); // labelWidth diganti lebar judul itu.
        } // akhir if
    } // akhir perulangan kolom
    for (std::size_t row = 0; row < table.rows.size(); ++row) { // satu kartu untuk setiap baris tabel.
        const std::vector<std::string>& cells = table.rows[row].cells; // cells adalah nama pendek (referensi) untuk sel baris ini, tanpa menyalinnya.
        if (row > 0) { // sebelum setiap kartu kecuali yang pertama,
            lines.push_back(StyledLine("", STYLE_NORMAL)); // ditambahkan baris kosong sebagai pemisah antarkartu.
        } // akhir if
        std::string title = cells.size() > 0 ? cells[0] : std::string(); // title diawali isi kolom pertama (nomor), atau teks kosong bila baris tanpa sel.
        if (cells.size() > 1) { // bila ada kolom kedua (nama),
            title += ". " + cells[1]; // judul menjadi "nomor. nama", misalnya "1. Gear".
        } // akhir if
        lines.push_back(StyledLine(title, table.rows[row].style == STYLE_NORMAL ? STYLE_BOLD : table.rows[row].style)); // judul kartu ditulis tebal,
        // kecuali baris itu punya gaya khusus (misalnya sorotan), yang tetap dipakai.
        for (std::size_t column = CARD_TITLE_COLUMNS; column < table.columns.size() && column < cells.size(); ++column) { // kolom ketiga dan seterusnya
            // yang selnya ada di baris ini.
            if (cells[column].empty()) { // sel kosong
                continue; // dilewati, jadi kartu tidak memuat baris "Judul :" tanpa isi.
            } // akhir if
            lines.push_back(StyledLine(CARD_INDENT + padRight(table.columns[column].header, labelWidth) + " : " + cells[column], // baris "Judul : isi",
                                       STYLE_NORMAL)); // menjorok 3 spasi dengan judul diratakan selebar labelWidth, bergaya biasa.
        } // akhir perulangan kolom
    } // akhir perulangan baris
} // akhir fungsi buildCardLines()

// drawTableBox() menggambar tabel di dalam kotak. Bentuk tabel biasa dicoba lebih dulu; bila tidak muat di lebar
// keluaran, dipakai bentuk kartu. Catatan ditambahkan di bawahnya, lalu semuanya digambar oleh drawBox().
void drawTableBox(std::ostream& out, const OutputStyle& style, const std::string& title, // drawTableBox(out, style, title, table, notes): sama dengan drawBox(),
                  const Table& table, const std::vector<StyledLine>& notes) { // ditambah table (isi tabel) dan notes (catatan di bawah tabel, boleh kosong).
    const std::size_t maxInner = boxInnerLimit(style); // maxInner adalah lebar isi terbesar kotak.
    std::vector<StyledLine> lines; // lines akan berisi semua baris isi kotak: tabel atau kartu, lalu catatan.
    if (!buildTableLines(table, style, maxInner, lines)) { // bila tabel tidak muat (hasilnya false),
        buildCardLines(table, lines); // dipakai baris-baris kartu sebagai gantinya.
    } // akhir if
    if (!notes.empty()) { // bagian catatan hanya dikerjakan bila ada catatan.
        // Catatan dilipat selebar tabelnya (paling sempit MIN_NOTE_WIDTH kolom), supaya kotak
        // tidak melebar hanya karena kalimat catatan yang panjang.
        std::size_t noteWidth = MIN_NOTE_WIDTH; // noteWidth dimulai dari 44 kolom.
        for (std::size_t i = 0; i < lines.size(); ++i) { // mencari baris tabel atau kartu yang paling lebar.
            if (displayWidth(lines[i].text) > noteWidth) { // bila baris ini lebih lebar dari noteWidth,
                noteWidth = displayWidth(lines[i].text); // noteWidth mengikuti baris itu.
            } // akhir if
        } // akhir perulangan for
        if (noteWidth > maxInner) { // tetapi catatan tidak boleh lebih lebar dari isi kotak (misalnya isi kotak di bawah 44 kolom),
            noteWidth = maxInner; // jadi dibatasi maxInner.
        } // akhir if
        lines.push_back(StyledLine("", STYLE_NORMAL)); // baris kosong pemisah antara tabel dan catatan.
        for (std::size_t i = 0; i < notes.size(); ++i) { // setiap catatan
            const std::vector<std::string> pieces = wrapText(notes[i].text, noteWidth, 0); // dilipat selebar noteWidth tanpa indentasi baris lanjutan (0).
            for (std::size_t k = 0; k < pieces.size(); ++k) { // setiap potongan catatan
                lines.push_back(StyledLine(pieces[k], notes[i].style)); // ditambahkan dengan gaya catatannya (di program ini selalu STYLE_DIM).
            } // akhir perulangan potongan
        } // akhir perulangan catatan
    } // akhir if notes
    drawBox(out, style, title, lines); // akhirnya semua baris digambar sebagai satu kotak berjudul.
} // akhir fungsi drawTableBox()

// printWrapped() mencetak teks tanpa kotak, dilipat selebar keluaran dikurangi margin kanan. Dipakai antara lain untuk baris
// keadaan di bawah menu (app.cpp), pesan daftar kosong (report.cpp), dan ketiga fungsi pesan singkat di bawah.
void printWrapped(std::ostream& out, const OutputStyle& style, TextStyle textStyle, // printWrapped(out, style, textStyle, text, hangingIndent): textStyle gaya teks,
                  const std::string& text, std::size_t hangingIndent) { // text isi yang dicetak, hangingIndent banyaknya spasi di depan baris lanjutan.
    const std::size_t width = static_cast<std::size_t>(style.width) - RIGHT_MARGIN; // width adalah lebar keluaran dikurangi satu kolom kosong di kanan;
    // style.width diubah dulu ke std::size_t supaya sejenis dengan RIGHT_MARGIN.
    const std::vector<std::string> pieces = wrapText(text, width, hangingIndent); // pieces adalah potongan-potongan teks hasil lipatan.
    for (std::size_t i = 0; i < pieces.size(); ++i) { // setiap potongan
        out << colorize(style, textStyle, pieces[i]) << '\n'; // diberi warna lalu ditulis sebagai satu baris; tiap baris membawa kode pembuka dan penutup warnanya sendiri.
    } // akhir perulangan for
} // akhir fungsi printWrapped()

// Tiga fungsi pesan singkat di bawah selalu menulis ke layar dengan gaya layar saat ini. Awalan [!], [OK], dan [i]
// membuat jenis pesan tetap bisa dikenali walaupun warna dimatikan. Indentasi baris lanjutan sama dengan panjang
// awalannya (4 untuk "[!] " dan "[i] ", 5 untuk "[OK] "), jadi baris kedua dan seterusnya lurus dengan awal kalimat.
void printError(const std::string& message) { // printError(message) menampilkan pesan kesalahan, misalnya ketikan yang tidak sah.
    // std::cout adalah aliran keluaran standar, yaitu saluran tulisan biasa ke layar; nilai dikirim ke sana dengan operator <<.
    // Pasangannya, std::cerr, adalah saluran khusus pesan galat; app.cpp memakainya untuk opsi yang tidak dikenal dan masukan
    // yang habis, jadi pesan itu tetap terlihat walau keluaran biasa dialihkan ke berkas. Keduanya bertipe std::ostream,
    // sehingga bisa dikirim ke parameter out milik fungsi gambar.
    printWrapped(std::cout, screenStyle(), STYLE_ERROR, "[!] " + message, 4); // "[!] " + message dicetak merah tebal; 4 adalah panjang "[!] ".
} // akhir fungsi printError()

void printSuccess(const std::string& message) { // printSuccess(message) menampilkan pesan berhasil, misalnya komponen tersimpan.
    printWrapped(std::cout, screenStyle(), STYLE_SUCCESS, "[OK] " + message, 5); // "[OK] " + message dicetak hijau tebal; 5 adalah panjang "[OK] ".
} // akhir fungsi printSuccess()

void printInfo(const std::string& message) { // printInfo(message) menampilkan keterangan, misalnya petunjuk atau aksi yang dibatalkan.
    printWrapped(std::cout, screenStyle(), STYLE_DIM, "[i] " + message, 4); // "[i] " + message dicetak abu-abu; 4 adalah panjang "[i] ".
} // akhir fungsi printInfo()
