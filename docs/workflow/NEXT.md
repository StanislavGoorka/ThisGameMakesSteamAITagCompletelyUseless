# Następny mały krok

Task: P019-02

## Cel

Dodaj jeden wybór następnego pola pościgu dla wskazanego impulsu, bez mutacji stanu. P019-01 jest zaimplementowane; P019 pozostaje ukończonym podziałem planu. Nie zaczynaj ruchu jednostek P019-03 ani kontaktu P020.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P019-01](history/P019-01.md), [raport podziału P019](history/P019.md), wpis P019-02 i jego zależność w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [predykat zajęcia](../../src/core/pulse_occupancy.hpp), [BFS](../../src/core/grid_pathfinding.hpp), [testy BFS](../../tests/unit/grid_pathfinding_tests.cpp), [testy zajęcia](../../tests/unit/pulse_occupancy_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`GameState::pulses` to `std::vector<Pulse>`; impuls ma `EntityId id` i `NodeId position`. Poprawny stan ma unikalne ID i najwyżej jeden impuls na polu. Kolekcja jest wartościowa i początkowo pusta. `pulse_occupies(const GameState&, NodeId)` odczytuje zajęcie wyłącznie z pozycji jednostek; nie zależy od kolejności kolekcji. Impulsy nie są wpisywane do `blocked_nodes`. Gracz może wejść na impuls i współdzielić z nim pole, a pakiet również nie jest blokowany. Kontakt jeszcze nie zmienia integralności i nie usuwa impulsów.

`shortest_route` wyznacza trasę na podstawie portów, z remisami N, E, S, W. Nie uwzględnia jednostek blokujących. Istniejące funkcje pakietu pozostają osobnymi krokami. Ostatni wynik po P019-01 to 16/16 CTest w Debug i Release; mutacja predykatu zajęcia została wykryta i wycofana.

## Wykonaj

Jedna funkcja tylko do odczytu: dla `const GameState&` i `EntityId` zwróć następny `NodeId` impulsu albo brak ruchu. Nazwę wybiera wykonawca.

- Znajdź impuls po ID, niezależnie od kolejności kolekcji. Brak wskazanego impulsu daje pusty wynik.
- Użyj istniejącego BFS do bieżącej pozycji gracza. Zwróć dokładnie pierwszą krawędź najkrótszej trasy, zgodnie z kolejnością remisów N, E, S, W. Bez trasy albo gdy impuls już stoi na graczu, zwróć brak ruchu.
- Po wyborze pierwszego kroku sprawdź `pulse_occupies`. Gdy cel jest zajęty przez inny impuls, czekaj. Nie szukaj objazdu, również gdy istnieje dłuższa wolna trasa. Nie zmieniaj BFS ani znaczenia `blocked_nodes`.
- Obliczaj z bieżącej topologii i pozycji gracza; nie zapisuj zamiaru w stanie. Funkcja nie przesuwa jednostek, nie zmienia kolekcji, integralności ani licznika tur.

## Dowód sukcesu i test błędu

Testuj pojedynczy krok zamiast skoku do gracza, remisy kierunków, pustą kolekcję i brak wskazanego ID, brak trasy, impuls na graczu i sąsiada będącego graczem. Sprawdź zmianę wyniku po zmianie topologii oraz pozycji gracza. Pole wybranego kroku zajęte przez inny impuls oznacza czekanie mimo dostępnego objazdu; po usunięciu tej jednostki krok staje się dostępny. Kolejność przechowywania impulsów nie zmienia wyboru. Każde wywołanie ma zachować pełny stan, łącznie z wszystkimi polami i turą.

Dodaj nowy test do trzech miejsc CMake zgodnie z poprzednimi testami. Celowa mutacja produkcyjna ma potwierdzić istotny test negatywny, np. pominięcie zajęcia wybranego pola. Wymuś `/t:Rebuild` odpowiedniego celu i sprawdź w logu rekompilację. Przywróć kod przed końcowymi testami Debug/Release.

## Nie rozszerzaj

Nie implementuj ruchu wielu impulsów, sortowania fazy zagrożeń, kontaktu, obrażeń, usuwania jednostek ani emisji. Nie dodawaj resolvera, generatora, wyjścia, rezerwy, długu, zapisu ani prezentacji. Nie generuj assetów ani treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatowanie według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z faktycznym wynikiem. Zapisz `history/P019-02.md`, a STATE i NEXT przestaw na P019-03 dopiero po spełnieniu kryteriów. Uruchom walidator i `git diff --check`, wykonaj lokalny commit własnych zmian, sprawdź hash i zakres. Zatrzymaj się przed P019-03.
