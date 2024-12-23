#ifndef __LIB__
#define __LIB__

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

    Zpracovává argumenty příkazové řádky a hledá volby pro výstupní soubor.
    Funkce kontroluje přítomnost volby "--output" nebo "-o" a ověřuje,
    zda je k dispozici platná cesta k výstupnímu souboru.

    Parametry:
    - argc: Počet argumentů příkazové řádky.
    - argv: Pole argumentů příkazové řádky.
    - output_path: Ukazatel pro uložení cesty k výstupnímu souboru.

    Návratová hodnota:
    - 0 při úspěšném zpracování.
    - INVALID_ARGUMENT při chybějícím nebo neplatném argumentu pro volbu "-o/--output".
   ____________________________________________________________________________
*/
int get_output_file(const int argc, char **argv, char *output_path);

/*
   ____________________________________________________________________________

    int get_input_file()

    Zpracovává argumenty příkazové řádky a hledá cestu ke vstupnímu souboru.
    První ne-option (nezačínající pomlčkou) argument je považován za cestu
    ke vstupnímu souboru.

    Parametry:
    - argc: Počet argumentů příkazové řádky.
    - argv: Pole argumentů příkazové řádky.
    - input_path: Ukazatel pro uložení cesty ke vstupnímu souboru.

    Návratová hodnota:
    - 0 při úspěšném zpracování.
    - NO_INPUT_SPECIFIED pokud nebyla zadána žádná cesta ke vstupnímu souboru.
   ____________________________________________________________________________
*/
int get_input_file(const int argc, char **argv, char *input_path);

/*
   ____________________________________________________________________________

    int load_input_file()

    Načítá obsah vstupního souboru, čte řádky a ukládá je do odpovídajících
    sekcí v datové struktuře Section_Buffers.

    Parametry:
    - input_file: Ukazatel na otevřený vstupní soubor.
    - section_buffers: Ukazatel na datovou strukturu pro ukládání obsahu sekcí.

    Návratová hodnota:
    - 0 při úspěchu.
    - SYNTAX_ERROR při syntaktické chybě v souboru.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
   ____________________________________________________________________________
*/
int load_input_file(FILE *input_file, Section_Buffers *section_buffers);

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
    - INPUT_FILE_NOT_FOUND vstupní soubor nenalezen.
    - INVALID_OUTPUT_DESTINATION nelze zapisovat do výstupního souboru.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
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
