#!/usr/bin/env python3
"""Penggambar flowchart ESTIMA: tata letak grid, garis siku, keluaran PNG siap cetak.

Flowchart ditulis secara deklaratif di ``docs/flowchart/charts.py`` lalu digambar
menjadi ``docs/flowchart/<nama>.png``. Satu-satunya ketergantungan adalah Pillow.

Cara pakai::

    python scripts/render_flowcharts.py              # gambar semua chart
    python scripts/render_flowcharts.py --list       # daftar nama chart
    python scripts/render_flowcharts.py --only NAMA  # gambar satu chart saja
    python scripts/render_flowcharts.py --check      # gagal bila PNG usang atau ada peringatan
    (opsi lain: --outdir DIR, --charts BERKAS.py, --font BERKAS.ttf)

API singkat (lihat contoh lengkap di docs/flowchart/charts.py)::

    from render_flowcharts import Chart

    def contoh() -> Chart:
        c = Chart("contoh", title="Contoh")       # nama = nama berkas PNG
        c.terminal("mulai", "Mulai", 0, 0)        # (kunci, teks, kolom, baris)
        c.io("baca", "Baca n", 0, 1)
        c.decision("cek", "n > 0 ?", 0, 2)
        c.process("pos", "status = positif", 0, 3)
        c.process("neg", "status = bukan\\npositif", 1, 3)   # \\n = ganti baris
        c.terminal("selesai", "Selesai", 0, 4)
        c.flow("mulai", "baca", "cek")             # rangkaian lurus ke bawah
        c.edge("cek", "pos", "Ya")
        c.edge("cek", "neg", "Tidak", exit="right")          # siku: kanan lalu turun
        c.edge("pos", "selesai")
        c.edge("neg", "selesai", enter="right")              # siku: turun lalu ke kiri
        return c

    CHARTS = [contoh()]

Jenis simpul: ``terminal`` (Mulai/Selesai), ``process`` (persegi panjang),
``io`` (jajaran genjang, masukan/keluaran), ``decision`` (belah ketupat), ``sub``
(proses terdefinisi, untuk pemanggilan fungsi), ``connector`` (lingkaran kecil
berhuruf) dan ``junction`` (titik tak terlihat di jalur garis, tempat garis lain
bergabung). Baris junction boleh pecahan: 2.5 berarti di tengah celah antara baris
2 dan 3, sehingga tidak memakan satu baris penuh.

Garis: ``edge(a, b, label=None, exit="bottom", enter="top", lane=None)``. ``exit``
adalah sisi simpul asal, ``enter`` sisi simpul tujuan (top/bottom/left/right).

* Keluar bawah, masuk atas, kolom sama: garis lurus.
* Keluar samping lalu masuk atas/bawah, atau sebaliknya: bentuk L.
* Keluar bawah, masuk atas, kolom berbeda: bentuk Z, belokan di celah tepat di
  atas tujuan (dua cabang if/else yang bergabung kembali memakai ini).
* ``lane``: garis lewat jalur tegak di x tertentu. Bilangan bulat = sumbu kolom itu;
  pecahan = di dalam celah sebelah kanan kolom ``floor(lane)``, misalnya 0.5 tepat
  di tengah celah kolom 0-1, 0.25 dekat kolom 0, -0.5 di kiri kolom 0, dan
  ``n - 0.5`` (n = jumlah kolom) di kanan kolom terakhir.
* Tanpa ``lane``, garis balik (keluar bawah tetapi tujuannya di atas) otomatis lewat
  kiri kolom, dan garis kanan-ke-kanan atau kiri-ke-kiri lewat celah di sisi itu.
* Garis yang masuk junction dari atas digambar tanpa panah karena menyambung lurus
  dengan jalur utama; garis dari samping ke junction tetap berpanah.

Memilih lane: garis balik perulangan di kiri (-0.5); cabang "Tidak" yang melompati
beberapa simpul di kanan kolomnya. Bila dua jalur tegak berbagi celah dan rentang
barisnya tumpang tindih, beri x berbeda: yang lebih pendek (bersarang di dalam)
lebih dekat ke kolom, misalnya 0.3, yang lebih panjang lebih jauh, misalnya 0.75.
Garis yang menembus simpul, garis yang bertumpuk atau bersilangan, label yang
menabrak, dan teks yang terlalu kecil bila dicetak di A4 dilaporkan sebagai
peringatan beserta nama garisnya.

Membutuhkan Python 3.8+ dan Pillow 8.2+.
"""

from __future__ import annotations

import argparse
import importlib.util
import math
import re
import sys
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple

from PIL import Image, ImageChops, ImageDraw, ImageFont

Point = Tuple[float, float]
Box = Tuple[float, float, float, float]  # x0, y0, x1, y1

REPO_ROOT = Path(__file__).resolve().parents[1]
DEFAULT_CHARTS = REPO_ROOT / "docs" / "flowchart" / "charts.py"

KINDS = ("terminal", "process", "io", "decision", "sub", "connector", "junction")
# Arah normal tiap sisi; dipakai untuk menjulurkan garis keluar dari simpul.
SIDES: Dict[str, Point] = {"top": (0, -1), "bottom": (0, 1), "left": (-1, 0), "right": (1, 0)}
VERTICAL = ("top", "bottom")

# Kandidat huruf: Arial di Windows/macOS, padanan metriknya (Liberation Sans) di Linux,
# DejaVu sebagai cadangan terakhir supaya skrip tetap jalan di mesin mana pun.
FONT_CANDIDATES = (
    ("C:/Windows/Fonts/arial.ttf", "C:/Windows/Fonts/arialbd.ttf"),
    ("/Library/Fonts/Arial.ttf", "/Library/Fonts/Arial Bold.ttf"),
    ("/System/Library/Fonts/Supplemental/Arial.ttf",
     "/System/Library/Fonts/Supplemental/Arial Bold.ttf"),
    ("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
     "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf"),
    ("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
     "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"),
    ("/System/Library/Fonts/Helvetica.ttc", "/System/Library/Fonts/Helvetica.ttc"),
)

# Ukuran halaman sasaran (A4 tegak dikurangi margin), dalam poin cetak.
PAGE_W_PT = 17 / 2.54 * 72
PAGE_H_PT = 23 / 2.54 * 72
MIN_PRINT_PT = 9.0
EPS = 0.01


@dataclass(frozen=True)
class Style:
    """Semua ukuran dalam piksel skala 1x; PNG akhir digambar pada ``out_scale``."""

    font_px: float = 14
    label_px: float = 12
    title_px: float = 16
    line_spacing: float = 1.2
    line_w: float = 1.5
    pad_x: float = 10
    pad_y: float = 6
    sub_bar: float = 6
    io_skew: float = 0.3      # kemiringan jajaran genjang relatif terhadap tingginya
    diamond_ratio: float = 2.5  # rasio lebar:tinggi minimum belah ketupat
    arrow_len: float = 8
    arrow_w: float = 7
    label_gap: float = 4
    col_gap: float = 60
    row_gap: float = 20
    margin: float = 16
    title_gap: float = 14
    out_scale: int = 2
    supersample: int = 3
    ink: str = "#1a1a1a"
    background: str = "#ffffff"


STYLE = Style()


@dataclass
class Node:
    key: str
    kind: str
    text: str
    col: int
    row: float
    w: float = 0.0
    h: float = 0.0
    cx: float = 0.0
    cy: float = 0.0

    @property
    def box(self) -> Box:
        return (self.cx - self.w / 2, self.cy - self.h / 2,
                self.cx + self.w / 2, self.cy + self.h / 2)


@dataclass
class Edge:
    src: str
    dst: str
    label: Optional[str]
    exit: str
    enter: str
    lane: Optional[float]
    points: Tuple[Point, ...] = ()
    label_box: Optional[Box] = None

    @property
    def name(self) -> str:
        return "%s -> %s" % (self.src, self.dst)


class Chart:
    """Satu flowchart: simpul di sel grid (kolom, baris) dan garis di antaranya."""

    def __init__(self, name: str, title: Optional[str] = None,
                 col_gap: Optional[float] = None, row_gap: Optional[float] = None) -> None:
        if not re.fullmatch(r"[a-z0-9_]+", name):
            raise ValueError("nama chart '%s' harus huruf kecil, angka, atau _" % name)
        self.name = name
        self.title = title
        self.col_gap = STYLE.col_gap if col_gap is None else col_gap
        self.row_gap = STYLE.row_gap if row_gap is None else row_gap
        self.nodes: Dict[str, Node] = {}
        self.edges: List[Edge] = []

    # --- API penulis chart -------------------------------------------------

    def terminal(self, key: str, text: str, col: int, row: int) -> str:
        return self._add(key, "terminal", text, col, row)

    def process(self, key: str, text: str, col: int, row: int) -> str:
        return self._add(key, "process", text, col, row)

    def io(self, key: str, text: str, col: int, row: int) -> str:
        return self._add(key, "io", text, col, row)

    def decision(self, key: str, text: str, col: int, row: int) -> str:
        return self._add(key, "decision", text, col, row)

    def sub(self, key: str, text: str, col: int, row: int) -> str:
        return self._add(key, "sub", text, col, row)

    def connector(self, key: str, letter: str, col: int, row: int) -> str:
        return self._add(key, "connector", letter, col, row)

    def junction(self, key: str, col: int, row: float) -> str:
        return self._add(key, "junction", "", col, row)

    def edge(self, src: str, dst: str, label: Optional[str] = None, exit: str = "bottom",
             enter: str = "top", lane: Optional[float] = None) -> None:
        for key in (src, dst):
            if key not in self.nodes:
                raise ValueError("%s: simpul '%s' belum didefinisikan" % (self.name, key))
        for side in (exit, enter):
            if side not in SIDES:
                raise ValueError("%s: sisi '%s' tidak dikenal (pakai %s)"
                                 % (self.name, side, "/".join(SIDES)))
        if label is not None and "\n" in label:
            raise ValueError("%s: label garis harus satu baris" % self.name)
        self.edges.append(Edge(src, dst, label, exit, enter, lane))

    def flow(self, *keys: str) -> None:
        """Rangkaian garis biasa (keluar bawah, masuk atas): flow(a, b, c) = a->b lalu b->c."""
        for src, dst in zip(keys, keys[1:]):
            self.edge(src, dst)

    def _add(self, key: str, kind: str, text: str, col: int, row: float) -> str:
        if key in self.nodes:
            raise ValueError("%s: kunci simpul '%s' dipakai dua kali" % (self.name, key))
        if col != int(col) or col < 0 or row < 0:
            raise ValueError("%s: posisi simpul '%s' tidak sah" % (self.name, key))
        if kind != "junction" and row != int(row):
            raise ValueError("%s: hanya junction yang boleh di baris pecahan" % self.name)
        for other in self.nodes.values():
            if other.kind != "junction" and kind != "junction" \
                    and (other.col, other.row) == (col, row):
                raise ValueError("%s: '%s' dan '%s' menempati sel yang sama"
                                 % (self.name, key, other.key))
        self.nodes[key] = Node(key, kind, text, int(col), row)
        return key


# --- Huruf dan ukuran teks ---------------------------------------------------


class Fonts:
    """Huruf yang sudah dimuat pada skala gambar kerja (out_scale x supersample)."""

    def __init__(self, font_path: Optional[str], scale: float) -> None:
        regular, bold = find_fonts(font_path)
        self.scale = scale
        self.text = ImageFont.truetype(regular, round(STYLE.font_px * scale))
        self.label = ImageFont.truetype(regular, round(STYLE.label_px * scale))
        self.title = ImageFont.truetype(bold, round(STYLE.title_px * scale))

    def measure(self, font: ImageFont.FreeTypeFont, size_px: float, text: str) -> Point:
        """Lebar dan tinggi blok teks multi-baris, dalam satuan 1x."""
        lines = text.split("\n")
        width = max(font.getlength(line) for line in lines) / self.scale
        return width, len(lines) * size_px * STYLE.line_spacing

    def ink(self, font: ImageFont.FreeTypeFont, text: str) -> Box:
        """Kotak tinta teks satu baris relatif terhadap titik gambarnya, dalam satuan 1x.

        Label Ya/Tidak diukur dari tintanya, bukan dari tinggi baris, supaya bisa
        rapat ke garis tanpa ruang kosong semu yang memicu tabrakan palsu.
        """
        x0, y0, x1, y1 = font.getbbox(text)
        return x0 / self.scale, y0 / self.scale, x1 / self.scale, y1 / self.scale


def find_fonts(font_path: Optional[str]) -> Tuple[str, str]:
    if font_path:
        if not Path(font_path).is_file():
            raise SystemExit("berkas huruf tidak ditemukan: %s" % font_path)
        return font_path, font_path
    for regular, bold in FONT_CANDIDATES:
        if Path(regular).is_file():
            return regular, bold if Path(bold).is_file() else regular
    raise SystemExit("tidak ada huruf Arial/Liberation/DejaVu; pakai opsi --font")


# --- Tata letak ---------------------------------------------------------------


def size_node(node: Node, fonts: Fonts) -> None:
    """Ukuran simpul diturunkan dari teks terukur agar bentuk sekecil mungkin tetapi lega."""
    if node.kind == "junction":
        return
    tw, th = fonts.measure(fonts.text, STYLE.font_px, node.text)
    s = STYLE
    if node.kind == "decision":
        # Kotak teks (tw x th) muat di belah ketupat W x H bila tw/W + th/H <= 1. Dengan
        # rasio k = W/H, W = tw + k*th; luas terkecil di k = sqrt(tw/th), tetapi rasio
        # dibatasi bawah supaya belah ketupat tetap pipih dan hemat tinggi.
        tw, th = tw + 8, th + 4
        k = max(math.sqrt(tw / th), s.diamond_ratio)
        node.w, node.h = tw + k * th, th + tw / k
    elif node.kind == "terminal":
        node.h = th + 2 * s.pad_y
        node.w = tw + node.h
    elif node.kind == "io":
        node.h = th + 2 * s.pad_y
        node.w = tw + 2 * s.pad_x + s.io_skew * node.h
    elif node.kind == "sub":
        node.w, node.h = tw + 2 * s.pad_x + 2 * s.sub_bar, th + 2 * s.pad_y
    elif node.kind == "connector":
        node.w = node.h = max(tw, th) + 10
    else:
        node.w, node.h = tw + 2 * s.pad_x, th + 2 * s.pad_y


class Grid:
    """Posisi kolom dan baris; lebar kolom/tinggi baris mengikuti simpul terbesarnya."""

    def __init__(self, chart: Chart) -> None:
        nodes = list(chart.nodes.values())
        ncols = max(n.col for n in nodes) + 1
        nrows = int(math.ceil(max(n.row for n in nodes))) + 1
        widths = [0.0] * ncols
        heights = [0.0] * nrows
        for n in nodes:
            widths[n.col] = max(widths[n.col], n.w)
            if n.row == int(n.row):
                heights[int(n.row)] = max(heights[int(n.row)], n.h)
        self.col_gap, self.row_gap = chart.col_gap, chart.row_gap
        self.lefts = _running_starts(widths, chart.col_gap)
        self.tops = _running_starts(heights, chart.row_gap)
        self.widths, self.heights = widths, heights

    def col_x(self, col: int) -> float:
        return self.lefts[col] + self.widths[col] / 2

    def row_y(self, row: float) -> float:
        k = int(math.floor(row))
        if row == k:
            return self.tops[k] + self.heights[k] / 2
        bottom = self.tops[k] + self.heights[k]
        return bottom + (row - k) * (self.tops[k + 1] - bottom)

    def lane_x(self, lane: float) -> float:
        """Bilangan bulat = sumbu kolom; pecahan = posisi di celah kanan kolom floor(lane)."""
        k = int(math.floor(lane))
        n = len(self.widths)
        if not -1 <= k < n:
            raise ValueError("lane %s di luar rentang -1..%d" % (lane, n))
        if lane == k:
            return self.col_x(k)
        right = self.lefts[k] + self.widths[k] if k >= 0 else self.lefts[0] - self.col_gap
        left_next = self.lefts[k + 1] if k + 1 < n else right + self.col_gap
        return right + (lane - k) * (left_next - right)


def _running_starts(sizes: Sequence[float], gap: float) -> List[float]:
    starts, pos = [], 0.0
    for size in sizes:
        starts.append(pos)
        pos += size + gap
    return starts


def layout(chart: Chart, fonts: Fonts) -> None:
    """Ukur simpul, tetapkan pusatnya di grid, lalu rutekan semua garis."""
    if not chart.nodes:
        raise ValueError("%s: chart kosong" % chart.name)
    for node in chart.nodes.values():
        size_node(node, fonts)
    grid = Grid(chart)
    for node in chart.nodes.values():
        node.cx, node.cy = grid.col_x(node.col), grid.row_y(node.row)
    for edge in chart.edges:
        edge.points = route(chart, grid, edge)


# --- Rute garis ----------------------------------------------------------------


def anchor(node: Node, side: str) -> Point:
    dx, dy = SIDES[side]
    half_w = node.w / 2
    if node.kind == "io" and dx:
        # Titik tengah sisi miring jajaran genjang, bukan sudut kotak pembatasnya.
        half_w -= STYLE.io_skew * node.h / 2
    return node.cx + dx * half_w, node.cy + dy * node.h / 2


def route(chart: Chart, grid: Grid, edge: Edge) -> Tuple[Point, ...]:
    a, b = chart.nodes[edge.src], chart.nodes[edge.dst]
    p0, pe = anchor(a, edge.exit), anchor(b, edge.enter)
    lane = edge.lane
    out_v, in_v = edge.exit in VERTICAL, edge.enter in VERTICAL
    if lane is None:
        if out_v and in_v:
            ahead = (pe[1] - p0[1]) * SIDES[edge.exit][1]
            if ahead < 0:
                lane = min(a.col, b.col) - 0.5      # garis balik: lewat tepi kiri
            elif abs(p0[0] - pe[0]) < EPS:
                return (p0, pe)
            else:
                ym = pe[1] + SIDES[edge.enter][1] * grid.row_gap / 2
                return simplify([p0, (p0[0], ym), (pe[0], ym), pe])
        elif not out_v and in_v:
            return simplify([p0, (pe[0], p0[1]), pe])
        elif out_v and not in_v:
            return simplify([p0, (p0[0], pe[1]), pe])
        elif abs(p0[1] - pe[1]) < EPS:
            return (p0, pe)
        elif edge.exit == edge.enter:
            side = SIDES[edge.exit][0]
            lane = (max(a.col, b.col) + 0.5) if side > 0 else (min(a.col, b.col) - 0.5)
        else:
            xm = (p0[0] + pe[0]) / 2
            return simplify([p0, (xm, p0[1]), (xm, pe[1]), pe])
    return lane_path(p0, edge.exit, pe, edge.enter, grid.lane_x(lane), grid.row_gap / 2)


def lane_path(p0: Point, exit: str, pe: Point, enter: str, x: float, stub: float) -> Tuple[Point, ...]:
    """Keluar sisi asal, ke jalur tegak x, menyusuri jalur, lalu masuk sisi tujuan.

    Sisi atas/bawah diberi juluran setengah celah baris supaya belokan mendatar
    berada di tengah celah, tidak menempel pada simpul.
    """
    pts = [p0]
    if exit in VERTICAL:
        pts.append((p0[0], p0[1] + SIDES[exit][1] * stub))
    pre = pe
    if enter in VERTICAL:
        pre = (pe[0], pe[1] + SIDES[enter][1] * stub)
    pts += [(x, pts[-1][1]), (x, pre[1]), pre, pe]
    return simplify(pts)


def simplify(points: List[Point]) -> Tuple[Point, ...]:
    """Buang titik kembar dan titik di tengah ruas lurus agar ruas mudah diperiksa."""
    out: List[Point] = []
    for p in points:
        if out and _same(out[-1], p):
            continue
        if len(out) >= 2 and _collinear(out[-2], out[-1], p):
            out[-1] = p
        else:
            out.append(p)
    return tuple(out)


def _same(p: Point, q: Point) -> bool:
    return abs(p[0] - q[0]) < EPS and abs(p[1] - q[1]) < EPS


def _collinear(p: Point, q: Point, r: Point) -> bool:
    return (abs(p[0] - q[0]) < EPS and abs(q[0] - r[0]) < EPS) or \
           (abs(p[1] - q[1]) < EPS and abs(q[1] - r[1]) < EPS)


# --- Label dan pemeriksaan tabrakan ----------------------------------------------


def place_labels(chart: Chart, fonts: Fonts) -> None:
    """Label diletakkan di samping awal garis; posisi kedua dipakai bila yang pertama menabrak."""
    for edge in chart.edges:
        if not edge.label:
            continue
        ix0, iy0, ix1, iy1 = fonts.ink(fonts.label, edge.label)
        w, h = ix1 - ix0, iy1 - iy0
        (x, y), g = edge.points[0], STYLE.label_gap
        if edge.exit in VERTICAL:
            y0 = y + g if edge.exit == "bottom" else y - g - h
            candidates = [(x + g, y0, x + g + w, y0 + h), (x - g - w, y0, x - g, y0 + h)]
        else:
            x0 = x + g if edge.exit == "right" else x - g - w
            candidates = [(x0, y - g - h, x0 + w, y - g), (x0, y + g, x0 + w, y + g + h)]
        edge.label_box = candidates[0]
        for box in candidates:
            if not label_conflicts(chart, box):
                edge.label_box = box
                break


def label_conflicts(chart: Chart, box: Box) -> List[str]:
    hits = ["simpul '%s'" % n.key for n in chart.nodes.values()
            if n.kind != "junction" and _overlap(box, n.box, 1)]
    for edge in chart.edges:
        if any(_overlap(box, _seg_box(p, q), 1) for p, q in _segments(edge)):
            hits.append("garis %s" % edge.name)
        if edge.label_box and edge.label_box != box and _overlap(box, edge.label_box, 1):
            hits.append("label garis %s" % edge.name)
    return hits


def find_problems(chart: Chart) -> List[str]:
    problems = []
    for edge in chart.edges:
        segs = _segments(edge)
        for i, (p, q) in enumerate(segs):
            for node in chart.nodes.values():
                if node.kind == "junction":
                    continue
                if (i == 0 and node.key == edge.src) or (i == len(segs) - 1 and node.key == edge.dst):
                    continue
                if _overlap(_seg_box(p, q), node.box, -1):
                    problems.append("garis %s menembus simpul '%s'" % (edge.name, node.key))
        if edge.label_box:
            for hit in label_conflicts(chart, edge.label_box):
                problems.append("label '%s' pada garis %s menabrak %s" % (edge.label, edge.name, hit))
    for i, e1 in enumerate(chart.edges):
        for e2 in chart.edges[i + 1:]:
            clash = _edge_clash(e1, e2)
            if clash:
                problems.append("garis %s dan %s %s" % (e1.name, e2.name, clash))
    return sorted(set(problems), key=problems.index)


def _edge_clash(e1: Edge, e2: Edge) -> Optional[str]:
    for p, q in _segments(e1):
        for r, s in _segments(e2):
            b1, b2 = _seg_box(p, q), _seg_box(r, s)
            if not _overlap(b1, b2, 0, strict=False):
                continue
            ix = (max(b1[0], b2[0]), max(b1[1], b2[1]), min(b1[2], b2[2]), min(b1[3], b2[3]))
            if ix[2] - ix[0] > EPS or ix[3] - ix[1] > EPS:
                # Dua garis yang bergabung ke tujuan yang sama boleh berbagi ruas akhir.
                if e1.dst != e2.dst:
                    return "bertumpuk"
            elif not any(_same((ix[0], ix[1]), end) for end in (p, q, r, s)):
                return "bersilangan"
    return None


def _segments(edge: Edge) -> List[Tuple[Point, Point]]:
    return list(zip(edge.points, edge.points[1:]))


def _seg_box(p: Point, q: Point) -> Box:
    return min(p[0], q[0]), min(p[1], q[1]), max(p[0], q[0]), max(p[1], q[1])


def _overlap(a: Box, b: Box, inflate: float, strict: bool = True) -> bool:
    """Apakah kotak a (diperbesar ``inflate``) dan b beririsan; inflate negatif = menyusut."""
    a = (a[0] - inflate, a[1] - inflate, a[2] + inflate, a[3] + inflate)
    if strict:
        return a[0] < b[2] and b[0] < a[2] and a[1] < b[3] and b[1] < a[3]
    return a[0] <= b[2] + EPS and b[0] <= a[2] + EPS and a[1] <= b[3] + EPS and b[1] <= a[3] + EPS


# --- Penggambaran ---------------------------------------------------------------


class Canvas:
    """Gambar kerja beresolusi tinggi; koordinat 1x digeser lalu dikali skala."""

    def __init__(self, bounds: Box, fonts: Fonts) -> None:
        s = STYLE
        self.scale = fonts.scale
        self.origin = (bounds[0] - s.margin, bounds[1] - s.margin)
        self.out_size = (int(math.ceil((bounds[2] - bounds[0] + 2 * s.margin) * s.out_scale)),
                         int(math.ceil((bounds[3] - bounds[1] + 2 * s.margin) * s.out_scale)))
        size = (self.out_size[0] * s.supersample, self.out_size[1] * s.supersample)
        self.image = Image.new("RGB", size, s.background)
        self.draw = ImageDraw.Draw(self.image)
        self.width = max(1, round(s.line_w * self.scale))

    def pt(self, p: Point) -> Tuple[float, float]:
        return (p[0] - self.origin[0]) * self.scale, (p[1] - self.origin[1]) * self.scale

    def outline(self, points: Sequence[Point], closed: bool = True) -> None:
        pts = [self.pt(p) for p in points]
        if closed:
            pts += pts[:2]  # ruas tambahan supaya sudut awal ikut membulat seperti sudut lain
        self.draw.line(pts, fill=STYLE.ink, width=self.width, joint="curve")

    def text(self, center: Point, text: str, font: ImageFont.FreeTypeFont, size_px: float) -> None:
        lines = text.split("\n")
        line_h = size_px * STYLE.line_spacing
        top = center[1] - len(lines) * line_h / 2
        for i, line in enumerate(lines):
            self.draw.text(self.pt((center[0], top + (i + 0.5) * line_h)), line,
                           font=font, fill=STYLE.ink, anchor="mm")

    def finish(self) -> Image.Image:
        return self.image.resize(self.out_size, Image.LANCZOS)


def draw_node(canvas: Canvas, node: Node, fonts: Fonts) -> None:
    x0, y0, x1, y1 = node.box
    cx, cy = node.cx, node.cy
    if node.kind == "junction":
        return
    if node.kind == "terminal":
        a, b = canvas.pt((x0, y0)), canvas.pt((x1, y1))
        canvas.draw.rounded_rectangle((a, b), radius=(b[1] - a[1]) / 2,
                                      outline=STYLE.ink, width=canvas.width)
    elif node.kind == "connector":
        canvas.draw.ellipse((canvas.pt((x0, y0)), canvas.pt((x1, y1))),
                            outline=STYLE.ink, width=canvas.width)
    elif node.kind == "decision":
        canvas.outline([(cx, y0), (x1, cy), (cx, y1), (x0, cy)])
    elif node.kind == "io":
        skew = STYLE.io_skew * node.h
        canvas.outline([(x0 + skew, y0), (x1, y0), (x1 - skew, y1), (x0, y1)])
    else:
        canvas.outline([(x0, y0), (x1, y0), (x1, y1), (x0, y1)])
        if node.kind == "sub":
            for x in (x0 + STYLE.sub_bar, x1 - STYLE.sub_bar):
                canvas.outline([(x, y0), (x, y1)], closed=False)
    canvas.text((cx, cy), node.text, fonts.text, STYLE.font_px)


def draw_edge(canvas: Canvas, edge: Edge, chart: Chart, fonts: Fonts) -> None:
    pts = list(edge.points)
    (px, py), (ex, ey) = pts[-2], pts[-1]
    length = math.hypot(ex - px, ey - py)
    ux, uy = (ex - px) / length, (ey - py) / length
    # Garis yang menyambung lurus ke junction dari atas adalah jalur utama itu sendiri;
    # panah di situ justru membuat jalur tampak terputus.
    target = chart.nodes[edge.dst]
    arrow = not (target.kind == "junction" and edge.enter == "top")
    if arrow:
        # Ujung garis berhenti di dalam kepala panah supaya ujung tumpulnya tidak menyembul.
        cut = min(STYLE.arrow_len * 0.6, length)
        pts[-1] = (ex - ux * cut, ey - uy * cut)
    canvas.outline(pts, closed=False)
    if arrow:
        base = (ex - ux * STYLE.arrow_len, ey - uy * STYLE.arrow_len)
        half = STYLE.arrow_w / 2
        head = [(ex, ey), (base[0] - uy * half, base[1] + ux * half),
                (base[0] + uy * half, base[1] - ux * half)]
        canvas.draw.polygon([canvas.pt(p) for p in head], fill=STYLE.ink)
    if edge.label and edge.label_box:
        # Titik gambar digeser sebesar offset tinta agar tinta tepat mengisi label_box.
        ix0, iy0 = fonts.ink(fonts.label, edge.label)[:2]
        x0, y0 = edge.label_box[:2]
        canvas.draw.text(canvas.pt((x0 - ix0, y0 - iy0)), edge.label,
                         font=fonts.label, fill=STYLE.ink)


def content_bounds(chart: Chart) -> Box:
    boxes = [n.box for n in chart.nodes.values()]
    boxes += [_seg_box(p, q) for e in chart.edges for p, q in _segments(e)]
    boxes += [e.label_box for e in chart.edges if e.label_box]
    return (min(b[0] for b in boxes), min(b[1] for b in boxes),
            max(b[2] for b in boxes), max(b[3] for b in boxes))


def render(chart: Chart, font_path: Optional[str] = None) -> Tuple[Image.Image, List[str], float]:
    """Gambar satu chart; kembalikan gambar, daftar masalah, dan ukuran huruf saat dicetak (pt)."""
    fonts = Fonts(font_path, STYLE.out_scale * STYLE.supersample)
    layout(chart, fonts)
    place_labels(chart, fonts)
    problems = find_problems(chart)
    x0, y0, x1, y1 = content_bounds(chart)
    title_h = 0.0
    if chart.title:
        tw, title_h = fonts.measure(fonts.title, STYLE.title_px, chart.title)
        cx = (x0 + x1) / 2
        x0, x1 = min(x0, cx - tw / 2), max(x1, cx + tw / 2)
        y0 -= title_h + STYLE.title_gap
    canvas = Canvas((x0, y0, x1, y1), fonts)
    if chart.title:
        canvas.text(((x0 + x1) / 2, y0 + title_h / 2), chart.title, fonts.title, STYLE.title_px)
    for node in chart.nodes.values():
        draw_node(canvas, node, fonts)
    for edge in chart.edges:
        draw_edge(canvas, edge, chart, fonts)
    image = canvas.finish()
    width_1x, height_1x = (image.size[0] / STYLE.out_scale, image.size[1] / STYLE.out_scale)
    print_pt = STYLE.font_px * min(PAGE_W_PT / width_1x, PAGE_H_PT / height_1x)
    return image, problems, print_pt


# --- Baris perintah ---------------------------------------------------------------


def load_charts(path: Path) -> List[Chart]:
    # Berkas chart menulis "from render_flowcharts import Chart"; saat skrip ini dijalankan
    # langsung, modulnya bernama __main__, jadi didaftarkan juga dengan nama aslinya agar
    # kelas Chart yang dipakai berkas chart sama persis dengan milik skrip ini.
    sys.modules.setdefault("render_flowcharts", sys.modules[__name__])
    # Tanpa ini Python meninggalkan __pycache__ di docs/flowchart, folder yang ikut repositori.
    sys.dont_write_bytecode = True
    spec = importlib.util.spec_from_file_location("flowchart_charts", str(path))
    if spec is None or spec.loader is None:
        raise SystemExit("tidak bisa memuat %s" % path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    charts = list(getattr(module, "CHARTS", []))
    names = [c.name for c in charts]
    if not charts or len(set(names)) != len(names):
        raise SystemExit("%s: CHARTS harus berisi chart dengan nama unik" % path)
    return charts


def same_pixels(a: Path, b: Path) -> bool:
    if not a.is_file() or not b.is_file():
        return False
    with Image.open(a) as ia, Image.open(b) as ib:
        ia, ib = ia.convert("RGB"), ib.convert("RGB")
        return ia.size == ib.size and ImageChops.difference(ia, ib).getbbox() is None


def render_all(charts: Sequence[Chart], outdir: Path, font: Optional[str]) -> int:
    outdir.mkdir(parents=True, exist_ok=True)
    warnings = 0
    for chart in charts:
        image, problems, print_pt = render(chart, font)
        path = outdir / ("%s.png" % chart.name)
        image.save(path, optimize=True)
        print("%s: %dx%d px; dimuatkan ke halaman 17x23 cm, teksnya %.1f pt"
              % (path.name, image.size[0], image.size[1], print_pt))
        if print_pt < MIN_PRINT_PT:
            problems.append("teks hanya %.1f pt bila dimuatkan ke halaman A4; rapatkan chart "
                            "atau pecah menjadi dua dengan connector" % print_pt)
        for problem in problems:
            print("  peringatan %s: %s" % (chart.name, problem), file=sys.stderr)
        warnings += len(problems)
    return warnings


def main(argv: Optional[Sequence[str]] = None) -> int:
    parser = argparse.ArgumentParser(description="Gambar flowchart dari docs/flowchart/charts.py.")
    parser.add_argument("--charts", type=Path, default=DEFAULT_CHARTS, help="berkas definisi chart")
    parser.add_argument("--outdir", type=Path, help="folder keluaran (bawaan: folder berkas chart)")
    parser.add_argument("--only", metavar="NAMA", action="append", help="gambar chart ini saja")
    parser.add_argument("--list", action="store_true", help="tampilkan nama chart lalu keluar")
    parser.add_argument("--check", action="store_true",
                        help="gambar ulang ke folder sementara; gagal bila PNG berbeda "
                             "atau ada peringatan")
    parser.add_argument("--font", help="berkas huruf .ttf pengganti Arial")
    args = parser.parse_args(argv)

    charts = load_charts(args.charts)
    if args.list:
        for chart in charts:
            print(chart.name)
        return 0
    if args.only:
        unknown = set(args.only) - {c.name for c in charts}
        if unknown:
            parser.error("chart tidak dikenal: %s" % ", ".join(sorted(unknown)))
        charts = [c for c in charts if c.name in args.only]
    outdir = args.outdir or args.charts.parent
    if not args.check:
        render_all(charts, outdir, args.font)
        return 0
    # Pemeriksaan juga gagal bila ada peringatan: PNG yang sama persis tetapi garisnya
    # menembus simpul tetap tidak layak masuk laporan.
    with tempfile.TemporaryDirectory() as tmp:
        warnings = render_all(charts, Path(tmp), args.font)
        stale = [c.name for c in charts
                 if not same_pixels(Path(tmp) / ("%s.png" % c.name), outdir / ("%s.png" % c.name))]
    for name in stale:
        print("usang atau hilang: %s" % (outdir / ("%s.png" % name)), file=sys.stderr)
    return 1 if stale or warnings else 0


if __name__ == "__main__":
    sys.exit(main())
