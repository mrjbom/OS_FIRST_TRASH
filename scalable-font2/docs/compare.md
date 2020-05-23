Scalable Screen Font 2.0 Comparitions
=====================================

Feature Comparition to Other Font Formats
-----------------------------------------

| Feature                | SSFN | ASC  | TTF  | OTF  | PST1 | PSF2 | BDF  | PNG  |
| ---------------------- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| Binary file            | ✔Yes | ✗No  | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  | ✔Yes |
| Varying width glyphs   | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  |
| Bitmap fonts           | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✗No  | ✔Yes | ✔Yes | ✗No  |
| Pixmap fonts           | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✔Yes |
| Scalable fonts         | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Quadratic curves       | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  |
| Cubic Bezier curves    | ✔Yes | ✔Yes | ✗No  | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Kerning information    | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Kerning groups         | ✔Yes | ✗No  | ✗No  | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  |
| Glyph Compression      | ✔Yes | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✗No  | ✔Yes |
| CLI converter          | ✔Yes | ✔Yes | ✗No  | ✗No  | ✔Yes | ✔Yes | ✔Yes | ✔Yes |
| Requires SO library(1) | ✗No  | ✗No  | ✔Yes | ✔Yes | ✔Yes | ✗No  | ✗No  | ✗No  |
| Codepage nightmare(2)  | ✗No  | ✗No  | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✔Yes | ✗No  |

Note(1): SSFN has a very small, simple to use renderer in an ANSI C header file. Others need the
freetype2 library, but PSF2 and BDF are simple enough to parse them manually.

Note(2): SSFN excusively uses UNICODE ISO-10646 standard as it's only character mapping (which is
usually encoded with UTF-8).

Legend:
 - SSFN: [Scalable Screen Font Binary Format](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md)
 - ASC: [Scalable Screen Font ASCII Format](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md)
 - TTF: TrueType Font
 - OTF: OpenType Font
 - PST1: PostScript Type 1
 - PSF2: PC Screen Format with UNICODE table (Linux Console)
 - BDF: X11 Bitmap Distribution Format
 - PNG: PNG or Truevision TARGA, used by SSFN to import pixel fonts

Compression Ratio
-----------------

The fonts can be gzipped, the normal renderer can uncompress them transparently. The SSFN compression is based
on the intimate knowledge of the glyphs and geometrical transformations, not on byte occurances, therefore SSFN
fonts can be further compressed with RFC1950 deflate efficiently.

### Vector Glyphs

Because vector font formats are just crazy, you can easily gain 60% - 80% extra space by
[converting them into SSFN](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnconv.md).

| File              | Size  | Ratio   | Format    |
| ----------------- | ----: | ------: | --------- |
| [FreeSerif.otf](https://www.gnu.org/software/freefont/)     | 2047K | 100.00% | OpenType  |
| FreeSerif.sfn     |  840K |  41.04% | SSFN      |
| FreeSerif.sfn.gz  |  595K |  29.08% | SSFN+zlib |
| UbuntuBold.ttf    |  333K | 100.00% | Truetype  |
| UbuntuBold.sfn    |   68K |  20.63% | SSFN      |
| UbuntuBold.sfn.gz |   40K |  11.99% | SSFN+zlib |
| [Vera.ttf](https://www.gnome.org/fonts/)          |   65K | 100.00% | Truetype  |
| Vera.sfn          |   18K |  27.58% | SSFN      |
| Vera.sfn.gz       |   12K |  19.61% | SSFN+zlib |

### Bitmap Glyphs

Compression depends mostly on the glyph bitmaps, but there's a good chance you can compress those too
if you have many similar glyphs as SSFN uses an effective deduplication algorithm.

| File              | Size  | Ratio   | Format    |
| ----------------- | ----: | ------: | --------- |
| [u_vga16.bdf](http://www.inp.nsk.su/~bolkhov/files/fonts/univga/)       |  386K | 100.00% | BDF       |
| u_vga16.sfn       |   59K |  15.31% | SSFN      |
| u_vga16.sfn.gz    |   22K |   5.81% | SSFN+zlib |
| [unifont8.psf](http://unifoundry.com/unifont/index.html)      |   70K | 100.00% | PSF2      |
| unifont8.sfn      |   75K | 107.28% | SSFN      |
| unifont8.sfn.gz   |   26K |  37.07% | SSFN+zlib |
| unifont16.psf     | 1125K | 100.00% | PSF2      |
| unifont16.sfn     | 1249K | 110.98% | SSFN      |
| unifont16.sfn.gz  |  526K |  46.71% | SSFN+zlib |
| unifont.sfn*      | 1325K | 110.75% | SSFN      |
| unifont.sfn.gz    |  554K |  46.31% | SSFN+zlib |

* - PSF2 contains either the 8x16 or the 16x16 glyphs, because it simply can't handle different glyph widths
of GNU unifont. SSFN contains per glyph metrics, so without zlib it requires about 10% more storage space than
PSF2, but it can store both 8x16 and 16x16 glyphs in a single font file.

(Also note that in the "fonts" directory you can find an unifont.sfn.gz which was converted from the BDF with
more meta information, and therefore is 817k in size.)

### Pixmap Glyphs

Same as bitmaps, all redundant blocks removed, plus fragments are RLE compressed. Images are quantized
to 254 individual colors, hence the great advantage over truecolor PNG.

| File              | Size  | Ratio   | Format    |
| ----------------- | ----: | ------: | --------- |
| emoji.png         |   29K | 100.00% | PNG       |
| emoji.sfn         |   16K |  55.56% | SSFN      |
| emoji.sfn.gz      |    9K |  30.44% | SSFN+zlib |
