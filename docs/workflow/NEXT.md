# Następny mały krok

Task: P014

## Cel

Dodaj walidację komendy ruchu gracza po wzajemnej krawędzi do sąsiedniego pola. Zależność P013 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P013](history/P013.md), wpis P014 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [sąsiedztwo](../../src/core/grid_adjacency.hpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Wykonaj

Użyj `GameState` i istniejącego wyznaczania sąsiadów. Uwzględnij zajęcie celu przez blokującą jednostkę. Przetestuj legalny ruch, brak wzajemnej krawędzi, granicę planszy, zablokowany cel i zachowanie stanu przy odrzuceniu. Rozliczanie pełnej tury jest osobnym krokiem.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P014.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
