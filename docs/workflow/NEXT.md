# Następny mały krok

Task: P010

## Cel

Dodaj jedną funkcję wyznaczającą najkrótszą trasę na siatce 7×7 za pomocą BFS. Zależność P009 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P009](history/P009.md), wpis P010 w [tasks.json](../plan/tasks.json), [GAME_DESIGN.md](../GAME_DESIGN.md), [ARCHITECTURE.md](../ARCHITECTURE.md), [TESTING.md](../TESTING.md), [sąsiedztwo](../../src/core/grid_adjacency.hpp) i [CMakeLists.txt](../../CMakeLists.txt).

## Wykonaj

Rozwijaj węzły przez `connected_neighbors` w kolejności N, E, S, W. Sprawdź brak trasy, przypadek `start == cel`, granice siatki i wybór jednej z równych tras w symetrycznej konfiguracji. Zakres obejmuje wyznaczanie trasy. Ruch pakietu, ruch zagrożeń i logika tury należą do późniejszych zadań.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P010.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
