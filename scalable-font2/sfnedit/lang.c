/*
 * sfnedit/lang.c
 *
 * Copyright (C) 2019 bzt (bztsrc@gitlab)
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * @brief Multilanguage support
 *
 */

#include <string.h>
#include <stdlib.h>
#include "lang.h"

/**
 * Translations
 */
char *dict[NUMLANGS][NUMTEXTS + 1] = {
    {
        "en",
        "Loading...",
        "memory allocation error",
        "unable to load %s",
        "bad file format %s",
        "%s is a collection, extract it first with sfn2sfn",
        "unable to save file",
        "Save modified font?",
        "unable to add color, should not happen",
        "unable to open display",
        "Help",
        "Properties",
        "Ranges",
        "Characters",
        "Edit",
        "Kerning",
        "Family",
        "Style",
        "Grid size",
        "Baseline",
        "Underline",
        "Unique name",
        "Family name",
        "Subfamily name",
        "Font revision",
        "Manufacturer",
        "License",
        "Add new character",
        "Save SSFN",
        "Save ASC",
        "No, quit",
        "Serif",
        "Sans Serif",
        "Decorative",
        "Monospace",
        "Handwriting",
        "Bold",
        "Italic",
        "Recalibrate",
        "Uncovered ranges",
        "Coverage",
        "Range",
        "Whole UNICODE range",
        "no glyph",
        "undefined",
        "local0 / default",
        "local1 / initial",
        "local2 / medial",
        "local3 / final",
        "local4",
        "local5",
        "local6",
        "Are you sure you want to delete?",
        "Yes",
        "No",

        "There's one main window per font with it's properties.",
        "You can open one glyph window per character.",
        "On the properties tab you can modify the meta information, add new character and save the font.\n\n"
            "Universal keyboard shortcuts (in any window):\n[Esc] - closes the window (on the main window closes the editor too)\n"
            "[Ctrl] + [S] - saves the font in SSFN format\n"
            "[Ctrl] + [Shift] + [S] - saves the font in ASC format\n"
            "[Up] / [Down] - switch input fields\n"
            "[Left] / [Right] / [Space] - change input field value\n",
        "The ranges tab shows a filterable overview of the defined glyphs. You can search for range name\n"
            "and with hex values for UNICODE code points too.\n\n[Space] - toggles uncovered ranges filter on/off\n"
            "[Up] / [Down] - scroll list\n"
            "Pressing any other key starts search.",
        "The characters tab lists the glyphs table. You can search for the character itself, its name and\n"
            "its code point. The list is limited to the range selected on the range tab.\n\n"
            "[Up] / [Down] - scroll the table\n"
            "[Left] / [Right] - switch variant\n"
            "[Ctrl] + [c] - copy to clipboard\n"
            "[Ctrl] + [x] - copy to clipboard and cut\n"
            "[Ctrl] + [v] - paste from clipboard\n"
            "leftclick and move - select range\n",
        "The edit tab allows the modification of glyph layers.\n\n"
            "[g] - turns grid on/off\n"
            "[l] - turns control lines on/off\n"
            "wheel - zoom in/out\n"
            "leftclick on background and move - move EM canvas\n",
        "Here you can set the relative distance between two characters.\n"
            "[r] - toggle right-to-left flag on glyph\n",
        "Here you can set either horizontal or vertical advance.\n"
            "[r] - toggle right-to-left flag on glyph\n"
            "leftclick - set advance\n"
            "[Shift] + leftclick - set baseline (globally)\n"
            "[Ctrl] + leftclick - set underline position (globally)\n",
        "Set the coloumns for hinting\n"
            "leftclick and move - relocate coloumn\n",
        "In edit layer mode you can modify the glyph itself\n"
            "[Shift] + wheel - change colors\n"
            "[Shift] + leftclick - select area\n"
            "[Ctrl] + leftclick - for pixmaps, pick a color from glyph\n"
            "leftclick - select a point or turn on pixel\n"
            "leftclick and move - move selected point\n"
            "rightclick - turn pixel off or delete last countour\n",
    },
    {
        "hu",
        "Betöltés...",
        "memória foglalási hiba",
        "nem sikerült megnyitni: %s",
        "nem megfelelő fájlformátum: %s",
        "%s egy kollekció, előbb szedd szét sfn2sfn-el",
        "nem tudom lementeni a fájlt",
        "Elmented a módosításokat?",
        "nem tudok színt hozzáadni, nem szabadna előfordulnia",
        "nem sikerült ablakot nyitni",
        "Súgó",
        "Jellemzők",
        "Tartományok",
        "Karakterek",
        "Szerkesztés",
        "Igazítás",
        "Család",
        "Stílus",
        "Rácsméret",
        "Alapvonal",
        "Aláhúzás",
        "Egyedi név",
        "Családnév",
        "Alcsaládnév",
        "Revizió",
        "Készítő",
        "Licensz",
        "Új karakter hozzáadása",
        "SSFN mentése",
        "ASC mentése",
        "Nem, kilépek",
        "Serif",
        "Sans Serif",
        "Dekoratív",
        "Monoköz",
        "Kézírás",
        "Félkövér",
        "Dőlt",
        "Rekalibrálás",
        "Lefedetlen tartományok",
        "Lefedettség",
        "Tartomány",
        "Teljes UNICODE tartomány",
        "nincs glif",
        "nem definiált",
        "lokál0 / alapért",
        "lokál1 / elején",
        "lokál2 / középen",
        "lokál3 / végén",
        "lokál4",
        "lokál5",
        "lokál6",
        "Biztos törölni szeretné?",
        "Igen",
        "Nem",

        "A fő ablak csak egy példányban van, itt található a font adatlapja.",
        "A glif ablakból több is nyitható, karakterenként egy.",
        "A tulajdonságok tabon lehet állítani a font adatait, itt lehet új karaktert hozzáadni és a\nfontot elmenteni.\n\n"
            "Általános billentyű kombinációk (bármelyik ablakban):\n[Esc] - bezárja az ablakot (főablakon a szerkesztőt is)\n"
            "[Ctrl] + [S] - elmenti a fontot SSFN fotmátumban\n[Ctrl] + [Shift] + [S] - elmenti a fontot ASC formátumban\n"
            "[Fel] / [Le] - beviteli mezőt vált\n[Jobbra] / [Balra] / [Szóköz] - beviteli érték változtatása",
        "A tartomány tabon szűrhető, milyen karakterek vannak definiálva. Kereshető a tartománynév és\n"
            "hexa kóddal az UNICODE kódpont is.\n\n[Szóköz] - a lefedetlen tartományok listázását kapcsolja ki/be\n"
            "[Fel] / [Le] - szkrollozza a listát\n"
            "Bármelyik másik gomb keresést kezdeményez.",
        "A glif tábla tab listázza a karaktereket. Kereshető a karakter maga, a neve és a hexa kódpontja.\n"
            "A lista a tartományok tabon kiválasztott tartományra van szűkítve.\n\n"
            "[Fel] / [Le] - szkrolloza a táblát\n"
            "[Jobbra] / [Balra] - variánsválasztás\n"
            "[Ctrl] + [c] - vágólapra másolás\n"
            "[Ctrl] + [x] - kivágás vágólapra\n"
            "[Ctrl] + [v] - beillesztés vágólapról\n"
            "balklikk és mozgatás - tartomány kijelölése",
        "Szerkesztő tabon lehet módosítani a glif rétegeket.\n\n"
            "[g] - be/kikapcsolja a rácsot\n"
            "[l] - be/kikapcsolja a kontrol vonalakat\n"
            "görgő - nagyítás/kicsinyítés\n"
            "balklikk a háttéren és mozgatás - vászon mozgatása\n",
        "Itt lehet két karakter relatív távolságát beállítani.\n"
            "[r] - be/kikapcsolja a jobbról-balra írást a glifen\n",
        "Az eltolás eszközzel lehet állítani a horizontális vagy\nvertikális eltolást.\n"
            "[r] - be/kikapcsolja a jobbról-balra írást a glifen\n"
            "balklikk - eltolás beállítása\n"
            "[Shift] + balklikk - alapvonal beállítása (globális)\n"
            "[Ctrl] + balklikk - aláhúzás helyének beállítása (globális)\n",
        "Itt lehet beállítani mely oszlopokhoz igazítsa a glifet\n"
            "balklikk és mozgatás - oszlop áthelyezése\n",
        "Réteg szerkesztése módban lehet módosítani magát a glifet\n"
            "[Shift] + görgő - színválasztás\n"
            "[Shift] + balklikk - kijelölés\n"
            "[Ctrl] + balklikk - pixmap esetén szín kivílasztás glifről\n"
            "balklikk - pont kiválasztása vagy pixel beállítása\n"
            "balklikk és mozgatás - pont áthelyezése\n"
            "jobbklikk - pixel törlése vagy az utolsó kontúr törlése\n",
    }
};
char **lang = NULL;

/**
 * Load a dictionary
 */
void lang_init()
{
    int i;
    char *env = getenv("LANG");

    for(i = 0, lang = NULL; env && i < NUMLANGS; i++) {
        if(!memcmp(env, dict[i][0], strlen(dict[i][0]))) {
            lang = &dict[i][1];
            break;
        }
    }
    if(!lang)
        lang = &dict[0][1];
}
