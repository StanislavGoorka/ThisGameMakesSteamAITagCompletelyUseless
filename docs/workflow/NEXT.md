# Następny mały krok

Task: P012

## Cel

Dodaj deterministyczne losowanie liczby z przedziału bez obciążenia modulo. Zależność P011 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P011](history/P011.md), wpis P012 w [tasks.json](../plan/tasks.json), [SplitMix64](../../src/core/splitmix64.hpp), [testy generatora](../../tests/unit/splitmix64_tests.cpp), [ARCHITECTURE.md](../ARCHITECTURE.md) i [TESTING.md](../TESTING.md).

## Wykonaj

Określ granice przedziału i zachowanie dla błędnych granic. Oprzyj losowanie na `SplitMix64::next_u64()` z odrzucaniem wartości, które powodowałyby obciążenie modulo. Sprawdź przedział jednoelementowy, wartości przy granicach, błędny przedział i powtarzalne wektory. Zachowaj wektory surowego generatora z P011.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P012.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
