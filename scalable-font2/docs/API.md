Scalable Screen Font Renderer 2.0 API
=====================================

Font Metrics
------------

Before you can use any font renderer, you should be familiar with its font metrics:

```
   (0,0).............________
        ............. ^ ^ ^ ^
        ............. | | | |
        :.XXX.XX:.... | | | |
        :XX..XX.:.... | | | |
        :XX..XX.:.... | | | | ascender / horizontal baseline
        :XX..XX.:.... | | | | relative size
        :XX..XX.:.... | | | |
        :XX..XX.:.... | | | |
        ;.XXXXX,;,,,,_|_|_|_v
        :....XX.:.... | | |
        |XX..XX.:.... | | |   glyph height
        :.XXXX..:...._|_|_v
        :...:...:.... | |   absolute size
    |---:...:...:...._|_v (font width, font height)
    |   |   |   | |   |
    |   |   |   |_|___v   advance y
    |   |   |   | |
    |   |<->|   | |  vertical baseline (center of the glyph, always width / 2)
    |   |<----->| |  glyph width
    |   |<------->|  advance x
    |   |<---->|     advance x with kerning x
    |<->|            overlap (with previous glyph) / left margin
```

As you can see the coordinate system is like on the screen: top left is 0, 0 and bottom
right is +x, +y.

Glyphs must be aligned on `baseline`. When displaying, horizontal baseline must be
substracted from current pen y position, or when advance y is not zero, then vertical
baseline must be substracted from pen x (see Text Directions below).

The `overlap` tells how much you should move your pen BEFORE the glyph is drawn. It applies
only to horizontal fonts, and it is usually 0. For some characters in Serif Italic, this is
important, like for 'f', which has a bottom tail that must be moved below the previous
glyph.

The `advance` tells how much you should move your pen (cursor pointer) AFTER the glyph is
drawn on screen (usually not the same as font width). It typically equals to width plus a
little, or in case of vertical letters (when advance y is not zero), height plus a little.

`Kerning` is very similar to advance, but it tells how much you should move your cursor
if the next character is a particular one. For example the advance is different for 'f'
if it's followed by 'M' or an 'i'. The advance is bigger for 'fM' and smaller for 'fi'.

`Size` is how SSFN specifies a glyph's size. It's from the top to the horizontal baseline.
This is unlike in SSFN 1.0, which scaled the ascender portion. With SSFN 2.0 the converter
cuts off bearing top and left, so the results are the same. This scaling is unortodox, but
guarantees that you get approximately similar sized glyphs regardless to font metrics (I
hate when I get totally different pixel sizes for different TTF fonts when I ask for the
same size). Passing `SSFN_STYLE_ABS_SIZE` will avoid this scheme, and will scale the
glyph's total height to size as usual in other renderers.

`Line` is the line's height, calculated during rendering. With `SSFN_STYLE_ABS_SIZE` that
equals to `size`, otherwise a bit bigger.

You don't have to worry about text directions. The renderer handles vertical and horizontal
fonts for you, but it can't mix differently oriented texts. The same applies to Right-to-Left
scripting systems. You have to tell explicitly to render right-to-left by implementing
[bidirectional text algorithm](http://www.unicode.org/reports/tr9) as specified by UNICODE,
and passing a flag to the SSFN renderer. The BiDi state machine is not part of SSFN, because
this is a low level font rasterizer, not a text shaper library.

Usage
-----

Because the renderer is a single ANSI C/C++ header file, no shared library needed. It depends
only on libc memory allocation, nothing else. You can configure the renderers by defining
special defines.

```c
/* configure the renderer here with defines */
#include <ssfn.h>
```

Once you have the include, you can start using the library, no need for initialization, but you must
make sure that the initial context is zerod out (by putting it in the bss segment it will be, or call
memset zero manually).

As my favourite principle is K.I.S.S., there's only a few, clearly named functions in the API:
 - load one or more fonts into the context using `ssfn_load()` on program start.
 - set up rendering configuration by specifing font family, style and size with `ssfn_select()`.
 - if you want to change the size for example, you can call `ssfn_select()` again, no need to load the fonts again.
 - fill up an `ssfn_buf_t` struct that describes your pixel buffer to render to.
 - if you need it, you can get the rendered text's dimensions in advance with `ssfn_bbox()`.
 - call `ssfn_render()` to rasterize a glyph in the given style and size for a UNICODE code point in UTF-8.
 - repeat the last step until you reach the end of the UTF-8 string.
 - when done with rendering, call `ssfn_free()`.

Quick Examples and Tutorials
----------------------------

To load a font:
```c
#define SSFN_IMPLEMENTATION
#include <ssfn.h>

/* C */
ssfn_t ctx;
memset(&ctx, 0, sizeof(ssfn_t));

ssfn_load(&ctx, &_binary_freeserif_sfn_start);

/* C++ */
SSFN::Font font = new SSFN::Font;
font.Load(&_binary_freeserif_sfn_start, 0);
```

To render a text to an existing pixel buffer or to the screen:
```c
ssfn_buf_t buf;
int ret;
char *str = "String to render";

buf.ptr = sdlsurface->pixels;
buf.w = sdlsurface->w;
buf.h = sdlsurface->h;
buf.p = sdlsurface->pitch;
buf.fg = 0xFF808080;
buf.x = buf.y = 100;

/* C */
while((ret = ssfn_render(&ctx, &buf, str)) > 0)
    str += ret;

/* C++ */
while((ret = font.Render(&buf, str)) > 0)
    str += ret;
```

To allocate a new pixel buffer with transparent background and the rendered text on it:
```c
/* C */
ssfn_buf_t *buf = ssfn_text(&ctx, str, 0xFF101010);

/* C++ */
ssfn_buf_t *buf = font.Text(str, 0xFF101010);
```
This latter is a drop-in replacement to SDL_ttf package's TTF_RenderUTF8_Blended() function.

Configuration
-------------

```c
#define SSFN_IMPLEMENTATION
```

Include the normal renderer implementation as well, not just the file format defines and function prototypes.

```c
#define SSFN_CONSOLEBITMAP_PALETTE
#define SSFN_CONSOLEBITMAP_HICOLOR
#define SSFN_CONSOLEBITMAP_TRUECOLOR
```

Only one of these can be specified at once. These include the simple renderer which is totally independent
from the rest of the API. It is a very specialized renderer, limited and optimized for OS kernel consoles.
It renderers directly to the framebuffer, and you specify the framebuffer's pixel format with one of these.

Variable Types
--------------

| typedef        | Description                                             |
| -------------- | ------------------------------------------------------- |
| `ssfn_t`       | the renderer context, it's internals are irrelevant     |
| `ssfn_font_t`  | the font stucture, same as the [SSFN file header](https://gitlab.com/bztsrc/scalable-font2/blob/master/docs/sfn_format.md) |
| `ssfn_buf_t`   | the pixel buffer to render to (see `ssfn_render` below) |
| `int`          | the returned error code (if any)                        |

Error Codes
-----------

All functions return an integer error code. To get the human readable string representation of the error
code in English, use `ssfn_error(errcode)`.

| Error               | Numeric | Description                                       |
| ------------------- | ------: | ------------------------------------------------- |
| `SSFN_OK`           |       0 | success                                           |
| `SSFN_ERR_ALLOC`    |      -1 | memory allocation error                           |
| `SSFN_ERR_BADFILE`  |      -2 | bad SSFN file format                              |
| `SSFN_ERR_NOFACE`   |      -3 | no font face selected, call `ssfn_select()` first |
| `SSFN_ERR_INVINP`   |      -4 | invalid input parameter (usually a NULL pointer)  |
| `SSFN_ERR_BADSTYLE` |      -5 | unsupported style                                 |
| `SSFN_ERR_BADSIZE`  |      -6 | unsupported size                                  |
| `SSFN_ERR_NOGLYPH`  |      -7 | glyph not found                                   |

Simple Renderer Function
------------------------

## Convert UTF-8 to UNICODE

```c
uint32_t ssfn_utf8(char **str);
```

Decodes an UTF-8 multibyte character. Be careful, this function does not check its input, expects that pointer is
valid and points to a string with only valid UTF-8 sequences. The behaviour with invalid input is undefined. All the
other functions check for valid input, this is an exception because it's expected to be called repeatedly many times.

### Parameters

| Parameter | Description                       |
| --------- | --------------------------------- |
| str       | pointer to a UTF-8 string pointer |

### Return value

UNICODE code point, and `str` pointer adjusted to the next multibyte sequence.


The following function is only available if the header is included with one of the `SSFN_CONSOLEBITMAP_x` defines
(independently to `SSFN_IMPLEMENTATION`).

## Render a Glyph

```c
int ssfn_putc(uint32_t unicode);
```

Render a character to screen. Unlike the functions in the normal renderer, this function does not check its input,
can't uncompress gzipped fonts on its own, and it can't handle font collections either. Passing invalid parameters
results in an undefined behaviour. This is as simple as it gets.

This is a very specialized renderer that does not use ssfn_t context. It operates on one font only, can't scale or
dynamically style the glyph, and it can only handle bitmap fonts. It doesn't care about baseline, but it handles
advances. For these limitations in return it does not allocate memory at all, has exactly zero dependency and
compiles to less than 2 kilobytes.

### Parameters

| Parameter | Description                  |
| --------- | ---------------------------- |
| unicode   | UNICODE code point to render |

Configuration is passed to it in two global variables (exclusive to this function, the normal renderer does not use them):

| Global Variable            | Description                                                                      |
| -------------------------- | -------------------------------------------------------------------------------- |
| `ssfn_font_t *ssfn_src`    | pointer to an SSFN font with bitmap glyphs only                                  |
| `ssfn_buf_t *ssfn_dst`     | the destination pixel buffer (see `ssfn_render` below)                           |

The define selects the destination buffer's pixel format. `SSFN_CONSOLEBITMAP_PALETTE` selects 1 byte
(indexed), `SSFN_CONSOLEBITMAP_HICOLOR` selects 2 bytes (5-5-5 or 5-6-5 RGB) and `SSFN_CONSOLEBITMAP_TRUECOLOR`
selects 4 bytes (8-8-8-8 xRGB). For performance reasons, 3 bytes (24 bit true color) mode is not supported.
If `.w` and `.h` is not set, then no clipping will be performed. With `ssfn_dst.bg` being full 32 bit wide
zero, `ssfn_putc` will operate in transparent background mode: it will only modify the destination buffer
where the font face is set. To clear the glyph's background, set it to some value where the most significant
byte (alpha channel for true-color mode) is 255, like 0xFF000000. This will fill thebackground with the index
0 (palette) or full black (hicolor and truecolor modes).

### Return value

Either `SSFN_OK`, `SSFN_ERR_NOGLYPH` or `SSFN_ERR_INVINP` (bad or gzip compressed font). On success it modifies
`ssfn_dst.x` and `ssfn_dst.y`.


Normal Renderer Functions
-------------------------

The following functions are only available if the header is included with `SSFN_IMPLEMENTATION` define.
They allocate memory and therefore depend on libc (or crafted compilers like gcc have all four functions
as built-ins), hence are expected to be used by normal user space applications. If you really want to use
this renderer from a kernel, you can replace all dependent functions with your own implemenations using the
following defines:

```c
#define SSFN_memcmp memcmp
#define SSFN_memset memset
#define SSFN_realloc realloc
#define SSFN_free free
```

If you don't specify these, then the SSFN header will try to figure it out if they are supported as built-ins
or provided by libc.

That's all, no more dependencies. :-)

## Initilization

```c
/* C */
ssfn_t ctx = { 0 };

/* C++ */
font = new SSFN::Font;
```

Nothing special, just make sure the context is zerod out.

## Load Fonts

```c
/* C */
int ssfn_load(ssfn_t *ctx, const void *data);

/* C++ */
int SSFN::Font.Load(const std::string &data);
int SSFN::Font.Load(const void *data);
```

Loads a font or font collection into the renderer context. For C++ there's a version with std::string and
also with a C-style character buffer.

### Parameters

| Parameter | Description                                    |
| --------- | ---------------------------------------------- |
| ctx       | pointer to the renderer's context              |
| data      | pointer to a font in memory                    |

You can load an SSFN file and pass it's address, or you can also use `ld -b binary` to convert an SSFN file
into an object and link that with your code. In this case you'll have a `_binary_(filename)_start` label.

You can also pass an SSFN font collection to this function, in which case all fonts within the collection
will be loaded into the context at once.

The font can be gzip compressed, `ssfn_load()` will transparently uncompress it (thanks to stb!).

### Return value

Error code. `SSFN_ERR_BADFILE` means bad (incorrect or inconsistent) SSFN format. Hint: use `sfnconv -d` to
debug what's wrong with it. The font validator prints out very helpful messages.

## Select Face

```c
/* C */
int ssfn_select(ssfn_t *ctx, int family, const char *name, int style, int size);

/* C++ */
int SSFN::Font.Select(int family, const std::string &name, int style, int size);
int SSFN::Font.Select(int family, const char *name, int style, int size);
```

Sets up renderer context to use a particular font family, style and size. Size specifies the relative
glyph size in pixels. In other words, if you set `size` to 32, then you'll get an exatly 32 pixel tall
'A', but a taller 'g'. See Font Metrics above. Passing `SSFN_STYLE_ABS_SIZE` will scale the glyph's
height to `size`. This is also the default for Monospace fonts.

### Parameters

| Parameter | Description                                                                          |
| --------- | ------------------------------------------------------------------------------------ |
| ctx       | pointer to the renderer's context                                                    |
| family    | font family group, see below                                                         |
| name      | pointer to an UTF-8 string with font's unique name if family is `SSFN_FAMILY_BYNAME` |
| style     | one or more font style defines OR'd together, see below                              |
| size      | rendered font's size in pixels, from 8 to 192                                        |

Parameter defines:

| family                  | Description                                                        |
| ----------------------- | ------------------------------------------------------------------ |
| `SSFN_FAMILY_SERIF`     | selects Serif fonts (letters with "feet", like Times New Roman)    |
| `SSFN_FAMILY_SANS`      | selects Sans Serif fonts (letters without "feet", like Helvetica)  |
| `SSFN_FAMILY_DECOR`     | selects Decorative fonts (like Dingbats or Fraktur)                |
| `SSFN_FAMILY_MONOSPACE` | selects Monospace fonts (like Fixed or GNU unifont)                |
| `SSFN_FAMILY_HAND`      | selects Handwriting fonts (like Cursive)                           |
| `SSFN_FAMILY_ANY`       | don't care, pick the first font which has the glyph                |
| `SSFN_FAMILY_BYNAME`    | use the `name` field to select precisely one font                  |

| style                   | Description                                                        |
| ----------------------- | ------------------------------------------------------------------ |
| `SSFN_STYLE_REGULAR`    | regular style                                                      |
| `SSFN_STYLE_BOLD`       | bold weight. If no font found, the renderer will mimic bold        |
| `SSFN_STYLE_ITALIC`     | italic or oblique slant. If no font found, the renderer will transform glyphs |
| `SSFN_STYLE_USRDEF1`    | user defined style (eg. to select a specific font in a collection) |
| `SSFN_STYLE_USRDEF2`    | user defined style                                                 |
| `SSFN_STYLE_UNDERLINE`  | under line glyphs. This is not stored in fonts, always generated   |
| `SSFN_STYLE_STHROUGH`   | strike-through glyphs. Not stored either, always generated         |
| `SSFN_STYLE_NOAA`       | don't use anti-aliasing (might be unreadable for small sizes)      |
| `SSFN_STYLE_NOKERN`     | don't use kerning relation when calculating advance offsets        |
| `SSFN_STYLE_NODEFGLYPH` | don't draw default default glyph for missing ones                  |
| `SSFN_STYLE_NOCACHE`    | don't use internal glyph cache (slower, but memory efficient)      |
| `SSFN_STYLE_RTL`        | render in Right-to-Left direction                                  |
| `SSFN_STYLE_ABS_SIZE`   | use absolute size (glyph's total height will be scaled to size)    |

### Return value

Error code. `SSFN_ERR_NOFACE` returned if no font could be found, otherwise `SSFN_ERR_BADx` refers to
the invalid argument. Calling this function will always flush the internal glyph cache.

## Render a Glyph

```c
/* C */
int ssfn_render(ssfn_t *ctx, ssfn_buf_t *dst, const char *str);

/* C++ */
int SSFN::Font.Render(ssfn_buf_t *dst, const std::string &str);
int SSFN::Font.Render(ssfn_buf_t *dst, const char *str);
```

Render one glyph. It is possible to load more fonts with different UNICODE code range coverage, the
renderer will pick the ones which have a glyph defined for `str`. If more fonts have glyphs for this
character, then the renderer will look for the best variant and style match to figure out which font to
use, unless you've asked for a specific font with `SSFN_FAMILY_BYNAME`. If there's no font for the
requested style, then the renderer will mimic bold or italic. It is also possible that one of the fonts
have ligatures, and more characters will be parsed for a single glyph.

Unlike the simple render, for which you can choose a pixel format using defines, this one only supports
32 bit packed pixel buffers. As it takes care for the anti-aliasing and alpha-blending as well, it
depends on a packed pixel format which has a separate alpha channel.

The renderer will assume ARGB channel order (blue is the least significant byte, alpha the most). To
support ABGR buffers, specify the buffer's width as negative, for example -1920.

### Parameters

| Parameter   | Description                                                                       |
| ----------- | --------------------------------------------------------------------------------- |
| ctx         | pointer to the renderer's context                                                 |
| dst         | destination pixel buffer to render to (see `ssfn_buf_t` fields below)             |
| str         | UNICODE code point of the character in UTF-8 to be rendered                       |

Destionation buffer descriptor struct:

| `ssfn_buf_t` | Description                                                                      |
| ------------ | -------------------------------------------------------------------------------- |
| `.ptr`       | the buffer's address (probably the linear frame buffer, but could be off-screen) |
| `.p`         | the buffer's bytes per line (pitch, not in pixels, in bytes!)                    |
| `.w`         | the buffer's width in pixels (optional, negative means ABGR, otherwise ARGB)     |
| `.h`         | the buffer's height in pixels (optional)                                         |
| `.fg`        | the foreground color in destination buffer's native format                       |
| `.bg`        | the background color (only used if non-zero)                                     |
| `.x`         | the coordinate to draw to, will be modified by advance values and kerning        |
| `.y`         | the coordinate to draw to, will be modified by advance values and kerning        |

### Return value

Error code (negative) or the number of bytes processed from the `str`. Zero means end of string. This call
generates the glyph and (if `SSFN_STYLE_NOCACHE` is not specified) stores it in the internal cache.
After that ssfn_render will blit the glyph to the pixel buffer using scaling and alpha-blending. Finally
it takes care of the advance and (if `SSFN_STYLE_NOKERN` not given) kerning offsets automatically, and
updates `.x` and `.y` fields in `dst`. The rendered line's height will be accumulated in `ctx->line` until
you reset it.

## Get Bounding Box

```c
/* C */
int ssfn_bbox(ssfn_t *ctx, const char *str, int *w, int *h, int *left, int *top);

/* C++ */
int SSFN::Font.BBox(const std::string &str, int *w, int *h, int *left, int *top);
int SSFN::Font.BBox(const char *str, int *w, int *h, int *left, int *top);

```

Returns the dimensions of a rendered text. This function handles horizontal and vertical texts, but not
mixed ones. It does not render the glyphs to any pixel buffers, but it refreshes the internal glyph cache
if a new glyph appears in `str`.

### Parameters

| Parameter   | Description                                                              |
| ----------- | ------------------------------------------------------------------------ |
| ctx         | pointer to the renderer's context                                        |
| str         | pointer to a zero terminated UTF-8 string                                |
| w           | pointer to an integer, returned width in pixels                          |
| h           | pointer to an integer, returned height in pixels                         |
| left        | pointer to an integer, returned left margin (overlap) in pixels          |
| top         | pointer to an integer, returned ascender (horizontal baseline) in pixels |

### Return value

Error code, and on success the bounding box size in `w`, `h`. When used with an ssfn_buf_t, the
ascender should be `y` as that's the baseline and left margin is the initial `x` coordinate as
for the first glyph there's no previous glyph to overlap on.

## Render Text to a New Pixel Buffer

```c
/* C */
ssfn_buf_t *ssfn_text(ssfn_t *ctx, const char *str, unsigned int fg);

/* C++ */
ssfn_buf_t *SSFN::Font.Text(const std::string &str, unsigned int fg);
ssfn_buf_t *SSFN::Font.Text(const char *str, unsigned int fg);
```

Allocates a new, transparent buffer of the required size and renders an UTF-8 string into it.
This has the same arguments as SDL_ttf package's TTF_RenderUTF8_Blended() function.

### Parameters

| Parameter   | Description                                                              |
| ----------- | ------------------------------------------------------------------------ |
| ctx         | pointer to the renderer's context                                        |
| str         | pointer to a zero terminated UTF-8 string                                |
| fg          | foreground color to use in buffer's native format                        |

### Return value

A newly allocated pixel buffer or NULL on error. The pixel buffer has to be freed with
```c
free(ret->ptr);
free(ret);
```

## Get Memory Usage

```c
/* C */
int ssfn_mem(ssfn_t *ctx);

/* C++ */
int SSFN::Font.Mem();
```

Returns how much memory a particular renderer context consumes. It is typically less than 64k, but strongly depends
how big and much glyphs are stored in the internal cache. Internal buffers can be freed with `ssfn_free()`.

### Parameters

| Parameter   | Description |
| ----------- | ----------- |
| ctx         | pointer to the renderer's context |

### Return value

Total memory consumed in bytes.

## Free Memory

```c
/* C */
void ssfn_free(ssfn_t *ctx);

/* C++ */
delete SSFN::Font;
```

Destructor of the renderer context. Frees all internal buffers and clears the context.

### Parameters

| Parameter | Description |
| --------- | ----------- |
| ctx       | pointer to the renderer's context |

### Return value

None.

## Error Handling

```c
/* C */
const char *ssfn_error(int errcode);

/* C++ */
const std::string SSFN::Font.ErrorStr(int err);
```

Returns the human readable string equivalent of an error code in English. In C this one is not
a real function, it is implemented as a macro.

### Parameters

| Parameter | Description |
| --------- | ----------- |
| errcode   | error code  |

### Return value

Pointer to a string constant. `SSFN_OK` returns an empty string, not NULL nor "Success".

