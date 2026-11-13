# Istniejąca nagroda w oknie

P072 jest wyłącznie podziałem. Łańcuch: P071-03 -> P072 -> P072-01 ->
P072-02 -> P072-03 -> P072-04 -> P073. Dzieci nie są jeszcze zaimplementowane.

Oferta pochodzi z istniejących reguł core i zawiera wyłącznie nieposiadane
ModuleId w kanonicznym porządku. Zero, jeden lub dwa elementy to legalne
przypadki. Model kandydata niczego nie przyznaje; zmiana runu/sektora/oferty
usuwa stary wybór. Strzałki lewo/prawo wybierają kandydata, Enter zatwierdza.
Poza fazą nagrody model nie udostępnia wyboru. Nie dodawać ID ani waluty.

Wiersze dwóch modułów, instrukcja i ewentualny komunikat błędu pochodzą
z TextCatalog, każdy z własnym prefiksem `[AI PLACEHOLDER]`. Wskazanie można
opisać oznaczonym katalogowym tekstem i danymi liczbowymi; bez nowej grafiki,
kolorowych proceduralnych znaczników, ikon lub kopiowania opisów koncepcji
jako finalnych treści. Brak ID daje odmowę, nigdy fallback identyfikatora.

Widok korzysta z istniejącego składu wielu wierszy. Każdy marker ma być
kompletny; zbyt małe okno może pominąć blok zgodnie z TEXT_PRESENTATION.md.
W widoku oferty nie ma hit-testu pól ani wykonywania komend planszy.

Integracja wywołuje istniejące choose_run_module i advance_run. Wybór
może być opublikowany dopiero razem z poprawnym nowym stanem widoku.
Pełny zestaw modułów daje ready_for_next i przejście bez fikcyjnej nagrody.
Filtr hold/focus nadal chroni jednokrotne Enter. Przejście usuwa poprzedni
preview i wskazanie. Dwie granice sektora testuje się względem pełnych
wyników core; ostatni sektor pozostaje terminalnym wynikiem P071.

Zapis checkpointu z UI, evaluator achievementów i powiadomienia pozostają
poza tym łańcuchem. P073 i P074 zachowują własne kontrakty. Bez pobrań.
