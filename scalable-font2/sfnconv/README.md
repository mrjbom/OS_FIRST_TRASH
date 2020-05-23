Scalable Screen Font 2.0 Converter Utility
==========================================

```
Scalable Screen Font 2.0 by bzt Copyright (C) 2020 MIT license
 https://gitlab.com/bztsrc/scalable-font2

./sfnconv [-c|-e|-d|-dd|-dd...|-D] [-C] [-U] [-A] [-R] [-B <size>|-V] [-g]
   [-b <p>] [-u <+p>] [-a <+p>] [-o] [-q] [-S <U+xxx>] [-E] [-t [b][i]<0..4>]
   [-n <name>] [-f <family>] [-s <subfamily>] [-v <ver>] [-m <manufacturer>]
   [-l <license>] [-r <from> <to>] <in> [ [-r <from> <to>] <in> ...] <out>

 -c:  create font collection
 -e:  extract font collection
 -d:  dump font (-d = header, -dd = string table, -ddd = fragments etc.)
 -D:  dump all tables in the font
 -C:  UNICODE range coverage report
 -U:  save uncompressed, non-gzipped output
 -A:  output SSFN ASCII
 -R:  replace characters from new files
 -B:  rasterize vector fonts to bitmaps
 -V:  vectorize bitmap fonts to scalable fonts
 -g:  save grid information for hinting
 -b:  horizontal baseline in pixels (1-255)
 -u:  underline position in pixels (relative to baseline)
 -a:  add a constant to advance (1-255, some fonts need it, others don't)
 -o:  use original width and height instead of calculated one
 -q:  quiet, don't report font errors
 -S:  skip a UNICODE code point, this flag can be repeated
 -E:  don't care about rounding errors
 -t:  set type b=bold,i=italic,u,U,0=Serif,1/s=Sans,2/d=Decor,3/m=Mono,4/h=Hand
 -n:  set font unique name
 -f:  set font family (like FreeSerif, Vera, Helvetica)
 -s:  set subfamily (like Regular, Medium, Bold, Oblique, Thin, etc.)
 -v:  set font version / revision (like creation date for example)
 -m:  set manufacturer (creator, designer, foundry)
 -l:  set license (like MIT, GPL or URL to the license)
 -r:  code point range, this flag can be repeated before each input
 in:  input font(s) SSFN,ASC,PST1,TTF,OTF,WinFNT,PCF,PSF2,BDF,hex,TGA,PNG*
 out: output SSFN/ASC filename**

*  - input files can be gzip compressed, like .psfu.gz, .bdf.gz or .hex.gz
** - output file will be gzip compressed by default (use -U to avoid)
```
