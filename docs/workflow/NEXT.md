# Następny mały krok

Task: P021-01

## Cel

Dodaj wartościową reprezentację podstawowych komend i jeden niemutujący predykat legalności. P021 jest ukończonym podziałem planu, a nie działającym resolverem. Zależność P020-02 jest zaimplementowana. Nie rozpoczynaj transakcji P021-02.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P021](history/P021.md), [raport P020-02](history/P020-02.md), wpisy P021, P021-01 i P021-02 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [ruch gracza](../../src/core/player_movement.hpp), [obrót gracza](../../src/core/player_rotation.hpp), [wait](../../src/core/player_wait.hpp), [naliczanie tury](../../src/core/turn_cost.hpp), [testy ruchu](../../tests/unit/player_movement_tests.cpp), [testy obrotu](../../tests/unit/player_rotation_tests.cpp), [testy wait](../../tests/unit/player_wait_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcje „Akcje” i „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) oraz [TESTING](../TESTING.md).

## Stan zastany

Nie ma `Command`, `TurnResult` ani resolvera. `move_player` i `rotate_from_player` wykonują samą akcję, bez kontroli życia i bez naliczania tury. Ruch wymaga wzajemnej krawędzi i braku osobnej flagi `blocked_nodes`; impuls nie blokuje gracza. Obrót wymaga własnego pola lub ortogonalnego sąsiada oraz obrotowego celu, bez wymagania połączenia i bez wpływu `blocked_nodes`. `wait_player` sprawdza życie i sam nalicza jedną turę przez `charge_turn`.

`GameState` ma wartościową kolekcję impulsów i wszystkie pola pakietu. `resolve_pulse_phase` jest gotową fazą kontaktów i ruchu, ale predykat legalności jej nie wywołuje. RNG jest osobnym `SplitMix64` poza stanem gry; helpery akcji go nie przyjmują. Ostatnie 20/20 CTest Debug/Release pochodzi z P020-02. Podział P021 nie zmienił kodu ani nie uruchamiał testów gry.

## Wykonaj

Jeden kontrakt: komenda opisuje zamiar, a predykat odczytuje jego legalność bez wykonania na wejściowym stanie.

- Reprezentacja obejmuje dokładnie `move`, `rotate` i `wait`. Cele ruchu/obrotu przechowuj jako silny `NodeId`; wait nie potrzebuje celu. Preferuj prostą wartościową reprezentację rozróżniającą te trzy warianty, bez stringów, surowych wskaźników i przypadkowego domyślnego celu. Nazwy typów, pliku i predykatu wybiera wykonawca.
- Predykat przyjmuje `const GameState&` oraz komendę i zwraca `bool`. Każda komenda wymaga dodatniego HP, także ruch i obrót, mimo że ich dotychczasowe helpery nie sprawdzają życia.
- Warunki ruchu i obrotu zachowaj zgodnie z istniejącymi helperami. Legalność wait zależy tylko od życia. Nie zmieniaj kontraktów tych funkcji. Możesz wykorzystać istniejącą walidację na niezależnej roboczej kopii, jeśli to najprostszy sposób uniknięcia rozbieżnych reguł; wejściowy stan i jego tura muszą pozostać nietknięte.
- Nie wprowadzaj pakietu, nie ruszaj jednostek na wejściowym stanie, nie obsługuj dostawy/kontaktu i nie naliczaj tury. Nie dodawaj pól `GameState` ani RNG do API.

## Dowód sukcesu i test błędu

Testy mają sprawdzić legalny ruch po wzajemnej krawędzi, brak krawędzi (także jednostronnej), brak zawijania na brzegu, osobną blokującą flagę oraz legalne wejście na impuls. Obrót: własne pole i ortogonalny obrotowy sąsiad, odrzucenie przekątnej, odległego lub nieruchomego pola; brak portów i `blocked_nodes` nie blokują legalnego obrotu. Wait żywego gracza jest legalny. Wszystkie trzy komendy przy HP=0 są nielegalne.

Dla każdego odczytu porównuj pełny stan przed i po, z niezerową turą, niepustą kolekcją impulsów, pakietem, odbiornikami, flagami dostaw i wejściem sektora. Dodaj także stan bez pakietu gotowy do jego wprowadzenia: walidacja nie może go utworzyć. Zgodność warunków z istniejącymi helperami sprawdź na osobnych kopiach; nie oczekuj od ruchu i obrotu helperów kontroli śmierci, którą zapewnia nowy predykat.

Nowy test zarejestruj w trzech miejscach CMake. Celowa mutacja produkcyjna musi wykazać istotny test negatywny, np. usunięcie kontroli życia albo pominięcie blokującej flagi. Wymuś `/t:Rebuild` właściwego celu i potwierdź rekompilację testu w logu. Przywróć kod przed końcowymi Debug/Release.

## Granica z P021-02

Resolver będzie korzystał z tego predykatu przed jakąkolwiek mutacją, a potem pracował na kopii i zwracał wartościowy wynik z informacją o przyjęciu. Przyszła kolejność: wprowadzenie pakietu, akcja gracza, ruch pakietu w nowej topologii, dostawa, jedna pełna faza impulsów, końcowy koszt jednej tury. Wait nie ma efektu na planszy; przy wspólnym końcowym `charge_turn` resolver nie powinien dodatkowo wywołać naliczającego koszt `wait_player`. Jego obecny kontrakt pozostaje bez zmian. Zewnętrzny RNG nie należy do wejścia resolvera i nie będzie losowany. Nie implementuj tych punktów w P021-01.

## Nie rozszerzaj

Nie dodawaj `TurnResult`, resolvera, systemu zdarzeń, wyjścia/limitu sektora, preview, overdrive, generatora, rezerwy, długu, zapisu i prezentacji. Nie generuj assetów ani treści dla gracza. Nie naprawiaj znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatter według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z faktem. Zapisz `history/P021-01.md`, a STATE/NEXT przestaw na P021-02 dopiero po spełnieniu kryteriów. Uruchom walidator i kontrole diff, wykonaj lokalny commit tylko własnych zmian, sprawdź hash, zakres i czystość. Zatrzymaj się przed P021-02.
