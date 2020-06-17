#!/usr/bin/php
<?php
/*
 * sfnedit/misc/icon.php
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
 *
 * @brief small tool to generate icon.h
 */

$f=fopen("../icon.h","w");
fprintf($f,"/*
 * sfnedit/icon.h
 *
 * --- Generated from misc data by icon.php ---
 *
 * Copyright (C) ".date("Y")." bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the \"Software\"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief various icons (window, tools, background logo etc.) and fonts
 *
 */

");
system("convert bg.png -alpha extract bga.png");
foreach(["icon32.png","icons.png","numbers.png","bga.png","logo.sfn"] as $fn) {
    fprintf($f,"uint8_t ".str_replace(".","_",$fn)."[] = {\n");
    $data=file_get_contents($fn);
    for($i=0;$i<strlen($data);$i++)
        fprintf($f,"0x%x,",ord($data[$i]));
    fprintf($f,"\n};\n\n");
}
fprintf($f,"uint8_t unifont_gz[] = {\n");
$data=file_get_contents("../../fonts/unifont.sfn.gz");
for($i=0;$i<strlen($data);$i++)
    fprintf($f,"0x%x,",ord($data[$i]));
fprintf($f,"\n};\n\n");
fclose($f);
unlink("bga.png");
