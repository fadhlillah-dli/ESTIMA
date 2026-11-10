// ===== text_format.cpp - pembentukan teks: angka, rupiah, berat, perataan, dan pelipatan baris =====
// Berkas ini berisi isi (definisi) fungsi-fungsi yang daftarnya ada di text_format.h. Fungsi di sini hanya menyusun teks
// lalu mengembalikannya; pencetakan ke layar atau ke berkas dikerjakan pemanggilnya (ui.cpp, report.cpp, app.cpp, prompt.cpp).
#include "estima/platform.h" // "estima/platform.h" selalu di-#include paling awal supaya penyesuaian compiler di dalamnya berlaku sebelum header lain dibaca.

#include "estima/text_format.h" // "estima/text_format.h" adalah header milik berkas ini; disertakan supaya compiler mencocokkan definisi di bawah dengan deklarasinya.

#include <cmath> // <cmath> digunakan untuk std::floor, yaitu pembulatan ke bawah yang dipakai saat membulatkan rupiah, gram, dan persen.
#include <cstddef> // <cstddef> digunakan untuk tipe std::size_t, tipe lebar dan posisi teks di seluruh berkas ini.
#include <iomanip> // <iomanip> digunakan untuk pengatur tampilan angka: std::setprecision, std::setw, dan std::setfill.
#include <sstream> // <sstream> digunakan untuk std::ostringstream, yaitu "kertas" tempat menulis angka menjadi teks.
#include <string> // <string> digunakan untuk std::string, tipe masukan dan hasil hampir semua fungsi di sini.
#include <vector> // <vector> digunakan untuk std::vector, tipe hasil wrapText() yang berisi baris-baris hasil lipatan.

// formatThousands() menjadi dasar semua penulisan angka di program. Angka diubah dulu menjadi deret digit memakai
// std::ostringstream, bukan std::to_string, karena std::to_string belum ada di C++98 yang mungkin dipakai compiler lama.
// Sesudah itu titik ribuan disisipkan sendiri, karena aturan C++ bawaan tidak mengenal titik ribuan gaya Indonesia.
std::string formatThousands(double wholeNumber) { // formatThousands(double wholeNumber) berfungsi untuk menulis angka dengan titik ribuan, misalnya "1.250.000".
    // Angka diubah dulu menjadi deret digit tanpa desimal, misalnya "1250000".
    // std::ostringstream adalah aliran keluaran yang menulis ke dalam sebuah teks, bukan ke layar. Cara menulisnya sama
    // dengan mencetak (memakai <<), lalu teks hasilnya diambil dengan .str(). Cocok untuk mengubah angka menjadi teks.
    std::ostringstream stream; // stream adalah ostringstream kosong yang akan menampung angka dalam bentuk teks.
    // std::fixed berfungsi untuk memaksa angka ditulis biasa (bukan bentuk ilmiah seperti 1.25e+06), dan
    // std::setprecision(n) berfungsi untuk menentukan banyaknya angka di belakang titik desimal saat std::fixed aktif.
    stream << std::fixed << std::setprecision(0) << wholeNumber; // std::fixed << std::setprecision(0) artinya wholeNumber ditulis utuh tanpa desimal;
    // tanpa std::fixed, setprecision(0) berarti hanya satu angka penting, sehingga 1250000 tertulis "1e+06": nilainya ikut berubah
    // dan tidak bisa diberi titik ribuan.
    const std::string digits = stream.str(); // digits = stream.str() artinya teks yang ditulis ke stream diambil, misalnya "1250000"; const karena tidak diubah.

    // Titik disisipkan tepat sebelum digit yang banyak digit dari dirinya sampai ujung kanan merupakan kelipatan tiga: 1.250.000.
    std::string result; // result adalah teks hasil yang disusun satu karakter demi satu karakter; awalnya kosong.
    // Fungsi anggota std::string yang dipakai di program ini (dipanggil dengan tanda titik, misalnya teks.size()):
    // size() memberi banyaknya byte (karakter) di dalam teks; substr(awal, panjang) mengambil potongan teks;
    // find(x) mencari posisi x di dalam teks (hasilnya std::string::npos bila tidak ketemu); erase(awal, panjang) membuang
    // sebagian teks; assign(n, c) mengganti seluruh isi teks dengan n buah karakter c; compare(...) membandingkan sebagian
    // teks dengan teks lain (hasil 0 berarti sama); empty() bernilai true bila teks kosong.
    // size() dan substr() dipakai di text_format.cpp, ui.cpp, dan input_parse.cpp; find() di ui.cpp dan input_parse.cpp; compare()
    // hanya di input_parse.cpp; erase() dan assign() hanya di text_format.cpp; empty() dipakai di banyak bagian program.
    const std::size_t length = digits.size(); // length = digits.size() adalah banyaknya digit; disimpan sekali karena dipakai berulang di dalam perulangan.
    for (std::size_t i = 0; i < length; ++i) { // for (i = 0; i < length; ++i) berfungsi untuk mendatangi setiap digit dari kiri ke kanan.
        // % (sisa bagi) berfungsi untuk menghasilkan sisa pembagian; misalnya 7 % 3 = 1, dan 6 % 3 = 0 (habis dibagi).
        if (i > 0 && (length - i) % 3 == 0) { // (length - i) % 3 == 0 artinya banyaknya digit dari posisi i sampai ujung kanan adalah kelipatan tiga;
            // i > 0 mencegah titik di paling depan (misalnya ".250" untuk angka 250).
            // char adalah tipe data untuk SATU karakter (satu byte), ditulis di antara tanda petik tunggal seperti '.' atau 'A'.
            // Berbeda dengan "..." (petik ganda) yang berupa teks berisi nol karakter atau lebih.
            result += '.'; // result += '.' artinya karakter titik ditambahkan di ujung result sebagai pemisah ribuan.
        } // akhir if penyisipan titik
        result += digits[i]; // result += digits[i] artinya digit ke-i (diambil dengan [nomor] seperti elemen array) disalin ke ujung result.
    } // akhir perulangan for setiap digit
    return result; // return result; mengembalikan angka yang sudah bertitik ribuan.
} // akhir fungsi formatThousands()

// formatRupiah() cukup menambahkan awalan "Rp" di depan hasil formatThousands(). Nilai dibulatkan lebih dulu sebagai
// pengaman, supaya yang ditulis selalu rupiah utuh walaupun nilai yang dikirim berpecahan.
std::string formatRupiah(double amount) { // formatRupiah(double amount) berfungsi untuk menulis amount sebagai uang, misalnya 821400 menjadi "Rp821.400".
    return "Rp" + formatThousands(std::floor(amount + 0.5)); // std::floor(amount + 0.5) artinya membulatkan ke rupiah terdekat (setengah ke atas naik);
    // tanda + di sini menyambung teks "Rp" dengan angka yang sudah bertitik ribuan.
} // akhir fungsi formatRupiah()

// formatWeight() menulis berat dengan tiga angka desimal karena berat diketik sampai ketelitian gram (maksimal 3 desimal).
std::string formatWeight(double weightKg) { // formatWeight(double weightKg) berfungsi untuk menulis berat, misalnya 44.4 menjadi "44,400 kg".
    // Berat dipecah menjadi kilogram utuh dan sisa gram supaya bagian desimalnya selalu tiga
    // angka (1,850) dan bagian utuhnya tetap mendapat titik ribuan (1.234,500).
    const double totalGram = std::floor(weightKg * 1000.0 + 0.5); // totalGram adalah berat dalam gram, dibulatkan ke gram terdekat; misalnya 44.4 kg menjadi 44400 gram.
    const double wholeKg = std::floor(totalGram / 1000.0); // wholeKg adalah bagian kilogram utuh; 44400 / 1000 = 44,4 lalu dibulatkan ke bawah menjadi 44.
    const int gramPart = static_cast<int>(totalGram - wholeKg * 1000.0); // gramPart adalah sisa gram (0 sampai 999); 44400 - 44000 = 400.
    // Dijadikan int karena nilainya sudah pasti bilangan bulat 0 sampai 999, jadi std::setw dan std::setfill di bawah bekerja pada angka bulat.

    std::ostringstream stream; // stream adalah ostringstream baru tempat menyusun teks berat.
    // std::setw(n) berfungsi untuk mengatur lebar minimum nilai BERIKUTNYA yang ditulis (hanya berlaku untuk satu nilai), dan
    // std::setfill(c) berfungsi untuk menentukan karakter pengisi bila nilainya lebih pendek dari lebar itu (bawaannya spasi).
    stream << formatThousands(wholeKg) << ',' << std::setw(3) << std::setfill('0') << gramPart << " kg"; // kg utuh, koma, gram 3 digit, lalu " kg".
    // std::setw(3) << std::setfill('0') << gramPart artinya gram selalu ditulis tiga digit dengan nol di depan: 5 gram menjadi "005", jadi 1,005 kg.
    return stream.str(); // return stream.str(); mengembalikan teks yang sudah tersusun, misalnya "44,400 kg".
} // akhir fungsi formatWeight()

// formatPercent() memakai cara yang sama dengan formatWeight(), tetapi dengan satu angka desimal (persepuluhan).
std::string formatPercent(double percent) { // formatPercent(double percent) berfungsi untuk menulis persen, misalnya 56.512 menjadi "56,5%".
    const double tenths = std::floor(percent * 10.0 + 0.5); // tenths adalah persen dalam satuan persepuluhan, dibulatkan; 56.512 x 10 = 565,12 lalu menjadi 565.
    const double whole = std::floor(tenths / 10.0); // whole adalah bagian persen yang utuh; 565 / 10 = 56,5 lalu dibulatkan ke bawah menjadi 56.
    const int tenthPart = static_cast<int>(tenths - whole * 10.0); // tenthPart adalah satu angka di belakang koma (0 sampai 9); 565 - 560 = 5.

    std::ostringstream stream; // stream adalah ostringstream baru tempat menyusun teks persen.
    stream << formatThousands(whole) << ',' << tenthPart << '%'; // bagian utuh, koma, satu angka desimal, lalu tanda '%'; hasilnya misalnya "56,5%".
    return stream.str(); // return stream.str(); mengembalikan teks persen yang sudah tersusun.
} // akhir fungsi formatPercent()

// true bila byte ini adalah byte lanjutan UTF-8 (berpola 10xxxxxx), yaitu bagian dari karakter
// yang byte awalnya sudah dihitung.
// Dasarnya: dalam UTF-8, satu karakter ditulis dengan 1 sampai 4 byte. Byte pertama sebuah karakter tidak pernah berpola
// 10xxxxxx, sedangkan byte kedua dan seterusnya selalu berpola itu. Jadi dengan melewati byte lanjutan, satu karakter
// terhitung tepat satu kali. Fungsi ini static karena hanya dipakai oleh fungsi lain di berkas ini.
static bool isContinuationByte(char byte) { // isContinuationByte(char byte) berfungsi untuk memeriksa apakah byte adalah byte lanjutan UTF-8.
    // unsigned char adalah char tanpa tanda negatif (nilai 0 sampai 255). Dipakai karena di banyak compiler char biasa
    // bertanda, sehingga byte UTF-8 (nilai 128 ke atas) terbaca negatif.
    // 0xC0 dan 0x80 adalah bilangan heksadesimal (basis 16): 0xC0 = 11000000 dan 0x80 = 10000000 dalam biner.
    // & (AND per bit) berfungsi untuk menyisakan bit yang bernilai 1 di kedua sisi; & 0xC0 artinya hanya dua bit teratas yang dilihat.
    return (static_cast<unsigned char>(byte) & 0xC0) == 0x80; // (byte & 0xC0) == 0x80 artinya dua bit teratas byte adalah 10, yaitu pola byte lanjutan.
} // akhir fungsi isContinuationByte()

// displayWidth() menghitung karakter, bukan byte, dengan menganggap setiap karakter selebar satu kolom. Anggapan ini
// benar untuk huruf ASCII dan untuk karakter garis kotak serta blok grafik yang dipakai program ini.
std::size_t displayWidth(const std::string& text) { // displayWidth(const std::string& text) berfungsi untuk menghitung lebar text di layar dalam kolom.
    std::size_t width = 0; // width adalah pencacah kolom; mulai 0 lalu bertambah satu untuk setiap karakter.
    for (std::size_t i = 0; i < text.size(); ++i) { // for (i = 0; i < text.size(); ++i) berfungsi untuk memeriksa setiap byte text dari awal sampai akhir.
        if (!isContinuationByte(text[i])) { // !isContinuationByte(text[i]) artinya byte ini adalah awal sebuah karakter, bukan byte lanjutan.
            width = width + 1; // width = width + 1 artinya satu karakter (satu kolom) lagi terhitung.
        } // akhir if awal karakter
    } // akhir perulangan for setiap byte
    return width; // return width; mengembalikan lebar text dalam kolom.
} // akhir fungsi displayWidth()

// padRight() dan padLeft() meratakan teks dengan spasi. Teks yang sudah selebar width atau lebih dikembalikan utuh,
// tidak dipotong, supaya isi sel tidak pernah hilang; lebar kolom tabel sendiri sudah dihitung dari isi terlebarnya.
std::string padRight(const std::string& text, std::size_t width) { // padRight(text, width) berfungsi untuk membuat text rata kiri selebar width kolom.
    const std::size_t current = displayWidth(text); // current adalah lebar text sekarang (dalam kolom, bukan byte).
    if (current >= width) { // current >= width artinya text sudah cukup lebar, jadi tidak perlu ditambah spasi.
        return text; // return text; mengembalikan text apa adanya.
    } // akhir if teks sudah cukup lebar
    // std::string(n, ' ') adalah cara membuat teks berisi n buah karakter yang sama, di sini n buah spasi.
    return text + std::string(width - current, ' '); // text + std::string(width - current, ' ') artinya kekurangan lebarnya diisi spasi di kanan.
} // akhir fungsi padRight()

std::string padLeft(const std::string& text, std::size_t width) { // padLeft(text, width) berfungsi untuk membuat text rata kanan selebar width kolom (kolom angka).
    const std::size_t current = displayWidth(text); // current adalah lebar text sekarang dalam kolom.
    if (current >= width) { // current >= width artinya text sudah cukup lebar, jadi dikembalikan tanpa spasi tambahan.
        return text; // return text; mengembalikan text apa adanya.
    } // akhir if teks sudah cukup lebar
    return std::string(width - current, ' ') + text; // std::string(width - current, ' ') + text artinya spasi ditaruh di KIRI, sehingga ujung kanan teks lurus.
} // akhir fungsi padLeft()

// repeatText() menerima sepotong teks (bukan satu char) karena garis kotak dan blok grafik UTF-8 terdiri dari tiga byte.
std::string repeatText(const std::string& piece, std::size_t times) { // repeatText(piece, times) berfungsi untuk mengulang piece sebanyak times kali.
    std::string result; // result adalah teks hasil; awalnya kosong.
    for (std::size_t i = 0; i < times; ++i) { // for (i = 0; i < times; ++i) berfungsi untuk mengulang tepat times kali.
        result += piece; // result += piece artinya piece disambungkan sekali lagi di ujung result.
    } // akhir perulangan for
    return result; // return result; mengembalikan teks yang sudah diulang, misalnya garis "----------".
} // akhir fungsi repeatText()

// Banyaknya byte di awal text yang lebarnya paling banyak maxWidth kolom, tanpa membelah satu
// karakter UTF-8 di tengah.
// Hasilnya berupa banyaknya byte (bukan kolom) karena substr() dan erase() bekerja dengan posisi byte.
static std::size_t prefixBytesForWidth(const std::string& text, std::size_t maxWidth) { // dipakai truncateText() dan wrapText() untuk memotong teks.
    std::size_t width = 0; // width adalah banyaknya kolom (karakter) yang sudah diambil.
    std::size_t index = 0; // index adalah posisi byte berikutnya; di akhir fungsi nilainya sama dengan banyaknya byte yang diambil.
    while (index < text.size() && width < maxWidth) { // while (...) berfungsi untuk mengambil satu karakter per putaran selama text belum habis dan lebar belum penuh.
        index = index + 1; // index = index + 1 artinya byte awal karakter ini diambil.
        while (index < text.size() && isContinuationByte(text[index])) { // while (...) berfungsi untuk ikut mengambil semua byte lanjutan karakter yang sama.
            index = index + 1; // index = index + 1 artinya satu byte lanjutan lagi diambil, supaya karakter UTF-8 tidak terbelah.
        } // akhir while byte lanjutan
        width = width + 1; // width = width + 1 artinya satu karakter utuh (satu kolom) sudah diambil.
    } // akhir while pengambilan karakter
    return index; // return index; mengembalikan banyaknya byte yang muat dalam maxWidth kolom.
} // akhir fungsi prefixBytesForWidth()

// truncateText() dipakai untuk judul kotak yang terlalu panjang. Tanda ".." (dua titik) dipakai sebagai penanda bahwa
// teks dipotong, dan memakan dua kolom; karena itu isi yang tersisa paling lebar maxWidth - 2 kolom.
std::string truncateText(const std::string& text, std::size_t maxWidth) { // truncateText(text, maxWidth) berfungsi untuk memendekkan text supaya muat maxWidth kolom.
    if (displayWidth(text) <= maxWidth) { // displayWidth(text) <= maxWidth artinya text sudah muat, jadi tidak perlu dipotong.
        return text; // return text; mengembalikan text utuh.
    } // akhir if teks sudah muat
    if (maxWidth <= 2) { // maxWidth <= 2 artinya ruangnya terlalu sempit untuk isi ditambah "..", jadi teks dipotong tanpa penanda.
        return text.substr(0, prefixBytesForWidth(text, maxWidth)); // text.substr(0, ...) artinya ambil potongan dari byte ke-0 sebanyak byte yang muat maxWidth kolom.
    } // akhir if ruang sangat sempit
    return text.substr(0, prefixBytesForWidth(text, maxWidth - 2)) + ".."; // potongan selebar maxWidth - 2 kolom ditambah "..", jadi lebarnya tepat maxWidth.
} // akhir fungsi truncateText()

// toLowerAscii() hanya mengubah huruf A-Z dan tidak bergantung pada pengaturan bahasa sistem (berbeda dengan std::tolower
// dari <cctype>), jadi hasilnya sama di semua komputer. Itu cukup, karena nama komponen dan nama berkas yang diterima
// program ini memang dibatasi pada karakter ASCII. Hasilnya dipakai untuk membandingkan teks tanpa membedakan huruf
// besar/kecil: nama komponen kembar, jawaban y/t, dan nama berkas laporan.
std::string toLowerAscii(const std::string& text) { // toLowerAscii(text) berfungsi untuk menghasilkan salinan text dengan huruf A-Z diganti a-z.
    std::string result = text; // result = text artinya text disalin dulu, karena text adalah const & sehingga tidak boleh diubah langsung.
    for (std::size_t i = 0; i < result.size(); ++i) { // for (i = 0; i < result.size(); ++i) berfungsi untuk memeriksa setiap karakter result.
        if (result[i] >= 'A' && result[i] <= 'Z') { // result[i] >= 'A' && result[i] <= 'Z' artinya karakter ini huruf besar; huruf A-Z berurutan di tabel ASCII.
            result[i] = static_cast<char>(result[i] - 'A' + 'a'); // result[i] - 'A' + 'a' artinya posisi huruf dalam abjad (0 untuk A) ditambahkan ke 'a'.
            // Hasil hitungan antar-char berupa int, jadi static_cast<char> mengembalikannya menjadi char sebelum disimpan.
        } // akhir if huruf besar
    } // akhir perulangan for setiap karakter
    return result; // return result; mengembalikan salinan yang sudah huruf kecil.
} // akhir fungsi toLowerAscii()

// wrapText() melipat teks per kata, mirip pengolah kata: kata dimasukkan ke baris yang sedang disusun selama masih muat,
// dan bila tidak muat baris itu ditutup lalu baris baru dimulai. Hasilnya dikumpulkan dalam std::vector karena banyaknya
// baris baru diketahui setelah selesai melipat. Parameter maxWidth dan hangingIndent dikirim sebagai salinan (bukan
// referensi), jadi boleh dikoreksi di dalam fungsi tanpa mengubah variabel milik pemanggil.
std::vector<std::string> wrapText(const std::string& text, std::size_t maxWidth, std::size_t hangingIndent) { // wrapText(...) berfungsi untuk melipat text
    // menjadi baris-baris selebar paling banyak maxWidth kolom, dengan baris lanjutan menjorok hangingIndent spasi.
    std::vector<std::string> lines; // lines adalah daftar baris hasil lipatan; awalnya kosong lalu diisi dengan push_back().
    if (maxWidth == 0) { // maxWidth == 0 artinya tidak ada ruang sama sekali; nilai ini tidak masuk akal untuk melipat.
        maxWidth = 1; // maxWidth = 1 artinya lebar dinaikkan ke satu kolom, supaya pemotongan kata di bawah selalu maju dan tidak berulang tanpa henti.
    } // akhir if lebar nol
    // Teks yang sudah muat dikembalikan apa adanya, termasuk spasi perata di dalamnya.
    if (displayWidth(text) <= maxWidth) { // displayWidth(text) <= maxWidth artinya seluruh text muat dalam satu baris.
        lines.push_back(text); // lines.push_back(text) artinya text dimasukkan utuh sebagai satu-satunya baris.
        return lines; // return lines; langsung mengembalikan daftar berisi satu baris itu.
    } // akhir if teks sudah muat
    // Indentasi harus menyisakan paling sedikit satu kolom untuk isi.
    if (hangingIndent + 1 > maxWidth) { // hangingIndent + 1 > maxWidth artinya setelah menjorok tidak tersisa satu kolom pun untuk kata.
        hangingIndent = 0; // hangingIndent = 0 artinya baris lanjutan tidak dibuat menjorok.
    } // akhir if indentasi terlalu besar

    // Spasi di awal teks dipertahankan sebagai indentasi baris pertama.
    std::size_t position = 0; // position adalah posisi byte di text yang sedang dibaca; maju terus dari kiri ke kanan.
    while (position < text.size() && text[position] == ' ') { // while (...) berfungsi untuk melewati spasi di awal text sambil menghitungnya.
        position = position + 1; // position = position + 1 artinya satu spasi awal dilewati.
    } // akhir while spasi awal
    std::size_t firstIndent = position; // firstIndent adalah banyaknya spasi di awal text, yang menjadi indentasi baris pertama.
    if (firstIndent + 1 > maxWidth) { // firstIndent + 1 > maxWidth artinya indentasi awal tidak menyisakan ruang untuk isi.
        firstIndent = 0; // firstIndent = 0 artinya baris pertama dimulai tanpa indentasi.
    } // akhir if indentasi awal terlalu besar

    // Tiga variabel di bawah adalah "keadaan" baris yang sedang disusun.
    std::string current(firstIndent, ' '); // current adalah isi baris yang sedang disusun, diawali firstIndent buah spasi.
    std::size_t currentWidth = firstIndent; // currentWidth adalah lebar current dalam kolom; disimpan terpisah supaya tidak perlu menghitung ulang.
    bool lineHasWord = false; // lineHasWord bernilai true bila current sudah berisi paling sedikit satu kata (bukan hanya indentasi).

    while (position < text.size()) { // while (position < text.size()) berfungsi untuk memproses kata demi kata sampai seluruh text terbaca.
        // Hitung spasi pemisah di depan kata, lalu ambil satu kata (deret karakter bukan spasi).
        std::size_t spaces = 0; // spaces adalah banyaknya spasi di antara kata sebelumnya dan kata berikutnya.
        while (position < text.size() && text[position] == ' ') { // while (...) berfungsi untuk melewati spasi pemisah sambil menghitungnya.
            position = position + 1; // position = position + 1 artinya satu spasi dilewati.
            spaces = spaces + 1; // spaces = spaces + 1 artinya spasi itu dihitung, supaya jarak asli bisa ditiru.
        } // akhir while spasi pemisah
        std::size_t wordEnd = position; // wordEnd mula-mula sama dengan awal kata, lalu digeser sampai ujung kata.
        while (wordEnd < text.size() && text[wordEnd] != ' ') { // while (...) berfungsi untuk mencari ujung kata, yaitu spasi berikutnya atau akhir text.
            wordEnd = wordEnd + 1; // wordEnd = wordEnd + 1 artinya satu karakter kata dilewati.
        } // akhir while pencarian ujung kata
        // break berfungsi untuk keluar dari perulangan saat itu juga, walaupun syarat perulangannya masih benar.
        if (wordEnd == position) { // wordEnd == position artinya tidak ada kata lagi; yang tersisa di ujung text hanya spasi.
            break; // break; keluar dari perulangan while karena tidak ada kata yang perlu dilipat.
        } // akhir if tidak ada kata lagi
        std::string word = text.substr(position, wordEnd - position); // word = text.substr(...) artinya kata diambil dari position sepanjang wordEnd - position byte.
        position = wordEnd; // position = wordEnd artinya pembacaan dilanjutkan dari ujung kata ini.

        // Di tengah baris, spasi asli dipertahankan apa adanya (termasuk spasi perata seperti
        // pada "Nama   : isi"); di awal baris tidak ada spasi yang ditambahkan.
        // operator ?: berfungsi untuk memilih salah satu dari dua nilai: syarat ? nilaiBilaBenar : nilaiBilaSalah.
        const std::size_t gap = lineHasWord ? spaces : 0; // gap artinya jarak sebelum kata = spaces bila baris sudah berisi kata, 0 bila belum.
        if (currentWidth + gap + displayWidth(word) <= maxWidth) { // currentWidth + gap + lebar kata <= maxWidth artinya kata masih muat di baris ini.
            // Kata masih muat di baris yang sedang disusun.
            current += std::string(gap, ' '); // current += std::string(gap, ' ') artinya spasi pemisah ditambahkan sebanyak gap.
            current += word; // current += word artinya kata disambungkan ke baris.
            currentWidth = currentWidth + gap + displayWidth(word); // currentWidth diperbarui dengan lebar spasi dan kata yang baru ditambahkan.
            lineHasWord = true; // lineHasWord = true artinya baris ini kini sudah berisi kata.
            continue; // continue; langsung lanjut ke kata berikutnya, karena bagian di bawah hanya untuk kata yang tidak muat.
        } // akhir if kata muat

        // Kata tidak muat: baris yang sedang disusun ditutup dan baris baru dimulai menjorok.
        if (lineHasWord) { // lineHasWord artinya baris sekarang sudah berisi kata, jadi baris itu bisa ditutup.
            lines.push_back(current); // lines.push_back(current) artinya baris yang sudah jadi disimpan ke daftar hasil.
            current.assign(hangingIndent, ' '); // current.assign(hangingIndent, ' ') artinya isi current diganti dengan hangingIndent buah spasi untuk baris baru.
            currentWidth = hangingIndent; // currentWidth = hangingIndent artinya lebar baris baru mula-mula selebar indentasinya.
            lineHasWord = false; // lineHasWord = false artinya baris baru belum berisi kata.
        } // akhir if menutup baris
        // Kata yang lebih lebar dari satu baris penuh (misalnya alamat web) dipotong paksa.
        // Di titik ini currentWidth selalu lebih kecil dari maxWidth, jadi maxWidth - currentWidth (sisa ruang) tidak negatif.
        while (displayWidth(word) > maxWidth - currentWidth) { // displayWidth(word) > maxWidth - currentWidth artinya kata masih lebih lebar dari sisa ruang baris.
            const std::size_t bytes = prefixBytesForWidth(word, maxWidth - currentWidth); // bytes adalah banyaknya byte awal kata yang tepat mengisi sisa ruang.
            lines.push_back(current + word.substr(0, bytes)); // potongan awal kata disambung ke baris lalu baris itu disimpan.
            word.erase(0, bytes); // word.erase(0, bytes) artinya potongan yang sudah ditulis dibuang dari word, sisanya diproses lagi.
            current.assign(hangingIndent, ' '); // current.assign(hangingIndent, ' ') artinya baris baru dimulai dengan indentasi lanjutan.
            currentWidth = hangingIndent; // currentWidth = hangingIndent artinya lebar baris baru diatur ulang.
        } // akhir while pemotongan kata panjang
        if (!word.empty()) { // !word.empty() artinya masih ada sisa kata (yang kini pasti muat) untuk ditulis.
            current += word; // current += word artinya sisa kata ditaruh di baris baru.
            currentWidth = currentWidth + displayWidth(word); // currentWidth bertambah selebar sisa kata itu.
            lineHasWord = true; // lineHasWord = true artinya baris baru sudah berisi kata.
        } // akhir if sisa kata
    } // akhir while pemrosesan kata

    if (lineHasWord || lines.empty()) { // lineHasWord || lines.empty() artinya baris terakhir berisi kata, atau belum ada baris sama sekali.
        // Syarat lines.empty() menjamin hasilnya tidak pernah kosong, sehingga pemanggil selalu menerima paling sedikit satu baris.
        lines.push_back(current); // lines.push_back(current) artinya baris terakhir yang masih disusun ikut disimpan.
    } // akhir if baris terakhir
    return lines; // return lines; mengembalikan semua baris hasil lipatan.
} // akhir fungsi wrapText()
