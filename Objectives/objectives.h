#ifndef __OBJECTIVES__
#define __OBJECTIVES__

#include "../LProblem/lp.h"

/*
   ____________________________________________________________________________

    int parse_objectives()

    Zpracovává sekci účelové funkce zadanou v LP souboru. Účelová funkce je
    normalizována, zjednodušena a jednotlivé proměnné jsou extrahovány spolu
    s jejich koeficienty. Koeficienty se ukládají do pole objective_row.

    Parametry:
    - expressions: Pole řetězců obsahující řádky ze sekce účelové funkce.
    - tableau: Ukazatel na simplexní tabulku, kde bude uložen typ optimalizace.
    - general_vars: Ukazatel na strukturu obsahující rozhodovací proměnné.
    - objective_row: Pole pro uložení koeficientů účelové funkce.
    - num_lines: Počet řádků v sekci účelové funkce.

    Návratová hodnota:
    - 0   při úspěchu.
    - 10, pokud je nalezena neznámá proměnná.
    - 93  při chybě alokace paměti nebo nevalidních vstupech.
    - Jiný kód chyby, pokud některá funkce v procesu selže.
   ____________________________________________________________________________
*/
int parse_objectives(char **expressions, SimplexTableau *tableau, General_vars *general_vars, double objective_row[], int num_lines);

#endif
