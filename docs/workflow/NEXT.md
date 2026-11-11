# Następny mały krok

Task: P019-03

## Cel

Dodaj jeden krok przesuwający impulsy w rosnącej kolejności `EntityId`, na bieżącym stanie po wcześniejszych ruchach. P019-01 i P019-02 są zaimplementowane; P019 jest ukończonym podziałem planu. Nie zaczynaj kontaktu i obrażeń P020.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P019-02](history/P019-02.md), [raport podziału P019](history/P019.md), wpis P019-03 i jego zależność w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [wybór kroku](../../src/core/pulse_chase.hpp), [predykat zajęcia](../../src/core/pulse_occupancy.hpp), [BFS](../../src/core/grid_pathfinding.hpp), [testy wyboru](../../tests/unit/pulse_chase_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`GameState::pulses` to wartościowy `std::vector<Pulse>`; impuls ma `EntityId id` i `NodeId position`. Poprawny stan ma unikalne ID i najwyżej jeden impuls na polu. Kolejność przechowywania nie określa kolejności działania. Impulsy nie są wpisywane do `blocked_nodes`; gracz i pakiet mogą współdzielić ich pole.

`next_pulse_step(const GameState&, EntityId)` zwraca jeden krok istniejącego BFS do bieżącej pozycji gracza albo pusty wynik. Uwzględnia aktualne porty i zajęcie pierwszego kroku przez impuls, z remisami N, E, S, W. Brak wskazanego ID, brak trasy i impuls już na graczu oznaczają czekanie. Zajęty pierwszy krok oznacza czekanie bez objazdu, nawet gdy istnieje dłuższa wolna trasa. Funkcja nie mutuje stanu i nie nalicza tury. Kontakt jeszcze nie zmienia integralności ani nie usuwa impulsów. Ostatni wynik P019-02 to 17/17 CTest w Debug i Release; test wykrył usunięcie kontroli zajęcia, a mutację wycofano.

## Wykonaj

Jeden kontrakt: przesuń każdy impuls najwyżej raz, w rosnącej kolejności `EntityId`, zachowując kolejność przechowywania kolekcji.

- Użyj `next_pulse_step` dla każdej jednostki dopiero w jej kolejce. Wybór musi czytać stan po wszystkich wcześniejszych ruchach; nie obliczaj całej listy zamiarów z początkowego obrazu zajęcia.
- Niższe ID może zwolnić pole wyższemu. Jednostka, która czekała z powodu blokady, nie ponawia próby po późniejszym zwolnieniu pola. Przy wspólnym celu pierwszeństwo ma niższe ID.
- Pusta kolekcja i brak możliwych ruchów zachowują cały stan. Zmieniaj wyłącznie pozycje impulsów, bez zmiany ich ID, kolejności, pozostałych pól stanu lub licznika tur. Nazwę funkcji i ewentualny wynik wybiera wykonawca.
- Nie usuwaj impulsów i nie zmieniaj integralności, nawet po dojściu na pole gracza. Kontakt i obrażenia należą do P020.

## Dowód sukcesu i test błędu

Testuj jeden ruch najwyżej o krawędź, pustą kolekcję, brak trasy oraz pełny stan przy czekaniu. Dla kilku permutacji tego samego zestawu jednostek oczekiwane pozycje według ID muszą być takie same, a kolejność kolekcji zachowana. Osobne scenariusze mają wykazać: niższe ID zwalnia pole wyższemu; niższe ID czeka, po czym wyższe zwalnia pole i nie powoduje powtórnej próby; wspólny cel przyznawany niższemu ID; zajęcie nie prowadzi do objazdu. Kontakt nie usuwa impulsu, nie zmienia integralności ani tury. Porównuj pełny stan z oczekiwanym, z niepustym pakietem i pozostałymi polami.

Dodaj nowy test w trzech miejscach CMake. Celowa mutacja produkcyjna ma potwierdzić istotny test negatywny, np. odwrócenie kolejności ID lub użycie kolejności kolekcji. Wymuś `/t:Rebuild` odpowiedniego celu i potwierdź rekompilację w logu. Przywróć kod i przebuduj cel przed końcowymi testami Debug/Release.

## Nie rozszerzaj

Nie implementuj kontaktu, obrażeń, usuwania jednostek, emisji, innych zagrożeń, resolvera, generatora, wyjścia, rezerwy, długu, zapisu ani prezentacji. Nie zmieniaj BFS i kroków pakietu. Nie generuj assetów ani treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatowanie według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z wynikiem. Zapisz `history/P019-03.md`, a STATE i NEXT przestaw na P020 dopiero po spełnieniu kryteriów. Następca ma ocenić rozmiar P020 przed implementacją zgodnie z workflow. Uruchom walidator i `git diff --check`, wykonaj lokalny commit własnych zmian, sprawdź hash i zakres. Zatrzymaj się przed P020.
