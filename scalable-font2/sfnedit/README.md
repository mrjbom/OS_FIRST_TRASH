Scalable Screen Font 2.0 Editor
===============================

This is a GUI font editor. For further details, read the [documentation](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfnedit.md).

Supported Formats
-----------------

It uses the same [libsfn](https://gitlab.com/bztsrc/scalable-font2/tree/master/libsfn) as the converter tool, so it can

### Import Fonts

- [SSFN](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md) (.sfn)
- [ASC](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/asc_format.md) (.asc)
- PostScript Type 1 (.ps)
- TrueType (.ttf)
- OpenType (.otf)
- Windows FNT (.fnt, .fon)
- X11 Bitmap Distribution Format (.bdf)
- X11 Portable Compiled Format (.pcf)
- PC Screen Font 2.0 (.psf, .psfu)
- GNU unifont hex (.hex)

### Save Fonts

- SSFN
- ASC

Dependencies
------------

For portability, it uses SDL2 (or X11 as fallback on platforms that has X11), and libsfn might have some additional,
but optional dependencies.

To compile, use
```sh
$ make
```

If despite SDL2 is detected, but you want to use X11, then
```sh
$ USE_X11=yes make
```

### Under Windows

You'll need a couple of tools, here's a step-by-step how to:
1. install [MinGW](https://osdn.net/projects/mingw/releases), this will give you gcc and zlib under Windows
2. download [SDL2-devel-X-mingw.tar.gz](http://libsdl.org/download-2.0.php) under the section Development Libraries
3. extract SDL2 into a directory under MinGW's home directory
4. open Makefile in Notepad, and edit MINGWSDL to the path where you've extracted the tarball, add the last SDL2-X part too
5. run `make`
