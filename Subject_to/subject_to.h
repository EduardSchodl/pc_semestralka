#ifndef __SUBJECTTO__
#define __SUBJECTTO__

#include "../LProblem/lp.h"

/*
   ____________________________________________________________________________

    int parse_subject_to()

    Zpracovává sekci "Subject To". Rozděluje omezení na části,
    zjednodušuje je, a plní simplexní tabulku.

    Parametry:
    - expressions: Pole ukazatelů na řetězce obsahující omezení.
    - num_of_constraints: Počet omezení v poli.
    - tableau: Ukazatel na strukturu simplexní tabulky.
    - general_vars: Ukazatel na strukturu obsahující informace o rozhodovacích proměnných.

    Návratová hodnota:
    - 0, pokud bylo zpracování úspěšné.
    - SYNTAX_ERROR, pokud je výraz nevalidní.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
    - Jiný kód chyby, pokud některá z fuknkcí selže.
   ____________________________________________________________________________
*/
int parse_subject_to(char **expression, int num_of_constraints, Simplex_Tableau *tableau, General_vars *general_vars);

/*
   ____________________________________________________________________________

    char* identify_delimiter()

    Identifikuje oddělovač ve výrazu pro typ omezení (např. <=, >=, <, >, =).

    Parametry:
    - expression: Ukazatel na řetězec obsahující vstupní výraz.

    Návratová hodnota:
    - Ukazatel na nalezený oddělovač, nebo NULL, pokud žádný oddělovač není nalezen.
   ____________________________________________________________________________
*/
char *identify_delimiter(const char *expression);

/*
   ____________________________________________________________________________

    void introduce_additional_vars()

    Přidává pomocné nebo umělé proměnné do simplexní tabulky na základě typu
    omezení (např. <=, >=, =).

    Parametry:
    - tableau: Ukazatel na strukturu simplexní tabulky.
    - delim: Ukazatel na řetězec obsahující oddělovač.
    - row: Index řádku simplexní tabulky.
    - col: Počáteční index sloupce pro přidání proměnných.
    - num_of_constraints: Celkový počet omezení.
   ____________________________________________________________________________
*/
void introduce_additional_vars(Simplex_Tableau *tableau, char *delim, int row, int col, int num_of_constraints);

/*
   ____________________________________________________________________________

    int split_expression()

    Rozděluje omezení na levou a pravou stranu na základě oddělovače.

    Parametry:
    - expression: Ukazatel na vstupní řetězec obsahující celé omezení.
    - name_pos: Ukazatel na pozici názvu omezení.
    - delim: Ukazatel na řetězec, kam bude uložen nalezený oddělovač.
    - left_side_expression: Ukazatel na levou stranu rozděleného výrazu.
    - right_side_expression: Ukazatel na pravou stranu rozděleného výrazu.

    Návratová hodnota:
    - 0, pokud bylo zpracování úspěšné.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
    - PARSING_ERROR, pokud není oddělovač nalezen.
   ____________________________________________________________________________
*/
int split_expression(char *expression, char *name_pos, char **delim, char **left_side_expression,
                     char **right_side_expression);

#endif
