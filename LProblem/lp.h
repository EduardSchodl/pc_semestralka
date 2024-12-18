#ifndef __LP__
#define __LP__
#include "../Generals/generals.h"
#include "../Bounds/bounds.h"

/*
   ____________________________________________________________________________

    typedef struct Simplex_Tableau

    Struktura, která reprezentuje simplexovou tabulku používanou při řešení
    problémů lineárního programování.

    Obsah:
    - type: Typ optimalizačního problému ("Maximize" nebo "Minimize").
    - tableau: Dvourozměrné pole (matice) obsahující simplexovou tabulku.
      - Řádky odpovídají omezením (včetně cílové funkce).
      - Sloupce odpovídají proměnným, včetně sloupce pravých stran.
    - col_count: Počet sloupců v simplexové tabulce.
    - row_count: Počet řádků v simplexové tabulce.
   ____________________________________________________________________________
*/
typedef struct {
    char *type;
    double **tableau;
    int col_count;
    int row_count;
    int *basic_vars;
} Simplex_Tableau;

/*
   ____________________________________________________________________________

    int simplex()

    Implementuje dvoufázový simplexní algoritmus. Na základě účelové funkce
    a omezení řeší lineární optimalizační problém.

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - objective_row: Pole obsahující koeficienty účelové funkce.
    - general_vars: Ukazatel na strukturu obsahující seznam rozhodovacích
                    proměnných.
    - bounds: Ukazatel na strukturu obsahující meze proměnných.

    Návratová hodnota:
    - 0 při úspěchu.
    - NO_FEASIBLE_SOLUTION, pokud není nalezeno přípustné řešení (Phase One selhalo).
    - FUNCTION_UNBOUNDED_ERROR, pokud je účelová funkce neomezená.
    - SOLUTION_OUT_OF_BOUNDS, pokud optimální řešení nesplňuje zadané meze.
    - RUNTIME_ERROR při chybě v průběhu vykonávání.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
   ____________________________________________________________________________
*/
int simplex(Simplex_Tableau *tableau, double objective_row[], General_vars *general_vars, Bounds *bounds,
            double *solution);

/*
   ____________________________________________________________________________

    int find_pivot_col()

    Najde pivotní sloupec na základě nejvíce záporného nebo nejvíce kladného
    prvku v řádku účelové funkce (dle typu optimalizace).

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - minimization: Příznak, zda jde o minimalizační úlohu (1 = minimalizace,
      0 = maximalizace).

    Návratová hodnota:
    - Index pivotního sloupce při úspěchu.
    - -1, pokud pivotní sloupec nelze najít.
   ____________________________________________________________________________
*/
int find_pivot_col(const Simplex_Tableau *tableau, int minimization);

/*
   ____________________________________________________________________________

    int find_pivot_row()

    Najde pivotní řádek pro zadaný pivotní sloupec. Využívá pravidlo
    minimálního podílu.

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - col_index: Index pivotního sloupce.

    Návratová hodnota:
    - Index pivotního řádku při úspěchu.
    - -1, pokud pivotní řádek nelze najít.
   ____________________________________________________________________________
*/
int find_pivot_row(const Simplex_Tableau *tableau, const int col_index);

/*
   ____________________________________________________________________________

    Simplex_Tableau *create_simplex_tableau()

    Alokuje a inicializuje simplexní tabulku na základě počtu omezení
    a proměnných.

    Parametry:
    - num_constraints: Počet omezení.
    - num_variables: Počet rozhodovacích proměnných.

    Návratová hodnota:
    - Ukazatel na nově vytvořenou simplexní tabulku.
    - NULL při chybě alokace paměti.
   ____________________________________________________________________________
*/
Simplex_Tableau *create_simplex_tableau(int num_constraints, int num_variables);

/*
   ____________________________________________________________________________

    int check_solution_bounds()

    Kontroluje, zda nalezené řešení splňuje meze zadané ve struktuře Bounds.

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - general_vars: Ukazatel na strukturu obsahující názvy proměnných.
    - bounds: Ukazatel na strukturu obsahující dolní a horní meze.

    Návratová hodnota:
    - 0  při splnění všech mezí.
    - 1, při chybě.
   ____________________________________________________________________________
*/
int check_solution_bounds(Simplex_Tableau *tableau, General_vars *general_vars, Bounds *bounds);

/*
   ____________________________________________________________________________

    void print_solution()

    Vypisuje optimální hodnoty proměnných.

    Parametry:
    - general_vars: Ukazatel na strukturu obsahující názvy proměnných.
    - solution: Pole řešení lineárního problému.
   ____________________________________________________________________________
*/
void print_solution(General_vars *general_vars, double *solution);

/*
   ____________________________________________________________________________

    void free_simplex_tableau()

    Uvolňuje paměť alokovanou pro simplexní tabulku.

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
   ____________________________________________________________________________
*/
void free_simplex_tableau(Simplex_Tableau *tableau);

/*
   ____________________________________________________________________________

    void print_tableau()

    Vypisuje simplexní tabulku v aktuálním stavu.

    Parametry:
    - simplex_tableau: Ukazatel na simplexní tabulku.
   ____________________________________________________________________________
*/
void print_tableau(Simplex_Tableau *simplex_tableau);

/*
   ____________________________________________________________________________

    int simplex_phase_one()

    Implementuje první fázi simplexního algoritmu (hledání přípustného
    řešení pomocí umělých proměnných).

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.

    Návratová hodnota:
    - 0 při úspěšném nalezení přípustného řešení.
    - 1, pokud nelze nalézt přípustné řešení nebo je úloha neomezená.
   ____________________________________________________________________________
*/
int simplex_phase_one(Simplex_Tableau *tableau);

/*
   ____________________________________________________________________________

    int simplex_phase_two()

    Implementuje druhou fázi simplexního algoritmu (optimalizace účelové
    funkce na základě přípustného řešení).

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - num_general_vars: Počet rozhodovacích proměnných.

    Návratová hodnota:
    - 0 při úspěšné optimalizaci.
    - 1, pokud je účelová funkce neomezená.
   ____________________________________________________________________________
*/
int simplex_phase_two(Simplex_Tableau *tableau, int num_general_vars);

/*
   ____________________________________________________________________________

    int remove_artificial_variables()

    Odstraňuje umělé proměnné ze simplexní tabulky po dokončení Phase One.

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - num_artificial_vars: Počet umělých proměnných.

    Návratová hodnota:
    - 0 při úspěchu.
    - 1 při chybě.
   ____________________________________________________________________________
*/
int remove_artificial_variables(Simplex_Tableau *tableau, int num_artificial_vars);

/*
   ____________________________________________________________________________

    int has_nonzero_in_objective_row()

    Kontroluje, zda řádek účelové funkce obsahuje nenulové hodnoty pro
    rozhodovací proměnné.

    Parametry:
    - tableau: Ukazatel na simplexní tabulku.
    - num_general_vars: Počet rozhodovacích proměnných.

    Návratová hodnota:
    - 1, pokud existují nenulové hodnoty.
    - 0, pokud jsou všechny hodnoty nulové.
   ____________________________________________________________________________
*/
int has_nonzero_in_objective_row(Simplex_Tableau *tableau, int num_general_vars);

/*
   ____________________________________________________________________________

    double my_fabs()

    Implementace absolutní hodnoty pro hodnoty typu double.

    Parametry:
    - x: Hodnota, jejíž absolutní hodnota má být vrácena.

    Návratová hodnota:
    - Absolutní hodnota x.
   ____________________________________________________________________________
*/
double my_fabs(double x);

/*
   ____________________________________________________________________________

    int insert_constraints_into_row()

    Zpracovává modifikovaný matematický výraz a ukládá koeficienty proměnných do
    pole.

    Parametry:
    - expression: Ukazatel na řetězec obsahující modifikovaný matematický výraz.
    - general_vars: Ukazatel na strukturu obsahující informace o rozhodovacích proměnných.
    - arr: Ukazatel na pole, kam se má zpracovaný výraz uložit.

    Návratová hodnota:
    - 0, pokud bylo zpracování úspěšné.
    - UNKNOWN_VARIABLE, pokud je proměnná neznámá.
    - SANITY_CHECK_ERROR při nevalidních vstupech.
   ____________________________________________________________________________
*/
int insert_constraints_into_row(char *expression, General_vars *general_vars, double *arr);

/*
   ____________________________________________________________________________

    void extract_solution()

    Provede konečnou Gaussovu eliminaci a extrahuje optimální řešení ze simplexové
    tabulky a ukládá jej do zadaného pole.

    Parametry:
    - tableau: Ukazatel na strukturu Simplex_Tableau obsahující simplexovou tabulku.
    - general_vars: Ukazatel na strukturu General_vars obsahující informace o proměnných.
    - solution: Ukazatel na pole, kam se uloží hodnoty optimálního řešení.
   ____________________________________________________________________________
*/
void extract_solution(Simplex_Tableau *tableau, const General_vars *general_vars, double *solution);

/*
   ____________________________________________________________________________

    void pivot_elimination()

    Provádí pivotaci na simplexové tabulce pomocí Gaussovy eliminace.
    Pivotní prvek je vybrán na základě přijatých parametrů.

    Parametry:
    - tableau: Ukazatel na strukturu Simplex_Tableau, která obsahuje aktuální tabulku.
    - pivot_row: Index řádku, který obsahuje pivotní prvek.
    - pivot_col: Index sloupce, který obsahuje pivotní prvek.
   ____________________________________________________________________________
*/
void perform_pivoting(Simplex_Tableau *tableau, int pivot_row, int pivot_col);

/*
   ____________________________________________________________________________

    int is_basic_variable()

    Kontroluje, zda daný sloupec v simplexové tabulce odpovídá základní proměnné.
    Sloupec odpovídá základní proměnné, pokud obsahuje přesně jednu hodnotu 1
    a ostatní hodnoty jsou 0.

    Parametry:
    - tableau: Ukazatel na simplexovou tabulku.
    - col_index: Index sloupce, který má být zkontrolován.

    Návratová hodnota:
    - 1, pokud sloupec odpovídá základní proměnné.
    - 0, pokud sloupec neodpovídá základní proměnné.
   ____________________________________________________________________________
*/
int is_basic_variable(const Simplex_Tableau *tableau, int col_index);

#endif
