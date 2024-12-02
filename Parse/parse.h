#ifndef __PARSE__
#define __PARSE__

#define INITIAL_SIZE 2

/*
   ____________________________________________________________________________

    typedef struct Term

    Struktura pro reprezentaci jednotlivého členu matematického výrazu.
    Obsahuje koeficient a proměnnou.

    Obsah:
    - coefficient: Hodnota koeficientu členu.
    - variable: Název proměnné spojené s členem.
                Maximální délka proměnné je 10 znaků.
   ____________________________________________________________________________
*/
typedef struct {
    double coefficient;
    char variable[10];
} Term;

/*
   ____________________________________________________________________________

    char* trim_white_space()

    Odstraňuje bílé znaky na začátku a na konci zadaného řetězce.

    Parametry:
    - str: Ukazatel na řetězec, který má být upraven.

    Návratová hodnota:
    - Ukazatel na řetězec bez bílých znaků na začátku a na konci.
   ____________________________________________________________________________
*/
char *trim_white_space(char *str);

/*
   ____________________________________________________________________________

    char* remove_spaces()

    Odstraňuje všechny mezery ze zadaného řetězce.

    Parametry:
    - str: Ukazatel na řetězec, který má být upraven.

    Návratová hodnota:
    - Ukazatel na řetězec bez mezer.
   ____________________________________________________________________________
*/
char *remove_spaces(char *str);

/*
   ____________________________________________________________________________

    int extract_variable_and_coefficient()

    Extrahuje proměnnou a její koeficient z daného výrazu.

    Parametry:
    - segment: Řetězec obsahující výraz s koeficientem a proměnnou.
    - variable: Ukazatel na řetězec, kam bude uložena proměnná.
    - coefficient: Ukazatel na hodnotu, kam bude uložen koeficient.

    Návratová hodnota:
    - 0, pokud je extrakce úspěšná.
    - 1, pokud je proměnná neplatná.
   ____________________________________________________________________________
*/
int extract_variable_and_coefficient(char *segment, char *variable, double *coefficient);

/*
   ____________________________________________________________________________

    double parse_coefficient()

    Analyzuje a vrací koeficient ze zadaného řetězce. Pokud není koeficient
    explicitně uveden, vrací výchozí hodnotu 1 nebo -1 na základě znaménka.

    Parametry:
    - token: Ukazatel na řetězec, který obsahuje koeficient.

    Návratová hodnota:
    - Hodnota koeficientu jako typ double.
   ____________________________________________________________________________
*/
double parse_coefficient(const char *token);

/*
   ____________________________________________________________________________

    void normalize_expression()

    Nahrazuje speciální znaky '{', '[' za '(' a '}', ']' za ')', aby bylo
    možné výrazy standardně zpracovat.

    Parametry:
    - expression: Ukazatel na řetězec, který má být normalizován.
   ____________________________________________________________________________
*/
void normalize_expression(char *expression);

/*
   ____________________________________________________________________________

    int simplify_expression()

    Zjednodušuje matematický výraz, rozpoznává závorky, koeficienty a proměnné.
    Vrací zjednodušený výraz.

    Parametry:
    - expression: Ukazatel na původní matematický výraz.
    - simplified_expression: Ukazatel na řetězec, kam bude uložen zjednodušený výraz.

    Návratová hodnota:
    - 0,  pokud bylo zpracování úspěšné.
    - 11, pokud jsou závorky nevyvážené.
    - 93, pokud jsou vstupní parametry neplatné.
   ____________________________________________________________________________
*/
int simplify_expression(const char *expression, char *simplified_expression);

/*
   ____________________________________________________________________________

    void add_term()

    Přidává výraz (koeficient a proměnnou) do seznamu výrazů. Pokud již proměnná
    existuje, aktualizuje její koeficient.

    Parametry:
    - terms: Pole výrazů.
    - term_count: Ukazatel na počet aktuálně zpracovaných výrazů.
    - coefficient: Koeficient výrazu.
    - variable: Název proměnné.
   ____________________________________________________________________________
*/
void add_term(Term terms[], int *term_count, double coefficient, const char *variable);

/*
   ____________________________________________________________________________

    int check_matching_parentheses()

    Kontroluje, zda počty otevíracích a uzavíracích závorek ve výrazu odpovídají.

    Parametry:
    - expression: Ukazatel na řetězec s výrazem.

    Návratová hodnota:
    - 0, pokud závorky odpovídají.
    - 1, pokud jsou závorky nevyvážené.
   ____________________________________________________________________________
*/
int check_matching_parentheses(const char *expression);

/*
   ____________________________________________________________________________

    void process_term()

    Zpracovává aktuální výraz a přidává jej do seznamu výrazů.

    Parametry:
    - terms: Pole výrazů.
    - term_count: Ukazatel na počet aktuálně zpracovaných výrazů.
    - coefficient: Koeficient výrazu.
    - sign: Znaménko koeficientu.
    - variable: Název proměnné.
   ____________________________________________________________________________
*/
void process_term(Term terms[], int *term_count, double coefficient, int sign, char *variable);

/*
   ____________________________________________________________________________

    void modify_expression()

    Modifikuje matematický výraz, aby byl ve správném formátu pro další zpracování.
    Například přidává '+' operátory tam, kde je potřeba.

    Parametry:
    - expression: Ukazatel na vstupní řetězec obsahující matematický výraz.
    - modified_expression: Ukazatel na výstupní řetězec pro modifikovaný výraz.
   ____________________________________________________________________________
*/
void modify_expression(char *expression, char *modified_expression);

#endif
