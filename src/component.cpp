// ===== component.cpp - operasi dasar pada daftar komponen =====
// Berkas ini berisi definisi (isi) dari semua yang dideklarasikan di include/estima/component.h: dua konstruktor
// dan empat fungsi pengelola daftar. Isinya sengaja pendek dan tidak membaca keyboard maupun mencetak ke layar,
// jadi bisa dipakai oleh menu, katalog data contoh, dan uji otomatis tanpa perubahan.
#include "estima/platform.h" // "estima/platform.h" di-#include paling awal supaya penyesuaian compiler sudah aktif sebelum header lain dibaca.

#include "estima/component.h" // "estima/component.h" adalah header milik berkas ini: struct Component, struct ComponentList,
// konstanta MAX_COMPONENTS, dan prototipe keempat fungsi yang isinya ditulis di bawah.

#include <string> // <string> digunakan untuk std::string (nama komponen dan nama yang dicari).

#include "estima/text_format.h" // "estima/text_format.h" digunakan untuk toLowerAscii(), yaitu pengubah huruf besar menjadi huruf kecil.

// Konstruktor memberi nilai awal yang pasti, supaya komponen yang belum diisi tidak pernah
// berisi angka sisa memori.
// Tanpa konstruktor ini, anggota int dan double milik 100 elemen array items bisa berisi angka acak.
// Definisi konstruktor ditulis NamaStruct::NamaStruct() (tanda :: artinya "milik", jadi konstruktor milik struct itu).
// Bagian sesudah titik dua adalah daftar inisialisasi anggota: setiap anggota langsung diberi nilai awal dalam
// kurung, misalnya quantity(0) artinya quantity dimulai dari 0. Isi kurung kurawalnya boleh kosong karena
// semua pekerjaan sudah selesai di daftar itu.
Component::Component() : name(), quantity(0), unitWeightKg(0.0), pricePerKg(0.0) { // name() artinya nama dibuat sebagai teks kosong,
// sedangkan quantity, unitWeightKg, dan pricePerKg dimulai dari 0.
} // akhir konstruktor Component()

ComponentList::ComponentList() : count(0) { // count(0) artinya daftar baru selalu kosong; elemen items sendiri sudah diberi nilai awal
// oleh konstruktor Component() di atas, yang dijalankan otomatis untuk setiap elemen array.
} // akhir konstruktor ComponentList()

// addComponent(): menambah satu komponen di ujung daftar. Elemen baru selalu ditaruh tepat sesudah elemen
// terakhir yang terisi, jadi isi daftar tetap rapat di depan dan urutannya sama dengan urutan pengisian.
bool addComponent(ComponentList& list, const Component& component) { // addComponent() berfungsi untuk menyalin component ke posisi kosong
// pertama di list; hasil true berarti berhasil, false berarti daftar sudah penuh.
    // if berfungsi untuk menjalankan perintah di dalam kurung kurawal hanya bila syarat di dalam kurung bernilai benar.
    // Pasangannya else (bila ada) berisi perintah yang dijalankan bila syaratnya salah.
    // Operator perbandingan menghasilkan true atau false: == sama dengan, != tidak sama dengan, < kurang dari,
    // > lebih dari, <= kurang dari atau sama dengan, >= lebih dari atau sama dengan.
    if (list.count >= MAX_COMPONENTS) { // list.count >= MAX_COMPONENTS artinya ke-100 tempat sudah terisi, jadi tidak ada tempat lagi;
    // dengan >= (bukan ==) keadaan count yang sudah melewati batas pun ikut tertolak.
        // return berfungsi untuk mengakhiri fungsi saat itu juga dan memberikan nilai hasilnya kepada pemanggil.
        return false; // return false artinya komponen tidak ditambahkan karena daftar penuh; isi daftar tidak berubah sama sekali.
    } // akhir if daftar penuh
    // Elemen baru menempati posisi count, yaitu tempat kosong pertama, lalu pencacah dinaikkan.
    list.items[list.count] = component; // list.items[list.count] = component artinya seluruh anggota component (nama, jumlah, berat,
    // harga) disalin sekaligus ke elemen nomor count; struct bisa disalin utuh dengan satu tanda =.
    list.count = list.count + 1; // list.count = list.count + 1 artinya pencacah bertambah satu karena ada satu elemen baru yang terisi.
    return true; // return true artinya komponen berhasil ditambahkan.
} // akhir fungsi addComponent()

// removeComponent(): menghapus satu komponen dari tengah, depan, atau ujung daftar tanpa meninggalkan tempat kosong.
bool removeComponent(ComponentList& list, int index) { // removeComponent() berfungsi untuk membuang komponen pada posisi index (mulai 0);
// hasil true berarti berhasil, false berarti index tidak menunjuk ke komponen yang ada.
    if (index < 0 || index >= list.count) { // index < 0 || index >= list.count artinya posisinya negatif atau berada di luar elemen
    // yang terisi (elemen terisi hanya 0 sampai count - 1), jadi tidak ada yang bisa dihapus.
        return false; // return false artinya tidak ada yang dihapus dan daftar tetap seperti semula.
    } // akhir if index tidak sah
    // Menghapus dari array berarti menimpa: setiap elemen di kanan index digeser satu langkah
    // ke kiri, sehingga tidak ada "lubang" di tengah dan urutan komponen lain tetap sama.
    // for berfungsi untuk mengulang perintah dengan pencacah. Isi kurungnya tiga bagian yang dipisah titik koma:
    // nilai awal pencacah, syarat untuk terus mengulang, dan langkah yang dijalankan setiap selesai satu putaran.
    // ++ berfungsi untuk menambah nilai variabel dengan 1, jadi ++i sama dengan i = i + 1.
    // += berfungsi untuk menambah nilai variabel dengan angka di kanannya, jadi total += x sama dengan total = total + x.
    for (int i = index; i < list.count - 1; ++i) { // i = index artinya penggeseran dimulai dari tempat yang dihapus;
    // i < list.count - 1 karena setiap putaran mengambil items[i + 1], dan elemen terisi terakhir adalah items[count - 1].
        list.items[i] = list.items[i + 1]; // list.items[i] = list.items[i + 1] artinya elemen di sebelah kanan disalin ke kiri satu langkah.
    } // akhir perulangan for penggeseran
    list.count = list.count - 1; // list.count = list.count - 1 artinya isi daftar berkurang satu.
    list.items[list.count] = Component(); // Component() membuat satu komponen baru bernilai awal (nama kosong, angka 0); di sini dipakai
    // untuk mengosongkan elemen terakhir lama (isinya sudah tersalin ke kiri, atau itu komponen yang baru dihapus bila yang
    // dihapus komponen terakhir), supaya tidak ada data sisa sesudah count.
    return true; // return true artinya komponen berhasil dihapus.
} // akhir fungsi removeComponent()

// clearComponents(): mengosongkan seluruh daftar, misalnya sebelum data contoh dimuat.
void clearComponents(ComponentList& list) { // clearComponents() berfungsi untuk mengembalikan list menjadi daftar kosong.
    for (int i = 0; i < list.count; ++i) { // i = 0 sampai i < list.count artinya hanya elemen yang terisi (items[0] sampai items[count - 1])
    // yang perlu dikosongkan; elemen sesudahnya memang sudah kosong.
        list.items[i] = Component(); // list.items[i] = Component() artinya elemen ke-i ditimpa komponen bernilai awal (nama kosong, angka 0).
    } // akhir perulangan for pengosongan
    list.count = 0; // list.count = 0 artinya daftar kini tidak berisi komponen sama sekali.
} // akhir fungsi clearComponents()

// findComponentByName(): mencari komponen lewat namanya; dipakai menu Tambah dan Ubah di src/app.cpp untuk menolak nama kembar.
int findComponentByName(const ComponentList& list, const std::string& name) { // findComponentByName() berfungsi untuk mencari posisi
// komponen bernama name; list memakai const & karena daftar hanya dibaca, tidak diubah.
    // Pencarian berurutan (linear search): daftar paling banyak 100 elemen dan tidak terurut,
    // jadi memeriksa satu per satu dari depan adalah cara paling sederhana yang tetap cepat.
    // Supaya "Gear", "GEAR", dan "gear" dianggap nama yang sama, kedua nama diubah dulu ke huruf kecil lalu dibandingkan.
    const std::string wanted = toLowerAscii(name); // wanted adalah nama yang dicari dalam huruf kecil; dihitung sekali di luar perulangan
    // supaya tidak diulang di setiap putaran, dan const karena nilainya tidak berubah lagi.
    for (int i = 0; i < list.count; ++i) { // i = 0 sampai i < list.count artinya memeriksa setiap komponen yang terisi, dari depan.
        if (toLowerAscii(list.items[i].name) == wanted) { // toLowerAscii(list.items[i].name) == wanted artinya nama komponen ke-i,
        // setelah diubah ke huruf kecil, sama persis dengan nama yang dicari.
            return i; // return i artinya posisi komponen yang cocok langsung diberikan dan pencarian berhenti di temuan pertama.
        } // akhir if nama cocok
    } // akhir perulangan for pencarian
    return -1; // return -1 artinya seluruh daftar sudah diperiksa dan tidak ada nama yang cocok; -1 tidak mungkin menjadi posisi array.
} // akhir fungsi findComponentByName()
