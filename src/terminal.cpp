// ===== terminal.cpp - kemampuan terminal: warna, UTF-8, lebar jendela, dan jenis masukan =====
// Berkas ini berisi isi dari lima fungsi yang dijanjikan include/estima/terminal.h. Di sinilah
// program bertanya langsung ke sistem operasi: apakah keluaran berupa terminal, apakah terminal
// mengerti kode warna, berapa lebar jendelanya, dan apakah masukan diketik dari keyboard.
#include "estima/platform.h" // platform.h wajib di-include paling awal karena ia menyetel penyesuaian compiler (misalnya untuk Visual C++) sebelum header lain dibaca

#include "estima/terminal.h" // terminal.h adalah header milik berkas ini sendiri; berisi konstanta lebar dan deklarasi lima fungsi yang isinya ditulis di bawah

#include <cstddef> // <cstddef> digunakan untuk NULL, yaitu nilai penunjuk kosong yang dipakai saat memeriksa hasil std::getenv
#include <cstdlib> // <cstdlib> digunakan untuk std::getenv (membaca variabel lingkungan), std::atexit (mendaftarkan fungsi penutup), dan std::strtol (teks ke long)
#include <cstring> // <cstring> digunakan untuk std::strcmp, yaitu pembanding dua teks gaya C (const char*)

// Windows dan sistem keluarga Unix (macOS, Linux, Android) memakai pustaka yang berbeda untuk
// bertanya ke terminal, jadi pustakanya dipilih saat kompilasi.
// #ifdef NAMA berfungsi untuk bertanya ke compiler apakah NAMA sudah didefinisikan. Bila ya, baris-baris
// sesudahnya sampai #else ikut dikompilasi; bila tidak, yang dikompilasi adalah baris di antara #else dan
// #endif. #endif menutup pilihan itu. Dengan cara ini satu berkas sumber bisa berisi kode khusus Windows
// dan kode khusus Unix sekaligus, dan compiler hanya mengambil bagian yang cocok dengan sistemnya.
#ifdef _WIN32 // _WIN32 adalah nama yang otomatis didefinisikan compiler saat membuat program untuk Windows (32 maupun 64 bit); artinya blok ini khusus Windows
    // windows.h punya kebiasaan membuat makro min/max yang bentrok dengan pustaka standar C++;
    // NOMINMAX mencegahnya, dan WIN32_LEAN_AND_MEAN melewatkan bagian Windows yang tidak dipakai.
    // #ifndef ("if not defined") berfungsi untuk bertanya ke compiler apakah sebuah nama BELUM didefinisikan;
    // bila belum, baris sampai #endif pasangannya ikut dibaca, bila sudah, baris-baris itu dilewati.
    #ifndef NOMINMAX // NOMINMAX diperiksa dulu karena bisa saja sudah didefinisikan sebelumnya (misalnya lewat opsi -DNOMINMAX), jadi tidak didefinisikan dua kali
    #define NOMINMAX 1 // NOMINMAX 1 artinya windows.h diminta tidak membuat makro min dan max, supaya std::min dan std::max tidak rusak
    #endif // akhir pemeriksaan NOMINMAX
    #ifndef WIN32_LEAN_AND_MEAN // WIN32_LEAN_AND_MEAN diperiksa dulu dengan alasan yang sama: hanya didefinisikan bila belum ada
    #define WIN32_LEAN_AND_MEAN 1 // WIN32_LEAN_AND_MEAN 1 artinya windows.h hanya memuat bagian inti, jadi kompilasi lebih cepat dan nama yang ikut masuk lebih sedikit
    #endif // akhir pemeriksaan WIN32_LEAN_AND_MEAN
    #include <windows.h> // <windows.h> adalah pustaka resmi Windows; dari sini dipakai fungsi konsol untuk code page UTF-8, mode warna, dan lebar jendela
    #include <io.h> // <io.h> digunakan untuk _isatty, yaitu pemeriksa apakah sebuah saluran terhubung ke konsol (versi Windows dari isatty)
    #include <stdio.h> // <stdio.h> digunakan untuk stdin (saluran masukan standar) dan _fileno, yang memberi nomor saluran tersebut
    // Header Windows lama (misalnya bawaan Dev-C++ 5) belum mengenal mode ini; nilainya tetap.
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING // ENABLE_VIRTUAL_TERMINAL_PROCESSING hanya didefinisikan sendiri bila windows.h yang dipakai belum menyediakannya
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004 // 0x0004 adalah nilai resmi bendera mode ini; awalan 0x artinya angka ditulis dalam heksadesimal (basis 16), nilainya 4
    #endif // akhir pemeriksaan ENABLE_VIRTUAL_TERMINAL_PROCESSING
#else // #else artinya bukan Windows (macOS, Linux, Android): yang dikompilasi adalah dua include di bawah ini
    #include <sys/ioctl.h> // <sys/ioctl.h> digunakan untuk ioctl, TIOCGWINSZ, dan struct winsize, yaitu cara menanyakan ukuran jendela ke terminal
    #include <unistd.h> // <unistd.h> digunakan untuk isatty serta STDIN_FILENO dan STDOUT_FILENO (nomor saluran masukan dan keluaran standar)
#endif // akhir pemilihan pustaka sesuai sistem operasi

// Keadaan terminal diperiksa sekali di initTerminal() lalu diingat di tiga variabel ini. static membatasi
// variabel ini pada satu unit kompilasi (lihat penjelasan static di calculation.cpp), jadi pada versi modul
// hanya terminal.cpp yang bisa mengubahnya. Bagian lain program cukup membacanya lewat colorsEnabled(),
// unicodeEnabled(), dan inputIsInteractive(). Awalan g_ adalah kebiasaan penamaan untuk menandai variabel
// yang hidup di luar fungsi.
// Nilai awalnya adalah pilihan aman sebelum initTerminal() dipanggil: tanpa warna, garis UTF-8, dan
// masukan dianggap bukan dari keyboard.
static bool g_colorEnabled = false; // g_colorEnabled menyimpan keputusan boleh berwarna atau tidak; awalnya false (tanpa warna)
static bool g_unicodeEnabled = true; // g_unicodeEnabled menyimpan apakah garis kotak memakai karakter UTF-8; awalnya true
static bool g_inputInteractive = false; // g_inputInteractive menyimpan apakah masukan diketik langsung di terminal; awalnya false

// true bila variabel lingkungan name ada dan isinya bukan teks kosong.
// Variabel lingkungan adalah pengaturan berbentuk NAMA=isi yang diwariskan terminal (atau shell) ke
// setiap program yang dijalankannya, misalnya NO_COLOR=1. Fungsi kecil ini dibuat karena pemeriksaan
// yang sama dipakai untuk NO_COLOR dan FORCE_COLOR. Parameternya const char* karena std::getenv
// memang meminta nama dalam bentuk teks gaya C.
static bool hasEnvironmentValue(const char* name) { // hasEnvironmentValue(const char* name) berfungsi untuk memeriksa apakah variabel lingkungan bernama name terisi
    // std::getenv(nama) berfungsi untuk membaca variabel lingkungan. Hasilnya penunjuk ke isi variabel itu
    // (teks gaya C), atau NULL bila variabel tersebut tidak ada sama sekali.
    // NULL adalah nilai penunjuk kosong, artinya penunjuk itu tidak menunjuk ke data apa pun; penunjuk
    // seperti ini tidak boleh dibaca isinya, jadi selalu diperiksa dulu sebelum dipakai.
    const char* value = std::getenv(name); // value adalah penunjuk ke isi variabel lingkungan name, atau NULL bila variabel itu tidak ada
    return value != NULL && value[0] != '\0'; // value != NULL diperiksa lebih dulu; karena && berhenti begitu syarat pertama salah, value[0] hanya dibaca bila
    // penunjuknya sah. value[0] != '\0' artinya karakter pertamanya bukan karakter penutup teks ('\0'), jadi isinya tidak kosong.
} // akhir fungsi hasEnvironmentValue()

// Aturan warna mengikuti kebiasaan program terminal: opsi --no-color dan variabel NO_COLOR
// mematikan warna, FORCE_COLOR (selain "0") memaksanya menyala, dan selebihnya warna hanya
// dipakai bila keluaran benar-benar terminal yang mengerti kode warna. Aturan ini dipisah ke fungsi
// sendiri karena dipakai oleh kedua versi initTerminal() (Windows dan Unix); yang berbeda hanya cara
// mengetahui terminalSupportsColor.
static bool decideColor(bool disableColor, bool terminalSupportsColor) { // decideColor(...) berfungsi untuk memutuskan warna dipakai atau tidak; hasil true berarti berwarna
    if (disableColor || hasEnvironmentValue("NO_COLOR")) { // artinya pengguna meminta tanpa warna, lewat opsi --no-color atau variabel NO_COLOR
        return false; // return false; warna dimatikan; permintaan ini didahulukan di atas aturan lain
    } // akhir if NO_COLOR
    if (hasEnvironmentValue("FORCE_COLOR") && std::strcmp(std::getenv("FORCE_COLOR"), "0") != 0) { // FORCE_COLOR terisi dan isinya bukan "0" artinya warna dipaksa menyala;
        // std::getenv di sini aman dibaca karena && memastikan FORCE_COLOR sudah terbukti ada, dan std::strcmp != 0 berarti isinya berbeda dari "0".
        return true; // return true; warna dipakai walaupun keluaran bukan terminal (misalnya saat dialihkan ke berkas)
    } // akhir if FORCE_COLOR
    return terminalSupportsColor; // return terminalSupportsColor; tanpa permintaan khusus, warna mengikuti kemampuan terminal yang sudah diperiksa pemanggil
} // akhir fungsi decideColor()

#ifdef _WIN32 // _WIN32: dari sini sampai #else adalah versi Windows dari restoreConsoleCodePage() dan initTerminal()

// UINT adalah tipe bilangan bulat tanpa tanda negatif milik Windows; dipakai karena fungsi code page
// Windows (GetConsoleOutputCP dan SetConsoleOutputCP) memakai tipe ini. Nilai 0 dipakai sebagai tanda
// "belum ada code page yang disimpan".
static UINT g_savedOutputCodePage = 0; // g_savedOutputCodePage menyimpan code page keluaran konsol sebelum diubah program, supaya bisa dikembalikan saat program selesai

// Dipanggil otomatis saat program selesai: mengembalikan code page konsol seperti semula,
// supaya jendela Command Prompt atau PowerShell pengguna tidak ikut berubah.
static void restoreConsoleCodePage() { // restoreConsoleCodePage() berfungsi untuk mengembalikan code page keluaran konsol ke nilai yang disimpan tadi
    if (g_savedOutputCodePage != 0) { // g_savedOutputCodePage != 0 artinya memang ada code page lama yang tersimpan, jadi aman untuk dikembalikan
        SetConsoleOutputCP(g_savedOutputCodePage); // SetConsoleOutputCP(g_savedOutputCodePage) berfungsi untuk memasang kembali code page keluaran yang lama
    } // akhir if
} // akhir fungsi restoreConsoleCodePage()

// Versi Windows: menyalakan UTF-8 dan mode warna pada konsol, lalu mencatat hasilnya.
void initTerminal(bool disableColor, bool forceAscii) { // initTerminal(bool disableColor, bool forceAscii) versi Windows; dipanggil sekali dari runApp()
    // HANDLE adalah tipe Windows untuk "pegangan", yaitu tanda pengenal sebuah saluran (di sini keluaran
    // standar) yang harus disebutkan setiap kali memanggil fungsi konsol Windows.
    const HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE); // GetStdHandle(STD_OUTPUT_HANDLE) berfungsi untuk mengambil pegangan ke keluaran standar (layar konsol)
    // DWORD adalah tipe bilangan bulat 32 bit tanpa tanda negatif milik Windows; mode konsol disimpan dalam tipe ini.
    DWORD mode = 0; // mode adalah tempat menyimpan mode konsol yang sedang aktif; isinya diisi GetConsoleMode di baris berikutnya
    // Tanda & di depan nama variabel (seperti &mode) berfungsi untuk mengambil ALAMAT variabel itu di memori.
    // Alamat dikirim supaya fungsi Windows bisa menulis hasilnya langsung ke variabel mode. Ini berbeda dari &
    // pada parameter fungsi (referensi) dan dari && (operator "dan").
    const bool outputIsConsole = output != INVALID_HANDLE_VALUE && GetConsoleMode(output, &mode) != 0; // outputIsConsole true bila pegangannya sah dan
    // GetConsoleMode berhasil membaca mode; GetConsoleMode gagal (hasil 0) bila keluaran dialihkan ke berkas atau pipa, jadi hasilnya sekaligus menjawab "apakah ini konsol".

    // Konsol Windows bawaan memakai code page lama (misalnya 437 atau 850) sehingga garis kotak
    // UTF-8 tampil sebagai huruf acak. Keluaran dipindah ke UTF-8 (65001); code page masukan
    // sengaja tidak diubah karena program hanya menerima ketikan ASCII.
    g_savedOutputCodePage = GetConsoleOutputCP(); // GetConsoleOutputCP() berfungsi untuk membaca code page keluaran yang sedang dipakai, lalu disimpan untuk dikembalikan nanti
    const bool utf8Ready = SetConsoleOutputCP(CP_UTF8) != 0; // SetConsoleOutputCP(CP_UTF8) mengganti keluaran konsol ke UTF-8 (CP_UTF8 bernilai 65001);
    // hasilnya 0 bila gagal, jadi utf8Ready true berarti penggantian berhasil.
    if (utf8Ready && g_savedOutputCodePage != 0) { // utf8Ready && g_savedOutputCodePage != 0 artinya code page memang berubah dan nilai lamanya diketahui
        // std::atexit(fungsi) berfungsi untuk mendaftarkan sebuah fungsi agar dipanggil otomatis ketika program
        // selesai secara normal (kembali dari main() atau lewat exit()). Nama fungsi ditulis tanpa tanda ()
        // karena yang dikirim adalah fungsinya sendiri untuk dipanggil nanti, bukan hasil memanggilnya sekarang.
        std::atexit(restoreConsoleCodePage); // std::atexit(restoreConsoleCodePage) memastikan code page lama dikembalikan saat program selesai;
        // ini berlaku baik lewat menu Keluar maupun saat masukan habis, karena keduanya selesai dengan kembali dari main().
    } // akhir if utf8Ready

    // Mode "virtual terminal" membuat konsol memahami kode warna ANSI. Konsol lama yang menolak
    // mode ini tetap bisa dipakai, hanya tanpa warna.
    // Tanda | (OR per bit) berfungsi untuk menyalakan satu bendera tambahan di dalam nilai mode tanpa
    // mematikan bendera lain yang sudah menyala.
    const bool vtReady = outputIsConsole && SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0; // vtReady true bila keluaran adalah konsol
    // dan SetConsoleMode berhasil menyalakan mode virtual terminal; bila bukan konsol, SetConsoleMode tidak dipanggil sama sekali karena && berhenti lebih dulu.

    g_colorEnabled = decideColor(disableColor, vtReady); // g_colorEnabled diisi keputusan warna; konsol dianggap mengerti warna hanya bila vtReady
    g_unicodeEnabled = !forceAscii && (utf8Ready || !outputIsConsole); // garis UTF-8 dipakai bila tidak ada opsi --ascii DAN (konsol sudah UTF-8 ATAU keluaran
    // bukan konsol, misalnya berkas, yang menyimpan byte UTF-8 apa adanya tanpa bergantung pada code page).
    g_inputInteractive = _isatty(_fileno(stdin)) != 0; // _fileno(stdin) memberi nomor saluran masukan standar, lalu _isatty memeriksa apakah saluran itu konsol;
    // hasil selain 0 berarti masukan diketik dari keyboard, 0 berarti dialihkan dari berkas atau pipa.
} // akhir fungsi initTerminal() versi Windows

#else // #else: bukan Windows; dari sini sampai #endif adalah versi macOS/Linux/Android dari initTerminal()

// Versi Unix: terminal macOS, Linux, dan Android umumnya sudah memakai UTF-8, jadi cukup memeriksa apakah keluaran dan
// masukan berupa terminal, serta apakah terminalnya mengerti warna.
void initTerminal(bool disableColor, bool forceAscii) { // initTerminal(bool disableColor, bool forceAscii) versi Unix; dipanggil sekali dari runApp()
    const bool outputIsTerminal = isatty(STDOUT_FILENO) != 0; // isatty(STDOUT_FILENO) memeriksa apakah keluaran standar terhubung ke terminal; hasil selain 0 berarti ya
    // TERM=dumb adalah cara terminal sederhana menyatakan dirinya tidak mengerti kode warna.
    const char* terminalType = std::getenv("TERM"); // std::getenv("TERM") membaca jenis terminal (misalnya xterm-256color), atau NULL bila tidak ada
    const bool dumbTerminal = terminalType != NULL && std::strcmp(terminalType, "dumb") == 0; // dumbTerminal true bila TERM ada dan isinya persis "dumb";
    // terminalType != NULL diperiksa dulu supaya std::strcmp tidak pernah menerima penunjuk kosong.

    g_colorEnabled = decideColor(disableColor, outputIsTerminal && !dumbTerminal); // terminal dianggap mengerti warna bila keluarannya terminal dan bukan jenis "dumb"
    g_unicodeEnabled = !forceAscii; // garis UTF-8 selalu dipakai kecuali pengguna memberi opsi --ascii
    g_inputInteractive = isatty(STDIN_FILENO) != 0; // isatty(STDIN_FILENO) != 0 artinya masukan standar adalah terminal, jadi pengguna mengetik langsung
} // akhir fungsi initTerminal() versi Unix

#endif // akhir pemilihan versi initTerminal() sesuai sistem operasi

// Tiga fungsi baca di bawah hanya mengembalikan nilai yang sudah dicatat initTerminal(). Bagian lain
// program memakai fungsi ini, bukan variabelnya langsung, jadi yang mengisi variabel itu hanya initTerminal().
bool colorsEnabled() { // colorsEnabled() berfungsi untuk memberi tahu apakah warna boleh dipakai
    return g_colorEnabled; // return g_colorEnabled; mengembalikan keputusan warna yang tersimpan
} // akhir fungsi colorsEnabled()

bool unicodeEnabled() { // unicodeEnabled() berfungsi untuk memberi tahu apakah garis kotak memakai karakter UTF-8
    return g_unicodeEnabled; // return g_unicodeEnabled; mengembalikan keputusan jenis garis yang tersimpan
} // akhir fungsi unicodeEnabled()

bool inputIsInteractive() { // inputIsInteractive() berfungsi untuk memberi tahu apakah masukan diketik langsung dari keyboard
    return g_inputInteractive; // return g_inputInteractive; mengembalikan hasil pemeriksaan masukan yang tersimpan
} // akhir fungsi inputIsInteractive()

// Lebar layar dalam kolom, dicari bertahap: tanya terminal dulu, lalu variabel COLUMNS, lalu lebar
// bawaan; hasil akhirnya dijepit ke rentang MIN_TERMINAL_WIDTH sampai MAX_TERMINAL_WIDTH.
int terminalWidth() { // terminalWidth() berfungsi untuk mengembalikan lebar tata letak dalam kolom (40 sampai 120)
    // Lebar ditanyakan setiap kali dipanggil, jadi tampilan ikut menyesuaikan bila jendela
    // diubah ukurannya di tengah pemakaian.
    int columns = 0; // columns = 0 artinya lebar belum diketahui; nilai 0 atau kurang dipakai sebagai tanda "coba cara berikutnya"
#ifdef _WIN32 // _WIN32: cara menanyakan lebar jendela di Windows
    CONSOLE_SCREEN_BUFFER_INFO info; // info adalah struct milik Windows yang akan diisi keterangan layar konsol, termasuk posisi tepi jendela yang terlihat
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)) { // GetConsoleScreenBufferInfo(...) mengisi info lewat alamatnya (&info);
        // hasil selain 0 berarti berhasil, sedangkan 0 berarti gagal, misalnya karena keluaran dialihkan ke berkas.
        columns = info.srWindow.Right - info.srWindow.Left + 1; // srWindow adalah kotak jendela yang terlihat; Right dan Left adalah nomor kolom tepi kanan dan kiri,
        // keduanya ikut terhitung, jadi lebarnya Right - Left + 1.
    } // akhir if GetConsoleScreenBufferInfo
#else // #else: cara menanyakan lebar jendela di macOS/Linux/Android
    // Kata struct di depan winsize adalah gaya penulisan bahasa C (pustaka ini berasal dari C); di C++ boleh
    // ditulis atau dihilangkan, artinya tetap sama: membuat variabel bertipe struct winsize.
    struct winsize size; // size adalah struct winsize yang akan diisi ukuran jendela terminal (ws_col = banyaknya kolom, ws_row = banyaknya baris)
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0) { // ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) meminta terminal mengisi ukuran jendelanya ke size;
        // TIOCGWINSZ adalah kode permintaan "beri tahu ukuran jendela", dan hasil 0 berarti berhasil (gagal bila keluaran bukan terminal).
        columns = size.ws_col; // columns diisi ws_col, yaitu banyaknya kolom jendela terminal
    } // akhir if ioctl
#endif // akhir pemilihan cara menanyakan lebar sesuai sistem operasi
    // Bila keluaran bukan terminal (dialihkan ke berkas atau pipa), dicoba variabel COLUMNS;
    // bila itu pun tidak ada, dipakai lebar bawaan 80 kolom.
    if (columns <= 0) { // columns <= 0 artinya terminal tidak menjawab, jadi lebar dicari dari variabel lingkungan COLUMNS
        const char* fromEnvironment = std::getenv("COLUMNS"); // fromEnvironment adalah isi variabel COLUMNS (misalnya "100"), atau NULL bila tidak ada
        if (fromEnvironment != NULL) { // fromEnvironment != NULL artinya COLUMNS ada, jadi isinya boleh dibaca
            // std::strtol(teks, NULL, 10) berfungsi untuk mengubah teks gaya C berisi angka menjadi long, misalnya "100" menjadi 100;
            // angka 10 artinya teks dibaca sebagai bilangan basis sepuluh, dan NULL artinya sisa teks sesudah angka tidak diperlukan.
            // Bila teksnya bukan angka, hasilnya 0. Dipilih strtol, bukan std::atoi, karena angka yang terlalu besar tetap aman:
            // strtol menjepit hasilnya ke batas long, sedangkan atoi tidak menjamin apa pun untuk angka sebesar itu.
            const long requested = std::strtol(fromEnvironment, NULL, 10); // requested adalah lebar yang diminta lewat COLUMNS, masih dalam long
            if (requested > MAX_TERMINAL_WIDTH) { // requested > MAX_TERMINAL_WIDTH artinya permintaan melebihi batas tata letak (120 kolom)
                columns = MAX_TERMINAL_WIDTH; // columns langsung diisi batas atas, supaya angka sebesar apa pun tidak perlu diubah ke int
            } else if (requested > 0) { // requested > 0 artinya permintaan berupa angka positif yang masih di dalam batas
                columns = static_cast<int>(requested); // static_cast<int> aman karena nilainya paling besar 120; columns diisi lebar yang diminta
            } // akhir if permintaan COLUMNS; nilai 0 atau negatif diabaikan dan ditangani if berikutnya
        } // akhir if fromEnvironment
    } // akhir if pencarian lewat COLUMNS
    if (columns <= 0) { // columns <= 0 masih benar artinya lebar tetap tidak diketahui
        columns = DEFAULT_TERMINAL_WIDTH; // columns diisi lebar bawaan DEFAULT_TERMINAL_WIDTH (80 kolom)
    } // akhir if lebar bawaan
    if (columns < MIN_TERMINAL_WIDTH) { // columns < MIN_TERMINAL_WIDTH artinya jendela lebih sempit dari 40 kolom
        columns = MIN_TERMINAL_WIDTH; // columns dinaikkan menjadi 40; tata letak tidak dibuat lebih sempit dari ini, terminal yang akan melipat barisnya
    } // akhir if batas bawah
    if (columns > MAX_TERMINAL_WIDTH) { // columns > MAX_TERMINAL_WIDTH artinya jendela lebih lebar dari 120 kolom
        columns = MAX_TERMINAL_WIDTH; // columns diturunkan menjadi 120 supaya baris teks tidak terlalu panjang untuk dibaca
    } // akhir if batas atas
    return columns; // return columns; mengembalikan lebar akhir yang pasti berada di antara 40 dan 120
} // akhir fungsi terminalWidth()
