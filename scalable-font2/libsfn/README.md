Scalable Screen Font 2.0 Utility Library
========================================

This is a library to manipulate SSFN files as well as import other formats. For the latter it has some dependencies.
This library is only used by the converter and the editor, and NOT needed by the renderer. The library relies on the
renderer, and not the other way around.

Dependencies are all optional. Without you'll get lower quality or missing features, but the lib will work. The deps
are automatically detected by the build system.

- zlib for compressing fonts on-the-fly (uncompression is working regardless)
- libimagequant to quantize truecolor pixel fonts (works without, but produces lower quality)
- potrace to convert bitmap fonts to scalable vector fonts (conversion not supported without)
- freetype2 to import PS Type 1 / OTF / TTF / PCF / WinFNT etc. (some font formats not supported without)

Compilation
-----------

To generate the static library, run
```
make libsfn.a
```
