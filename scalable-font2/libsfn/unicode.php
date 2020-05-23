#!/usr/bin/php
<?php
/*
 * libsfn/unicode.php
 *
 * Copyright (C) 2020 bzt (bztsrc@gitlab)
 *
 * @brief small tool to generate unicode.h
 *
 * a simple search'n'replace would do on the texts, but we have to count undefined
 * code points in each block for proper code point coverage reports.
 *
 * See:
 *   http://www.unicode.org/Public/UCD/latest/ucd/Blocks.txt
 *   http://www.unicode.org/Public/UCD/latest/ucd/UnicodeData.txt
 */

echo("Counting UNICODE blocks... ");
$blocks = [];
foreach(file("Blocks.txt") as $line) {
    if(preg_match("/^([0-9a-fA-F]+)\.\.([0-9a-fA-F]+);\ (.+)/", $line, $m)) {
        $s = hexdec($m[1]); $e = hexdec($m[2]);
        if($s == 0xE000) $e = 0xEFFF;
        $blocks[] = [ $e-$s+1, $s, $e, $m[3] ];
        /* the upper part of Private Use Area is used for ligatures by SSFN */
        if($s == 0xE000)
            $blocks[] = [ 0xF8FF-0xF000+1, 0xF000, 0xF8FF, "Ligatures" ];
    }
}
foreach(file("UnicodeData.txt") as $line) {
    $l = explode(";",$line);
    $i = hexdec($l[0]);
    $j = substr($l[1],-6)=="First>"; /* blocks which have no character definitions at all */
    $u[]=[$i, $l[1][0]=='<' && !empty($l[10])? $l[10] : $l[1], $l[4]=="R" || $l[4]=="AL"];
    foreach($blocks as $k=>$b)
        if($i >= $b[1] && $i <= $b[2]) { if($j) $blocks[$k][0]=0; else $blocks[$k][0]--; }
}
$s="/*
 * libsfn/unicode.h
 *
 * --- Generated from Blocks.txt and UnicodeData.txt by unicode.php ---
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
 * @brief UNICODE blocks and code point names
 *
 */

/*** UNICODE blocks ***/
typedef struct {
    int cnt;
    int undef;
    int start;
    int end;
    char *name;
} unicode_block_t;
#define UNICODE_NUMBLOCKS ".count($blocks)."
#ifndef _UNICODE_BLOCKSDATA
extern unicode_block_t ublocks[];
#else
unicode_block_t ublocks[] = {
";
foreach($blocks as $k=>$b) {
    $s.=sprintf("    { 0,%2d, 0x%06x, 0x%06x, \"%s\" }",$b[0]<0||$b[0]>=$b[2]-$b[1]?0:$b[0],$b[1],$b[2],
        str_replace("\"","",$b[3])).(isset($blocks[$k+1])?",":"")."\n";
}
$s.="};\n#endif\n\n";
$s.="/*** UNICODE code point names ***/
#define tolower(X)  (((X) >= 'A') && ((X) <= 'Z') ? ('a'+((X)-'A')) : (X))
int unicmp(char *a, char *b);
void uniname_free();
int uniname(int unicode);
char *utf8(int i);
typedef struct {
    int unicode;
    int rtl;
    char *name;
} uniname_t;
#define UNICODE_NUMNAMES ".count($u)."
#ifndef _UNICODE_NAMESDATA
extern uniname_t uninames[UNICODE_NUMNAMES+1];
#else
uniname_t uninames[UNICODE_NUMNAMES+1];
";
echo("OK\nCompressing UNICODE names... ");

$u[0][1]="";
$i=0; $N="";
foreach($u as $k=>$n) {
    if($i < $n[0]) {
        while($i + 32768 < $n[0]) {
            $N.=pack("v", -32768);
            $i+=32768;
        }
        $N.=pack("v", -($n[0]-$i));
    }
    $N.=chr(intval($n[2]));
    $N.=str_replace("\'","\\\'",str_replace("\"","",$n[1])).chr(0);
    $i=$n[0] + 1;
}
$N = gzcompress($N, 9);
$s.="#define UNICODE_DAT_SIZE ".strlen($N)."
unsigned char unicode_dat[UNICODE_DAT_SIZE] = {";
for($i = 0; $i < strlen($N); $i++)
    $s.=($i?",":"").sprintf("0x%02x",ord($N[$i]));
$s.="};\n#endif\n\n";
echo("OK\n");
file_put_contents("unicode.h", $s);
