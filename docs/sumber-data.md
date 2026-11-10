# Sumber data harga acuan material

Dokumen ini mencatat dari mana setiap angka di daftar harga acuan ESTIMA (`src/material_catalog.cpp`) berasal,
supaya angkanya bisa diperiksa ulang oleh siapa pun dan mudah diperbarui ketika harga pasar berubah.

- **Tanggal:** semua sumber dibuka pada 10 November 2026. Harga yang masuk nilai tengah bertanggal April sampai
  Oktober 2026, atau tidak bertanggal sama sekali (kebanyakan penawaran toko daring). Tanggal tiap harga ditulis
  di rinciannya.
- **Yang dicari:** harga bahan setengah jadi (pelat, batang bulat, batang cor, lembaran) yang biasa dibeli bengkel mesin
  di Indonesia, dalam rupiah per kilogram.
- **Cara menghitung:** harga yang dijual per batang, per lembar, atau per potong diubah ke rupiah per kilogram dari
  ukuran dan massa jenis bahannya (volume x massa jenis = massa; harga / massa). Hitungannya ditulis pada tiap titik harga.
- **Harga acuan:** nilai tengah (median) titik-titik harga yang sebanding, lalu dibulatkan ke Rp500 terdekat untuk harga
  di bawah Rp100.000 per kg dan ke Rp5.000 terdekat untuk harga di atasnya.
- **Pemeriksaan:** setiap harga yang dicantumkan sudah dicocokkan dengan halaman sumbernya dan konversinya dihitung
  ulang. Harga yang angkanya tidak bisa dipastikan dari halaman sumbernya tidak dicantumkan dan tidak ikut dihitung.
- **Status pajak:** kebanyakan sumber tidak menyebut apakah harganya sudah termasuk PPN, jadi angka tidak disetarakan.

Harga di toko daring diambil dari halaman pembanding harga BigGo Indonesia, yang menampilkan judul, harga, dan nama toko
dari Shopee, Tokopedia, Lazada, Blibli, dan TikTok Shop; tanggal tiap penawaran tidak tercantum, hanya label halaman
"Nov 2026".

## Ringkasan

| Material di program | Massa jenis (kg/m3) | Nilai tengah | Harga acuan | Rentang yang ditemui | Titik harga (dipakai / ditemui) | Keyakinan |
| --- | ---: | ---: | ---: | --- | ---: | --- |
| Baja SS400 | 7.850 | Rp13.798 | **Rp14.000** | Rp11.200 - Rp19.700 | 7 / 8 | sedang |
| Baja karbon S45C | 7.850 | Rp19.643 | **Rp19.500** | Rp19.300 - Rp36.500 | 2 / 4 | sedang-rendah |
| Baja paduan SCM440 | 7.850 | Rp27.000 | **Rp27.000** | Rp27.000 - Rp49.200 | 1 / 3 | rendah |
| Stainless SUS304 | 7.930 | Rp52.926 | **Rp53.000** | Rp49.000 - Rp90.000 | 3 / 8 | sedang |
| Stainless SUS316L | 7.980 | Rp100.381 | **Rp100.000** | Rp95.000 - Rp144.000 | 3 / 5 | sedang |
| Besi cor FC250 | 7.200 | Rp37.923 | **Rp38.000** | Rp36.500 - Rp81.000 | 7 / 7 | rendah |
| Aluminium 6061-T6 | 2.700 | Rp135.174 | **Rp135.000** | Rp112.000 - Rp193.000 | 6 / 9 | sedang |
| Kuningan C3604 | 8.470 | Rp250.437 | **Rp250.000** | Rp214.000 - Rp301.000 | 5 / 6 | sedang |
| Perunggu BC6 | 8.830 | Rp324.619 | **Rp325.000** | Rp239.000 - Rp472.000 | 4 / 6 | rendah |
| Nylon MC (PA6) | 1.150 | Rp119.280 | **Rp120.000** | Rp102.000 - Rp171.000 | 12 / 17 | sedang |
| POM (polyacetal) | 1.410 | Rp101.137 | **Rp100.000** | Rp86.000 - Rp128.000 | 11 / 14 | sedang |

Pada kolom titik harga, angka pertama adalah banyaknya harga yang masuk nilai tengah, dan angka kedua banyaknya harga
yang dicantumkan di rincian (termasuk harga pembanding yang tidak ikut dihitung).

Keyakinan "rendah" berarti sumbernya sedikit, kebanyakan berasal dari penjual yang sama, atau sebaran harganya lebar;
"sedang-rendah" berada di antara sedang dan rendah. Angka seperti itu cukup untuk estimasi awal, tetapi sebaiknya
diganti dengan penawaran pemasok sendiri. Di program, pengguna selalu bisa mengetik harga lain.

## Kurs dan harga pembanding dunia

Harga komoditas dunia dipakai untuk menilai kewajaran: bahan setengah jadi di tingkat pengecer semestinya lebih mahal
daripada bahan mentahnya, tetapi tidak sampai berlipat puluhan kali.

- Kurs: **Rp18.059 per dolar AS**, JISDOR (Jakarta Interbank Spot Dollar Rate) dari Bank Indonesia, tanggal 2026-11-10. Sumber: <https://www.bi.go.id/id/statistik/informasi-kurs/jisdor/default.aspx>
- Konversi: harga (USD per ton) / 1000 x kurs = rupiah per kilogram.
- Nilai impor (UN Comtrade) dan harga mineral acuan pemerintah di bagian Rincian dikonversi dengan kurs pajak Kementerian
  Keuangan yang berlaku 4-10 November 2026, yaitu Rp18.141 per dolar AS. Angka-angka itu hanya pembanding dan tidak
  ikut nilai tengah.

| Komoditas | Harga | Tanggal data | Setara per kg | Sumber |
| --- | ---: | --- | ---: | --- |
| Aluminium, harga tunai resmi LME | 3.147 USD/ton | 2026-11-09 | Rp56.832 | [Westmetall, "Market data: Official LME prices"](https://www.westmetall.com/en/markdaten.php?action=table&field=LME_Al_cash) |
| Tembaga, harga tunai resmi LME | 14.030 USD/ton | 2026-11-09 | Rp253.368 | [Westmetall, "Market data: Official LME prices"](https://www.westmetall.com/en/markdaten.php?action=table&field=LME_Cu_cash) |
| Seng, harga tunai resmi LME | 3.754 USD/ton | 2026-11-09 | Rp67.793 | [Westmetall, "Market data: Official LME prices"](https://www.westmetall.com/en/markdaten.php?action=table&field=LME_Zn_cash) |
| Nikel, harga tunai resmi LME | 16.190 USD/ton | 2026-11-09 | Rp292.375 | [Westmetall, "Market data: Official LME prices"](https://www.westmetall.com/en/markdaten.php?action=table&field=LME_Ni_cash) |
| Timah, harga tunai resmi LME | 46.350 USD/ton | 2026-11-09 | Rp837.035 | [Westmetall, "Market data: Official LME prices"](https://www.westmetall.com/en/markdaten.php?action=table&field=LME_Sn_cash) |
| Baja gulungan canai panas (HRC), harga ekspor Indonesia, FOB | 515 USD/ton FOB | 2026-11-10 | Rp9.300 | [Shanghai Metals Market, "[SMM Steel] Indonesian HRC Holds at USD 515/tonne FOB, but March Shipment Dates Erode Mills’ Pricing Power"](https://news.metal.com/newscontent/MUEV57UPVPAFWKEVB355TW4SMYF2RJNO4YUSVPI-smm-steel-indonesian-hrc-holds-at-usd-515tonne-fob-but-march-shipment-dates-erode-mills-pricing-power) |
| Baja HRC SS400 3*1250*C, ekspor Tiongkok, FOB | 485 USD/ton FOB Tiongkok | 2026-11-10 | Rp8.759 | [Mysteel Global, tabel "China Steel Export Prices" di halaman harga harian "Hot-rolled steel export prices: Shanghai port"](https://www.mysteel.net/daily-prices/7236438-hot-rolled-steel-export-prices-shanghai-port) |
| Baja HRC Asia Tenggara, CFR Vietnam | 532 USD/ton CFR Vietnam | 2026-11-05 | Rp9.607 | [Shanghai Metals Market, "[SMM Steel] Vietnam HRC Import Prices Hold Steady Amid Persistent Bid-Offer Gap"](https://news.metal.com/newscontent/104207519-smm-steel-vietnam-hrc-import-prices-hold-steady-amid-persistent-bid-offer-gap) |
| Billet baja, Indonesia, FOB | sekitar 468 USD/ton FOB | 2026-11-09 | Rp8.452 | [Shanghai Metals Market, "[SMM Steel] Indonesian Billet Steady at USD 468/tonne FOB as Firmer Chinese Demand Hints at Upside"](https://news.metal.com/newscontent/BWTELLSMZYWPONBYY74VXQG6TTKJ55DYZEER4IA-smm-steel-indonesian-billet-steady-at-usd-468tonne-fob-as-firmer-chinese-demand-hints-at-upside) |
| Gulungan stainless 304/2B, ekspor Indonesia, FOB | 2.152,5 USD/ton FOB Indonesia | 2026-11-06 | Rp38.872 | [Shanghai Metals Market, "[SMM Analysis] Ex-China Stainless Prices Diverge: Indonesia Rises on Nickel, Europe and Malaysia Slide"](https://news.metal.com/newscontent/104209867-smm-analysis-ex-china-stainless-prices-diverge-indonesia-rises-on-nickel-europe-and-malaysia-slide) |
| Gulungan stainless 304 canai dingin, harga spot dalam negeri Tiongkok, Hongwang 2*1240mm, Wuxi | 2.128 USD/ton | 2026-11-04 | Rp38.430 | [Mysteel Global, "WEEKLY: China 304 stainless prices rally amid futures surge"](https://www.mysteel.net/analysis/5147545-weekly-china-304-stainless-prices-rally-amid-futures-surge) |
| Gulungan stainless 304/NO.1, ekspor Indonesia, FOB | 2.032,5 USD/ton FOB Indonesia | tidak pasti (lihat catatan) | Rp36.705 | [Shanghai Metals Market, "[SMM Stainless Steel Flash] Indonesia Stainless Steel Prices Rise Across the Board by USD30/ton Today"](https://news.metal.com/newscontent/104126024-smm-stainless-steel-flash-indonesia-stainless-steel-prices-rise-across-the-board-by-usd30ton-today) |
| Gulungan stainless 304 canai dingin, rata-rata dunia MEPS | 2.749 USD/ton | 2025-09 | Rp49.644 | [MEPS International, "World Stainless Steel Prices"](https://mepsinternational.com/gb/en/products/world-stainless-steel-prices) |
| Gulungan stainless 304 canai panas, rata-rata dunia MEPS | 2.640 USD/ton | 2025-09 | Rp47.676 | [MEPS International, "World Stainless Steel Prices"](https://mepsinternational.com/gb/en/products/world-stainless-steel-prices) |
| Bijih besi halus 62% Fe, CFR Tiongkok, harian | 92,71 USD/ton | 2026-11-09 | Rp1.674 | [Trading Economics, "Iron Ore"](https://tradingeconomics.com/commodity/iron-ore) |
| Bijih besi halus 62% Fe, CFR Tiongkok, rata-rata bulanan Oktober 2026 | 90,8 USD/ton kering | 2026-10 | Rp1.640 | [Bank Dunia, "Commodities Price Data", 3 November 2026](https://thedocs.worldbank.org/en/doc/74e8be41ceb20fa0da750cda2f6b9e4e-0050012026/related/CMO-Pink-Sheet-November-2026.pdf) |
| Aluminium, rata-rata bulanan LME Oktober 2026 | 3.061 USD/ton | 2026-10 | Rp55.279 | [Bank Dunia, "Commodities Price Data", 3 November 2026](https://thedocs.worldbank.org/en/doc/74e8be41ceb20fa0da750cda2f6b9e4e-0050012026/related/CMO-Pink-Sheet-November-2026.pdf) |
| Tembaga, rata-rata bulanan LME Oktober 2026 | 14.508 USD/ton | 2026-10 | Rp262.000 | [Bank Dunia, "Commodities Price Data", 3 November 2026](https://thedocs.worldbank.org/en/doc/74e8be41ceb20fa0da750cda2f6b9e4e-0050012026/related/CMO-Pink-Sheet-November-2026.pdf) |
| Seng, rata-rata bulanan LME Oktober 2026 | 3.818 USD/ton | 2026-10 | Rp68.949 | [Bank Dunia, "Commodities Price Data", 3 November 2026](https://thedocs.worldbank.org/en/doc/74e8be41ceb20fa0da750cda2f6b9e4e-0050012026/related/CMO-Pink-Sheet-November-2026.pdf) |
| Nikel, rata-rata bulanan LME Oktober 2026 | 15.436 USD/ton | 2026-10 | Rp278.759 | [Bank Dunia, "Commodities Price Data", 3 November 2026](https://thedocs.worldbank.org/en/doc/74e8be41ceb20fa0da750cda2f6b9e4e-0050012026/related/CMO-Pink-Sheet-November-2026.pdf) |
| Timah, rata-rata bulanan LME Oktober 2026 | 51.696 USD/ton | 2026-10 | Rp933.578 | [Bank Dunia, "Commodities Price Data", 3 November 2026](https://thedocs.worldbank.org/en/doc/74e8be41ceb20fa0da750cda2f6b9e4e-0050012026/related/CMO-Pink-Sheet-November-2026.pdf) |

Catatan: harga gulungan stainless 304/NO.1 (dan 316L/NO.1 di bagian Rincian) dari SMM tertulis di daftar "Latest prices"
pada halaman artikelnya. Angka yang sama persis tampil di artikel bertanggal 24 Juli 2026 dan di artikel bertanggal
21 September 2026, jadi daftar itu tampaknya menampilkan harga terbaru saat halaman dibuka (10 November 2026), bukan
harga pada tanggal artikel. Karena itu tanggal datanya ditulis tidak pasti.

## Rincian tiap material

### Baja SS400

- Bahan yang dicari: Baja karbon rendah / struktural SS400 (pelat; round bar sebagai pembanding)
- Massa jenis untuk konversi: 7.850 kg/m3 (sumber: <https://www.distributorplate.com/produk/plat-ss400/>)
- Titik harga yang masuk nilai tengah: Rp11.197; Rp11.486; Rp12.754; Rp13.798; Rp14.044; Rp16.773; Rp16.787
- Nilai tengah Rp13.798 per kg, dibulatkan menjadi **Rp14.000 per kg**.

Nilai tengah tujuh harga pelat dan batang bulat dari tiga distributor, bertanggal April sampai Oktober 2026. Harga eceran potongan kecil hanya dipakai sebagai batas atas rentang. Empat harga lain dari standar harga satuan pemerintah daerah tidak dicantumkan, karena angkanya tidak bisa dicocokkan dengan pasti ke baris yang benar di dokumen PDF-nya.

- **Rp11.197 per kg** - SMS Perkasa, Surabaya - Plat Besi Hitam (daftar harga distributor)
  - Produk: Plat besi hitam 10 mm x 4 ft x 8 ft, per lembar, loco Surabaya; grade SS400 tidak disebut
  - Tertulis di halaman: "Daftar Harga Plat Besi Hitam (Loco Surabaya) per 2 Oktober 2026 ... Plat Besi Hitam 10.0mm x 4ft x 8ft [STD, NB] / 233 kg / Rp 2.613.000"
  - Tanggal data: 2026-10-02
  - Konversi: V = 0,010 x 1,2192 x 2,4384 = 0,029729 m3; m = 0,029729 x 7850 = 233,37 kg; 2.613.000 / 233,37 = Rp11.197/kg
  - Tautan: <https://www.smsperkasa.com/produk/plat-besi-hitam>
- **Rp11.486 per kg** - SMS Perkasa, Surabaya - Plat Kapal (daftar harga distributor)
  - Produk: Plat kapal 10 mm x 5 ft x 20 ft, per lembar, loco Surabaya
  - Tertulis di halaman: "Daftar Harga Plat Kapal (Loco Surabaya) per 28 Oktober 2026 ... Plat Kapal 10.0mm x 5ft x 20ft [GF] / 729 kg / Rp 8.376.700"
  - Tanggal data: 2026-10-28
  - Konversi: V = 0,010 x 1,524 x 6,096 = 0,092903 m3; m = 729,28 kg; 8.376.700 / 729,28 = Rp11.486/kg
  - Tautan: <https://www.smsperkasa.com/produk/plat-kapal>
- **Rp12.754 per kg** - Hagane Perkasa Makmur (Indotrading) - Plat SS400 20 mm (katalog daring)
  - Produk: Plat SS400 20 mm x 4 ft x 8 ft, per lembar
  - Tertulis di halaman: "Plat SS400 20mm x 4' x 8' (ft) SNI ... Rp 5.953.000 / Lembar ... 27 Jul 2026 ... Harga di atas adalah pricelist sebelum diskon"
  - Tanggal data: 2026-07-27
  - Konversi: V = 0,020 x 1,2192 x 2,4384 = 0,059458 m3; m = 466,75 kg; 5.953.000 / 466,75 = Rp12.754/kg
  - Tautan: <https://haganeperkasamakmur.web.indotrading.com/product/plat-ss400-20mm-x-4-x-8-ft-sni-p1902258.aspx>
- **Rp13.798 per kg** - Hagane Perkasa Makmur (Indotrading) - Round bar SS400 12 mm (katalog daring)
  - Produk: Round bar SS400 diameter 12 mm x 6 m, per batang
  - Tertulis di halaman: "Besi Assental SS400 / Besi Round Bar 12mm x 6M SNI ... Rp 73.500 / Batang ... Diperbarui 17 Jul 2026"
  - Tanggal data: 2026-07-17
  - Konversi: A = pi/4 x 0,012^2 = 1,1310e-4 m2; V = 6,7858e-4 m3; m = 5,327 kg; 73.500 / 5,327 = Rp13.798/kg
  - Tautan: <https://haganeperkasamakmur.web.indotrading.com/product/besi-assental-ss400-besi-round-bar-12mm-x-6m-sni-p1895952.aspx>
- **Rp14.044 per kg** - Hagane Perkasa Makmur (Indotrading) - Plat Hitam A36 (katalog daring)
  - Produk: Plat hitam ASTM A36 10 mm x 1200 x 2400 mm, per lembar
  - Tertulis di halaman: "Plat Hitam A36 10mm x 1200 x 2400 SNI ... Rp 3.175.000 / Lembar ... Diperbarui 27 Jul 2026"
  - Tanggal data: 2026-07-27
  - Konversi: V = 0,010 x 1,2 x 2,4 = 0,0288 m3; m = 226,08 kg; 3.175.000 / 226,08 = Rp14.044/kg
  - Tautan: <https://haganeperkasamakmur.web.indotrading.com/product/plat-hitam-a36-10mm-x-1200-x-2400-sni-p1902092.aspx>
- **Rp16.773 per kg** - SMS Perkasa, Surabaya - Besi As ST41 (round bar) (daftar harga distributor)
  - Produk: Round bar ST41 diameter 25 mm x 6 m, per batang
  - Tertulis di halaman: "Daftar Harga Besi As ST41 (Loco Surabaya) per 11 Agustus 2026 ... Besi As ST41 25mm x 6M [NB] / 23.12 kg / Rp 387.800"
  - Tanggal data: 2026-08-11
  - Konversi: A = pi/4 x 0,025^2 = 4,9087e-4 m2; V = 2,9452e-3 m3; m = 23,12 kg; 387.800 / 23,12 = Rp16.773/kg
  - Tautan: <https://www.smsperkasa.com/produk/besi-as-st41>
- **Rp16.787 per kg** - Sahabat Besi Baja, Surabaya - Plat kapal 5 mm SS400 (daftar harga distributor)
  - Produk: Plat kapal SS400 5 mm x 1,5 m x 6 m, per lembar
  - Tertulis di halaman: "Plat Besi / Plat Baja KAPAL 5 MM SS400 ... IDR 5,930,000 / Lembar ... Size 1.5M x 6M ... Updated Apr 2, 2026"
  - Tanggal data: 2026-04-02
  - Konversi: V = 0,005 x 1,5 x 6 = 0,045 m3; m = 353,25 kg; 5.930.000 / 353,25 = Rp16.787/kg
  - Tautan: <https://en.sahabatbesibaja.com/product/plat-besi-plat-baja-kapal-5-mm-ss400-p1389881.aspx>
- **Rp19.745 per kg** - Shopee toko Rian Baja (toko daring, lewat BigGo)
  - Produk: Potongan pelat SS400 10 x 400 x 500 mm, eceran
  - Tertulis di halaman: "Besi Plat 10mm 400 x 500mm SS400 Rp310.000 (Rian Baja, KAB. BEKASI)"
  - Tanggal data: tidak tercantum (judul halaman 'Terbaru Nov 2026')
  - Konversi: V = 0,010 x 0,4 x 0,5 = 0,002 m3; m = 15,70 kg; 310.000 / 15,70 = Rp19.745/kg
  - Tautan: <https://biggo.id/s/plat%20ss400/>

### Baja karbon S45C

- Bahan yang dicari: Baja karbon menengah S45C (round bar, as hitam)
- Massa jenis untuk konversi: 7.850 kg/m3 (sumber: <https://xometry.asia/wp-content/uploads/2021/03/1773390136-S45C-Datasheet.pdf>)
- Titik harga yang masuk nilai tengah: Rp19.287; Rp19.999
- Nilai tengah Rp19.643 per kg, dibulatkan menjadi **Rp19.500 per kg**.

Nilai tengah dua harga distributor untuk batang bulat diameter 50 mm panjang 6 m (Juni dan Agustus 2026). Harga potongan eceran menjadi batas atas rentang. Sumber setingkat distributor hanya dua, jadi angka ini cukup untuk estimasi awal tetapi sebaiknya dicek ulang ke pemasok sebelum dipakai menawar.

- **Rp19.287 per kg** - Hagane Perkasa Makmur (Indotrading) - Besi As S45C 50 mm (katalog daring)
  - Produk: Besi As S45C 50 mm x 6 m, per batang
  - Tertulis di halaman: "Besi AS S45C 50mm x 6M Full SNI Asental AS Hitam ... Rp 1.783.700 / Pieces ... Diperbarui 18 Jun 2026 ... Harga di atas adalah pricelist sebelum diskon."
  - Tanggal data: 2026-06-18
  - Konversi: m = 92,48 kg; 1.783.700 / 92,48 = Rp19.287/kg
  - Tautan: <https://haganeperkasamakmur.web.indotrading.com/product/besi-as-s45c-50mm-x-6m-full-sni-asental-as-hitam-p1296362.aspx>
- **Rp19.999 per kg** - SMS Perkasa, Surabaya - Besi As S45C (daftar harga distributor)
  - Produk: Besi As S45C diameter 50 mm x 6 m, per batang, loco Surabaya
  - Tertulis di halaman: "Daftar Harga Besi As S45C (Loco Surabaya) per 11 Agustus 2026 ... Besi As S45C 50mm x 6M [NB] / 94 kg / Rp 1.849.500"
  - Tanggal data: 2026-08-11
  - Konversi: A = pi/4 x 0,05^2 = 1,9635e-3 m2; V = 0,0117810 m3; m = 92,48 kg (situs 94 kg); 1.849.500 / 92,48 = Rp19.999/kg (dengan 94 kg: 19.676)
  - Tautan: <https://www.smsperkasa.com/produk/besi-as-s45c>
- **Rp35.122 per kg** - Shopee toko Ultra Metal (toko daring, lewat BigGo)
  - Produk: Potongan as S45C diameter 45 mm, per 1 cm panjang, eceran
  - Tertulis di halaman: "AS S45C DIAMETER 45MM / AS ST60 / AS CARBON STEEL S45C PER 1CM Rp4.385 (Ultra Metal)"
  - Tanggal data: tidak tercantum (judul halaman 'Terbaru Nov 2026')
  - Konversi: A = pi/4 x 0,045^2 = 1,5904e-3 m2; 1 cm = 1,5904e-5 m3; m = 0,12485 kg; 4.385 / 0,12485 = Rp35.122/kg
  - Tautan: <https://biggo.id/s/as%20s45c/>
- **Rp36.494 per kg** - Shopee toko IndotekMachinery (toko daring, lewat BigGo)
  - Produk: Potongan as S45C diameter 100 mm x 50 cm, eceran
  - Tertulis di halaman: "As S45C 100mm Panjang 50cm AS S45C Rp1.125.000 (IndotekMachinery)"
  - Tanggal data: tidak tercantum (judul halaman 'Terbaru Nov 2026')
  - Konversi: A = pi/4 x 0,1^2 = 7,854e-3 m2; V = 3,927e-3 m3; m = 30,83 kg; 1.125.000 / 30,83 = Rp36.494/kg
  - Tautan: <https://biggo.id/s/as%20s45c/>

Selain itu ada 2 harga lain yang tidak dicantumkan: 1 karena angkanya tidak bisa dipastikan dari halaman sumbernya; 1 karena harganya lama, bukan harga 2026.

### Baja paduan SCM440

- Bahan yang dicari: Baja paduan krom-molibdenum SCM440 (round bar)
- Massa jenis untuk konversi: 7.850 kg/m3 (sumber: <https://otaisteel.com/?p=17666>)
- Titik harga yang masuk nilai tengah: Rp27.000
- Nilai tengah Rp27.000 per kg, dibulatkan menjadi **Rp27.000 per kg**.

Hanya satu harga setingkat distributor yang ditemukan (diperbarui 29 Oktober 2026); dua harga lainnya adalah potongan eceran yang jauh lebih mahal dan hanya dipakai sebagai batas atas. Angka distributor itu dipakai apa adanya, jadi anggaplah sebagai perkiraan kasar.

- **Rp27.000 per kg** - PT. Gitamulia Cemerlang (Indotrading) - Alloy Steel Type SCM 440 (katalog daring)
  - Produk: Alloy Steel SCM 440, per kilogram (min. 1 kg); bentuk/dimensi tidak dinyatakan
  - Tertulis di halaman: "Alloy Steel Type SCM 440 ... Rp 27.000 / Kilogram ... Diperbarui 29 Okt 2026 ... Harga di atas adalah pricelist sebelum diskon"
  - Tanggal data: 2026-10-29 (tanggal 'Diperbarui' di halaman; harganya sendiri bisa lebih lama)
  - Konversi: sudah per kg
  - Tautan: <https://gitamuliacemerlang.web.indotrading.com/product/alloy-steel-type-scm-440-p924977.aspx>
- **Rp48.152 per kg** - Shopee toko Ultra Metal (toko daring, lewat BigGo)
  - Produk: Potongan as SCM440/AISI 4140 diameter 40 mm, per 1 cm, eceran
  - Tertulis di halaman: "AS BESI BAJA VCL 140 ( 40MM ) AISI 4140 SCM 440 PER 1CM Rp4.750 (Ultra Metal)"
  - Tanggal data: tidak tercantum (judul halaman 'Terbaru Nov 2026')
  - Konversi: A = pi/4 x 0,04^2 = 1,2566e-3 m2; 1 cm = 1,2566e-5 m3; m = 0,09865 kg; 4.750 / 0,09865 = Rp48.152/kg
  - Tautan: <https://biggo.id/s/as%20vcl%20140/>
- **Rp49.166 per kg** - Shopee toko Bintang Teknik Hydraulic (toko daring, lewat BigGo)
  - Produk: Potongan as SCM 440 diameter 40 mm x 1 m, eceran
  - Tertulis di halaman: "as besi scm 440 ukuran 40,mm x 1,mtr Rp485.000 (Bintang Teknik Hydraulic)"
  - Tanggal data: tidak tercantum (judul halaman 'Terbaru Nov 2026')
  - Konversi: V = 1,2566e-3 m3; m = 9,865 kg; 485.000 / 9,865 = Rp49.166/kg
  - Tautan: <https://biggo.id/s/scm%20440/>

Selain itu ada 2 harga lain yang tidak dicantumkan: 1 karena angkanya tidak bisa dipastikan dari halaman sumbernya; 1 karena bahannya bukan yang dicari.

### Stainless SUS304

- Bahan yang dicari: Stainless steel SUS304 (AISI 304) - round bar / pelat
- Massa jenis untuk konversi: 7.930 kg/m3 (sumber: <https://www.jssa.gr.jp/contents/faq-article/q6/>)
- Titik harga yang masuk nilai tengah: Rp49.000; Rp52.926; Rp54.300
- Nilai tengah Rp52.926 per kg, dibulatkan menjadi **Rp53.000 per kg**.

Nilai tengah tiga harga pelat dan batang dari dua distributor dan satu toko daring. Satu dari tiga harga itu, Rp54.300 dari Sutindo Store, berasal dari halaman tanpa tanggal yang tampaknya belum diperbarui sejak 2023-2024; tanpa harga itu nilai tengahnya Rp50.963 dan dibulatkan menjadi Rp51.000. Harga acuan tetap Rp53.000 karena masih dekat dengan daftar harga distributor SMS Perkasa bertanggal 26 Oktober 2026 (Rp49.000 per kg untuk pelat, sudah termasuk PPN). Harga patokan ekspor (FOB) dan estimasi umum dari artikel daring lebih rendah, sedangkan potongan eceran kecil di toko daring lebih tinggi. Harga FOB dan estimasi itu hanya dipakai untuk menilai kewajaran; kolom rentang di Ringkasan hanya memuat harga jual di Indonesia.

- **Rp36.705 per kg** - SMM - Indonesia stainless steel FOB flash (harga patokan bursa)
  - Produk: Coil 304 No.1 mill edge, FOB Indonesia, USD/ton
  - Tertulis di halaman: "Published: Jul 24, 2026 14:02 (GMT+8) ... 304/NO.1 coil mill edge (FOB) avg. USD 2,032.5/ton"
  - Tanggal data: tidak pasti; artikelnya bertanggal 24 Juli 2026, tetapi angkanya dari daftar "Latest prices" di halaman itu (lihat catatan di bagian Kurs dan harga pembanding dunia)
  - Konversi: 2.032,5 USD/ton / 1000 = 2,0325 USD/kg x 18.059 (JISDOR BI 10 Nov 2026) = Rp36.705/kg
  - Tautan: <https://news.metal.com/en/newscontent/104023423-smm-stainless-steel-flash-indonesia-stainless-steel-prices-rise-again-across-the-board-by-usd30ton>
- **Rp47.500 per kg** - Konstruksiin.com - Harga Stainless Steel per Kg (artikel daring)
  - Produk: Estimasi umum SS304, bentuk tidak dijelaskan
  - Tertulis di halaman: "Berikut estimasi harga di pasaran Indonesia (2025): ... Stainless Steel 304: Rp 40.000 – Rp 55.000 per kg"
  - Tanggal data: tidak tercantum; artikel menyebutnya estimasi 2025
  - Konversi: sudah per kg; titik tengah 47.500
  - Tautan: <https://konstruksiin.com/harga-stainless-steel-per-kg/>
- **Rp49.000 per kg** - SMS Perkasa, Surabaya - Plat Stainless SS304 (daftar harga distributor)
  - Produk: Pelat SS304 3,0 mm x 4 ft x 8 ft, F1, per lembar 70,8 kg, loco Surabaya
  - Tertulis di halaman: "Daftar Harga Plat Stainless SS304 (Loco Surabaya) per 26 Oktober 2026 ... 3.0mm x 4ft x 8ft [F1, NB] ... 70.8 kg ... Rp 3.469.200 ... Harga yang tertera di website SMS Perkasa sudah termasuk PPN."
  - Tanggal data: 2026-10-26
  - Konversi: 3.469.200 / 70,8 = Rp49.000/kg (cek: 1,0 mm 2B 1.156.400 / 23,6 = 49.000; 6,0 mm F1 6.938.400 / 141,6 = 49.000)
  - Tautan: <https://www.smsperkasa.com/produk/plat-stainless-ss304>
- **Rp52.926 per kg** - Tokopedia - As Stainless 304 / Round Bar 63,5 mm (toko daring, lewat BigGo)
  - Produk: Round bar SS304 d 63,5 mm x 1200 mm, per potong
  - Tertulis di halaman: "As Stainless 304 / Round Bar (Diameter 63,5) Panjang 1200mm / 120cm ... Rp1.595.000 ... Tokopedia"
  - Tanggal data: tidak tercantum per penawaran; judul halaman 'Harga round bar stainless 304 Terbaru Nov 2026'
  - Konversi: V = pi/4 x 0,0635^2 x 1,2 = 0,0038003 m3; m = V x 7930 = 30,136 kg; 1.595.000 / 30,136 = Rp52.926/kg
  - Tautan: <https://biggo.id/s/round+bar+stainless+304/>
- **Rp54.300 per kg** - Sutindo Store - As Stainless Steel SS 304 1 In (daftar harga distributor)
  - Produk: Round bar SS304 1 inci, 6-6,15 m, per batang 24 kg
  - Tertulis di halaman: "Rp1.303.200 ... Grade: SS 304 ... Panjang: 6 – 6.15 Meter ... Berat: 24 KG / Batang ... ©2024 Sutindo Store"
  - Tanggal data: tidak tercantum di halaman; kaki halaman bertahun 2024 dan data produknya terakhir diubah 2023-10-09, jadi harganya kemungkinan dari 2023
  - Konversi: 1.303.200 / 24 = Rp54.300/kg
  - Tautan: <https://store.sutindo.com/product/as-stainless-steel-ss-304-1-in-x-6-6-15-meter/>
- **Rp77.956 per kg** - Shopee 'SASA SUMBERJAYA LOGAM' (toko daring, lewat BigGo)
  - Produk: As SS304 d 22 mm x 1000 mm, per potong
  - Tertulis di halaman: "As stainless 304 diameter 22mm x 1000mm/ As stainless steel 304 / Sus 304 ... Rp235.000 ... SASA SUMBERJAYA LOGAM"
  - Tanggal data: tidak tercantum per penawaran; judul halaman 'Terbaru Nov 2026'
  - Konversi: V = 0,00038013 m3; m = 3,0145 kg; 235.000 / 3,0145 = Rp77.956/kg
  - Tautan: <https://biggo.id/s/as+stainless+304/>
- **Rp80.922 per kg** - Shopee 'Pusat spring' (toko daring, lewat BigGo)
  - Produk: As SS304 d 25 mm x 1000 mm, per potong
  - Tertulis di halaman: "AS STAINLESS 304 DIAMETER 25mm × 1000mm / AS SUS 304 /AS STAINLESS STEEL ... Rp315.000 ... Pusat spring"
  - Tanggal data: tidak tercantum per penawaran; judul halaman 'Terbaru Nov 2026'
  - Konversi: V = pi/4 x 0,025^2 x 1 = 0,00049087 m3; m = 3,8926 kg; 315.000 / 3,8926 = Rp80.922/kg
  - Tautan: <https://biggo.id/s/as+stainless+304/>
- **Rp89.914 per kg** - Shopee demora.el, Jakarta Selatan (toko daring, lewat BigGo)
  - Produk: As SS304 d 25 mm x 500 mm, per potong
  - Tertulis di halaman: "Round bar as stainless 304 25mm x 500mm ... Rp175.000 ... Shopee (penjual demora.el, Kota Jakarta Selatan)"
  - Tanggal data: tidak tercantum per penawaran; judul halaman 'Terbaru Nov 2026'
  - Konversi: V = pi/4 x 0,025^2 x 0,5 = 0,00024544 m3; m = 1,9463 kg; 175.000 / 1,9463 = Rp89.914/kg
  - Tautan: <https://biggo.id/s/round+bar+stainless+304/>

Selain itu ada 1 harga lain yang tidak dicantumkan karena angkanya tidak bisa dipastikan dari halaman sumbernya.

### Stainless SUS316L

- Bahan yang dicari: Stainless steel SUS316 (AISI 316) - round bar / pelat
- Massa jenis untuk konversi: 7.980 kg/m3 (sumber: <https://www.jssa.gr.jp/contents/faq-article/q6/>)
- Titik harga yang masuk nilai tengah: Rp94.874; Rp100.381; Rp143.598
- Nilai tengah Rp100.381 per kg, dibulatkan menjadi **Rp100.000 per kg**.

Nilai tengah tiga harga dari dua distributor dan satu toko daring. Satu dari tiga harga itu, Rp94.874 dari Sutindo Store, berasal dari halaman tanpa tanggal yang tampaknya belum diperbarui sejak 2023-2024; tanpa harga itu nilai tengahnya Rp121.989,5 dan dibulatkan menjadi Rp120.000. Harga acuan tetap Rp100.000 karena masih dekat dengan daftar harga distributor SMS Perkasa bertanggal 26 Oktober 2026 (Rp98.983 sampai Rp102.506 per kg untuk pelat berbagai tebal, sudah termasuk PPN). Harga patokan ekspor (FOB) dan estimasi umum dari artikel daring lebih rendah dan hanya dipakai untuk menilai kewajaran. Semua sumber yang ditemukan di Indonesia adalah varian karbon rendah 316L, sehingga nama di program ditulis SUS316L.

- **Rp74.629 per kg** - SMM - Indonesia stainless steel FOB flash (harga patokan bursa)
  - Produk: Coil 316L No.1 mill edge, FOB Indonesia, USD/ton
  - Tertulis di halaman: "Published: Jul 24, 2026 14:02 (GMT+8) ... 316L/NO.1 coil mill edge (FOB) avg. USD 4,132.5/ton"
  - Tanggal data: tidak pasti; artikelnya bertanggal 24 Juli 2026, tetapi angkanya dari daftar "Latest prices" di halaman itu (lihat catatan di bagian Kurs dan harga pembanding dunia)
  - Konversi: 4.132,5 / 1000 = 4,1325 USD/kg x 18.059 (JISDOR 10 Nov 2026) = Rp74.629/kg
  - Tautan: <https://news.metal.com/en/newscontent/104023423-smm-stainless-steel-flash-indonesia-stainless-steel-prices-rise-again-across-the-board-by-usd30ton>
- **Rp77.500 per kg** - Konstruksiin.com - Harga Stainless Steel per Kg (artikel daring)
  - Produk: Estimasi umum SS316
  - Tertulis di halaman: "Stainless Steel 316: Rp 65.000 – Rp 90.000 per kg"
  - Tanggal data: tidak tercantum; artikel menyebutnya estimasi 2025
  - Konversi: sudah per kg; titik tengah 77.500
  - Tautan: <https://konstruksiin.com/harga-stainless-steel-per-kg/>
- **Rp94.874 per kg** - Sutindo Store - As Stainless Steel SS 316L 5/8 In (daftar harga distributor)
  - Produk: Round bar SS316L 5/8 in, 6-6,35 m, 9,56 kg/batang
  - Tertulis di halaman: "Rp907.000 ... Grade: SS 316L ... Panjang: 6 – 6.35 Meter ... Berat: 9.56 KG / Batang ... ©2024 Sutindo Store"
  - Tanggal data: tidak tercantum di halaman; kaki halaman bertahun 2024 dan produk serupa di toko yang sama terakhir diubah 2023-10-09, jadi harganya kemungkinan lama
  - Konversi: 907.000 / 9,56 = Rp94.874/kg
  - Tautan: <https://store.sutindo.com/product/as-stainless-steel-ss-316l-5-8-in-x-6-6-35-meter/>
- **Rp100.381 per kg** - SMS Perkasa, Surabaya - Plat Stainless SS316L (daftar harga distributor)
  - Produk: Pelat SS316L 6,0 mm x 4 ft x 8 ft F1, 142,2 kg/lembar, loco Surabaya
  - Tertulis di halaman: "Daftar Harga Plat Stainless SS316L (Loco Surabaya) per 26 Oktober 2026 ... 6.0mm (F1, NB) Berat 142.2 kg Harga Rp 14.274.200 ... sudah termasuk PPN"
  - Tanggal data: 2026-10-26
  - Konversi: 14.274.200 / 142,2 = Rp100.381/kg (3,0 mm F1: 7.037.700 / 71,1 = 98.983; 1,0 mm 2B 2.429.400 / 23,7 = 102.506; 1,5 mm 2B 3.602.200 / 35,55 = 101.328; 2,0 mm 2B 4.803.300 / 46,96 = 102.285)
  - Tautan: <https://www.smsperkasa.com/produk/plat-stainless-ss316l>
- **Rp143.598 per kg** - Tokopedia - as rod stainless 316 d 50 mm (toko daring, lewat BigGo)
  - Produk: As SS316 d 50 mm x 1000 mm, per potong
  - Tertulis di halaman: "as rod stainless diameter 50 mm x panjang 1000mm /316 ... Rp2.250.000 ... Tokopedia (toko sjaya-70)"
  - Tanggal data: tidak tercantum per penawaran; judul halaman 'Harga as stainless 316 Terbaru Nov 2026'
  - Konversi: V = pi/4 x 0,05^2 x 1 = 0,0019635 m3; m = V x 7980 = 15,669 kg; 2.250.000 / 15,669 = Rp143.598/kg
  - Tautan: <https://biggo.id/s/as+stainless+316/>

Selain itu ada 3 harga lain yang tidak dicantumkan karena angkanya tidak bisa dipastikan dari halaman sumbernya.

### Besi cor FC250

- Bahan yang dicari: Besi cor kelabu FC250 (JIS G5501) - as cor / bar pejal
- Massa jenis untuk konversi: 7.200 kg/m3 (sumber: <https://www.mfgrobots.com/Article/material/metal/39356.html>)
- Titik harga yang masuk nilai tengah: Rp36.474; Rp37.275; Rp37.333; Rp37.923; Rp41.053; Rp51.684; Rp81.127
- Nilai tengah Rp37.923 per kg, dibulatkan menjadi **Rp38.000 per kg**.

Nilai tengah tujuh penawaran batang cor FC25 bermassa sedikitnya 1 kg. Enam dari tujuh penawaran berasal dari toko yang sama, dan tidak ditemukan harga pengecoran per pola untuk 2026, jadi angka ini perkiraan kasar.

- **Rp36.474 per kg** - Shopee Sutindo Global Store, Surabaya (toko daring, lewat BigGo)
  - Produk: As cor pejal FC25 d 65 mm x 49 cm
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter 65MM Panjang Custom ... Rp210.000 ~ Rp427.000; 49 CM Rp427.000 ... Sutindo Global Store"
  - Tanggal data: tidak tercantum per penawaran; judul halaman 'Harga as cor fc25 Terbaru Nov 2026'
  - Konversi: V = pi/4 x 0,065^2 x 0,49 = 0,0016260 m3; m = V x 7200 = 11,707 kg; 427.000 / 11,707 = Rp36.474/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>
- **Rp37.275 per kg** - Shopee Sutindo Global Store (toko daring, lewat BigGo)
  - Produk: As cor FC25 d 60 mm x 17 cm
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter 60MM Panjang Custom ... 17 CM Rp129.000"
  - Tanggal data: tidak tercantum per penawaran; halaman berlabel 'Nov 2026'
  - Konversi: V = 0,00048066 m3; m = 3,4608 kg; 129.000 / 3,4608 = Rp37.275/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>
- **Rp37.333 per kg** - Shopee Sutindo Global Store (toko daring, lewat BigGo)
  - Produk: As cor FC25 d 75 mm x 16 cm
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter 75MM Panjang Custom ... 16 CM Rp190.000"
  - Tanggal data: tidak tercantum per penawaran; halaman berlabel 'Nov 2026'
  - Konversi: V = 0,00070686 m3; m = 5,0894 kg; 190.000 / 5,0894 = Rp37.333/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>
- **Rp37.923 per kg** - Shopee Sutindo Global Store (toko daring, lewat BigGo)
  - Produk: As cor FC25 d 30 mm, batang utuh +/- 3 m
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter Kecil Panjang +/- 3 Meter ... 30MM,UTUH Rp579.000"
  - Tanggal data: tidak tercantum per penawaran; halaman berlabel 'Nov 2026'
  - Konversi: V = pi/4 x 0,03^2 x 3 = 0,0021206 m3; m = 15,268 kg; 579.000 / 15,268 = Rp37.923/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>
- **Rp41.053 per kg** - Shopee Sutindo Global Store (toko daring, lewat BigGo)
  - Produk: As cor FC25 d 40 mm x 21 cm
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter 40MM Panjang Custom ... 21 CM Rp78.000"
  - Tanggal data: tidak tercantum per penawaran; halaman berlabel 'Nov 2026'
  - Konversi: V = 0,00026389 m3; m = 1,900 kg; 78.000 / 1,900 = Rp41.053/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>
- **Rp51.684 per kg** - Shopee Sutindo Official Store (toko daring, lewat BigGo)
  - Produk: As cor FC25 d 25 mm, batang +/- 3 m
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter Kecil Panjang +/- 3 Meter ... 25 MM Rp548.000"
  - Tanggal data: tidak tercantum per penawaran; halaman berlabel 'Nov 2026'
  - Konversi: V = 0,0014726 m3; m = 10,603 kg; 548.000 / 10,603 = Rp51.684/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>
- **Rp81.127 per kg** - Shopee Logam Jaya Makmur, Jakarta Barat (toko daring, lewat BigGo)
  - Produk: As cor FC25 d 25 mm x 56,5 cm
  - Tertulis di halaman: "AS COR SOLID FC25 Diameter 25MM Panjang Custom ... Rp162.000 ~ Rp217.000; 56.5 CM Rp162.000"
  - Tanggal data: tidak tercantum per penawaran; halaman berlabel 'Nov 2026'
  - Konversi: V = 0,00027734 m3; m = 1,9969 kg; 162.000 / 1,9969 = Rp81.127/kg
  - Tautan: <https://biggo.id/s/as+cor+fc25/>

Selain itu ada 3 harga lain yang tidak dicantumkan: 2 karena angkanya tidak bisa dipastikan dari halaman sumbernya; 1 karena harganya lama, bukan harga 2026.

### Aluminium 6061-T6

- Bahan yang dicari: Aluminium paduan 6061-T6 (round bar / pelat)
- Massa jenis untuk konversi: 2.700 kg/m3 (sumber: <https://www.aalco.co.uk/datasheets/Aluminium-Alloy-6061-T6-Extrusions_145.ashx>)
- Titik harga yang masuk nilai tengah: Rp111.934; Rp126.894; Rp135.165; Rp135.183; Rp157.190; Rp193.099
- Nilai tengah Rp135.174 per kg, dibulatkan menjadi **Rp135.000 per kg**.

Nilai tengah enam penawaran batang dan pelat di toko daring. Harga mineral acuan (HMA) pemerintah, nilai impor (CIF), dan perkiraan harga di situs satu penjual bertanggal Januari 2026 jauh lebih rendah; ketiganya hanya dipakai sebagai batas bawah kewajaran, bukan sebagai harga beli bengkel.

- **Rp55.274 per kg** - HMA ESDM periode I November 2026, dikutip Indonesian Mining Association dari Bisnis.com (dokumen pemerintah)
  - Produk: Aluminium primer, USD/dmt (bukan bar/pelat)
  - Tertulis di halaman: "HMA aluminium turun dari S$3.172,93 per dmt pada periode kedua Oktober menjadi US$3.046,9"
  - Tanggal data: 2026-11-01
  - Konversi: 3.046,9 USD/ton / 1000 = USD 3,0469/kg x Rp18.141 (kurs pajak Kemenkeu 04-10 Nov 2026) = Rp55.274/kg
  - Tautan: <https://ima-api.org/harga-acuan-batu-bara-hba-awal-november-2026-kalori-tinggi-turun-jadi-us1256/>
- **Rp79.914 per kg** - UN Comtrade - impor Indonesia HS 760429 Juli 2026 (data statistik)
  - Produk: Impor batang/profil paduan aluminium selain berongga, semua paduan; CIF USD
  - Tertulis di halaman: "cmdCode 760429 / altQty 1102751.577 / cifvalue 4857786.53 / period 202607"
  - Tanggal data: 2026-07
  - Konversi: 4.857.786,53 / 1.102.751,577 = USD 4,4052/kg x Rp18.141 (kurs pajak Kemenkeu 04-10 Nov 2026) = Rp79.914/kg
  - Tautan: <https://comtradeapi.un.org/public/v1/preview/C/M/HS?reporterCode=360&period=202607&cmdCode=760429,740721,740729&flowCode=M&partnerCode=0&partner2Code=0&customsCode=C00&motCode=0&includeDesc=true>
- **Rp80.000 per kg** - PT Jaya Abadi Sinar Alumindo (jasalumindo.com) (katalog daring)
  - Produk: Round bar 6061 per kg (estimasi, tabel harga 2026)
  - Tertulis di halaman: "Round Bar (As) 6061 / Per Kg / Rp 70.000 - Rp 90.000 ; Plat Aluminium 6061 / Per Kg / Rp 65.000 - Rp 85.000"
  - Tanggal data: 2026-01-26 (Jan 26, 2026)
  - Konversi: sudah per kg; titik tengah (70.000 + 90.000) / 2 = Rp80.000/kg
  - Tautan: <https://jasalumindo.com/jual-aluminium-6061-harga-terbaik-dari-pabrik/>
- **Rp111.934 per kg** - makmurjayastell (Shopee, Kota Jakarta Utara) - pelat potongan (toko daring, lewat BigGo)
  - Produk: Pelat aluminium 6061 tebal 30 x 150 x 50 mm; harga per potong
  - Tertulis di halaman: "plat aluminium 6061 tebal 30mm x 150mm x 50mm plat aluminium dural 6061 - Rp68.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = 3 x 15 x 5 = 225 cm3; m = 0,6075 kg; 68.000 / 0,6075 = Rp111.934/kg
  - Tautan: <https://biggo.id/s/plat%20aluminium%206061>
- **Rp126.894 per kg** - Sutindo Global Store / Sutindo Official Store (Shopee) (toko daring, lewat BigGo)
  - Produk: As 6061-T6 diameter 1 3/4 inci (44,45 mm), judul 1 batang 3 m; harga per batang (label 'POT 1 METERAN' tidak dijelaskan di halaman)
  - Tertulis di halaman: "'1 3/4 Inchi,POT 1 METERAN' Rp1.595.000 (judul: AS Dural Aluminium 6061 Diameter Besar Panjang 3 METER - T6; kisaran Rp1.595.000 ~ Rp3.977.000)"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 4,445^2 x 300 = 4.655 cm3; m = 4.655 x 2,70 g/cm3 = 12,57 kg; 1.595.000 / 12,57 = Rp126.894/kg
  - Tautan: <https://biggo.id/s/sutindo%20aluminium%206061>
- **Rp135.165 per kg** - SOEMBER SUKSES (Lazada) (toko daring, lewat BigGo)
  - Produk: As dural 6061 diameter 32 mm x 385 mm; harga per potong (temper tidak disebut)
  - Tertulis di halaman: "As Dural 6061 Diameter 32mm x 385mm - Rp113.000 - Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 3,2^2 x 38,5 = 309,6 cm3; m = 0,836 kg; 113.000 / 0,836 = Rp135.165/kg
  - Tautan: <https://biggo.id/s/as%20aluminium%206061>
- **Rp135.183 per kg** - Rangka Raya Store (Shopee) (toko daring, lewat BigGo)
  - Produk: As aluminium 6061 diameter 10 mm x 3 m; harga per batang (temper tidak disebut)
  - Tertulis di halaman: "'AS ALUMINIUM 6061 10MM X 3M SAA // Batang Aluminium Padat untuk CNC & Fabrikasi, Industri, Otomotif' Rp86.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 1,0^2 x 300 = 235,6 cm3; m = 0,6362 kg; 86.000 / 0,6362 = Rp135.183/kg
  - Tautan: <https://biggo.id/s/aluminium%206061%20batang%203%20meter>
- **Rp157.190 per kg** - BINTANGJAYATEHNIK72 (Kab. Bekasi, Shopee) (toko daring, lewat BigGo)
  - Produk: As aluminium 6061 diameter 30 mm, potongan 100 cm; harga per potong
  - Tertulis di halaman: "AS ALUMINIUM 30mm 6061 ( AS ALUMINIUM POTONGAN ) - kisaran Rp60.000 ~ Rp350.000, varian 30MM-100CM Rp300.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 3,0^2 x 100 = 706,9 cm3; m = 1,9085 kg; 300.000 / 1,9085 = Rp157.190/kg
  - Tautan: <https://biggo.id/s/as%20aluminium%206061>
- **Rp193.099 per kg** - renowaben (Shopee) - pelat potongan (toko daring, lewat BigGo)
  - Produk: Pelat 6061-T6 12 x 130 x 600 mm; harga per potong
  - Tertulis di halaman: "Plat Aluminium 12 X 130 X 600 T6 6061 - Rp488.000 - Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = 1,2 x 13 x 60 = 936 cm3; m = 2,527 kg; 488.000 / 2,527 = Rp193.099/kg
  - Tautan: <https://biggo.id/s/aluminium%206061%20t6>

Selain itu ada 1 harga lain yang tidak dicantumkan karena angkanya tidak bisa dipastikan dari halaman sumbernya.

### Kuningan C3604

- Bahan yang dicari: Kuningan (brass) C3604 / CuZn39Pb3 (round bar)
- Massa jenis untuk konversi: 8.470 kg/m3 (sumber: <https://www.aalco.co.uk/datasheets/Copper-and-Copper-Alloys-CW614N-Brass-Rod_31.ashx>)
- Titik harga yang masuk nilai tengah: Rp214.061; Rp226.000; Rp250.437; Rp265.571; Rp300.940
- Nilai tengah Rp250.437 per kg, dibulatkan menjadi **Rp250.000 per kg**.

Nilai tengah lima penawaran batang bulat di toko daring. Tidak semua penjual menyebut grade C3604, jadi angka ini mewakili kuningan batang yang umum dijual untuk dibubut. Nilai impor (CIF) dari data statistik jauh lebih rendah dan hanya dipakai sebagai pembanding.

- **Rp174.079 per kg** - UN Comtrade - impor Indonesia HS 740721 Juli 2026 (data statistik)
  - Produk: Impor batang/profil kuningan semua grade, CIF USD
  - Tertulis di halaman: "cmdCode 740721 / altQty 106566.255 / cifvalue 1022596.073 / period 202607"
  - Tanggal data: 2026-07
  - Konversi: 1.022.596,073 / 106.566,255 = USD 9,5959/kg x Rp18.141 (kurs pajak Kemenkeu 04-10 Nov 2026) = Rp174.079/kg
  - Tautan: <https://comtradeapi.un.org/public/v1/preview/C/M/HS?reporterCode=360&period=202607&cmdCode=760429,740721,740729&flowCode=M&partnerCode=0&partner2Code=0&customsCode=C00&motCode=0&includeDesc=true>
- **Rp214.061 per kg** - IZDIHAR TEHNiK MANDIRI 02 (Shopee) (toko daring, lewat BigGo)
  - Produk: As kuningan diameter 25 mm x 1000 mm; per batang
  - Tertulis di halaman: "As kuningan diameter 25mm x 1000mm / As kuningan bulat: Rp890.000 - Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = 490,9 cm3; m = 4,158 kg; 890.000 / 4,158 = Rp214.061/kg
  - Tautan: <https://biggo.id/s/as%20kuningan%201000mm>
- **Rp226.000 per kg** - Mitrus (Kota Tangerang, Shopee) (toko daring, lewat BigGo)
  - Produk: Sisa stok kuningan ukuran acak, per kg
  - Tertulis di halaman: "AS Bronze Kuningan Sisa Stock Kiloan ... kisaran Rp226.000 ~ Rp348.000, 'Kuningan Rp226.000'"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: sudah per kg
  - Tautan: <https://biggo.id/s/bronze%20kiloan>
- **Rp250.437 per kg** - GCI Store (Kota Tangerang, Shopee) (toko daring, lewat BigGo)
  - Produk: As kuningan bulat diameter 7/8 inci (22,225 mm), potongan 48 cm; per potong
  - Tertulis di halaman: "'As Kuningan Bulat 7/8 Inchi x Panjang Custom - GCI Sutindo': '48 CM Rp395.000'"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 2,2225^2 x 48 = 186,2 cm3; m = 186,2 x 8,47 = 1,577 kg; 395.000 / 1,577 = Rp250.437/kg
  - Tautan: <https://biggo.id/s/sutindo%20as%20kuningan>
- **Rp265.571 per kg** - Aruna metal supply (Shopee) (toko daring, lewat BigGo)
  - Produk: As kuningan diameter 30 mm x 1000 mm; per batang
  - Tertulis di halaman: "As kuningan diameter 30mm x 1000mm / As kuningan batangan: Rp1.590.000 - Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = 706,9 cm3; m = 706,9 x 8,47 = 5,987 kg; 1.590.000 / 5,987 = Rp265.571/kg
  - Tautan: <https://biggo.id/s/as%20kuningan%201000mm>
- **Rp300.940 per kg** - Aruna metal supply (Shopee) (toko daring, lewat BigGo)
  - Produk: As kuningan diameter 32 mm x 1000 mm; per batang
  - Tertulis di halaman: "As kuningan 32mm x 1000mm / as kuningan bulat / kuningan bulat / kuningan batangan: Rp2.050.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = 804,25 cm3; m = 6,812 kg; 2.050.000 / 6,812 = Rp300.940/kg
  - Tautan: <https://biggo.id/s/as%20kuningan%201000mm>

Selain itu ada 3 harga lain yang tidak dicantumkan: 2 karena angkanya tidak bisa dipastikan dari halaman sumbernya; 1 karena ternyata harga yang sama dengan salah satu harga di atas, hanya nama tokonya tercatat salah.

### Perunggu BC6

- Bahan yang dicari: Perunggu (bronze) gunmetal LG2 / BC6 (bar pejal dan hollow/bushing stock)
- Massa jenis untuk konversi: 8.830 kg/m3 (sumber: <https://www.concast.com/c83600.php>)
- Titik harga yang masuk nilai tengah: Rp239.120; Rp279.378; Rp369.860; Rp472.149
- Nilai tengah Rp324.619 per kg, dibulatkan menjadi **Rp325.000 per kg**.

Nilai tengah empat penawaran gunmetal LG2, paduan yang setara dengan BC6 (CAC406). Sebarannya lebar karena harga per kilogram sangat bergantung pada ukuran batang, jadi angka ini perkiraan kasar. Nilai impor (CIF) hanya dipakai sebagai pembanding, dan potongan 1 cm yang grade-nya tidak disebut tidak ikut dihitung.

- **Rp239.120 per kg** - SURYA BANGUN TEKNIK 01 (Shopee) (toko daring, lewat BigGo)
  - Produk: Hollow bronze LG2 OD 4 in x ID 3 in x 92 cm; per potong
  - Tertulis di halaman: "hollow bronze 4' x 3' x 92cm - hollow bronze LG2 - Rp6.890.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x (10,16^2 - 7,62^2) x 92 = 3.263,9 cm3; m = 28,82 kg; 6.890.000 / 28,82 = Rp239.120/kg
  - Tautan: <https://biggo.id/s/hollow%20bronze>
- **Rp270.670 per kg** - UN Comtrade - impor Indonesia HS 740729 Juli 2026 (data statistik)
  - Produk: Impor batang/profil paduan tembaga selain kuningan, CIF USD
  - Tertulis di halaman: "cmdCode 740729 / netWgt 96467.893 / cifvalue 1439335.411 / period 202607"
  - Tanggal data: 2026-07
  - Konversi: 1.439.335,411 / 96.467,893 = USD 14,9204/kg x Rp18.141 (kurs pajak Kemenkeu 04-10 Nov 2026) = Rp270.670/kg
  - Tautan: <https://comtradeapi.un.org/public/v1/preview/C/M/HS?reporterCode=360&period=202607&cmdCode=760429,740721,740729&flowCode=M&partnerCode=0&partner2Code=0&customsCode=C00&motCode=0&includeDesc=true>
- **Rp279.378 per kg** - MATERIAL TEKNIK (Shopee) (toko daring, lewat BigGo)
  - Produk: As bronze LG2 pejal 2 inci (50,8 mm) x 1000 mm; per batang
  - Tertulis di halaman: "As bronze / bushibg / lg2 / 2' x 1000mm as bronze lg2 - Rp5.000.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 5,08^2 x 100 = 2.026,8 cm3; m = 2.026,8 x 8,83 = 17,90 kg; 5.000.000 / 17,90 = Rp279.378/kg
  - Tautan: <https://biggo.id/s/bronze%20lg2>
- **Rp369.860 per kg** - Tokopedia - toko 'indoteknikglodok' (toko daring, lewat BigGo)
  - Produk: Hollow bronze LG2 OD 10 in x ID 8 in x 310 mm; per potong
  - Tertulis di halaman: "hollow bronze LG2 10' x 8' x panjang 310mm - Rp18.468.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x (25,4^2 - 20,32^2) x 31 = 5.654,8 cm3; m = 49,93 kg; 18.468.000 / 49,93 = Rp369.860/kg
  - Tautan: <https://biggo.id/s/bronze%20lg2>
- **Rp472.149 per kg** - Galaxy logam (Shopee) (toko daring, lewat BigGo)
  - Produk: As bronze LG2 pejal 2 in x 200 mm; per potong
  - Tertulis di halaman: "As bronze LG2 diameter 2'inch x 200 mm / bronze lg2 - Rp1.690.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = 405,4 cm3; m = 3,579 kg; 1.690.000 / 3,579 = Rp472.149/kg
  - Tautan: <https://biggo.id/s/bronze%20lg2>
- **Rp561.378 per kg** - Ultra Metal (Shopee) (toko daring, lewat BigGo)
  - Produk: As bronze pejal 46 mm dijual per 1 cm; grade tidak disebut
  - Tertulis di halaman: "AS BRONZE 46MM / AS PADAT BRONZE PER 1CM - Rp82.380 - Harga Naik Rp14.000"
  - Tanggal data: tidak tercantum per penawaran
  - Konversi: V = pi/4 x 4,6^2 x 1 = 16,62 cm3; m = 0,1467 kg; 82.380 / 0,1467 = Rp561.378/kg
  - Tautan: <https://biggo.id/s/as%20bronze>

Selain itu ada 3 harga lain yang tidak dicantumkan karena angkanya tidak bisa dipastikan dari halaman sumbernya.

### Nylon MC (PA6)

- Bahan yang dicari: Nylon / Polyamide PA6 (cast nylon, MC nylon) - rod atau sheet
- Massa jenis untuk konversi: 1.150 kg/m3 (sumber: <https://www.ensingerplastics.com/en/shapes/pa6-tecast-t-natural>)
- Titik harga yang masuk nilai tengah: Rp102.413; Rp103.510; Rp110.716; Rp110.716; Rp110.716; Rp116.771; Rp121.788; Rp122.593; Rp124.002; Rp124.002; Rp125.331; Rp125.403
- Nilai tengah Rp119.280 per kg, dibulatkan menjadi **Rp120.000 per kg**.

Nilai tengah dua belas penawaran batang pejal dari dua belas toko. Tiga harga lembaran hanya dipakai sebagai pembanding. Harga katalog distributor komponen industri jauh lebih tinggi dan tidak ikut dihitung.

- **Rp102.413 per kg** - Tokopedia - toko 'cikalberkah' (toko daring, lewat BigGo)
  - Produk: Nylon PA6 natural (gading) rod pejal, diameter 100 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Nylon PA6 Rod diameter 100mm x 100cm - Nylon gading batangan (Polyamide 6) … Rp925.000 … Tokopedia … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (10,0 cm)^2 x 100 cm = 7.853,98 cm3; m = 7.853,98 x 1,15 g/cm3 = 9,0321 kg; Rp925.000 / 9,0321 kg = Rp102.413/kg
  - Tautan: <https://biggo.id/s/Nylon%20PA6%20Rod/>
- **Rp103.510 per kg** - Shopee - toko _Glodokpacking45 (toko daring, lewat BigGo)
  - Produk: Nylon PA6 natural (kuning gading) rod pejal, diameter 65 mm x 1 m; harga per batang
  - Tertulis di halaman: "nylon gading atau nylon kuning gading batang ( batangan ) PA6 rod diameter 65mm x 1mtr … Rp395.000 … Shopee … _Glodokpacking45 … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (6,5 cm)^2 x 100 cm = 3.318,31 cm3; m = 3.318,31 x 1,15 = 3,8161 kg; Rp395.000 / 3,8161 kg = Rp103.510/kg
  - Tautan: <https://biggo.id/s/Nylon%20gading%20batangan/>
- **Rp110.716 per kg** - Shopee - toko infinity.el, Kota Jakarta Pusat (toko daring, lewat BigGo)
  - Produk: Cast nylon (hard nylon) rod pejal, diameter 50 mm x 1 m; harga per batang
  - Tertulis di halaman: "Cast nilon rod batangan hard nilon roundbard 50 mm x 1 meter … Rp250.000 … Shopee … infinity.el … KOTA JAKARTA PUSAT … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (5,0 cm)^2 x 100 cm = 1.963,50 cm3; m = 1.963,50 x 1,15 = 2,2580 kg; Rp250.000 / 2,2580 kg = Rp110.716/kg
  - Tautan: <https://biggo.id/s/Cast%20nilon%20rod%20batangan/>
- **Rp110.716 per kg** - Blibli - toko Lindo Sarana Teknik (toko daring, lewat BigGo)
  - Produk: MC nylon PA6 blue (MC Blue) rod pejal, diameter 100 mm x 1000 mm; harga per batang
  - Tertulis di halaman: "mc nylon PA6 blue rod 100mm x 1000mm … Rp1.000.000 … Blibli … Lindo Sarana Teknik … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (10,0 cm)^2 x 100 cm = 7.853,98 cm3; m = 7.853,98 x 1,15 = 9,0321 kg; Rp1.000.000 / 9,0321 kg = Rp110.716/kg
  - Tautan: <https://biggo.id/s/Nylon%20PA6%20Rod/>
- **Rp110.716 per kg** - Shopee - toko Sahabat Jaya Utama (toko daring, lewat BigGo)
  - Produk: MC Nylon Blue rod pejal, diameter 30 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Nylon Biru Rod / MC Nylon Blue 30mm x 100cm nylon rod biru … Rp90.000 … Shopee … Sahabat Jaya Utama … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (3,0 cm)^2 x 100 cm = 706,86 cm3; m = 706,86 x 1,15 = 0,8129 kg; Rp90.000 / 0,8129 kg = Rp110.716/kg
  - Tautan: <https://biggo.id/s/MC%20Nylon%20Rod/>
- **Rp116.771 per kg** - Shopee - toko Abdi jaya makmur glodok (toko daring, lewat BigGo)
  - Produk: Nylon PA6 natural (gading) rod pejal, diameter 80 mm x 100 cm; harga per batang
  - Tertulis di halaman: "nylon PA6 Rod/nylon gading batangan DIA 80mm x 100cm … Rp675.000 … Shopee … Abdi jaya makmur glodok … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (8,0 cm)^2 x 100 cm = 5.026,55 cm3; m = 5.026,55 x 1,15 = 5,7805 kg; Rp675.000 / 5,7805 kg = Rp116.771/kg
  - Tautan: <https://biggo.id/s/Nylon%20PA6%20Rod/>
- **Rp121.788 per kg** - Shopee - toko indo jaya packing abadi (toko daring, lewat BigGo)
  - Produk: Nylon PA6G cast nylon rod pejal, diameter 100 mm x 1 m; harga per batang
  - Tertulis di halaman: "nylon PA6G - cast nylon rod dia 100mm x 1m … Rp1.100.000 … Shopee … indo jaya packing abadi … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (10,0 cm)^2 x 100 cm = 7.853,98 cm3; m = 9,0321 kg; Rp1.100.000 / 9,0321 kg = Rp121.788/kg
  - Tautan: <https://biggo.id/s/Nylon%20PA6%20Rod/>
- **Rp122.593 per kg** - Shopee - PT. Langgeng Trada Teknik, Kota Bandung (toko daring, lewat BigGo)
  - Produk: MC Nylon (Blue) rod, potongan eceran diameter 85 mm x 5 cm; harga per potong (varian termurah pada penawaran dengan beberapa varian)
  - Tertulis di halaman: "MC NYLON ROD (Blue) / Batang MC Nylon Biru Ukuran Sedang … Rp40.000 ~ Rp100.000 … 85mm*5cm … Rp40.000 … Shopee … PT. Langgeng Trada Teknik … KOTA BANDUNG"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (8,5 cm)^2 x 5 cm = 283,73 cm3; m = 283,73 x 1,15 = 0,3263 kg; Rp40.000 / 0,3263 kg = Rp122.593/kg
  - Tautan: <https://biggo.id/s/MC%20Nylon%20Rod/>
- **Rp124.002 per kg** - Shopee - toko Megah Jaya Makmur (toko daring, lewat BigGo)
  - Produk: Cast nylon rod pejal, diameter 50 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Cast Nylon Rod 50mmx100cm / Cast Nylon Batangan … Rp280.000 … Shopee … Megah Jaya Makmur … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (5,0 cm)^2 x 100 cm = 1.963,50 cm3; m = 2,2580 kg; Rp280.000 / 2,2580 kg = Rp124.002/kg
  - Tautan: <https://biggo.id/s/Cast%20Nylon%20Rod/>
- **Rp124.002 per kg** - Blibli - toko Arta Suksestama Packing (toko daring, lewat BigGo)
  - Produk: Cast nylon rod pejal, diameter 75 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Cast Nylon Rod Dia. 75mm x 100cm - Cast Nylon Batangan … Rp630.000 … Blibli … Arta Suksestama Packing … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (7,5 cm)^2 x 100 cm = 4.417,86 cm3; m = 4.417,86 x 1,15 = 5,0805 kg; Rp630.000 / 5,0805 kg = Rp124.002/kg
  - Tautan: <https://biggo.id/s/Cast%20Nylon%20Rod/>
- **Rp125.331 per kg** - Shopee - toko Anugrah packing (toko daring, lewat BigGo)
  - Produk: Cast nylon rod pejal, diameter 50 mm x 100 cm; harga per batang
  - Tertulis di halaman: "cast nylon batangan 50mm x 100 cm / cast nylon rod … Rp283.000 … Shopee … Anugrah packing … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (5,0 cm)^2 x 100 cm = 1.963,50 cm3; m = 2,2580 kg; Rp283.000 / 2,2580 kg = Rp125.331/kg
  - Tautan: <https://biggo.id/s/Cast%20Nylon%20Rod/>
- **Rp125.403 per kg** - Shopee - toko berkah usaha packing (toko daring, lewat BigGo)
  - Produk: Nylon PA6 natural (gading) rod pejal, diameter 70 mm x 1 m; harga per batang
  - Tertulis di halaman: "nylon gading batangan - nylon PA6 rod diameter 70mm x 1meter … Rp555.000 … Shopee … berkah usaha packing … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (7,0 cm)^2 x 100 cm = 3.848,45 cm3; m = 3.848,45 x 1,15 = 4,4257 kg; Rp555.000 / 4,4257 kg = Rp125.403/kg
  - Tautan: <https://biggo.id/s/Nylon%20PA6%20Rod/>
- **Rp139.493 per kg** - Shopee - toko Sahabat Jaya Utama, lembaran (toko daring, lewat BigGo)
  - Produk: Cast nylon sheet (lembaran penuh), tebal 6 mm x 100 cm x 200 cm; harga per lembar
  - Tertulis di halaman: "cast nylon sheet / cast nylon lembaran 6mm 100cm x 200cm … Rp1.925.000 … Shopee … Sahabat Jaya Utama … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 0,6 cm x 100 cm x 200 cm = 12.000 cm3; m = 12.000 x 1,15 = 13,80 kg; Rp1.925.000 / 13,80 kg = Rp139.493/kg
  - Tautan: <https://biggo.id/s/Cast%20Nylon%20Sheet/>
- **Rp171.033 per kg** - Shopee - toko Cahaya Sejahtera Teknik, lembaran (toko daring, lewat BigGo)
  - Produk: Cast nylon sheet (lembaran penuh), tebal 8 mm x 1 m x 2 m; harga per lembar
  - Tertulis di halaman: "cast nylon sheet / cast nylon lembaran 8mm 1m x 2m … Rp3.147.000 … Shopee … Cahaya Sejahtera Teknik … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 0,8 cm x 100 cm x 200 cm = 16.000 cm3; m = 16.000 x 1,15 = 18,40 kg; Rp3.147.000 / 18,40 kg = Rp171.033/kg
  - Tautan: <https://biggo.id/s/Cast%20Nylon%20Sheet/>
- **Rp196.087 per kg** - Shopee - toko Jaya Abadi 21, Kota Jakarta Barat, lembaran MC Blue potongan (toko daring, lewat BigGo)
  - Produk: Nylon MC Blue sheet, potongan tebal 10 mm x 100 cm x 100 cm; harga per lembar potong
  - Tertulis di halaman: "Nylon MC Blue lembaran ( MC Blue Sheet ) 10 mm 100 x 100 CM … Rp2.255.000 … Shopee … Jaya Abadi 21 … KOTA JAKARTA BARAT … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 1,0 cm x 100 cm x 100 cm = 10.000 cm3; m = 10.000 x 1,15 = 11,50 kg; Rp2.255.000 / 11,50 kg = Rp196.087/kg
  - Tautan: <https://biggo.id/s/MC%20Blue%20Lembaran/>
- **Rp439.921 per kg** - RS Components Indonesia (rs-online.id; © RS Components Pte Ltd, Singapura) - RS Stock No. 0514619 (daftar harga distributor)
  - Produk: RS PRO Natural Nylon Rod (Cast Nylon 6), diameter 125 mm x 500 mm; harga per unit (indikatif)
  - Tertulis di halaman: "RS PRO Natural Nylon Rod, 500mm x 125mm Diameter … IDR 3,104,219.55 … *price per unit indicative … Out of stock … 1.15 g/cm³"
  - Tanggal data: tidak tercantum (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (12,5 cm)^2 x 50 cm = 6.135,92 cm3; m = 6.135,92 x 1,15 = 7,0563 kg; Rp3.104.219,55 / 7,0563 kg = Rp439.921/kg
  - Tautan: <https://www.rs-online.id/?p=180>
- **Rp458.028 per kg** - RS Components Indonesia (rs-online.id; © RS Components Pte Ltd, Singapura) - RS Stock No. 1847080 (daftar harga distributor)
  - Produk: RS PRO Natural Nylon Rod (cast nylon 6), diameter 70 mm x 500 mm; harga per unit (indikatif)
  - Tertulis di halaman: "RS PRO Natural Nylon Rod, 500mm x 70mm Diameter … IDR 1,013,552.07 … *price per unit indicative … Out of stock … 1.15 g/cm³"
  - Tanggal data: tidak tercantum (harga tayang saat diakses 2026-11-10; 'Nov 10, 2026' di halaman hanya tanggal Statement of Conformity)
  - Konversi: V = pi/4 x (7,0 cm)^2 x 50 cm = 1.924,23 cm3; m = 1.924,23 x 1,15 = 2,2129 kg; Rp1.013.552,07 / 2,2129 kg = Rp458.028/kg
  - Tautan: <https://www.rs-online.id/?p=222>

Selain itu ada 1 harga lain yang tidak dicantumkan karena angkanya tidak bisa dipastikan dari halaman sumbernya.

### POM (polyacetal)

- Bahan yang dicari: POM / Polyacetal (Delrin) - rod atau sheet
- Massa jenis untuk konversi: 1.410 kg/m3 (sumber: <https://www.ensingerplastics.com/en-us/shapes/acetal-tecaform-ah-natural>)
- Titik harga yang masuk nilai tengah: Rp85.697; Rp86.879; Rp90.301; Rp95.719; Rp95.745; Rp101.137; Rp101.939; Rp105.908; Rp108.156; Rp113.859; Rp128.205
- Nilai tengah Rp101.137 per kg, dibulatkan menjadi **Rp100.000 per kg**.

Nilai tengah sebelas penawaran (enam batang 1 m dan lima lembaran penuh), satu penawaran per penjual. Dua harga katalog distributor komponen industri jauh lebih tinggi dan tidak ikut dihitung, begitu pula satu angka pembanding regional yang bukan harga Indonesia.

- **Rp85.697 per kg** - Tokopedia - toko 'indusrti', lembaran (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM hitam sheet (lembaran penuh), tebal 15 mm x 100 cm x 200 cm; harga per lembar
  - Tertulis di halaman: "polyacetal pom hitam lembaran 15mm 100cm x 200cm … Rp3.625.000 … Tokopedia … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 1,5 cm x 100 cm x 200 cm = 30.000 cm3; m = 30.000 x 1,41 g/cm3 = 42,30 kg; Rp3.625.000 / 42,30 kg = Rp85.697/kg
  - Tautan: <https://biggo.id/s/Polyacetal%20100cm%20x%20200cm/>
- **Rp86.879 per kg** - TikTok Shop - nama toko tidak ditampilkan, lembaran (toko daring, lewat BigGo)
  - Produk: POM putih sheet (lembaran penuh), tebal 15 mm x 1 m x 2 m; harga per lembar
  - Tertulis di halaman: "pom white lembaran 15mm x 1m x 2m … Rp3.675.000 … Tiktok Shop … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 1,5 cm x 100 cm x 200 cm = 30.000 cm3; m = 42,30 kg; Rp3.675.000 / 42,30 kg = Rp86.879/kg
  - Tautan: <https://biggo.id/s/POM%20Lembaran/>
- **Rp90.301 per kg** - Shopee - toko Join Packing Mandiri 02, Kota Jakarta Barat (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM rod pejal, diameter 100 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Polyacetal POM Batangan ( POM ROD ) Tebel 100mm x 100cm … Rp1.000.000 … Shopee … Join Packing Mandiri 02 … KOTA JAKARTA BARAT … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (10,0 cm)^2 x 100 cm = 7.853,98 cm3; m = 7.853,98 x 1,41 = 11,0741 kg; Rp1.000.000 / 11,0741 kg = Rp90.301/kg
  - Tautan: <https://biggo.id/s/POM%20Rod%20100cm/>
- **Rp95.719 per kg** - Shopee - toko shootingpack (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM rod pejal, diameter 50 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Polycetal pom rod/Pom batangan 50mm×100cm … Rp265.000 … Shopee … shootingpack … Harga Naik Rp15.000"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (5,0 cm)^2 x 100 cm = 1.963,50 cm3; m = 1.963,50 x 1,41 = 2,7685 kg; Rp265.000 / 2,7685 kg = Rp95.719/kg
  - Tautan: <https://biggo.id/s/POM%20Rod%20100cm/>
- **Rp95.745 per kg** - Shopee - toko PackersOnline(NS), Kota Jakarta Barat, lembaran (toko daring, lewat BigGo)
  - Produk: POM/polyacetal sheet (lembaran penuh), tebal 5 mm x 100 cm x 200 cm; harga per lembar
  - Tertulis di halaman: "POM lembaran/polyacetal sheet 5mm 100cm x 200cm … Rp1.350.000 … Shopee … PackersOnline(NS) … KOTA JAKARTA BARAT … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 0,5 cm x 100 cm x 200 cm = 10.000 cm3; m = 10.000 x 1,41 = 14,10 kg; Rp1.350.000 / 14,10 kg = Rp95.745/kg
  - Tautan: <https://biggo.id/s/Polyacetal%20100cm%20x%20200cm/>
- **Rp99.325 per kg** - IndexBox - laporan pasar 'Asia-Pacific Acetal Copolymer Rod Stock Precision' (pembanding regional)
  - Produk: Acetal copolymer (POM-C) rod komersial standar, diameter 10-100 mm; kisaran harga regional Asia-Pasifik dalam USD per kg
  - Tertulis di halaman: "Standard commercial rod (common diameters 10–100 mm) typically trades in the USD 4–7 per kg range"
  - Tanggal data: 2026-06-19 ('Report Update Jun 19, 2026')
  - Konversi: sudah per kg (USD): USD 4-7/kg x Rp18.059/USD (JISDOR Bank Indonesia 10 November 2026) = Rp72.236-126.413/kg; titik tengah USD 5,5/kg x 18.059 = Rp99.324,5, dibulatkan menjadi Rp99.325/kg
  - Tautan: <https://www.indexbox.io/store/asia-pacific-acetal-copolymer-rod-stock-precision-market-analysis-forecast-size-trends-and-insights/>
- **Rp101.137 per kg** - Tokopedia - toko 'gordengplastik' (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM putih rod pejal, diameter 25 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Polyacetal pom white pom rod 25mm x 100cm Polyacetal … Rp70.000 … Tokopedia … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (2,5 cm)^2 x 100 cm = 490,87 cm3; m = 490,87 x 1,41 = 0,6921 kg; Rp70.000 / 0,6921 kg = Rp101.137/kg
  - Tautan: <https://biggo.id/s/Polyacetal%20POM%20Rod/>
- **Rp101.939 per kg** - TikTok Shop - nama toko tidak ditampilkan (toko daring, lewat BigGo)
  - Produk: POM/polyacetal hitam rod pejal, diameter 75 mm x 100 cm; harga per batang
  - Tertulis di halaman: "Pom Rod Hitam Batangan 75mm x 100cm / Pom Rod Polyacetal … Rp635.000 … Tiktok Shop … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (7,5 cm)^2 x 100 cm = 4.417,86 cm3; m = 4.417,86 x 1,41 = 6,2292 kg; Rp635.000 / 6,2292 kg = Rp101.939/kg
  - Tautan: <https://biggo.id/s/Polyacetal%20POM%20Rod/>
- **Rp105.908 per kg** - Tokopedia - toko 'irma-jaya-teknik' (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM rod pejal, diameter 90 mm x 100 cm; harga per batang
  - Tertulis di halaman: "poliacetal/pom rod 90mm × 100cm … Rp950.000 … Tokopedia … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (9,0 cm)^2 x 100 cm = 6.361,73 cm3; m = 6.361,73 x 1,41 = 8,9700 kg; Rp950.000 / 8,9700 kg = Rp105.908/kg
  - Tautan: <https://biggo.id/s/POM%20Rod%20100cm/>
- **Rp108.156 per kg** - Shopee - toko Anugrah packing, lembaran (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM sheet (lembaran penuh), tebal 10 mm x 100 cm x 200 cm; harga per lembar
  - Tertulis di halaman: "polyacetal sheet / pom lembaran 10mm x 100 cm x 200 cm … Rp3.050.000 … Shopee … Anugrah packing … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 1,0 cm x 100 cm x 200 cm = 20.000 cm3; m = 20.000 x 1,41 = 28,20 kg; Rp3.050.000 / 28,20 kg = Rp108.156/kg
  - Tautan: <https://biggo.id/s/Polyacetal%20100cm%20x%20200cm/>
- **Rp113.859 per kg** - Shopee - toko DX StudioCenter, Kota Jakarta Timur, lembaran (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM sheet (lembaran penuh), tebal 12 mm x 100 cm x 200 cm; harga per lembar
  - Tertulis di halaman: "polyacetal pom lembaran 12mm 100cm x 200cm … Rp3.853.000 … Shopee … DX StudioCenter … KOTA JAKARTA TIMUR … Harga Stabil"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = 1,2 cm x 100 cm x 200 cm = 24.000 cm3; m = 24.000 x 1,41 = 33,84 kg; Rp3.853.000 / 33,84 kg = Rp113.859/kg
  - Tautan: <https://biggo.id/s/Polyacetal%20100cm%20x%20200cm/>
- **Rp128.205 per kg** - Shopee - toko sumber jaya teknindo (toko daring, lewat BigGo)
  - Produk: Polyacetal/POM putih rod pejal, diameter 90 mm x 100 cm; harga per batang
  - Tertulis di halaman: "polyacetal pom rod 90mm x 100cm / pom Batangan … Rp1.150.000 … Putih … Shopee … sumber jaya teknindo"
  - Tanggal data: tidak tercantum per penawaran; halaman BigGo berlabel 'Terbaru Nov 2026' (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (9,0 cm)^2 x 100 cm = 6.361,73 cm3; m = 8,9700 kg; Rp1.150.000 / 8,9700 kg = Rp128.205/kg
  - Tautan: <https://biggo.id/s/POM%20Rod%20100cm/>
- **Rp498.057 per kg** - RS Components Indonesia (rs-online.id; © RS Components Pte Ltd, Singapura) - RS Stock No. 0139546 (daftar harga distributor)
  - Produk: RS PRO White Acetal Rod (acetal copolymer, POM-C), diameter 50 mm x 1 m; harga per unit (indikatif)
  - Tertulis di halaman: "RS PRO White Acetal Rod, 1m x 50mm Diameter … IDR 1,378,883.94 … *price per unit indicative … Out of stock … 1.41 g/cm³"
  - Tanggal data: tidak tercantum (harga tayang saat diakses 2026-11-10; 'Nov 10, 2026' di halaman hanya tanggal Statement of Conformity)
  - Konversi: V = pi/4 x (5,0 cm)^2 x 100 cm = 1.963,50 cm3; m = 1.963,50 x 1,41 = 2,7685 kg; Rp1.378.883,94 / 2,7685 kg = Rp498.057/kg
  - Tautan: <https://www.rs-online.id/?p=172>
- **Rp728.089 per kg** - RS Components Indonesia (rs-online.id) - RS Stock No. 4083986, Delrin (DuPont) (daftar harga distributor)
  - Produk: White Acetal Rod bahan Delrin (acetal homopolymer, POM-H), diameter 80 mm x 500 mm; harga per unit (indikatif)
  - Tertulis di halaman: "RS PRO White Acetal Rod, 500mm x 80mm Diameter … IDR 2,598,439.97 … *price per unit indicative … Out of stock … Homopolymer … 1.42 g/cm³"
  - Tanggal data: tidak tercantum (harga tayang saat diakses 2026-11-10)
  - Konversi: V = pi/4 x (8,0 cm)^2 x 50 cm = 2.513,27 cm3; m = 2.513,27 x 1,42 g/cm3 (densitas Delrin menurut halaman) = 3,5688 kg; Rp2.598.439,97 / 3,5688 kg = Rp728.089/kg
  - Tautan: <https://www.rs-online.id/?p=578834>

## Berat komponen pada data contoh

Menu "Muat data contoh" berisi lima komponen. Beratnya dihitung dari ukuran dan massa jenis bahan di atas, sedangkan
harganya mengikuti daftar harga acuan.

| Komponen | Bahan | Ukuran yang diandaikan | Hitungan | Berat per unit |
| --- | --- | --- | --- | ---: |
| Gear | Baja karbon S45C | cakram diameter 120 mm (modul 3, 40 gigi), tebal 30 mm, lubang 30 mm | pi/4 x (12^2 - 3^2) x 3 = 318,1 cm3; x 7,85 g/cm3 | 2,497 kg |
| Shaft | Baja paduan SCM440 | batang diameter 40 mm, panjang 500 mm | pi/4 x 4^2 x 50 = 628,3 cm3; x 7,85 g/cm3 | 4,932 kg |
| Bracket | Baja SS400 | siku 100 x 100 mm, tebal 8 mm, lebar 80 mm | (100 x 80 x 8) + (92 x 80 x 8) = 122,88 cm3; x 7,85 g/cm3 | 0,965 kg |
| Housing | Besi cor FC250 | rumah bantalan, volume bahan sekitar 944 cm3 | 944 cm3 x 7,2 g/cm3 = 6.796,8 g, dibulatkan menjadi 6,800 kg | 6,800 kg |
| Bushing | Perunggu BC6 | diameter luar 40 mm, dalam 30 mm, panjang 40 mm | pi/4 x (4^2 - 3^2) x 4 = 21,99 cm3; x 8,83 g/cm3 | 0,194 kg |

## Cara memperbarui harga

1. Kumpulkan harga terbaru per material, sebaiknya tiga atau lebih dari penjual yang berbeda, lalu ubah ke rupiah per
   kilogram. Bila harganya kurang dari tiga atau kebanyakan dari penjual yang sama, turunkan tingkat keyakinannya di tabel
   Ringkasan, seperti pada S45C, SCM440, dan FC250.
2. Ambil nilai tengahnya dan bulatkan dengan aturan di atas.
3. Ubah angka di `MATERIAL_CATALOG` dan teks `CATALOG_PERIOD` pada `src/material_catalog.cpp`.
4. Jalankan pengujian (`ctest` atau `make test`), lalu bangkitkan ulang berkas tunggal dengan
   `python3 scripts/amalgamate.py` (di Windows tulis `python` sebagai ganti `python3`).
5. Perbarui angka yang ikut berubah di `README.md`: tabel di bagian Harga acuan material, contoh Rekap di bagian Demo,
   serta baris kedua dan ketiga tabel data uji di bagian Cara pakai. Salin angkanya dari keluaran program: contoh Rekap
   dan baris kedua dari sesi Gear, Shaft, dan Bracket dengan harga acuan, baris ketiga dari menu 6 (Muat data contoh).
6. Catat sumber barunya di dokumen ini dan tulis perubahannya di `CHANGELOG.md`.
