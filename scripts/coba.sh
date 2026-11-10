#!/bin/sh
# Mencoba ESTIMA sekali jalan tanpa memasang apa pun (macOS, Linux, dan Android lewat Termux):
#   curl -fsSL https://raw.githubusercontent.com/fadhlillah-dli/UTS_2531070011/main/scripts/coba.sh | sh
#
# Skrip ini mengunduh paket rilis terbaru ke folder sementara, mencocokkan SHA-256-nya dengan daftar SHA256SUMS
# milik rilis itu, menjalankan programnya, lalu menghapus folder tersebut begitu program ditutup (juga bila
# dihentikan dengan Ctrl+C). Tidak ada yang dipasang; PATH dan berkas konfigurasi shell tidak disentuh.
#
# Semua langkah ada di dalam fungsi main yang baru dipanggil di baris paling akhir. Dengan begitu shell sudah
# membaca skrip sampai habis sebelum menjalankan apa pun: unduhan skrip yang terputus di tengah tidak akan
# dijalankan separuh, dan pada `curl ... | sh` program tidak bisa keliru membaca sisa teks skrip sebagai ketikan.
set -eu

main() {
  REPO="fadhlillah-dli/UTS_2531070011"
  # Dua alamat berikut bisa diganti lewat variabel lingkungan supaya skrip ini bisa diuji terhadap rilis tiruan
  # yang dilayani dari komputer sendiri. Tidak ada pemeriksaan yang dilewati karenanya: dari alamat mana pun,
  # paket hanya dijalankan bila SHA-256-nya cocok dengan SHA256SUMS dari rilis yang sama.
  WEB="${ESTIMA_COBA_BASE:-https://github.com/$REPO}"
  API="${ESTIMA_COBA_API:-https://api.github.com/repos/$REPO}"
  HELP="Petunjuk kompilasi sendiri ada di README: https://github.com/$REPO"

  say() { printf '%s\n' "$*"; }
  die() { printf 'coba.sh: %s\n' "$*" >&2; exit 1; }
  # Setiap permintaan diberi batas waktu sambung supaya koneksi yang macet berakhir dengan pesan, bukan diam lama.
  fetch() { curl -sS --connect-timeout 20 "$@"; }

  command -v curl >/dev/null 2>&1 || die "curl tidak ditemukan. Pasang curl lebih dulu, lalu jalankan lagi."

  os="$(uname -s)"
  arch="$(uname -m)"
  # Termux diperiksa lebih dulu karena di sana uname -s juga menjawab "Linux", padahal paket Linux (glibc statis)
  # tidak bisa jalan di Android: program dimuat lewat linker Android dan aplikasi dibatasi seccomp. uname -o
  # menjawab "Android" di Termux; galatnya dibuang untuk uname lama yang belum mengenal opsi -o.
  if [ "$(uname -o 2>/dev/null)" = "Android" ] || [ -n "${TERMUX_VERSION:-}" ]; then os="Android"; fi
  case "$os" in
    Darwin)
      # Paket macOS dibangun untuk macOS 11 (Big Sur) ke atas. sw_vers menjawab misalnya "15.5" atau "10.15.7"; angka
      # di depan titik pertama adalah versi utamanya. Bila jawabannya tidak terbaca, pemeriksaan ini dilewati.
      # SYSTEM_VERSION_COMPAT=0 memastikan jawabannya versi yang sebenarnya: bila variabel itu bernilai 1 di lingkungan
      # pengguna, macOS 11 ke atas menjawab "10.16" demi program lama.
      macos="$(SYSTEM_VERSION_COMPAT=0 sw_vers -productVersion 2>/dev/null || true)"
      case "${macos%%.*}" in
        ''|*[!0-9]*) ;;
        *) [ "${macos%%.*}" -ge 11 ] ||
             die "paket macOS butuh macOS 11 (Big Sur) atau lebih baru (komputer ini: macOS $macos). $HELP" ;;
      esac
      label="macos-universal"; ext="zip" ;;
    Android)
      [ "$arch" = "aarch64" ] || die "paket Android hanya tersedia untuk aarch64 (perangkat ini: $arch). $HELP"
      label="android-aarch64"; ext="tar.gz" ;;
    Linux)
      # x86_64 adalah PC, laptop, dan server Intel/AMD; aarch64 adalah ARM 64-bit seperti Raspberry Pi 4/5 dan
      # server ARM. "arm64" hanya berjaga-jaga, karena sebagian sistem menyebut arsitektur yang sama dengan nama itu.
      case "$arch" in
        x86_64)        label="linux-x86_64" ;;
        aarch64|arm64) label="linux-aarch64" ;;
        *) die "paket Linux hanya tersedia untuk x86_64 dan aarch64 (komputer ini: $arch). $HELP" ;;
      esac
      ext="tar.gz" ;;
    MINGW*|MSYS*|CYGWIN*)
      die "di Windows jalankan dari PowerShell: irm https://raw.githubusercontent.com/$REPO/main/scripts/coba.ps1 | iex" ;;
    *) die "sistem $os belum didukung skrip ini. $HELP" ;;
  esac

  # Jebakan (trap) dipasang SEBELUM folder dibuat dan berlaku untuk akhir skrip maupun sinyal Ctrl+C, kill, dan
  # terminal yang ditutup, supaya folder sementara terhapus apa pun cara skrip ini berakhir.
  tmp=""
  trap 'if [ -n "$tmp" ]; then rm -rf "$tmp"; fi' EXIT INT TERM HUP
  tmp_root="${TMPDIR:-/tmp}"
  tmp="$(mktemp -d "${tmp_root%/}/estima-coba-XXXXXX")" || die "folder sementara tidak bisa dibuat di $tmp_root"

  say "Mencari rilis terbaru $REPO ..."
  # Versi terbaru dibaca dari pengalihan halaman releases/latest, jadi tidak memakai jatah API GitHub yang dibatasi
  # per alamat IP. Pengalihan ke .../releases/tag/<versi> berarti rilis ada; pengalihan ke .../releases (tanpa tag)
  # atau jawaban 404 berarti rilis belum ada. Jawaban selain itu (gangguan sementara) dicoba sekali lagi lewat API.
  no_release="belum ada rilis di $WEB/releases. Paket siap pakai tersedia setelah rilis pertama terbit. $HELP"
  probe="$(fetch -I -o /dev/null -w '%{http_code} %{redirect_url}' "$WEB/releases/latest")" || probe=""
  version="$(printf '%s\n' "${probe#* }" | sed -n 's#.*/releases/tag/##p')"
  if [ -z "$version" ]; then
    case "$probe" in
      "404 "*|"302 "*/releases) die "$no_release" ;;
    esac
    code="$(fetch -L -o "$tmp/latest.json" -w '%{http_code}' "$API/releases/latest")" || code=""
    case "$code" in
      200) version="$(sed -n 's/.*"tag_name": *"\([^"]*\)".*/\1/p' "$tmp/latest.json" | head -n 1)" ;;
      404) die "$no_release" ;;
    esac
    [ -n "$version" ] ||
      die "tidak bisa membaca versi rilis terbaru${code:+ (jawaban GitHub: $code)}. Periksa koneksi internet, lalu coba lagi."
  fi
  # Nama versi ikut membentuk nama berkas di folder sementara, jadi hanya huruf, angka, titik, garis bawah, dan
  # tanda hubung yang diterima.
  case "$version" in
    *[!A-Za-z0-9._-]*) die "nama versi rilis tidak dikenali: $version" ;;
  esac
  asset="estima-$version-$label.$ext"
  base="$WEB/releases/download/$version"

  say "Mengunduh $asset ..."
  fetch -fL -o "$tmp/$asset" "$base/$asset" || die "gagal mengunduh $asset dari rilis $version"
  fetch -fL -o "$tmp/SHA256SUMS" "$base/SHA256SUMS" || die "gagal mengunduh SHA256SUMS dari rilis $version"

  # Verifikasi ini wajib: tanpa baris yang cocok di SHA256SUMS, atau tanpa alat penghitung SHA-256, program tidak
  # dijalankan. Baris SHA256SUMS berbentuk "<sha256>  <nama berkas>" (tanda * di depan nama berarti mode biner).
  expected=""
  while read -r sum name; do
    if [ "$name" = "$asset" ] || [ "$name" = "*$asset" ]; then expected="$sum"; break; fi
  done < "$tmp/SHA256SUMS"
  [ -n "$expected" ] || die "SHA256SUMS rilis $version tidak memuat $asset. Dibatalkan."
  if command -v sha256sum >/dev/null 2>&1; then actual="$(sha256sum "$tmp/$asset" | cut -d' ' -f1)"
  elif command -v shasum >/dev/null 2>&1; then actual="$(shasum -a 256 "$tmp/$asset" | cut -d' ' -f1)"
  else die "sha256sum maupun shasum tidak ditemukan, jadi unduhan tidak bisa diverifikasi. Dibatalkan."
  fi
  [ "$expected" = "$actual" ] || die "SHA-256 $asset tidak cocok; unduhan rusak atau diubah. Dibatalkan."
  say "SHA-256 cocok."

  # Hanya berkas program yang dikeluarkan dari paket. Arsip tar menyimpannya sebagai ./estima; bentuk tanpa ./
  # dicoba juga karena tidak semua tar menyamakan kedua penulisan itu.
  case "$ext" in
    zip)
      command -v unzip >/dev/null 2>&1 || die "unzip tidak ditemukan. Pasang unzip lebih dulu, lalu jalankan lagi."
      unzip -q -o "$tmp/$asset" estima -d "$tmp" || die "paket $asset tidak bisa dibuka" ;;
    tar.gz)
      tar -xzf "$tmp/$asset" -C "$tmp" ./estima 2>/dev/null || tar -xzf "$tmp/$asset" -C "$tmp" estima ||
        die "paket $asset tidak bisa dibuka" ;;
  esac
  [ -f "$tmp/estima" ] || die "berkas estima tidak ada di dalam paket $asset"
  chmod +x "$tmp/estima"

  say "Menjalankan ESTIMA $version dari folder sementara; folder itu dihapus begitu program ditutup."
  say ""
  # Pada `curl ... | sh` stdin sudah terpakai untuk teks skrip ini, jadi ketikan pengguna diambil langsung dari
  # terminal (/dev/tty). Bila terminal tidak ada (misalnya di CI), stdin diteruskan apa adanya. `|| status=$?`
  # menjaga agar kode keluar program tidak menghentikan skrip sebelum folder sementara dibersihkan.
  status=0
  if ( : </dev/tty ) 2>/dev/null; then "$tmp/estima" </dev/tty || status=$?
  else "$tmp/estima" || status=$?
  fi

  rm -rf "$tmp" || true
  say ""
  if [ -e "$tmp" ]; then
    say "Peringatan: folder sementara $tmp tidak bisa dihapus; hapus folder itu secara manual."
  else
    tmp=""
    say "Selesai. Folder sementara sudah dihapus; tidak ada yang terpasang."
  fi
  exit "$status"
}

main "$@"
