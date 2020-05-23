Scalable Screen Font 2.0 File Format
====================================

- File extension: `.sfn`
- Mime type: `font/x-ssfont`

(This spec is about the binary format. See [asc_format.md](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md)
for the human readable ASCII version, which is the "source file" format. This is
about the "compiled file" format, created by the [converter](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnconv.md).)

As this font format was primarily designed for screen, it differs from TrueType and
OpenType fonts in many aspects. First, it uses pixels, not points (same as when TTF
used with 72 DPI). Second, it allows smaller EM sizes and third, it uses screen based
Y coordinate direction: the points are stored as (0,0) is in the upper left corner of
the grid. Unlike the others, SSFN does not allow glyphs to reach over the grid's edges,
all bounding boxes and control points must fit into the grid. So when you render a
character into a (let's say) 64 x 64 bitmap, you can be sure that the whole glyph is
in it, no parts are missing. Also unlike the others, SSFN fonts can store bitmap and
pixel fonts too (even mixed with vector ones, let's say every character has a vector
contour, except for the emojis which have a colored pixmap.)

SSFN format does not utilize flexible layout, instead it focuses on the
[smallest common dominator among font formats](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/compare.md),
and stores only the minimal required information.

As for character encoding, SSFN exclusively uses UNICODE (ISO-10464) by design. For ligatures, all the standard
ligatures (fi, fl, ffi, ae, etc.) and many others have their own UNICODE code points. For those which don't, you
can assign code points in the Private Use Area to UTF-8 sequence combinations.

Kerning is splitted into three places in the file: previous character is selected by the kerning fragment descriptor
in the Character Mappings table. The next character is selected by a group in the kerning fragment pointed by the
descriptor, and finally the coordinate is stored in the Kerning Table pointed by an offset in that kerning group.

All integers are in little-endian format. Offsets are always relative to font magic, except for Kerning Table
offsets, which are relative to the table.

The basic structure of the format is as follows:

| Block      | Description |
| ---------- | ----------- |
| Header     | A fixed sized struct starting with 'SFN2' magic and basic font information |
| Strings    | Human readable UTF-8 strings, like font's name and license |
| Fragments  | Sub-glyph information table |
| Characters | UNICODE - glyph mappings |
| Ligatures  | UNICODE - UTF-8 ligature mappings (optional) |
| Kerning    | Kerning table (character combination distances, optional) |
| Colormap   | Palette for color commands and pixmap fragments (optional) |
| End magic  | Terminating '2NFS' magic bytes |

The description of the detailed on disk format follows.

Header
------

If the file starts with the bytes 0x1f and 0x8b, then it is stream compressed. You might want to uncompress it first
using a gzip decompression filter (inflate). For command line, use `gzip -d`. The normal renderer is able to uncompress
these fonts transparently, but in lack of memory management the simple renderer can't.

An SSFN file can contain more fonts. In this case the file starts with the header

| Offset | Length | Description                          |
| -----: | -----: | ------------------------------------ |
|      0 |      4 | magic, 'SFNC'                        |
|      4 |      4 | size of the font collection in bytes |

And concatenated fonts in SSFN 2.0 format follows (with magic 'SFN2', see the header bellow). With font collections,
the entire file is gzip compressed, and not the fonts individually. When there's only one font in the file, it starts
with the 32 bytes long font header:

| Offset | Length | Description                                 |
| -----: | -----: | ------------------------------------------- |
|      0 |      4 | font magic, 'SFN2'                          |
|      4 |      4 | size of the font in bytes                   |
|      8 |      1 | font type (family code and style) (1)       |
|      9 |      1 | format revision (2)                         |
|     10 |      1 | overall width                               |
|     11 |      1 | overall height                              |
|     12 |      1 | horizontal baseline                         |
|     13 |      1 | underline position                          |
|     14 |      2 | fragments table offset (relative to magic)  |
|     16 |      4 | character table offset                      |
|     20 |      4 | ligature table offset                       |
|     24 |      4 | kerning table offset                        |
|     28 |      4 | color map offset (3)                        |

Note(1): type bits are:

| BitOffset | Description                                                             |
| --------: | ----------------------------------------------------------------------- |
|     0 - 3 | font family (0-Serif, 1-Sans, 2-Decorative, 3-Monospace, 4-Handwriting) |
|     4 - 7 | font style (bit 4: bold, bit 5: italic, bits 6-7: user defined styles)  |

Note(2): font format revision must be 0 as of writing

Note(3): only if there's a color map. The number of color entires is (font size - 12 - color map offset) / 4.

String Table
------------

Strings are zero terminated UTF-8 strings, up to 255 bytes long each without control characters (less than 32
code points not allowed). If one of the first 6 keys is not defined, then it's coded as an empty string (a single
zero byte). If there's no human readable information for them at all, then this block is coded as 6 zero bytes.

| String | Description                                                       |
| -----: | ----------------------------------------------------------------- |
|    1st | Unique name of the font                                           |
|    2nd | Font family name                                                  |
|    3rd | Font sub-family name                                              |
|    4th | Version or revision information of the font                       |
|    5th | Manufacturer or designer's name, could be an URL to their website |
|    6th | License and copyright information, could be an URL to the license |

Other strings (starting from the 7th) are used for ligatures. If they are not defined, there's no zero bytes
for them.

Fragments Table
---------------

SSFN does not store glyphs directly. Instead it has fragments which are contours or bitmap blocks with normalized
coordinates. A character glyph is constructed by one or more transformed fragments. Usually many glyphs share one
or more fragments.

A fragment starts with a header, which most significant bits describe it's type. The fragment descriptors refer to
these headers by their relative offsets to the magic for fast access (see section Characters Table below).

| 1st byte | Description         |
| -------- | ------------------- |
| 0xxxxxxx | Contour             |
| 100xxxxx | Bitmap              |
| 101xxxxx | Pixel map           |
| 110xxxxx | Kerning group       |
| 111xxxxx | Hinting information |

### Contour Fragment

| 1st byte | More bytes | Description                                                 |
| -------- | ---------: | ----------------------------------------------------------- |
| 00nnnnnn | -          | Contour, up to 64 elements                                  |
| 01NNNNNN | 1          | Contour, number of elements: (N << 8 + additional byte) + 1 |

Followed by a byte array for (N+1) commands, where each byte encodes 4 commands (least significant bits first):

| Bits  | Description  |
| ----- | ------------ |
| 0 - 1 | 1st command  |
| 2 - 3 | 2nd command  |
| 4 - 5 | 3rd command  |
| 6 - 7 | 4th command  |

The last command byte is padded with zero bits.

| Bits | Command        | Argument bytes                                |
| ---- | -------------- | --------------------------------------------- |
|   00 | move to        | 2, x y (first two arguments)                  |
|   01 | line to        | 4, x y                                        |
|   10 | quad curve to  | 4, x y a b (where (a,b) is the control point) |
|   11 | cubic curve to | 6, x y a b c d                                |

Commands array followed by the argument bytes. The first two bytes are reserved for the mandatory first
"move to" command. Other bytes depend on the command (number of arguments specified by the table above).

### Bitmap Fragment

| 1st byte | More bytes | Description                         |
| -------- | ---------- | ----------------------------------- |
| 100ppppp | h          | Bitmap dimensions (up to 256 x 256) |

First byte encodes type and the pitch (number of bytes per row), the second byte the number of rows (height).
Bitmap widths are always multiple of 8.

After the header comes the bitmap data. A set bit means foreground color (part of font face), cleared bits
means background (transparency). Least significant bit is on the left, most significant is on the right.

### Pixel Map Fragment

| 1st byte | More bytes | Description                  |
| -------- | ---------- | ---------------------------- |
| 101sssss | s,w,h      | Pixel map with color palette |

For pixmaps (w + 1) x (h + 1) compressed indeces follows on (s + 1) bytes. The index 255 encodes background
(transparency). The 254 is foreground and other indeces (0-253) select an ARGB color from the color map and
are part of the font face. It is not possible to store separate alpha channel with this format (color map
has that). If you want to include a compressed pixmap bigger than 8k, then you're definitely doing the font
thing wrong.

Compression: TGA RLE (not that bad ratio and damn easy to decode with a minimal code). Packets start with a
header byte, which encodes N (= header & 0x7F). If header bit 7 is set, then next byte should be repeated
N+1 times, if not, then the next N+1 bytes are all indeces.

### Kerning Group Fragment

| 1st byte | More bytes | Description                                           |
| -------- | ---------- | ----------------------------------------------------- |
| 110cccNN | n          | Kerning Group Records (up to 1024)                    |

For kerning groups there are (n + 1) times 8 bytes records: the first and last code points followed by a
coordinate offset:

| Offset | Length | Description                                          |
| -----: | -----: | ---------------------------------------------------- |
|      0 |      2 | first code point in group                            |
|      2 |      1 | first code point most significant bits NNNNffff      |
|      3 |      2 | last code point in group                             |
|      5 |      1 | last code point most significant bits MMMMllll       |
|      6 |      2 | kerning group offset ((N << 20) + (M << 16) + n) + 1 |

The code points represents an interval, the next character in the kerning relation. The offset points to a
list of coordinates in the Kerning Table and those offsets are relative to the table and not to the font magic.

The bits "c" in the header indicates kerning context, for future developments, it is zero for now.

### Hinting information

| 1st byte | More bytes | Description                                           |
| -------- | ---------- | ----------------------------------------------------- |
| 111nnnnn | -          | number of autohint grid relative distances (up to 32) |

After that cames N+1 relative coordinates, one byte each. The format for horizontal and vertical hinting info
are stored alike. If a fragment descriptor with non-zero x points to this fragment, then it is a vertical grid
information.

Character Mappings
------------------

SSFN describes the vast UNICODE code point space in runs. There are two different kind of runs, one if there's
a glyph defined for the code point, and one if not (skip runs). The table always describes the entire 0 ..
0x10FFFF UNICODE range. If UNICODE Inc. decides to increase that range, then file revision must be incremented.

| 1st byte | More bytes | Description |
| -------- | ---------- | ---------------------------------------------------------------- |
| 0xxxxxxx | 6+x        | There's a glyph for this character                               |
| 10nnnnnn | -          | Skip N + 1 code points (up to 64)                                |
| 11NNNNNN | 1          | Skip (N << 8 + additional byte) + 1 code points (up to 16128)    |
| 11111111 | -          | Skip 65536 code points                                           |

The glyph for the 0th character is interpreted as a not defined glyph, which can be shown for every character
that is skipped. If no glyph is defined for the 0th character, then the application should render an empty box
(it can also write the hex code point inside the box).

### Glyph Header

If the characters table describe a glyph at a given code point position, then it starts with a 6 bytes long
header.

| Offset | Length | Description                                       |
| -----: | -----: | ------------------------------------------------- |
|      0 |      1 | attributes, frag type and overlap, 0foooooo       |
|      1 |      1 | number of fragments (up to 255)                   |
|      2 |      1 | glyph width                                       |
|      3 |      1 | glyph height                                      |
|      4 |      1 | advance x (pen movement after the glyph is drawn) |
|      5 |      1 | advance y                                         |

Bits in attributes:
- o is the x overlap (up to 63, that's 1/4th of total width). Has to be subtracted from pen_x before draw.
- f encodes large fragment descriptor: 0 = 5 bytes, 1 = 6 bytes.
- 0 most significant bit must be zero.

If advance x is zero and y is non-zero, then the character direction is vertical up-to-down.

This header is followed by fragment descriptors. It is possible that a character does not have any fragments
(like the space). Descriptors are 5 or 6 bytes long, depending on "f" bit, but the same size for one character.

### Fragment Descriptors

Depending on the bits in glyph header, for f = 0 the offset is 3 bytes (up to 16M), with f = 1 it is 4 bytes:

| Offset | Length | Description               |
| -----: | -----: | ------------------------- |
|      0 |      1 | x offset                  |
|      1 |      1 | y offset                  |
|      2 |    3/4 | fragment offset           |

The fragment offset is relative to the font magic, and coordinate offsets are relative to character's grid.
For hinting grid, the coordinate is one bigger, so x=1 and y=0 encodes vertical grid, and x=0 and y=1
horizontal grid hints (however they both mean 0 offsets to the fragment). For example x=4 and y=0 means
vertical grid shifted by 3 to the right.

Similarily, for kerning fragment descriptors x=1 and y=0 encodes vertical kerning groups, while x=0 and y=1
means horizontal groups. Here the actual values of x and y don't matter, they just select the direction.

When x and y offsets are both 255, then instead a fragment offset, a color index follows:

| Offset | Length | Description               |
| -----: | -----: | ------------------------- |
|      0 |      1 | magic 255                 |
|      1 |      1 | magic 255                 |
|      2 |      1 | color index               |
|      3 |    2/3 | zero, 3 times if "f" set  |

The color descriptor selects an ARGB from the color map. The next fragment must be rasterized with
that color. Color index above 253 are invalid (see Pixmap fragment and Color Map).

There are as many fragment descriptors as the second byte in the glyph header indicates. Their order is
important: if grid fragments exists, they must be the first, followed by kerning descriptors if those exists.
There can be only one grid and kerning descriptor per direction, meaning up to 4: v-grid, h-grid, v-kern and
h-kern. Other descriptors can be repeated as many times as needed. Color commands must always preceed their
corresponding contour/bitmap/pixmap fragment descriptors that they colorize.

Ligature Table (optional)
-------------------------

If there's no ligature information associated with the font, then the ligature table offset in the font header
is zero. This table assigns UTF-8 sequences to the Private Use Area U+F000 - U+F8FF.

The table is an offset table, each record 2 bytes. These offsets are relative to the font magic, and the last
is always a 0 offset. They point to strings in the string table. The first is for the UNICODE U+F000, the second
for the U+F001 etc. When a renderer sees one of these sequences in the input stream, it translates more UTF-8
characters into a single UNICODE code point. Although it is not mandatory, but strongly recommended that fonts
contain ligatures in lexicographical order to speed up look ups.

Also note that all standard ligatures (like 'fi', 'ffl' etc.) and many more has their own standardized UNICODE
code points, no need to assign one from the Private Use Area. Use `grep LIGATURE UnicodeData.txt` to list them.

Kerning Table (optional)
------------------------

If there's no kerning information associated with the font, then the kerning table offset in the font header is
zero. This table represents the relative coordinates for the kerning.

The table itself is just a concatenated groups of RLE compressed list of signed bytes. Offsets in Kerning Group
records point to one of the header byte in one of the RLE packets. The distance of the first and last code point
in the fragment specifies how many coordinates to decode in the stream. These are not separated groups, an offset
might point in the middle of another group's coordinate list.

Color Map (optional)
--------------------

Color map only exists if there's at least one pixmap fragment or color descriptor in the font. Its offset is
stored in the font header, and the size can be calculated as `font size - 12 - color map offset`. Color map stores
four A,R,G,B bytes for each color (blue is the least significant byte), for index 0-253. Values for indeces 254 and
255 are *not* stored, as those encode foreground and background colors.

End Magic
---------

The SSFN file is closed by the magic in big endian format: '2NFS'.

That's all folks!
