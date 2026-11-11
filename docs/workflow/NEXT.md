# Następny mały krok

Task: P024-04

## Cel i lektura

Dodaj mały adapter CLI dla działającego runnera replay P024-03. Debug i Release zaliczyły po 27/27 CTest; walidator ma 131 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P024-03](history/P024-03.md), P024-04 i zależność w [kolejce](../plan/tasks.json), [runner](../../tools/replay/replay_runner.hpp), [fixture](../../tools/replay/seeded_fixture.hpp), [hash](../../tools/replay/state_hash.hpp), [test runnera](../../tests/unit/replay_runner_tests.cpp), [komendy](../../src/core/command.hpp), [identyfikatory](../../src/core/identifiers.hpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md) i komendy [README](../../README.md).

## Wykonaj

Jeden adapter pod tools/replay/, poza paczką gry, bez zmian reguł. Przyjmij dziesiętne version seed i opcjonalne tokeny wait, move:NodeId, rotate:NodeId. Pełny parse wszystkich argumentów musi zakończyć się przed uruchomieniem run_replay i wypisaniem śladu. Liczby wymagają co najmniej jednej cyfry, pełnego zużycia tokena i kontroli zakresu; nie przyjmuj minusów, plusów lub białych znaków w tokenie. NodeId tylko 0–48, seed pełny uint64, version uint32. Nie dodawaj parsera JSON, plików ani ogólnego formatu zapisu.

Runner obsługuje wersję 1 i zwraca optional ReplayResult: initial_hash, steps (accepted/outcome/hash) i state. Odrzucona komenda gry jest poprawnym wpisem replay, więc sama nie powoduje błędu CLI. Nieobsługiwana wersja lub błąd składni daje niezerowy kod bez częściowego śladu sukcesu; diagnostyka jest wyłącznie deweloperska. Wypisz stabilny numeryczny ślad initial_hash i kolejnych wpisów z jawnym mapowaniem outcome. Wybierz proste stałe formatowanie i opisz je w README; nie wyświetlaj planszy. Nie kopiuj reguł, faz, fixture lub hasha.

## Dowód sukcesu i błędu

CTest musi uruchomić rzeczywiste exe w Debug/Release, np. przez skrypt cmake -P i execute_process, z przekazaną ścieżką $<TARGET_FILE:...>. Dwa identyczne wywołania mają identyczny stdout i kod 0. Porównaj też dokładny oczekiwany ślad znanego korpusu, nie tylko dwa uruchomienia. Pusta sekwencja i seedy 0/UINT64_MAX są poprawne; znane initial_hash to 7229561696845526205 i 18070937588695573379. Wykorzystaj mały korpus runnera z odmową i legalnymi próbami.

Przypadki błędne: brak version lub seed, nieznany token, błędny separator/argument komendy, trailing junk w każdym rodzaju liczby, minus, plus, przepełnienie version/seed/NodeId, NodeId 49 i wersje 0/2/UINT32_MAX. Sprawdź niezerowy kod i pusty stdout, również dla błędu po wcześniejszym poprawnym tokenie. Ślad nie może pojawić się częściowo. Nie wymagaj konkretnych tekstów diagnostyki, jeśli kod i brak sukcesu są jednoznaczne.

Celowo zmień parser lub kod wyjścia produkcyjnego CLI; wymuś /t:Rebuild właściwego celu, potwierdź rekompilację i dokładny oczekiwany błąd CTest. Przywróć kod i przebuduj przed końcowymi testami.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator projektu i kontrole diff. README ma podawać rzeczywiście uruchomioną komendę CLI i format numerycznego outcome. Zaktualizuj ARCHITECTURE, zapisz history/P024-04.md, a STATE/NEXT przestaw na P025 po kryteriach. P025 to jeden niewyświetlany eksperyment rdzenia z trzema sekwencjami decyzji, bez udawania playtestu; nie realizuj go teraz.

Obowiązkowy lokalny commit własnych zmian i sprawdzenie hasha/zakresu/czystości, bez pushu. Nie implementuj drugiego ID, generatora P028, runu, overdrive, zapisu ani prezentacji. Nie generuj assetów lub treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem.
