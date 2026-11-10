// ===== calculation.cpp - rumus perhitungan berat dan biaya =====
// Berkas ini berisi isi (definisi) fungsi-fungsi yang dideklarasikan di include/estima/calculation.h:
// rumus total berat, total biaya, penjumlahan seluruh komponen, pencarian komponen termahal, dan peringkat biaya.
#include "estima/platform.h" // "estima/platform.h" selalu di-#include paling awal agar penyesuaian compiler (khusus Visual C++) berlaku sebelum header lain.

#include "estima/calculation.h" // "estima/calculation.h" berisi prototipe fungsi yang isinya ditulis di bawah, sehingga compiler bisa mencocokkan keduanya.

// <cmath> adalah header pustaka standar untuk fungsi matematika; dari sini dipakai std::floor.
#include <cmath> // <cmath> digunakan untuk std::floor, yaitu pembulatan ke bawah yang menjadi dasar pembulatan berat dan biaya.

// Berat ditulis pengguna dalam kilogram dengan paling banyak tiga angka desimal (ketelitian gram).
// Pecahan desimal seperti 1,85 tidak bisa disimpan tepat dalam bentuk biner, sehingga 24 x 1,85
// yang dihitung langsung menghasilkan 44,400000000000006. Supaya hasil hitung tepat, berat
// diubah dulu ke gram (bilangan bulat), dikalikan, lalu dikembalikan ke kilogram di langkah akhir.
// static pada fungsi atau variabel tingkat berkas berfungsi untuk membatasi nama itu pada satu unit kompilasi, yaitu satu
// berkas .cpp yang dikompilasi beserta semua header yang di-#include-nya. Pada versi modul, berkas .cpp lain tidak bisa
// memanggil toWholeGram dan totalWeightGram; pada berkas tunggal, seluruh program memang satu unit kompilasi, jadi static di
// sini menjadi penanda bahwa keduanya hanya pembantu rumus di bawah, bukan bagian dari daftar fungsi di calculation.h.
static double toWholeGram(double weightKg) { // toWholeGram(weightKg) berfungsi untuk mengubah berat kg menjadi gram bulat, misalnya 1.85 menjadi 1850.
    // std::floor(x) adalah fungsi pembulatan ke bawah: membuang pecahan dan mengambil bilangan bulat terdekat yang tidak lebih besar dari x,
    // misalnya std::floor(1850.4) = 1850. Hasilnya tetap bertipe double.
    return std::floor(weightKg * 1000.0 + 0.5); // weightKg * 1000.0 + 0.5 lalu dibulatkan ke bawah artinya dibulatkan ke gram terdekat;
    // + 0.5 diperlukan karena hasil kali desimal di komputer kadang sedikit kurang dari angka yang benar, misalnya 1.005 x 1000 menjadi
    // 1004,9999999999999; tanpa + 0.5 hasil itu akan terpotong menjadi 1004 gram, bukan 1005 gram.
} // akhir fungsi toWholeGram()

// totalWeightGram menghitung total berat satu komponen dalam gram bulat. Dibuat sebagai fungsi tersendiri karena
// rumus yang sama dipakai tiga kali: untuk total berat, untuk total biaya, dan untuk penjumlahan berat seluruh komponen.
static double totalWeightGram(const Component& component) { // totalWeightGram(component) berfungsi untuk menghitung jumlah x berat per unit dalam gram.
    // static_cast<tipe>(nilai) berfungsi untuk mengubah sebuah nilai ke tipe lain secara terang-terangan (tertulis jelas di kode).
    return static_cast<double>(component.quantity) * toWholeGram(component.unitWeightKg); // static_cast<double>(component.quantity) mengubah jumlah (int) menjadi double
    // sebelum dikalikan. Tanpa static_cast pun int akan diubah otomatis saat dikali double; penulisan tegas ini hanya
    // membuat perubahan tipenya terlihat jelas oleh pembaca.
    // Hasil terbesarnya 1.000.000 unit x 10.000.000 gram, masih bilangan bulat yang bisa disimpan double dengan tepat.
} // akhir fungsi totalWeightGram()

// [Kriteria soal: fungsi menghitung total berat] Total berat = jumlah x berat per unit, dalam kg.
double calculateTotalWeight(const Component& component) { // calculateTotalWeight(component) berfungsi untuk menghitung total berat satu komponen dalam kg.
    return totalWeightGram(component) / 1000.0; // totalWeightGram(component) / 1000.0 artinya gram dikembalikan ke kilogram; ditulis 1000.0 (bukan 1000)
    // supaya jelas pembagiannya pembagian desimal, sehingga 44400 gram menjadi 44.4 kg.
} // akhir fungsi calculateTotalWeight()

// [Kriteria soal: fungsi menghitung total biaya] Total biaya = total berat x harga per kg, dibulatkan ke rupiah terdekat.
double calculateTotalCost(const Component& component) { // calculateTotalCost(component) berfungsi untuk menghitung total biaya satu komponen dalam rupiah bulat.
    // gram x (rupiah per kg) memberi biaya dalam seperseribu rupiah, masih berupa bilangan bulat.
    // Menambah 500 lalu membagi 1000 dan membuang pecahannya sama dengan membulatkan ke rupiah
    // terdekat (setengah rupiah ke atas dibulatkan naik).
    const double costPerMille = totalWeightGram(component) * component.pricePerKg; // costPerMille adalah biaya dalam seperseribu rupiah (gram x harga per kg);
    // bilangan bulat karena gram dan harga sama-sama bulat. const karena nilainya tidak diubah lagi.
    return std::floor((costPerMille + 500.0) / 1000.0); // (costPerMille + 500.0) / 1000.0 lalu std::floor artinya dibagi 1000 dan dibulatkan ke rupiah terdekat;
    // contoh: 1500 seperseribu rupiah (Rp1,5) menjadi floor(2000 / 1000) = Rp2.
} // akhir fungsi calculateTotalCost()

// Total berat seluruh komponen. Penjumlahannya dilakukan dalam gram bulat lebih dulu, baru dibagi 1000 sekali di akhir,
// supaya hasilnya tetap tepat (menjumlahkan banyak angka kg berpecahan bisa menimbulkan galat kecil).
double sumTotalWeight(const Component items[], int count) { // sumTotalWeight(items, count) berfungsi untuk menjumlahkan total berat semua komponen dalam kg.
    // Akumulasi: totalGram mulai dari 0 lalu ditambah berat tiap komponen satu per satu.
    double totalGram = 0.0; // totalGram = 0.0 adalah penampung jumlah berat dalam gram; harus dimulai dari 0 sebelum ditambah.
    for (int i = 0; i < count; ++i) { // for dari i = 0 sampai i < count artinya mengunjungi setiap komponen yang terisi, dari posisi 0 sampai count - 1.
        totalGram += totalWeightGram(items[i]); // totalGram += totalWeightGram(items[i]) artinya berat komponen ke-i (gram) ditambahkan ke penampung.
    } // akhir perulangan for
    return totalGram / 1000.0; // totalGram / 1000.0 artinya jumlah gram diubah sekali ke kilogram lalu dikembalikan.
} // akhir fungsi sumTotalWeight()

// Total biaya seluruh komponen = jumlah dari total biaya tiap komponen (yang masing-masing sudah dibulatkan ke rupiah),
// sehingga angka total di rekap selalu sama dengan penjumlahan angka per baris yang terlihat di tabel.
double sumTotalCost(const Component items[], int count) { // sumTotalCost(items, count) berfungsi untuk menjumlahkan total biaya semua komponen dalam rupiah.
    double total = 0.0; // total = 0.0 adalah penampung jumlah biaya, dimulai dari 0.
    for (int i = 0; i < count; ++i) { // for i = 0 sampai count - 1 artinya setiap komponen dikunjungi satu kali.
        total += calculateTotalCost(items[i]); // total += calculateTotalCost(items[i]) artinya biaya komponen ke-i ditambahkan ke total.
    } // akhir perulangan for
    return total; // return total; mengembalikan jumlah biaya seluruh komponen.
} // akhir fungsi sumTotalCost()

// Jumlah unit seluruh komponen, untuk baris "Jumlah unit" dan baris TOTAL di rekap.
long sumQuantity(const Component items[], int count) { // sumQuantity(items, count) berfungsi untuk menjumlahkan jumlah unit semua komponen.
    long total = 0; // total = 0 bertipe long, sama dengan tipe hasil fungsi, sebagai penampung jumlah unit.
    for (int i = 0; i < count; ++i) { // for i = 0 sampai count - 1 artinya mengunjungi setiap komponen yang terisi.
        total += items[i].quantity; // total += items[i].quantity artinya jumlah unit komponen ke-i (int) ditambahkan ke total (long).
    } // akhir perulangan for
    return total; // return total; mengembalikan jumlah unit seluruh komponen.
} // akhir fungsi sumQuantity()

// [Kriteria soal: fungsi menemukan komponen termahal]
// Dikerjakan dalam dua langkah (cari nilai tertinggi, lalu kumpulkan semua yang sama dengan nilai itu) supaya
// komponen yang biayanya kembar ikut terlaporkan semuanya, bukan hanya yang pertama ditemukan.
int findMostExpensive(const Component items[], int count, int resultIndexes[]) { // findMostExpensive(...) berfungsi untuk mengisi resultIndexes[] dengan posisi
    // komponen termahal dan mengembalikan banyaknya.
    if (count <= 0) { // count <= 0 artinya daftar kosong, jadi tidak ada komponen yang bisa dibandingkan.
        return 0; // return 0; berarti tidak ada komponen termahal yang ditemukan; resultIndexes[] tidak disentuh.
    } // akhir if daftar kosong
    // Langkah 1: cari nilai biaya tertinggi. Elemen pertama dianggap tertinggi dulu, lalu
    // dibandingkan dengan setiap elemen berikutnya.
    double highest = calculateTotalCost(items[0]); // highest diisi biaya komponen pertama (posisi 0) sebagai nilai tertinggi sementara.
    for (int i = 1; i < count; ++i) { // for dimulai dari i = 1 karena komponen posisi 0 sudah dipakai sebagai nilai awal highest.
        const double cost = calculateTotalCost(items[i]); // cost adalah biaya komponen ke-i, disimpan dulu supaya tidak dihitung dua kali di baris berikutnya.
        if (cost > highest) { // cost > highest memakai lebih besar (bukan >=) karena biaya yang sama tidak mengubah nilai tertinggi.
            highest = cost; // highest = cost; artinya nilai tertinggi diganti dengan biaya yang lebih besar ini.
        } // akhir if pembanding
    } // akhir perulangan langkah 1
    // Langkah 2: kumpulkan SEMUA komponen yang biayanya sama dengan nilai tertinggi itu, karena
    // bisa saja ada lebih dari satu. Biaya selalu bilangan bulat rupiah, jadi aman dibandingkan
    // dengan ==.
    int found = 0; // found = 0 adalah pencacah komponen termahal yang sudah ditemukan, sekaligus posisi kosong berikutnya di resultIndexes[].
    for (int i = 0; i < count; ++i) { // for dimulai lagi dari i = 0 karena kali ini SEMUA komponen, termasuk yang pertama, harus diperiksa.
        if (calculateTotalCost(items[i]) == highest) { // calculateTotalCost(items[i]) == highest artinya komponen ke-i termasuk yang termahal.
            resultIndexes[found] = i; // resultIndexes[found] = i; artinya posisi komponen ini dicatat di tempat kosong berikutnya.
            found = found + 1; // found = found + 1; artinya pencacah naik satu, jadi catatan berikutnya tidak menimpa yang ini.
        } // akhir if biaya sama dengan tertinggi
    } // akhir perulangan langkah 2
    return found; // return found; mengembalikan banyaknya komponen termahal (paling sedikit 1 bila daftar tidak kosong);
    // posisinya tercatat urut dari depan daftar.
} // akhir fungsi findMostExpensive()

// rankByCost tidak memindahkan isi array komponen: yang diurutkan hanya array order[] berisi nomor posisi.
// Dengan begitu nomor urut komponen di daftar tetap sama setelah peringkat ditampilkan.
void rankByCost(const Component items[], int count, int order[]) { // rankByCost(items, count, order) berfungsi untuk mengisi order[] dengan posisi komponen
    // dari biaya terbesar ke terkecil.
    for (int i = 0; i < count; ++i) { // for i = 0 sampai count - 1 berfungsi untuk mengisi order[] dengan urutan awal 0, 1, 2, ... (sesuai urutan pengisian).
        order[i] = i; // order[i] = i; artinya posisi ke-i mula-mula ditempati komponen nomor i.
    } // akhir perulangan pengisian awal
    // Insertion sort pada array nomor urut: setiap nomor disisipkan ke posisi yang benar di
    // antara nomor-nomor sebelumnya yang sudah terurut. Dipilih karena datanya sedikit (paling
    // banyak 100), langkahnya mudah diikuti, dan komponen dengan biaya sama tetap pada urutan
    // pengisiannya. Yang diurutkan nomor urutnya saja, jadi data komponen tidak ikut dipindah.
    for (int i = 1; i < count; ++i) { // for dimulai dari i = 1 karena satu elemen pertama (order[0]) sendirian sudah dianggap terurut.
        const int current = order[i]; // current adalah nomor komponen yang sedang disisipkan, disimpan karena tempatnya di order[] bisa tertimpa saat penggeseran.
        const double currentCost = calculateTotalCost(items[current]); // currentCost adalah biaya komponen current, yang dibandingkan dengan biaya di sebelah kirinya.
        int position = i - 1; // position = i - 1 artinya pemeriksaan dimulai dari elemen tepat di kiri current.
        // while berfungsi untuk mengulang perintah di dalamnya selama syaratnya masih benar; dipakai di sini karena banyaknya
        // langkah geser tidak diketahui sebelumnya (berhenti begitu ketemu tempat yang pas).
        while (position >= 0 && calculateTotalCost(items[order[position]]) < currentCost) { // syarat while: masih ada elemen di kiri (position >= 0)
            // DAN biaya elemen itu lebih kecil dari currentCost. position >= 0 ditulis lebih dulu karena && berhenti memeriksa bila syarat pertama salah,
            // jadi order[-1] tidak pernah dibaca. Pembanding < (bukan <=) membuat biaya yang sama tidak digeser, sehingga urutan pengisiannya tetap.
            order[position + 1] = order[position]; // order[position + 1] = order[position]; artinya nomor yang biayanya lebih kecil digeser satu langkah ke kanan.
            position = position - 1; // position = position - 1; artinya pemeriksaan pindah satu langkah ke kiri.
        } // akhir perulangan while
        order[position + 1] = current; // order[position + 1] = current; artinya current diletakkan di celah yang sudah dikosongkan, tepat di kanan elemen
        // yang biayanya tidak lebih kecil (atau di posisi 0 bila current paling mahal sejauh ini).
    } // akhir perulangan insertion sort
} // akhir fungsi rankByCost()

// Porsi biaya dalam persen, dengan penjagaan supaya tidak terjadi pembagian dengan nol.
double costSharePercent(double cost, double totalCost) { // costSharePercent(cost, totalCost) berfungsi untuk menghitung berapa persen cost dari totalCost.
    if (totalCost <= 0.0) { // totalCost <= 0.0 artinya total biaya nol, sehingga pembagian tidak boleh dilakukan (membagi dengan nol).
        return 0.0; // return 0.0; artinya porsinya dianggap 0 persen.
    } // akhir if penjaga pembagian nol
    return cost / totalCost * 100.0; // cost / totalCost * 100.0 artinya perbandingan biaya terhadap total dikali 100 supaya menjadi persen, misalnya 50 / 200 x 100 = 25.
} // akhir fungsi costSharePercent()
