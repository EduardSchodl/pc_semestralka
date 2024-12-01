#ifndef __SUBJECTTO__
#define __SUBJECTTO__

#include "../LProblem/lp.h"

/*
   ____________________________________________________________________________

    int parse_subject_to()

    Zpracovává sekci "Subject To" z LP souboru a plní simplexovou tabulku koeficienty
    a pravými stranami rovnic nebo nerovnic.

    Parametry:
    - char **expressions: Pole řetězců, které obsahuje výrazy ze sekce "Subject To".
    - int num_of_constraints: Počet omezujících podmínek ve vstupních datech.
    - SimplexTableau *tableau: Ukazatel na datovou strukturu SimplexTableau, která bude naplněna daty
                               z parsovaných výrazů.
    - General_vars *general_vars: Ukazatel na datovou strukturu s informacemi o rozhodovacích proměnných.

    Návratová hodnota:
    - 0: V případě úspěšného zpracování sekce "Subject To".
    - 10: Pokud je nalezena neznámá proměnná.
    - 93: V případě chyby, jako je nevalidní vstup nebo chyba při parsování.

    Funkce:
    - Normalizuje výrazy a ověřuje jejich syntaxi.
    - Rozděluje výrazy na levou a pravou stranu pomocí delimitérů ("<=", ">=", "<", ">", "=").
    - Extrahuje proměnné a jejich koeficienty z levé strany a naplňuje odpovídající
      buňky v SimplexTableau.
    - Zajišťuje správné zpracování nerovností přidáním pomocných/slack proměnných.
    - Upravuje tabulku pro rovnice a nerovnice podle potřeb simplexové metody.
   ____________________________________________________________________________
*/
int parse_subject_to(char **expression, int num_of_constraints, SimplexTableau *tableau, General_vars *general_vars);

char* identify_delimiter(const char* expression);

#endif
