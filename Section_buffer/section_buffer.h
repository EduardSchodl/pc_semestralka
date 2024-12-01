#ifndef __SECTION_BUFFER__
#define __SECTION_BUFFER__

/*
   ____________________________________________________________________________

    typedef struct SectionBuffers

    Datová struktura, která slouží k uchovávání obsahu jednotlivých sekcí
    vstupního souboru. Obsahuje samostatné buffery pro různé sekce
    a odpovídající počty řádků.

    Obsah:
    - char **general_lines: Pole řetězců obsahující řádky sekce "Generals".
    - int general_count: Počet řádků v sekci "Generals".
    - char **subject_to_lines: Pole řetězců obsahující řádky sekce "Subject To".
    - int subject_to_count: Počet řádků v sekci "Subject To".
    - char **objective_lines: Pole řetězců obsahující řádky sekce "Objective".
    - int objective_count: Počet řádků v sekci "Objective".
    - char **bounds_lines: Pole řetězců obsahující řádky sekce "Bounds".
    - int bounds_count: Počet řádků v sekci "Bounds".
   ____________________________________________________________________________
*/
typedef struct {
    char **general_lines;
    int general_count;
    char **subject_to_lines;
    int subject_to_count;
    char **objective_lines;
    int objective_count;
    char **bounds_lines;
    int bounds_count;
} SectionBuffers;

/*
   ____________________________________________________________________________

    SectionBuffers* create_section_buffers()

    Vytváří a inicializuje strukturu SectionBuffers, která uchovává oddělené
    části vstupního souboru pro jednotlivé sekce.

    Parametry:
    - initial_size: Počáteční velikost alokace pro každý buffer.

    Návratová hodnota:
    - Ukazatel na nově vytvořenou strukturu SectionBuffers.
    - NULL, pokud došlo k chybě při alokaci paměti.
   ____________________________________________________________________________
*/
SectionBuffers* create_section_buffers(int initial_size);

/*
   ____________________________________________________________________________

    void free_section_buffers()

    Uvolňuje paměť alokovanou pro strukturu SectionBuffers a všechny její části.

    Parametry:
    - buffers: Ukazatel na strukturu SectionBuffers, která má být uvolněna.
   ____________________________________________________________________________
*/
void free_section_buffers(SectionBuffers *buffers);

/*
   ____________________________________________________________________________

    void add_line_to_buffer()

    Přidává nový řádek do bufferu příslušné sekce. Pokud je potřeba, buffer se
    dynamicky zvětší.

    Parametry:
    - buffer: Ukazatel na pole řetězců reprezentující buffer sekce.
    - count: Ukazatel na počet aktuálně uložených řádků v bufferu.
    - line: Řetězec, který má být přidán do bufferu.
   ____________________________________________________________________________
*/
void add_line_to_buffer(char ***buffer, int *count, char *line);

#endif
