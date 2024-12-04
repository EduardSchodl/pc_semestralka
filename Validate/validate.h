#ifndef __VALIDATE__
#define __VALIDATE__

#define MAX_LINE_LENGTH 256
#define MAX_VARIABLES 100
#define MAX_SECTIONS 6
#define MAX_VAR_NAME 50

#include "../Generals/generals.h"

/*
   ____________________________________________________________________________

    int is_var_known()

    Kontroluje, zda je proměnná uvedena v seznamu rozhodujících proměnných.

    Parametry:
    - general_vars: Struktura obsahující seznam rozhodujících proměnných.
    - var_name: Název proměnné, která má být ověřena.

    Návratová hodnota:
    - 0,  pokud je proměnná známá.
    - 10, pokud proměnná není v seznamu.
    - 93, pokud vstupní parametry jsou neplatné.
   ____________________________________________________________________________
*/
int is_var_known(const General_vars *general_vars, const char *var_name);

/*
   ____________________________________________________________________________

    int is_valid_string()

    Kontroluje, zda daný řetězec je platný jako název proměnné.

    Parametry:
    - str: Řetězec, který má být ověřen.

    Návratová hodnota:
    - 0, pokud je řetězec platný.
    - 1, pokud obsahuje neplatné znaky nebo nezačíná písmenem.
   ____________________________________________________________________________
*/
int is_valid_string(const char *str);

/*
   ____________________________________________________________________________

    int contains_only_valid_operators()

    Kontroluje, zda řetězec obsahuje pouze platné operátory.

    Parametry:
    - str: Řetězec, který má být ověřen.

    Návratová hodnota:
    - 0, pokud obsahuje pouze platné operátory.
    - 1, pokud obsahuje neplatné znaky.
   ____________________________________________________________________________
*/
int contains_only_valid_operators(const char *str);

/*
   ____________________________________________________________________________

    int is_valid_operator()

    Kontroluje, zda je řetězec platným operátorem (např. "<", "<=", ">", ">=").

    Parametry:
    - str: Řetězec, který má být ověřen.

    Návratová hodnota:
    - 0, pokud je řetězec platným operátorem.
    - 1, jinak.
   ____________________________________________________________________________
*/
int is_valid_operator(const char *str);

/*
   ____________________________________________________________________________

    int contains_invalid_operator_sequence()

    Kontroluje, zda řetězec obsahuje neplatnou kombinaci operátorů a klíčových slov.

    Parametry:
    - str: Řetězec, který má být ověřen.

    Návratová hodnota:
    - 0, pokud neobsahuje neplatné kombinace.
    - 1, pokud obsahuje neplatné kombinace.
   ____________________________________________________________________________
*/
int contains_invalid_operator_sequence(char *str);

/*
   ____________________________________________________________________________

    int bounds_valid_operators()

    Kontroluje, zda řetězec obsahuje pouze platné operátory pro sekci Bounds.

    Parametry:
    - str: Řetězec, který má být ověřen.

    Návratová hodnota:
    - 0, pokud obsahuje pouze platné operátory.
    - 1, pokud obsahuje neplatné znaky.
   ____________________________________________________________________________
*/
int bounds_valid_operators(const char *str);

/*
   ____________________________________________________________________________

    int check_unused_variables()

    Kontroluje, zda jsou všechny proměnné ze seznamu rozhodujících proměnných použity.

    Parametry:
    - general_vars: Struktura obsahující seznam rozhodujících proměnných.

    Návratová hodnota:
    - 0, pokud jsou všechny proměnné použity.
    - 1, pokud některá proměnná nebyla použita.
   ____________________________________________________________________________
*/
int check_unused_variables(General_vars *general_vars);

/*
   ____________________________________________________________________________

    int check_invalid_chars()

    Kontroluje, zda řetězec obsahuje nepovolené znaky.

    Parametry:
    - line: Řetězec, který má být ověřen.
    - invalid_chars: Znaky, které jsou nepovolené.

    Návratová hodnota:
    - 0, pokud řetězec neobsahuje nepovolené znaky.
    - 1, pokud řetězec obsahuje nepovolené znaky.
   ____________________________________________________________________________
*/
int check_invalid_chars(char *line, char *invalid_chars);


int is_operator(char c);
int is_var_start(char c);
int is_var_part(char c);
int validate_expression(const char *expression);

#endif
