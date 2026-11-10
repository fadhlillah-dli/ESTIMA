// ===== main.cpp - titik masuk program =====
// Berkas ini paling pendek di seluruh program: isinya hanya fungsi main(), tempat program mulai berjalan.
#include "estima/platform.h" // platform.h di-include PERTAMA di setiap berkas .cpp di folder src/, sesuai aturan yang tertulis di header itu.

#include "estima/app.h" // app.h berisi deklarasi runApp(), satu-satunya fungsi yang dipanggil main().

// main() sengaja hanya meneruskan ke runApp(): seluruh alur ada di modul app sehingga bisa
// dipakai ulang dan diuji tanpa bergantung pada berkas ini.
// Contohnya, uji unit ditautkan dengan semua berkas src/*.cpp KECUALI main.cpp, karena program uji punya main() sendiri.
//
// main(int argc, char* argv[]) adalah fungsi yang pertama kali dijalankan sistem operasi saat program dibuka; setiap program C++
// wajib punya tepat satu fungsi main. argc (argument count) adalah banyaknya kata pada baris perintah, termasuk nama program
// itu sendiri; argv (argument vector) adalah array teks berisi kata-kata itu, misalnya perintah "estima --ascii" menghasilkan
// argc = 2, argv[0] = nama program, dan argv[1] = "--ascii".
// Nilai yang dikembalikan main() dengan return menjadi kode keluar program, yaitu angka yang diterima sistem operasi (atau
// skrip dan uji otomatis yang menjalankan program) untuk mengetahui bagaimana program berakhir: 0 berarti selesai dengan baik,
// angka lain berarti ada masalah. Di ESTIMA angkanya adalah EXIT_CODE_OK (0), EXIT_CODE_INPUT_CLOSED (1), atau EXIT_CODE_BAD_USAGE (2).
int main(int argc, char* argv[]) { // main(int argc, char* argv[]) adalah titik masuk program; hasil int-nya menjadi kode keluar.
    return runApp(argc, argv); // runApp(argc, argv) menjalankan seluruh program dengan opsi baris perintah yang sama, lalu kode keluarnya langsung diteruskan
    // ke sistem operasi lewat return.
} // akhir fungsi main()
