# Następny mały krok

Task: P009

## Cel

Dodaj komendę obrotu pojedynczego węzła o 90° zgodnie z ruchem wskazówek zegara. Zależność P008 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P008](history/P008.md), wpis P009 w [tasks.json](../plan/tasks.json), [GAME_DESIGN.md](../GAME_DESIGN.md), [ARCHITECTURE.md](../ARCHITECTURE.md), [TESTING.md](../TESTING.md), [maska portów](../../src/core/port_mask.hpp), [sąsiedztwo](../../src/core/grid_adjacency.hpp) i [CMakeLists.txt](../../CMakeLists.txt).

## Wykonaj

Zastosuj `PortMask::rotate_clockwise` do wybranego obrotowego węzła. Sprawdź, czy obrót zmienia połączenia tego węzła, czy odmowa obrotu węzła nieruchomego zachowuje jego stan oraz czy cztery obroty przywracają topologię. Zakres obejmuje wyłącznie obrót jednego węzła. Wyszukiwanie trasy należy do P010, a rozliczenie kosztu do logiki tury.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P009.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
