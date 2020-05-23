Scalable Screen Font 2.0 Converter Utility
==========================================

SSFN is not just a renderer. It is also a file format, which comes with a command line tool to manage
[SSFN font](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md) files.

Supported Font Formats
----------------------

### Converting Vector Fonts

The `sfnconv` tool uses the [freetype2](http://www.freetype.org) library so it can convert anything
that freetype2 can read. This includes all the common formats, TrueType (.ttf) and OpenType (.otf).

SSFN uses a data-loss compression. See the [comparition](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/compare.md)
table on how this influence the resulting file sizes. Storing a font in SSFN usually requires half the
file size.

### Converting Bitmap Fonts

Reading bitmap font has no library dependencies, and the tool can read the most common bitmap
formats: [PC Screen Font](https://www.win.tue.nl/~aeb/linux/kbd/font-formats-1.html) (.psfu,
Linux Console's format), [X11 Bitmap Distribution Format](https://www.x.org/docs/BDF/bdf.pdf) (.bdf) and
[GNU unifont](http://unifoundry.com/unifont/index.html) (.hex). Other bitmap formats, like X11
Portable Compiled Font (.pcf), Windows FNT/FON (.fnt) are supported through freetype2.

Bitmap fonts are also compressed, but with a loss-less deduplication algorithm.

### Converting Pixmap Fonts

Pixmaps differ to bitmaps in that they contain not bits, but pixels, so they can include colored glyphs.
This is primarly used for fancy colorful emoji icons. The converter can read [Truevision TARGA](http://www.gamers.org/dEngine/quake3/TGA.txt)
(.tga) and Portable Network Graphics (.png) files to generate such pixmap SSFN fonts. TGA is a very simple
and common format (the Gimp can save it, and Imagemagick can convert to it too), and PNG is the most
widespread for lossless images, literally all image manipulator programs can handle it (JPEG is not good
for fonts as it uses data-loss compression on pixels).

In images, full black reads as transparency. For compatibility, the full black FF000000 is transparency with
alpha channel too, but you can use FF000001 or FF000200. Green of 1 is also considered full black, because
many programs (like the Gimp for example) saves FF000100.

The glyphs can be either stored vertically or horizontally in the image file, and UNICODE range (flag `-r`)
must be specified. If image is taller than wide, then image width will became the glyph's width, and image
height will be divided by the size of the range, thus giving the height of each glyph. If image is wider than
tall, then image height will became glyph height and image width divided by range size will be glyph width.
For example 256 characters each with a 8 x 16 glyph can be stored in a 8 x 4096 or in an 2048 x 16 image.

Pixmaps fonts are also deduplicated the same way as bitmap fonts, and in addition they will be RLE
compressed too. The combination of these two algorithms usually yields better compression ratios than
PNG files for example (that's because SSFN does not store empty lines and it stores repeating image
blocks only once, while PNG compresses all occurances separately, including empty parts).

### Converting SSFN ASCII Fonts

SSFN has a plain text, human readable "source file" format, called [ASC](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md).
The converter can read and write these as well. SSFN ASCII format is a text editor friendly format (with
optional UTF-8 sequences in data). It was designed in a way that you can edit and copy'n'paste glyphs from
one file to another.

Using the Converter
-------------------

### Specifying Meta Information

Because not all bitmap font formats has font type, name and licensing information, you can pass those
as arguments on the command line. In addition to bold and italic, there's two user-defined style, "u"
and "U". You can use those to select a font from a font collection (see below). Family type can be
specified using a number from 0 to 4, or alternatively with a letter.

```
 -t:    set type b=bold,i=italic,u=usrdef1,U=usrdef2,0=Serif,1/s=Sans,2/d=Decorative,3/m=Monospace,4/h=Handwriting
```

Example:
```sh
$ ./sfnconv -t b1 FreeSansBold.otf FreeSansBold.sfn
```

Strings can be specified as:
```
 -n:    set font unique name
 -f:    set font family (like FreeSerif, Vera, Helvetica)
 -s:    set subfamily (like Regular, Medium, Bold, Oblique, Thin, etc.)
 -v:    set font version / revision (like creation date for example)
 -m:    set manufacturer (creator, designer, foundry)
 -l:    set license (like MIT, GPL or URL to the license)
```

Example:
```sh
$ ./sfnconv -n "My Console Font" -f "MyOS" -s "Regular" -v "2020-05" -m "me https://github.com/me/myfont" -l "MIT" my.psf my.sfn
```

### Specifying Input Glyphs

You can add one or more input files on the command line. They can be mixed, bitmaps and vector
fonts for example. All input file can be prefixed with a range specifier, which will load only
the specified range from that input file.

```sh
$ ./sfnconv -r 0x900 0x97F unifont.hex console.sfn
```
Or
```sh
$ ./sfnconv -r U+0900 U+097F unifont.hex console.sfn
```

UTF-8 sequences are also accepted, if they are enclosed in single quotes (shell requires slashes):
```sh
$ ./sfnconv -r U+0900 U+097F unifont.hex -r \'A\' \'Z\' u_vga16.bdf console.sfn
```
Or you can use UNICODE block names, for example:
```sh
$ ./sfnconv -r "Cyrillic Extended-C" unifont.hex -r cjkunifiedideographs cjk.bdf console.sfn
```

By default, the glyph in the first input file is used. You can replace glyphs from subsequent
files when `-R` specified:

```sh
$ ./sfnconv unifont.hex -R fixedglyphs.bdf console.sfn
```

Sometimes fonts contain bad glyphs or invalid metrics. The converter warns you about these.
```sh
$ ./sfnconv UbuntuBold.ttf UbuntuBold.sfn
Loaded 'UbuntuBold.ttf' (FreeType2, 0 - 10FFFF)
  Name 'Ubuntu' num_glyphs: 1264, units_per_EM: 1000, ascender: 932, underline: -183, vector
Inconsistent font: U+00048A, xMax 786 != xMin+w 788
Inconsistent font: U+00048B, xMax 619 != xMin+w 620
Inconsistent font: U+0004C5, xMax 809 != xMin+w 811
Inconsistent font: U+0004C6, xMax 654 != xMin+w 655
Inconsistent font: U+0004C9, xMax 765 != xMin+w 767
Inconsistent font: U+0004CA, xMax 606 != xMin+w 607
Inconsistent font: U+0004CD, xMax 941 != xMin+w 943
Inconsistent font: U+0004CE, xMax 783 != xMin+w 784
Inconsistent font: ascender 932 != max(yMax) 962
  Numchars: 1194, Bounding box: (-170, -221), (3475, 962) dx 3645 dy 1183, w: 3455, h: 1142, baseline: 962
  Scaling to 3455 x 3455, average: 570 x 741
Irregular dimensions in font U+001FAA:  width: 1444
Irregular dimensions in font U+00EFFD:  width: 1668
Irregular dimensions in font U+00F200:  width: 3455 (6 times the average, can't be right!!!)
  Kerning 1086 pairs
Saving 'UbuntuBold.sfn' (bin, compress)
 Numchars: 1189, Numlayers: 2641
Done. Compressed to 14.69%
```

You can exclude certain glyphs with `-S` (skip), and you can specify this flag multiple times:
```sh
$ ./sfnconv -S U+EFFD -S U+F200 UbuntuBold.ttf UbuntuBold.sfn
      ...
  Numchars: 1192, Bounding box: (-170, -221), (1452, 962) dx 1622 dy 1183, w: 1444, h: 1142, baseline: 962
  Scaling to 1444 x 1444, average: 565 x 740
      ...
```

### Specifying or Altering Font Metrics

Some formats don't store these, and many fonts has invalid values, so you might need to specify them
manually. It helps a lot if you open OTF/TTF fonts in [FontForge](https://fontforge.github.io) and
save them to a new file, as FontForge does a very good job in validation and correction on metrics.

```
 -b:    horizontal baseline in pixels
 -u:    underline position in pixels (relative to baseline)
 -a:    add a constant to advance (some fonts need it, others don't)
 -o:    use original width and height instead of calculated one
 -E:    don't care about rounding errors
```

Example:
```sh
$ ./sfnconv -b 30 -u 2 -a 6 -o -E Vera.ttf Vera.sfn
```

### Specifying Output Format

You can also tweak how the output should be saved, and how much output should be printed.

```
 -U:    save uncompressed, non-gzipped output
 -A:    output SSFN ASCII
 -B:    rasterize vector fonts to bitmaps
 -V:    vectorize bitmap fonts to scalable fonts
 -g:    save grid information for hinting
 -q:    quiet, don't report font errors
```

Creating and Extracting Font Collections
----------------------------------------

You can put more fonts into a single file. This has the advatange that you can load them
at once using `ssfn_load`. It is typical to create Italic and Bold variations for a typeface,
and then pack them with the regular font into one file.

```sh
$ ./sfnconv -c VeraR.sfn VeraB.sfn VeraI.sfn VeraBI.sfn Vera.sfn
```

If you call extract without specifying file names, the converter will list the fonts in the
collection. The first coloumn is the same as with the `-t` flag, and separated by a tab the
rest of the line is the font's unique name (specified by `-n`):
```sh
$ ./sfnconv -e Vera.sfn
-t      -n
1       Bitstream Vera Sans
b1      Bitstream Vera Sans Bold
i1      Bitstream Vera Sans Oblique
bi1     Bitstream Vera Sans Bold Oblique
```
Then you must specify exactly as many filenames as the number of fonts in the collection to
actually extract the fonts:
```sh
$ ./sfnconv -e Vera.sfn VeraR.sfn VeraBd.sfn VeraIt.sfn VeraBI.sfn
```

UNICODE Code Point Coverage Reports
-----------------------------------

You can use the `-C` flag to quickly check the code point range coverage in an SSFN font. This will
generate reports like this:

```sh
$ ./sfnconv -C u_vga16.sfn
Loaded 'u_vga16.sfn' (SSFN BIN, 0 - 10FFFF)

| Coverage | NumChar | Start  | End    | Description                                |
| -------: | ------: | ------ | ------ | ------------------------------------------ |
|    75.7% |      97 | 000000 | 00007F | Basic Latin                                |
|    75.0% |      96 | 000080 | 0000FF | Latin-1 Supplement                         |
|   100.0% |     128 | 000100 | 00017F | Latin Extended-A                           |
|    85.5% |     178 | 000180 | 00024F | Latin Extended-B                           |
   ...
|   100.0% |      26 | 00FE50 | 00FE6F | Small Form Variants                        |
|    99.2% |     140 | 00FE70 | 00FEFF | Arabic Presentation Forms-B                |
|    20.0% |       1 | 00FFF0 | 00FFFF | Specials                                   |
| -------- | ------- | ------------------------------------------------------------ |
|    62.0% |    2899 |     = = = = = = = =   Overall Coverage   = = = = = = = =     |
```

Checking SSFN Font Integrity and Validity
-----------------------------------------

You can use the converter to dump and debug SSFN fonts.

|  Flag    |  Description                               |
| -------- | ------------------------------------------ |
| `-d`     | do basic checks and dump the font's header |
| `-dd`    | dumps the string tables (like font name, manufacturer, license etc.) |
| `-ddd`   | dumps the fragments table                  |
| `-dddd`  | dumps the characters and ligatures tables  |
| `-ddddd` | dumps the kerning offset table             |
| `-D`     | dumps everything, even inconsistent files  |

Dumping a table will do a through check on that table. If everything went well, you should
see a
```
Font parsed OK.
```
message. Examples:
```sh
$ ./sfnconv -dd FreeSerifI.sfn
Dumping 'FreeSerifI.sfn'

font/x-ssfont Scalable Screen Font

---Header---
magic:           'SFN2'
size:            251797
type:            20 SSFN_FAMILY_SERIF, SSFN_STYLE_ITALIC
features:        00 rev 0
width, height:   255 207
baseline:        154
underline:       180
fragments_offs:  0x000000d9
characters_offs: 0x00030c98
ligature_offs:   0x00000000
kerning_offs:    0x0003beee
cmap_offs:       0x00000000

---String Table---
0. name         "GNU: FreeSerif Italic: 2012"
1. family       "FreeSerif"
2. subfamily    "Italic"
3. revision     "Version 0412.2268"
4. manufacturer "https://savannah.gnu.org/projects/freefont/"
5. license      "Copyright 2002, 2003, 2005, 2008, 2009, 2010, 2012 GNU Freefont contributors."

Font parsed OK.
```

Fragments table:
```sh
$ ./sfnconv -ddd ../fonts/FreeSerif.sfn
Dumping '../fonts/FreeSerif.sfn'

font/x-ssfont Scalable Screen Font

---Header---
magic:           'SFN2'
size:            840260
type:            00 SSFN_FAMILY_SERIF
features:        00 rev 0
width, height:   255 214
baseline:        142
underline:       165
fragments_offs:  0x000000da
characters_offs: 0x000a8df9
ligature_offs:   0x00000000
kerning_offs:    0x000cb496
cmap_offs:       0x00000000
name:            "GNU: FreeSerif Normal: 2012"

---Fragments Table---
0000da: 04  SSFN_FRAG_CONTOUR n=5
 54:0=0 SSFN_CONTOUR_MOVE  p= 17,  8
 54:2=1 SSFN_CONTOUR_LINE  p=  8,  0
 54:4=1 SSFN_CONTOUR_LINE  p=  0,  7
 54:6=1 SSFN_CONTOUR_LINE  p=  9, 16
 01:0=1 SSFN_CONTOUR_LINE  p= 17,  8
   ...

0001ac: c0 01 00 SSFN_FRAG_KERNING n=2 c=0
 4c 00 00 4c 00 00 9a 1d  U+00004C..U+00004C o=0cd230
 f6 00 00 f6 00 00 9a 1d  U+0000F6..U+0000F6 o=0cd230

0001bf: 04  SSFN_FRAG_CONTOUR n=5
 fc:0=0 SSFN_CONTOUR_MOVE  p= 17,  8
 fc:2=3 SSFN_CONTOUR_CUBIC p=  8,  0 c1= 17,  4 c2= 13,  0
 fc:4=3 SSFN_CONTOUR_CUBIC p=  0,  8 c1=  3,  0 c2=  0,  4
 fc:6=3 SSFN_CONTOUR_CUBIC p=  8, 17 c1=  0, 13 c2=  3, 17
 03:0=3 SSFN_CONTOUR_CUBIC p= 17,  8 c1= 13, 17 c2= 17, 13
   ...
Font parsed OK.
```

Characters table:
```sh
$ ./sfnconv -dddd ../fonts/FreeSerif.sfn
Dumping '../fonts/FreeSerif.sfn'

font/x-ssfont Scalable Screen Font

---Header---
magic:           'SFN2'
size:            840260
type:            00 SSFN_FAMILY_SERIF
features:        00 rev 0
width, height:   255 214
baseline:        142
underline:       165
fragments_offs:  0x000000da
characters_offs: 0x000a8df9
ligature_offs:   0x00000000
kerning_offs:    0x000cb496
cmap_offs:       0x00000000
name:            "GNU: FreeSerif Normal: 2012"

---Characters Table---
0a8df9: 9f                 --- skip    32 code points ---
0a8dfa: 00 00 00 00 29 00  --- U+000020 n=0 f=0 o=0 w=0 h=0 ax=41 ay=0 ---
0a8e00: 00 02 11 91 37 00  --- U+000021 n=2 f=0 o=0 w=17 h=145 ax=55 ay=0 ---
           00 26 ea 49 00 x=  0 y= 38 frag=0049ea  SSFN_FRAG_CONTOUR
           00 7f e8 43 00 x=  0 y=127 frag=0043e8  SSFN_FRAG_CONTOUR
0a8e10: 00 03 28 4c 42 00  --- U+000022 n=3 f=0 o=0 w=40 h=76 ax=66 ay=0 ---
           01 00 9f 2d 01 x=  1 y=  0 frag=012d9f  SSFN_FRAG_KERNING
           00 26 02 a4 00 x=  0 y= 38 frag=00a402  SSFN_FRAG_CONTOUR
           1a 26 a9 13 02 x= 26 y= 38 frag=0213a9  SSFN_FRAG_CONTOUR
0a8e25: 00 03 4c 8f 52 00  --- U+000023 n=3 f=0 o=0 w=76 h=143 ax=82 ay=0 ---
           01 00 44 25 00 x=  1 y=  0 frag=002544  SSFN_FRAG_KERNING
           00 28 b8 1e 08 x=  0 y= 40 frag=081eb8  SSFN_FRAG_CONTOUR
           1a 4f 39 51 01 x= 26 y= 79 frag=015139  SSFN_FRAG_CONTOUR
0a8e3a: 00 04 40 9d 52 00  --- U+000024 n=4 f=0 o=0 w=64 h=157 ax=82 ay=0 ---
           01 00 ac 01 00 x=  1 y=  0 frag=0001ac  SSFN_FRAG_KERNING
           00 1e b8 04 06 x=  0 y= 30 frag=0604b8  SSFN_FRAG_CONTOUR
           0c 2c 9b 2e 01 x= 12 y= 44 frag=012e9b  SSFN_FRAG_CONTOUR
           22 61 ad 2e 01 x= 34 y= 97 frag=012ead  SSFN_FRAG_CONTOUR
   ...
Font parsed OK.
```
