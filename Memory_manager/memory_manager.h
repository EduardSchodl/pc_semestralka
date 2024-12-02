#ifndef __MEMORY_MANAGER__
#define __MEMORY_MANAGER__

/*
   ____________________________________________________________________________

    void *tracked_malloc(size_t size)

    Alokuje blok paměti o zadané velikosti a zvýší počítadlo celkového počtu
    alokovaných ukazatelů.

    Parametry:
    - size: Velikost alokovaného bloku paměti.

    Návratová hodnota:
    - Ukazatel na alokovaný paměťový blok nebo NULL, pokud alokace selže.
   ____________________________________________________________________________
*/
void *tracked_malloc(size_t size);

/*
   ____________________________________________________________________________

    void *tracked_realloc(void *ptr, size_t size)

    Realokuje blok paměti na novou velikost. Pokud je původní ukazatel NULL,
    chová se stejně jako tracked_malloc. Pokud realokace selže, původní
    ukazatel zůstane nezměněn. Zvyšuje počítadlo celkového počtu alokovaných ukazatelů.

    Parametry:
    - ptr: Ukazatel na dříve alokovaný paměťový blok.
    - size: Nová velikost paměťového bloku.

    Návratová hodnota:
    - Ukazatel na realokovaný paměťový blok nebo NULL, pokud realokace selže.
   ____________________________________________________________________________
*/
void *tracked_realloc(void *ptr, size_t size);

/*
   ____________________________________________________________________________

    void *tracked_calloc(size_t nmemb, size_t size)

    Alokuje paměť pro pole prvků, kde každý prvek inicializuje na 0.
    Zvyšuje počítadlo celkového počtu alokovaných ukazatelů.

    Parametry:
    - nmemb: Počet alokovaných prvků.
    - size: Velikost každého prvku.

    Návratová hodnota:
    - Ukazatel na alokovaný paměťový blok nebo NULL, pokud alokace selže.
   ____________________________________________________________________________
*/
void *tracked_calloc(size_t nmemb, size_t size);

/*
   ____________________________________________________________________________

    void tracked_free(void *ptr)

    Uvolní dříve alokovaný blok paměti a sníží počítadlo celkového počtu
    alokovaných ukazatelů.

    Parametry:
    - ptr: Ukazatel na paměťový blok, který má být uvolněn. Pokud je NULL,
           žádná akce se neprovede.
   ____________________________________________________________________________
*/
void tracked_free(void *ptr);

/*
   ____________________________________________________________________________

    void report_memory_usage()

    Vypíše zprávu o aktuálním využití paměti, včetně počtu zbývajících
    alokací. Upozorní na jakékoli zjištěné úniky paměti.
   ____________________________________________________________________________
*/
void report_memory_usage();

#endif
