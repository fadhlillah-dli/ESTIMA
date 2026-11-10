// ===== calculation.h - rumus perhitungan berat dan biaya =====
// Semua fungsi di sini murni menghitung: tidak membaca keyboard dan tidak mencetak ke layar.
// Karena itu fungsi-fungsi ini bisa dipakai ulang oleh tampilan layar (menu rekap dan peringkat) maupun
// laporan berkas, dan bisa diuji otomatis dengan angka yang sudah diketahui jawabannya.
// Header ini hanya berisi deklarasi (prototipe); isi setiap fungsi ditulis di src/calculation.cpp.
#ifndef ESTIMA_CALCULATION_H // ESTIMA_CALCULATION_H adalah nama penjaga header ini; bila belum dikenal, isi berkas di bawah ikut dikompilasi.
#define ESTIMA_CALCULATION_H // ESTIMA_CALCULATION_H didefinisikan sekarang, jadi bila header ini ter-#include lagi isinya dilewati.

#include "estima/component.h" // "estima/component.h" dibutuhkan karena semua fungsi di bawah menerima struct Component sebagai masukan.

// [Kriteria soal: fungsi menghitung total berat]
// Total berat satu komponen (kg) = jumlah x berat per unit.
// Hasilnya double karena berat bisa berpecahan (misalnya 44,400 kg). Komponen dikirim sebagai const
// Component& supaya struct-nya tidak disalin dan dijamin tidak diubah oleh fungsi ini.
double calculateTotalWeight(const Component& component); // calculateTotalWeight(component) berfungsi untuk menghitung total berat satu komponen dalam kg;
// dipakai di tabel rekap dan ringkasan komponen (src/report.cpp).

// [Kriteria soal: fungsi menghitung total biaya]
// Total biaya satu komponen (rupiah) = total berat x harga per kg, dibulatkan ke rupiah terdekat.
// Tipe hasilnya double, bukan int: biaya satu komponen bisa mencapai hampir Rp1 triliun (batasnya MAX_COMPONENT_COST),
// jauh di atas batas int (sekitar 2,1 miliar), sedangkan double masih menyimpan bilangan bulat sebesar itu dengan tepat.
double calculateTotalCost(const Component& component); // calculateTotalCost(component) berfungsi untuk menghitung total biaya satu komponen dalam rupiah bulat;
// dipakai di rekap, peringkat, ringkasan komponen, dan pemeriksaan batas MAX_COMPONENT_COST di src/app.cpp.

// Total berat seluruh komponen (kg). Parameter items[] adalah array komponen dan count jumlah
// elemen yang terisi.
// Parameter array (items[]) bersama jumlah elemennya (count) adalah cara C++ mengirim sebuah array ke fungsi:
// yang sebenarnya dikirim hanya alamat elemen pertamanya, sehingga fungsi tidak tahu panjang array
// kecuali diberi tahu lewat parameter tambahan seperti count. Pemanggil menulis sumTotalWeight(list.items, list.count).
double sumTotalWeight(const Component items[], int count); // sumTotalWeight(items, count) berfungsi untuk menjumlahkan total berat count komponen pertama di array items;
// const di depan Component artinya elemen array hanya dibaca, tidak diubah.

// Total biaya seluruh komponen (rupiah).
double sumTotalCost(const Component items[], int count); // sumTotalCost(items, count) berfungsi untuk menjumlahkan total biaya semua komponen;
// hasilnya dipakai sebagai total keseluruhan biaya di rekap dan sebagai dasar porsi persen.

// Jumlah unit seluruh komponen.
// long adalah tipe bilangan bulat yang jangkauannya tidak pernah lebih kecil dari int; standar C++ menjamin long
// sanggup menyimpan paling sedikit sampai sekitar 2,1 miliar (di Linux dan macOS 64-bit bahkan jauh lebih besar).
// Jumlah unit terbesar yang mungkin adalah 100 komponen x 1.000.000 unit = 100.000.000 unit, jadi selalu muat di long.
long sumQuantity(const Component items[], int count); // sumQuantity(items, count) berfungsi untuk menjumlahkan jumlah unit semua komponen;
// hasilnya ditampilkan sebagai "Jumlah unit" dan baris TOTAL di rekap.

// [Kriteria soal: fungsi menemukan komponen termahal]
// Menemukan komponen dengan biaya tertinggi. Posisi komponennya ditulis ke resultIndexes[]
// (harus muat count elemen) dan banyaknya dikembalikan. Hasilnya bisa lebih dari satu bila ada
// beberapa komponen dengan biaya tertinggi yang sama; hasil 0 berarti daftar kosong.
// Hasil berupa array posisi (bukan satu Component) dipilih supaya komponen yang biayanya kembar tidak ada yang terlewat,
// dan posisi itu bisa dipakai untuk menyorot barisnya di tabel rekap.
int findMostExpensive(const Component items[], int count, int resultIndexes[]); // findMostExpensive(...) berfungsi untuk mencari komponen termahal;
// resultIndexes[] tanpa const karena array ini justru diisi oleh fungsi, dan nilai kembaliannya (int) adalah banyaknya posisi yang ditulis.

// Menyusun peringkat biaya dari yang terbesar. order[] (harus muat count elemen) diisi posisi
// komponen sesuai urutan peringkat; data aslinya tidak dipindah.
// Komponen yang biayanya sama tetap berurutan sesuai urutan pengisiannya.
void rankByCost(const Component items[], int count, int order[]); // rankByCost(items, count, order) berfungsi untuk mengisi order[] dengan posisi komponen;
// urutannya dari biaya terbesar. void karena hasilnya dikirim lewat array order[], bukan lewat return. Dipakai oleh menu Peringkat biaya (printRanking).

// Porsi sebuah biaya terhadap total, dalam persen (0 bila total 0).
double costSharePercent(double cost, double totalCost); // costSharePercent(cost, totalCost) berfungsi untuk menghitung cost / totalCost x 100;
// dipakai untuk kolom "Porsi" di peringkat dan keterangan "(..% dari total biaya)" di rekap.

#endif // ESTIMA_CALCULATION_H (akhir penjaga header calculation.h)
