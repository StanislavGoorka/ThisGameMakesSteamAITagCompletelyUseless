# Następny mały krok

Task: P007

## Cel

Dodaj czterobitową maskę portów N, E, S, W i obrót o 90° zgodnie z ruchem wskazówek zegara. Zależność P006 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P006](history/P006.md), wpis P007 w [tasks.json](../plan/tasks.json), [GAME_DESIGN.md](../GAME_DESIGN.md), [ARCHITECTURE.md](../ARCHITECTURE.md), [TESTING.md](../TESTING.md) i [CMakeLists.txt](../../CMakeLists.txt).

## Wykonaj

Ustal mapowanie czterech kierunków na bity i zaimplementuj obrót maski. Sprawdź wynik obrotu dla wszystkich 16 masek, powrót do wyjściowej maski po czterech obrotach i zachowanie liczby ustawionych bitów. Zakres kroku obejmuje maskę, obrót i testy. Sąsiedztwo grafu jest osobnym zadaniem P008.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P007.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
