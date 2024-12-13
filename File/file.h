#ifndef __LIB__
#define __LIB__

#include <stdio.h>
#include "../LProblem/lp.h"
#include "../Generals/generals.h"
#include "../Section_buffer/section_buffer.h"

/*
   ____________________________________________________________________________

    int process_line_args()

    Zpracovává argumenty příkazové řádky a získává cestu ke vstupnímu a
    výstupnímu souboru.

    Parametry:
    - argc: Počet argumentů příkazové řádky.
    - argv: Pole argumentů příkazové řádky.
    - input_path: Ukazatel pro uložení cesty ke vstupnímu souboru.
    - output_path: Ukazatel pro uložení cesty k výstupnímu souboru.

    Návratová hodnota:
    - 0 při úspěšném zpracování.
    - Chybové kódy při selhání.
   ____________________________________________________________________________
*/
int process_line_args(const int argc, char **argv, char *input_path, char *output_path);

/*
   ____________________________________________________________________________

    int check_filename_ext()

    Kontroluje, zda zadaný název souboru má požadovanou příponu.

    Parametry:
    - filename: Název souboru, který má být kontrolován.
    - ext: Očekávaná přípona.

    Návratová hodnota:
    - 0, pokud má soubor správnou příponu.
    - 1, pokud přípona nesouhlasí nebo pokud není soubor validní.
   ____________________________________________________________________________
*/
int check_filename_ext(const char *filename, const char *ext);

/*
   ____________________________________________________________________________

    int get_output_file()

    Získává cestu k výstupnímu souboru z příkazové řádky.

    Parametry:
    - argc: Počet argumentů příkazové řádky.
    - argv: Pole argumentů příkazové řádky.
    - output_path: Ukazatel na řetězec, kam se uloží cesta k výstupnímu souboru.

    Návratová hodnota:
    - 0 při úspěchu.
    - 93 při chybě nebo nevalidním argumentu.
   ____________________________________________________________________________
*/
int get_output_file(const int argc, char **argv, char *output_path);

/*
   ____________________________________________________________________________

    int get_input_file()

    Získává cestu ke vstupnímu souboru z příkazové řádky.

    Parametry:
    - argc: Počet argumentů příkazové řádky.
    - argv: Pole argumentů příkazové řádky.
    - input_path: Ukazatel na řetězec, kam se uloží cesta ke vstupnímu souboru.

    Návratová hodnota:
    - 0 při úspěchu.
    - 93 při chybě nebo nevalidním argumentu.
   ____________________________________________________________________________
*/
int get_input_file(const int argc, char **argv, char *input_path);

/*
   ____________________________________________________________________________

    int load_input_file()

    Načítá obsah vstupního souboru, čte řádky a ukládá je do odpovídajících
    sekcí v datové struktuře SectionBuffers.

    Parametry:
    - input_file: Ukazatel na otevřený vstupní soubor.
    - section_buffers: Ukazatel na datovou strukturu pro ukládání obsahu sekcí.

    Návratová hodnota:
    - 0 při úspěchu.
    - 11 při syntaktické chybě v souboru.
   ____________________________________________________________________________
*/
int load_input_file(FILE *input_file, SectionBuffers *section_buffers);

/*
   ____________________________________________________________________________

    int open_file()

    Otevírá soubor na zadané cestě v určeném režimu.

    Parametry:
    - file_path: Cesta k souboru.
    - mode: Režim otevření souboru ("r" pro čtení, "w" pro zápis).
    - file: Ukazatel na ukazatel otevřeného souboru.

    Návratová hodnota:
    - 0 při úspěšném otevření.
    - 1 nebo 2 při chybě (soubor neexistuje nebo nelze zapisovat).
   ____________________________________________________________________________
*/
int open_file(char *file_path, char *mode, FILE **file);

/*
   ____________________________________________________________________________

    void write_output_file()

    Zapisuje výsledky řešení simplexu do výstupního souboru.

    Parametry:
    - output_file: Ukazatel na otevřený výstupní soubor.
    - solution: Pole řešení lineárního problému.
    - general_vars: Ukazatel na strukturu obsahující názvy proměnných.
   ____________________________________________________________________________
*/
void write_output_file(FILE *output_file, double *solution, General_vars *general_vars);

#endif