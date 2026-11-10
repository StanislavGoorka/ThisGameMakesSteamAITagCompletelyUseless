# Następny mały krok

Task: P013

## Cel

Dodaj minimalny `GameState`, który przechowuje planszę, pozycję gracza i licznik tur jako wartości. Zależność P012 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P012](history/P012.md), wpis P013 w [tasks.json](../plan/tasks.json), [typy identyfikatorów](../../src/core/identifiers.hpp), [reprezentacja węzłów](../../src/core/node_rotation.hpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Wykonaj

Użyj istniejących typów węzła i pozycji na planszy 7×7. Ustal sposób utworzenia poprawnego stanu początkowego z licznikiem tur równym zero. Przetestuj poprawny i błędny stan początkowy oraz zmianę kopii bez wpływu na oryginał. Reguły ruchu i kolejne akcje mają osobne wpisy w kolejce.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P013.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
