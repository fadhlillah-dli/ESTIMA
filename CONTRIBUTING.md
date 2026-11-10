# Berkontribusi

Terima kasih sudah tertarik ikut mengembangkan ESTIMA. Alurnya singkat:

1. Fork repositori ini, lalu buat branch dari `main`, misalnya `fitur/simpan-data` atau `perbaikan/lebar-tabel`.
2. Buat perubahan di `include/estima/` dan `src/`. Jangan menyunting `amalgamation/UTS_2531070011.cpp` langsung:
   berkas itu disusun ulang dari modul dengan `python3 scripts/amalgamate.py`, dan CI menolak bila keduanya berbeda.
3. Ikuti konvensi kode di README:
   - identifier berbahasa Inggris, komentar dan teks tampilan berbahasa Indonesia;
   - hanya fitur C++ yang sudah ada sejak C++98 (tanpa `auto`, range-for, lambda, `nullptr`, `long long`,
     `std::to_string`), supaya kode tetap lolos dari C++98 sampai C++23, dan berkas sumber ASCII murni;
   - setiap baris kode diberi komentar ujung yang menjelaskan apa yang dikerjakan baris itu, dan setiap array,
     struct, serta fungsi diberi komentar yang menjelaskan mengapa bentuk itu dipilih.
4. Pastikan semuanya lulus sebelum mengirim:

   ```bash
   make test
   python3 tests/e2e/check_layout.py --exe build/estima
   python3 scripts/amalgamate.py --check
   ```

   Skrip uji butuh Python 3.8 ke atas. Di Windows, perintah Python-nya `python`, bukan `python3`, dan baris pertama
   menjadi `mingw32-make test PYTHON=python`.

   Bila mengubah tampilan, perhatikan lebar baris: setiap baris buatan program harus muat di terminal 40 sampai
   120 kolom, dan gate tata letak memeriksanya.
5. Bila mengubah harga acuan, catat sumber barunya di `docs/sumber-data.md` (tautan, tanggal, hitungan konversi),
   lalu samakan angka di `README.md` (tabel harga acuan, tabel data uji, dan contoh rekap) dengan keluaran program.
6. Tulis perubahan di `CHANGELOG.md` bagian `[Unreleased]`, lalu kirim pull request dengan penjelasan singkat:
   apa yang diubah dan mengapa.

Laporan bug dan usulan fitur silakan lewat tab Issues. Masalah keamanan dilaporkan secara privat, lihat
[SECURITY.md](SECURITY.md).
