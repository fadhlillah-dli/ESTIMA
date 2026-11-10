# Mencoba ESTIMA sekali jalan tanpa memasang apa pun (Windows PowerShell 5.1 atau PowerShell 7):
#   irm https://raw.githubusercontent.com/fadhlillah-dli/UTS_2531070011/main/scripts/coba.ps1 | iex
#
# Skrip ini mengunduh paket rilis Windows terbaru ke folder sementara, mencocokkan SHA-256-nya dengan daftar
# SHA256SUMS milik rilis itu, menjalankan programnya, lalu menghapus folder tersebut begitu program ditutup (juga
# bila dihentikan dengan Ctrl+C). Tidak ada yang dipasang, PATH dan registri tidak diubah, dan hak Administrator
# tidak diperlukan. Kode keluar program diteruskan lewat $LASTEXITCODE; kegagalan skrip memberi nilai 1.
#
# Seluruh isi dibungkus & { ... } supaya berjalan di lingkupnya sendiri: `iex` menjalankan teks ini di sesi milik
# pengguna, dan variabel maupun $ErrorActionPreference dari skrip ini tidak boleh tertinggal di sana. Berkas ini
# sengaja hanya berisi karakter ASCII karena Windows PowerShell 5.1 membaca berkas tanpa BOM dengan code page ANSI.
& {
    $ErrorActionPreference = 'Stop'
    # Bilah progres Invoke-WebRequest di PowerShell 5.1 memperlambat unduhan berkali-kali lipat.
    $ProgressPreference = 'SilentlyContinue'
    # Profil pengguna PowerShell 7.4+ bisa menyalakan preferensi ini sehingga kode keluar bukan 0 (termasuk karena
    # Ctrl+C) berubah menjadi error. Nilainya hanya berlaku di dalam blok ini.
    $PSNativeCommandUseErrorActionPreference = $false

    $repo = 'fadhlillah-dli/UTS_2531070011'
    # Dua alamat berikut bisa diganti lewat variabel lingkungan supaya skrip ini bisa diuji terhadap rilis tiruan
    # yang dilayani dari komputer sendiri. Tidak ada pemeriksaan yang dilewati karenanya: dari alamat mana pun,
    # paket hanya dijalankan bila SHA-256-nya cocok dengan SHA256SUMS dari rilis yang sama.
    $web = if ($env:ESTIMA_COBA_BASE) { $env:ESTIMA_COBA_BASE.TrimEnd('/') } else { "https://github.com/$repo" }
    $api = if ($env:ESTIMA_COBA_API) { $env:ESTIMA_COBA_API.TrimEnd('/') } else { "https://api.github.com/repos/$repo" }
    $help = "Petunjuk kompilasi sendiri ada di README: https://github.com/$repo"
    $agent = 'estima-coba'

    function Say([string] $text) { Write-Host $text }

    # Kode status HTTP dicari di sepanjang rantai exception, karena bentuknya berbeda antarversi: PowerShell 5.1
    # membungkus WebException di dalam MethodInvocationException, sedangkan PowerShell 7 memakai
    # HttpResponseException. Nilai 0 berarti tidak ada jawaban HTTP sama sekali (misalnya koneksi putus).
    function Get-HttpStatus($errorRecord) {
        $e = $errorRecord.Exception
        while ($e) {
            if ($e.Response -and $e.Response.StatusCode) { return [int] $e.Response.StatusCode }
            $e = $e.InnerException
        }
        return 0
    }

    # Mengunduh satu berkas rilis; bila gagal, pesannya menyebut berkas dan versinya, bukan hanya pesan .NET.
    function Save-ReleaseFile([string] $url, [string] $path, [string] $what) {
        try {
            Invoke-WebRequest -Uri $url -OutFile $path -UserAgent $agent -UseBasicParsing
        } catch {
            throw "gagal mengunduh $what. Penyebab: $($_.Exception.Message)"
        }
    }

    $tmp = $null
    $exitCode = 1
    $savedInputEncoding = [Console]::InputEncoding
    $savedOutputEncoding = [Console]::OutputEncoding
    try {
        # PowerShell 7 juga ada untuk macOS dan Linux, tetapi skrip ini hanya mengambil paket Windows.
        if ($PSVersionTable.PSVersion.Major -ge 6 -and -not $IsWindows) {
            throw "di macOS dan Linux pakai: curl -fsSL https://raw.githubusercontent.com/$repo/main/scripts/coba.sh | sh"
        }
        # Arsitektur dibaca dari nilai milik mesin (registri hanya dibaca), karena proses x64 yang diemulasi di
        # Windows on Arm melihat arsitektur prosesnya sendiri. Paketnya x86_64: Windows 11 on Arm (build 22000 ke
        # atas) menjalankannya lewat emulasi, sedangkan Windows 10 on Arm dan Windows 32-bit tidak bisa.
        $arch = [Environment]::GetEnvironmentVariable('PROCESSOR_ARCHITECTURE', 'Machine')
        $build = [int] (Get-ItemProperty -Path 'HKLM:\SOFTWARE\Microsoft\Windows NT\CurrentVersion').CurrentBuildNumber
        if (-not ($arch -eq 'AMD64' -or ($arch -eq 'ARM64' -and $build -ge 22000))) {
            throw "paket Windows hanya tersedia untuk x86_64 (atau Windows 11 on Arm lewat emulasi); komputer ini: $arch, build $build. $help"
        }
        # Windows PowerShell 5.1 di Windows 10 lama belum menyalakan TLS 1.2, padahal GitHub hanya menerima TLS 1.2 ke atas.
        if ($PSVersionTable.PSVersion.Major -lt 6) {
            [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12
        }

        Say "Mencari rilis terbaru $repo ..."
        # Versi terbaru dibaca dari pengalihan halaman releases/latest, jadi tidak memakai jatah API GitHub yang
        # dibatasi per alamat IP. Pengalihan ke .../releases/tag/<versi> berarti rilis ada; pengalihan ke .../releases
        # (tanpa tag) atau jawaban 404 berarti rilis belum ada. Selain itu (gangguan sementara) dicoba lewat API.
        $noRelease = "belum ada rilis di $web/releases. Paket siap pakai tersedia setelah rilis pertama terbit. $help"
        $version = $null
        $status = 0
        $location = $null
        $reason = $null
        try {
            $request = [Net.HttpWebRequest]::Create("$web/releases/latest")
            $request.Method = 'HEAD'
            $request.AllowAutoRedirect = $false
            $request.UserAgent = $agent
            $response = $request.GetResponse()
            try {
                $status = [int] $response.StatusCode
                $location = $response.Headers['Location']
            } finally { $response.Close() }
        } catch {
            # PowerShell 7 melempar exception untuk jawaban 3xx bila pengalihan dimatikan; Location tetap dibaca.
            $status = Get-HttpStatus $_
            $e = $_.Exception
            while ($e -and -not $location) {
                if ($e.Response) { $location = $e.Response.Headers['Location'] }
                $e = $e.InnerException
            }
            $reason = $_.Exception.Message
        }
        if ($location -match '/releases/tag/([^/?#]+)') {
            $version = $Matches[1]
        } elseif ($status -eq 404 -or ($location -and $location -match '/releases/?$')) {
            throw $noRelease
        } else {
            try {
                $version = (Invoke-RestMethod -Uri "$api/releases/latest" -UserAgent $agent -UseBasicParsing).tag_name
            } catch {
                if ((Get-HttpStatus $_) -eq 404) { throw $noRelease }
                $reason = $_.Exception.Message
            }
        }
        if (-not $version) {
            $why = if ($reason) { " Penyebab: $reason" } else { '' }
            throw "tidak bisa membaca versi rilis terbaru. Periksa koneksi internet, lalu coba lagi.$why"
        }
        # Nama versi ikut membentuk nama berkas di folder sementara, jadi hanya huruf, angka, titik, garis bawah, dan
        # tanda hubung yang diterima.
        if ($version -notmatch '^[A-Za-z0-9._-]+$') { throw "nama versi rilis tidak dikenali: $version" }
        $asset = "estima-$version-windows-x86_64.zip"
        $base = "$web/releases/download/$version"

        # Folder dibuat di dalam try supaya finally di bawah tetap menghapusnya, termasuk bila Ctrl+C ditekan tepat
        # sesudah folder dibuat.
        $tmp = Join-Path ([IO.Path]::GetTempPath()) ('estima-coba-' + [IO.Path]::GetRandomFileName())
        New-Item -ItemType Directory -Path $tmp | Out-Null

        Say "Mengunduh $asset ..."
        $zip = Join-Path $tmp $asset
        $sums = Join-Path $tmp 'SHA256SUMS'
        Save-ReleaseFile "$base/$asset" $zip "$asset dari rilis $version"
        Save-ReleaseFile "$base/SHA256SUMS" $sums "SHA256SUMS dari rilis $version"

        # Verifikasi ini wajib: tanpa baris yang cocok di SHA256SUMS, program tidak dijalankan. Baris SHA256SUMS
        # berbentuk "<sha256>  <nama berkas>" (tanda * di depan nama berarti mode biner).
        $expected = $null
        foreach ($line in [IO.File]::ReadAllLines($sums)) {
            if ($line -match '^([0-9A-Fa-f]{64}) [ *](.+)$' -and $Matches[2] -ceq $asset) {
                $expected = $Matches[1].ToLowerInvariant()
                break
            }
        }
        if (-not $expected) { throw "SHA256SUMS rilis $version tidak memuat $asset. Dibatalkan." }
        $hasher = [Security.Cryptography.SHA256]::Create()
        try {
            $stream = [IO.File]::OpenRead($zip)
            try { $actual = ([BitConverter]::ToString($hasher.ComputeHash($stream)) -replace '-', '').ToLowerInvariant() }
            finally { $stream.Dispose() }
        } finally { $hasher.Dispose() }
        if ($expected -ne $actual) { throw "SHA-256 $asset tidak cocok; unduhan rusak atau diubah. Dibatalkan." }
        Say 'SHA-256 cocok.'

        # Hanya estima.exe yang dikeluarkan dari paket, lewat kelas ZipFile milik .NET. Expand-Archive tidak dipakai
        # karena ia modul skrip yang gagal dimuat bila execution policy bernilai Restricted (bawaan Windows).
        Add-Type -AssemblyName System.IO.Compression.FileSystem
        $exe = Join-Path $tmp 'estima.exe'
        $archive = [IO.Compression.ZipFile]::OpenRead($zip)
        try {
            $entry = $archive.GetEntry('estima.exe')
            if (-not $entry) { throw "berkas estima.exe tidak ada di dalam paket $asset" }
            [IO.Compression.ZipFileExtensions]::ExtractToFile($entry, $exe, $true)
        } finally { $archive.Dispose() }

        Say "Menjalankan ESTIMA $version dari folder sementara; folder itu dihapus begitu program ditutup."
        Say ''
        if ($env:ESTIMA_INPUT) {
            # Mode terskrip (dipakai CI): ketikan dibaca dari berkas yang ditunjuk ESTIMA_INPUT, keluaran ditampung
            # di folder sementara lalu dicetak, karena Start-Process tidak bisa meneruskannya langsung ke layar.
            $outFile = Join-Path $tmp 'keluaran.txt'
            $errFile = Join-Path $tmp 'galat.txt'
            $proc = Start-Process -FilePath $exe -RedirectStandardInput $env:ESTIMA_INPUT `
                -RedirectStandardOutput $outFile -RedirectStandardError $errFile -NoNewWindow -Wait -PassThru
            $exitCode = $proc.ExitCode
            foreach ($file in @($outFile, $errFile)) {
                if (Test-Path -LiteralPath $file) { Write-Host ([IO.File]::ReadAllText($file)) }
            }
        } else {
            & $exe
            $exitCode = $LASTEXITCODE
        }
    } catch {
        Write-Host "coba.ps1: $($_.Exception.Message)" -ForegroundColor Red
        $exitCode = 1
    } finally {
        # Program mengubah code page konsol ke UTF-8; jendela PowerShell pengguna dikembalikan seperti semula.
        try {
            [Console]::InputEncoding = $savedInputEncoding
            [Console]::OutputEncoding = $savedOutputEncoding
        } catch { }
        if ($tmp) {
            Remove-Item -LiteralPath $tmp -Recurse -Force -ErrorAction SilentlyContinue
            Write-Host ''
            if (Test-Path -LiteralPath $tmp) {
                Write-Host "Peringatan: folder sementara $tmp tidak bisa dihapus; hapus folder itu secara manual." -ForegroundColor Yellow
            } else {
                Write-Host "Selesai (kode keluar $exitCode). Folder sementara sudah dihapus; tidak ada yang terpasang."
            }
        }
        # `exit` tidak dipakai karena pada `irm ... | iex` perintah itu ikut menutup jendela PowerShell pengguna.
        $global:LASTEXITCODE = $exitCode
    }
}
