#ifndef __BOUNDS__
#define __BOUNDS__

#include "../Generals/generals.h"

/*
   ____________________________________________________________________________

    typedef struct Bounds

    Struktura, která slouží k reprezentaci mezí jednotlivých proměnných
    v problému lineárního programování.

    Obsah:
    - lower_bound: Pole dolních mezí odpovídajících proměnných.
    - upper_bound: Pole horních mezí odpovídajících proměnných.
    - num_vars: Počet proměnných, pro které jsou meze definovány.
    - max_vars: Maximální kapacita pole pro dolní a horní meze.
   ____________________________________________________________________________
*/
typedef struct {
    double *lower_bound;
    double *upper_bound;
    int num_vars;
    int max_vars;
} Bounds;

/*
   ____________________________________________________________________________

    Bounds *create_bounds()

    Inicializuje strukturu Bounds, která obsahuje dolní a horní meze
    proměnných. Přiřazuje výchozí hodnoty (dolní = 0.0, horní = INFINITY)
    a alokuje paměť podle zadané počáteční velikosti.

    Parametry:
    - initial_size: Počet proměnných, pro které bude alokována paměť.

    Návratová hodnota:
    - Ukazatel na inicializovanou strukturu Bounds nebo NULL, pokud selže
      alokace paměti.
   ____________________________________________________________________________
*/
Bounds *create_bounds(const int initial_size);

/*
   ____________________________________________________________________________

    void free_bounds()

    Uvolňuje paměť alokovanou pro strukturu Bounds, která obsahuje informace
    o dolních a horních mezích proměnných.

    Parametry:
    - bounds: Ukazatel na strukturu Bounds.
   ____________________________________________________________________________
*/
void free_bounds(Bounds *bounds);

/*
   ____________________________________________________________________________

    void add_bound()

    Přidává dolní a horní mez k proměnné podle jejího indexu ve struktuře
    General_vars. Pokud index přesahuje aktuální kapacitu, funkce dynamicky rozšiřuje
    alokovanou paměť a inicializuje nové mezní hodnoty výchozími hodnotami.

    Parametry:
    - bounds: Ukazatel na strukturu Bounds.
    - lower_bound: Dolní mez proměnné.
    - upper_bound: Horní mez proměnné.
    - var_index: Index proměnné ve struktuře General_vars.
   ____________________________________________________________________________
*/
void add_bound(Bounds *bounds, const double lower_bound, const double upper_bound, int var_index);

/*
   ____________________________________________________________________________

    int parse_bounds()

    Zpracovává sekci Bounds z .lp souboru. Rozpoznává názvy proměnných
    a přiřazuje jim dolní a horní meze.
    Pokud je proměnná označena jako "free", nastaví její dolní mez na -INFINITY
    a horní mez na INFINITY.

    Parametry:
    - bounds: Ukazatel na ukazatel struktury Bounds, který bude inicializován
              a naplněn během zpracování.
    - general_vars: Ukazatel na strukturu General_vars, která obsahuje proměnné.
    - lines: Pole řetězců reprezentujících řádky sekce Bounds.
    - num_lines: Počet řádků v sekci Bounds.

    Návratová hodnota:
    - 0 při úspěchu.
    - Chybové kódy při syntaktické nebo paměťové chybě.
   ____________________________________________________________________________
*/
int parse_bounds(Bounds **bounds, General_vars *general_vars, char **lines, int num_lines);

/*
   ____________________________________________________________________________

    int is_number()

    Kontroluje, zda zadaný řetězec představuje validní číslo.

    Parametry:
    - str: Řetězec, který má být kontrolován.

    Návratová hodnota:
    - 0, pokud řetězec představuje validní číslo.
    - 1, pokud ne.
   ____________________________________________________________________________
*/
int is_number(char *str);

#endif
