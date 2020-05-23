Scalable Screen Font 2.0 ASCII Format
======================================

- File extension: `.asc`
- Mime type: `text/x-ssfont`

(For the binary format that the renderer uses, see [sfn_format.md](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md)).

This is merely a convenience format to help font designers. It is a plain ASCII file, with some UTF-8 strings
in it, each line is terminated by a '\n' (U+000A) character. Carrige return '\r' (U+000D) is optional to support
Windows based text editors. It is grep friendly, and it was designed in a way that you can safely copy'n'paste glyph
definitions from one file to another. It can be considered as an easily editable "source file" format of SSFN fonts.

The `sfnconv -A` utility outputs in this format, and can parse this to convert the font back to binary. Read more
about the [converter](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnconv.md)).

Header
------

File starts with the magic line:

```
# Scalable Screen Font #
```

Other Lines
-----------

### Properties

Lines starting with `$` encode header field values, and are in the form: `$(key) (value)` optionally followed
by comments. Not all font prorepties are stored in an ASCII file, only those that can't be calculated. Glyph
dimensions and number of characters are only there for the user's information, it is not parsed rather recalculated.

The following keys are recognized: `$type` with a numeric argument (0=Serif, 1=Sans, 2=Decorative, 3=Monospace,
4=Handwriting). Slant is defined by `$style`, with an argument. The argument is only parsed for `b` (bold), `i`
(for italic), `1` and `2` (two user-defined styles) characters, everything else is regular. Keys `$baseline` and
`$underline` both have a numeric argument specifying the line's vertical position. Font's strings table given by
`$name` (unique font name), `$family`, `$subfamily`, `$revision`, `$manufacturer` and `$license`. These keys have
double quote '"' enclosed string arguments. Unknown keys are simply skipped.

### Glyphs

Lines starting with `===U+` specify a new character, and parsed as
```
===U+(code point)===w(width)=h(height)=x(horizontal advance)=y(vertical advance)=o(overlap)="(sequence)"===
```
This line might be followed by the name of the code point. For control characters (code point < 32), the (sequence) part
is missing. Usually that is one multibyte character, the code point's representation in UTF-8. This is just informative,
except for the Private Use Area U+F000 - U+F8FF where the sequence is parsed and can contain more multibytes. This is
used to express ligatures, for example for the code point U+F000 the sequence could be 'fi', and the glyph could describe
the 'fi' ligature (however 'fi' has its own standard UNICODE code point U+FB01, so this is just an example).

Lines after this header define the glyph. A character may consist of several layers, and bitmap glyphs can be mixed
with vector contours. The commands defining these are specified in a way that they are unique to a layer, so by
examining the first character of a line you can tell the layer's kind.

However it is not mandatory, but glyph definitions are always separated by an empty line for visibility.

#### Hinting Grids

These define partritions of the coordinate system, and they are optional. Normally hinting grid is calculated
automatically when font loaded. Specify only if you're not satisfied with the generated grid (up to 32 x 32
coordinates).

```
H (x0) (x1) ...(xN)
V (y0) (y1) ...(yN)
```

#### Foreground Color

Bitmap and contour layers might be prefixed by a foreground color command.

```
f (aarrggbb)
```

#### Vector Contours

Vector layers are specified via contour commands (move to (m), line to (l), quadratic curve to (q), cubic curve to (c)).
A vector layer always starts with a "move to" command, `m` and each "move to" starts a new vector layer:
```
m (x),(y)
l (x),(y)
q (x),(y) (a),(b)
c (x),(y) (a),(b) (c),(d)
```

Others, line to (l), quadratic Bezier curve (q) to and cubic Bezier curve to (b) commands can be repeated as many times
as needed to describe a path. If the last command does not return to the move to command's coordinate, then an implicit
line to command will be added to close the path.

#### Bitmaps

Lines starting with either a dot `.` or `X` specify a bitmap layer. Dot is the background, and X marks the foreground.
A bitmap layer has as many lines as the glyph's height, and each line has as many '.' / 'X' characters as the width
rounded up to multiple of 8.

#### Pixmaps

Similar to bitmaps, but instead of a character there's a `(aarrggbb)` hex octet for each pixel, separated by spaces.
Has as many lines and color codes in each line as the glyph's dimensions.  The values `00000000`, `FF000000` and
`FF000100` mean transparency, which can be written as eight hypens `--------` for increased visibility. To encode black
foreground color, use `FF000001` or `FF000200` (imaging tools like the Gimp might convert full black to `FF000100`).

### Kerning

The line starting with the `k` command encode kerning information. Argument list is a comma ',' separated list of
`(UTF-8 next character) (vertical advance) (horizontal advance)` triplets. If your input method does not allow
direct generation of a certain UTF-8 sequence, then you can also use the `U+000000` notation.

The horizontal advance in kerning is optional, and not stored in the file if it's zero.

### End Tag

To be sure that the file wasn't truncated, the font is closed with a `# End #` line.

Examples
--------

A sample with vector contour layers:
```
# Scalable Screen Font #

$glyphdim 255 197
$type 0 (Serif)
$style italic
$baseline 155
$underline 181
$name "GNU: FreeSerif Italic: 2012"
$family "FreeSerif"
$subfamily "Italic"
$revision "Version 0412.2268"
$manufacturer "https://savannah.gnu.org/projects/freefont/"
$license "Copyright 2002, 2003, 2005, 2008, 2009, 2010, 2012 GNU Freefont contributors."

===U+000020===w0=h0=x43=y0=o0=" "===SPACE===

===U+000021===w45=h116=x57=y0=o0="!"===EXCLAMATION MARK===
m 13,123
l 16,124
c 45,48 40,60 45,60
c 38,40 45,43 42,40
c 13,123 26,40 29,51

m 8,138
c 0,147 4,138 0,143
c 8,157 0,153 3,157
c 18,148 13,157 18,152
c 8,138 18,143 13,138

===U+00002D===w40=h10=x57=y0=o0="-"===HYPHEN-MINUS===
m 40,111
l 2,111
l 0,122
l 37,122
l 40,111

k * -3, , -3, 5 -5 0, 6 -3 0, 7 -10 0, 8 -10 0, 9 -2 0, : -10 0, G -8 0, K -8 0, W -3 0, X -3 0, Z -3 0, Ŀ -8 0, Œ -8 0

# End #
```

And for bitmap and pixel map fonts:
```
# Scalable Screen Font #

$glyphdim 8 16
$type 0 (Serif)
$style regular
$baseline 12
$underline 0
$name "Vga Unicode"
$family "VGA"
$subfamily "Medium"
$revision ""
$manufacturer "Bolkhov"
$license "Copyright (c) 2000 Dmitry Bolkhovityanov, bolkhov@inp.nsk.su"

===U+000000===w8=h16=x8=y0=o0===NOGLYPH===
........
........
XX.XX.X.
......X.
X.......
X.....X.
......X.
X.......
X.....X.
......X.
X.......
X.XX.XX.
........
........
........
........

===U+00F000===w8=h16=x8=y0=o0="fi"===LIGATURE-FI===
........
........
..XXX...
.XX.XX..
.XX..XX.
.XX..XX.
XXXX....
.XX..XX.
.XX..XX.
.XX..XX.
.XX..XX.
XXXX XX.
........
........
........
........

===U+000041===w24=h19=x23=y0=o0="A"===LATIN CAPITAL LETTER A===
-------- -------- -------- -------- -------- -------- 7F877657 7F9E947C 7F9E947C 7F9E947C 7F9E947C 7F9E947C 7F9E947C 7F9E947C 7FA4917F 7FA79676 7F9D9182 7F9E947C 7FA4917F 7F948478 7FA79676 7FA4917F 7F302009 --------
-------- -------- -------- -------- -------- 7F372103 FFB08D46 FFB99755 FFA9863F FFA9863F FFA9863F FFA9863F FFB08D46 FFB08D46 FFA9863F FFA18644 FFA18644 FF9F8C3F FFA18644 FFAC8556 FFAA8D38 FF9F8C3F 7F302009 --------
-------- -------- -------- -------- 7F563E0A FFA37515 FF986818 FFA37515 FF986818 FF986818 FF9E6E02 FF9E6E02 FF9E6E02 FF986818 FFA46B0D FF976D0D FF9E6E02 FF976D0D FF976D0D FFA46B0D FF986818 FFB08D46 7F3A2917 --------
-------- -------- -------- 7F47380E FFB3863B FFE09C2A FFBD901C FFBD901C FFBD901C FFBD901C FFBD901C FFC99B2C FFC99B2C FFC99B2C FFC48E23 FFBD901C FFCC8D22 FFC48E23 FFBD901C FFC48E23 FFC48E23 FFE5CB8C 7F453729 --------
-------- -------- 7F483005 FFCB9B47 FFE7A93A FFD8A133 FFD8A133 FFD8A133 FFE7A93A FFE7A93A FFE7A93A FFE7A93A FFE7A93A FFE7A93A FFE3AC42 FFC99B2C FFEBA745 FFE7A93A FFC99B2C FFE3AC42 FFD8A133 FFE6CA9A 7F453729 --------
-------- -------- 7FBB9C3F FFEABA68 FFEABA68 FFDAC676 FFE3BF71 FFEABD5A FFEBB871 FFEABD5A FFE1BF5C FFAA8D38 FF63490D FF795318 FFA18644 FFF4B26C FFE3BF71 FFE1BF5C FFE3BF71 FFE3BF71 FFE7AE5C FFF9E6B2 7F453729 --------
-------- 7F6B561F FFF0CC7F FFF3C696 FFF3C696 FFEFD896 FFE4CD82 FFF3C696 FFF2CA8B FFD9C37D FF846F2D 7F200F02 7F161301 7F1B0F07 FF9F9255 FFFAD599 FFE3BF71 FFF0CC7F FFF2CA8B FFF2CA8B FFF2CA8B FFDFDBB5 7F453729 --------
-------- 7FBAAC79 FFF9E6B2 FFF2D4B1 FFFDE0B6 FFFDE3A7 FFFDE0B6 FFE6CA9A FFFDE0B6 FFA9863F 7F2A190C -------- -------- 7F4F4538 FFFDFACE FFDFD59C FFF9E6B2 FFFDE3A7 FFEFD896 FFF3D6A4 FFF4E8B4 FFDFDBB5 7F353527 --------
-------- 7FB6A889 FFFAFAE9 FFFCF6D5 FFF3ECC7 FFFBEBC9 FFFDF4DE FFECF2E4 FFDFD59C 7F372103 -------- -------- -------- 7F373917 FFDFD0AB FFECF2E4 FFF9EDE9 FFF3ECC7 FFFBEBC9 FFFAE3D2 FFF4F8D6 7F827265 -------- --------
-------- 7F665446 FFEDECDE FFE6F6DE FFF0FADD FFF9EDE9 FFCEDCD5 FFB9A594 FFCECAA5 7F1C1C0E -------- -------- -------- 7F513617 FF624438 FFCEDCD5 FFF1EDED FFECF2E4 FFF1EDED FFF9EDE9 FFF0FADD 7F827265 -------- --------
7F493C02 FF2A190C FF221727 FF131E26 FF202820 FF2D2432 FF0D0801 FF8E6D55 FFB9A594 7F56492C 7F624438 7F665446 7F504046 7F563E0A FF2D1508 FF05110D FF2D2432 FF202820 FF171C21 FF363437 FFE6F6DE 7F877657 -------- --------
7F493C02 FF372103 FF0D0409 FF0D0409 FF0D0801 FF0D0801 FF0D0409 FF353527 FF877657 FF665446 FF697054 FF697054 FF80743E FF373917 FF2D1508 FF0D0801 FF0D0409 FF0D0801 FF0D0409 FF0D0409 FFF4F8D6 7F827265 -------- --------
7F4C3706 FF372103 FF271902 FF1F1705 FF2A190C FF231B01 FF161C02 FF0D0801 FF1D070E FF271902 FF0D0801 FF1D070E FF200F02 FF2D110F FF1F1705 FF161301 FF231B01 FF2D2102 FF200F02 FF2D2102 FFFDFACE 7F756F65 -------- --------
7F563E0A FF342E0B FF222001 FF322200 FF3D2101 FF302009 FF1C1C0E FF282104 FF302009 FF372103 FF432710 FF302009 FF3B2E01 FF231B01 FF271902 FF282104 FF1C1C0E FF222001 FF2D2102 FF3B2E01 FFF2E9D5 7F756F65 -------- --------
7F5D3D00 FF3B2B0B FF3A2917 FF412904 FF4D2B04 FF301A00 FF513617 FFDFD0AB FF7A6A4C FF372103 FF523705 FF4C3706 FF4C3706 FF513617 FF523705 FF483005 FF372103 FF412904 FF4D2B04 FF412904 FFF8E3BE 7F706C55 -------- --------
7F563E0A FF47380E FF47380E FF4C3706 FF523705 FF3B2B0B FF5C4422 FFFCF6D5 7F706C55 7F3B2B0B 7F3B2B0B 7F342E0B 7F3B2E01 FF412904 FF4C3706 FF483005 FF523705 FF513617 FF5B380B FF563E0A FFFDF4DE 7F827265 -------- --------
7F563E0A FF5D490D FF5D490D FF63490D FF63490D FF5D490D FF72542F FFFCF6D5 7F5D584B -------- -------- -------- 7F2D1508 FF563E0A FF5D490D FF5D490D FF5D490D FF5D490D FF5D490D FF6B561F FFC4B5A3 7F2A2627 -------- --------
7F674719 FF674719 FF6F4810 FF6F4810 FF6F4810 FF674719 FF755818 FFB9B79A 7F4F4538 -------- -------- -------- 7F372103 FF6F4810 FF63490D FF63490D FF63490D FF63490D FF63490D FF6B561F FF877657 7F200F02 -------- --------
7F302009 7F322200 7F372103 7F372103 7F372103 7F322200 7F2D1508 7F1F1705 7F1F1705 -------- -------- -------- 7F1F1705 7F391A03 7F2D2102 7F301A00 7F301A00 7F432710 7F301A00 7F200F02 7F302009 7F0D0409 -------- --------

```
In these examples different kinds of layers are not mixed, but they could be.
