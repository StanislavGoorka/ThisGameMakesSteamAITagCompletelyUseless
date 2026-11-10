# Następny mały krok

Task: P006

## Cel

Dodaj silne typy identyfikatorów domeny `NodeId` i `EntityId` w core. Zależność P005 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P005](history/P005.md), wpis P006 w [tasks.json](../plan/tasks.json), [ARCHITECTURE.md](../ARCHITECTURE.md), [TESTING.md](../TESTING.md) i [CMakeLists.txt](../../CMakeLists.txt).

## Wykonaj

Zdefiniuj oba typy i ich zakres zgodnie z architekturą. Dodaj testy wartości granicznych i sprawdzenia w czasie kompilacji, że niedozwolone konwersje są odrzucane, w tym konwersje między typami. Zakres kroku obejmuje identyfikatory i ich testy.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P006.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
