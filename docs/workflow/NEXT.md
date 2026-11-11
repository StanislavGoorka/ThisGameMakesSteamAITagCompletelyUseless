# Następny mały krok

Task: P026

## Cel i lektura

Oceń zakres overdrive przed implementacją. P025 potwierdziło wpływ obrotu na wspólną trasę pakietu i impulsu w jednym niewyświetlanym eksperymencie. Debug i Release zaliczyły po 29/29 CTest; walidator ma 131 zadań. To dowód reguł, bez oceny grywalności i balansu.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P025](history/P025.md), P026 i zależność P025 w [kolejce](../plan/tasks.json), [GAME_DESIGN](../GAME_DESIGN.md), [ARCHITECTURE](../ARCHITECTURE.md), [TESTING](../TESTING.md), [GameState](../../src/core/game_state.hpp), [komendy](../../src/core/command.hpp), [resolver](../../src/core/turn_resolver.hpp), [dostawę](../../src/core/packet_delivery.hpp), [obrót](../../src/core/player_rotation.hpp), [hash](../../tools/replay/state_hash.hpp), [runner replay](../../tools/replay/replay_runner.hpp), [CLI](../../tools/replay/main.cpp), [test eksperymentu](../../tests/unit/core_experiment_tests.cpp), [CMake](../../CMakeLists.txt) i komendy [README](../../README.md).

## Najpierw ustal mały zakres

GameState nie ma rezerwy ani długu. Command obsługuje move/rotate/wait, hash/replay v1 opisuje obecny pełny stan i reguły. P026 obejmuje więc dane zasobów, zmianę dostaw, akcję, integrację legalności/resolvera oraz utrzymanie pełnego hasha. Jeżeli są to osobne kontrakty przekraczające mikro-krok, ten krok ma być wyłącznie podziałem P026 na konkretne dzieci, bez implementacji dzieci. Zachowaj ID rodzica, przepnij konsumentów na faktyczną implementację i ustaw pierwsze gotowe dziecko w STATE/NEXT. Ukończenie planu nie oznacza działającego overdrive.

Reguły GAME_DESIGN: rezerwa startuje od 2 na run, maksimum3, dostawa dodaje1. Dług startuje od0. Overdrive wymaga rezerwy co najmniej1 i celu jak rotate; dwa obroty celu, koszt jednej tury i jednej rezerwy, przyrost długu o1. Nielegalność nie zmienia żadnego pola. Przeliczenie długu na spawn między sektorami należy do P027. Nie twórz runu ani generatora.

Podział powinien określić testy granic rezerwy/długu, koszt dokładnie raz, pełną niezmienność odmowy, zgodność preview z resolverem i aktualizację wszystkich same_state. Zmiana GameState wymaga pełnego hasha; zdecyduj jawnie o wersji kodowania/replay zamiast po cichu zmieniać v1. Nie pytaj o rutynowe decyzje mechanik.

## Weryfikacja i przekazanie

Dla podziału sprawdź graf, kolejność dzieci, przepięcie konsumentów i odmowę przedwczesnego startu na izolowanych próbkach walidatora. Nie uruchamiaj dzieci. Dla zmiany C++ obowiązują pełne Debug/Release, formatter i istotna negatywna kontrola po potwierdzonym Rebuild. Zawsze uruchom walidator i kontrole diff.

Zapisz history/P026.md, STATE/NEXT i zgodne README/ARCHITECTURE. Obowiązkowy lokalny commit wyłącznie własnych zmian; sprawdź hash/zakres/czystość, bez pushu. Zatrzymaj się po tym jednym ID. Nie generuj assetów lub treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem.
