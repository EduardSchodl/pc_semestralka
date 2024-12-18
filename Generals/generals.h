#ifndef __GENERALS__
#define __GENERALS__

/*
   ____________________________________________________________________________

    typedef struct General_vars

    Struktura, která slouží k uchovávání informací o rozhodovacích proměnných
    v problému lineárního programování.

    Obsah:
    - general_vars: Pole ukazatelů na názvy rozhodovacích proměnných.
    - num_general_vars: Počet aktuálně uložených rozhodovacích proměnných.
    - max_vars: Maximální kapacita pole pro rozhodovací proměnné.
    - used_vars: Pole příznaků, které označují, zda byla daná proměnná použita
                 (1 = použitá, 0 = nepoužitá).
   ____________________________________________________________________________
*/
typedef struct {
    char **general_vars;
    int num_general_vars;
    int max_vars;
    int *used_vars;
} General_vars;

/*
   ____________________________________________________________________________

    General_vars* create_general_vars()

    Alokuje paměť pro strukturu General_vars a inicializuje ji s počáteční
    velikostí.

    Parametry:
    - initial_size: Počáteční velikost pole názvů proměnných a příznaků použití.

    Návratová hodnota:
    - Ukazatel na nově vytvořenou strukturu General_vars.
    - NULL při chybě alokace paměti.
   ____________________________________________________________________________
*/
General_vars *create_general_vars(const int initial_size);

/*
   ____________________________________________________________________________

    int parse_generals()

    Zpracovává sekci Generals, která obsahuje seznam názvů rozhodovacích
    proměnných. Názvy jsou načteny a uloženy do datové struktury
    General_vars.

    Parametry:
    - general_vars: Ukazatel na ukazatel na strukturu General_vars.
    - lines: Pole řádků obsahujících názvy proměnných.
    - num_lines: Počet řádků.

    Návratová hodnota:
    - 0 při úspěšném zpracování.
    - SYNTAX_ERROR při syntaktické chybě v názvu proměnné.
    - MEMORY_ERROR při chybě v paměti.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
   ____________________________________________________________________________
*/
int parse_generals(General_vars **general_vars, char **lines, const int num_lines);

/*
   ____________________________________________________________________________

    void add_variable()

    Přidává nový název proměnné do datové struktury General_vars. Pokud
    velikost pole přesáhne aktuální kapacitu, pole se dynamicky zvětší.

    Parametry:
    - general_vars: Ukazatel na strukturu General_vars.
    - var_name: Název přidávané proměnné.
   ____________________________________________________________________________
*/
void add_variable(General_vars *general_vars, const char *var_name);

/*
   ____________________________________________________________________________

    void free_general_vars()

    Uvolňuje paměť alokovanou pro strukturu General_vars, včetně názvů
    proměnných a příznaků použití.

    Parametry:
    - general_vars: Ukazatel na strukturu General_vars.
   ____________________________________________________________________________
*/
void free_general_vars(General_vars *general_vars);

/*
   ____________________________________________________________________________

    int get_var_index()

    Vrací index proměnné v poli názvů proměnných.

    Parametry:
    - general_vars: Ukazatel na strukturu General_vars.
    - var_name: Název proměnné, jejíž index je hledán.

    Návratová hodnota:
    - Index proměnné při úspěšném nalezení.
    - -1, pokud proměnná není v seznamu nalezena nebo jsou neplatné vstupy.
   ____________________________________________________________________________
*/
int get_var_index(General_vars *general_vars, char *var_name);

#endif
