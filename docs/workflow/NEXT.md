# Następny mały krok

Task: P008

## Cel

Wyznacz sąsiedztwo na siatce 7×7. Krawędź łączy ortogonalnie sąsiadujące pola, jeśli oba mają port skierowany do drugiego pola. Zależność P007 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P007](history/P007.md), wpis P008 w [tasks.json](../plan/tasks.json), [GAME_DESIGN.md](../GAME_DESIGN.md), [ARCHITECTURE.md](../ARCHITECTURE.md), [TESTING.md](../TESTING.md), [identyfikatory](../../src/core/identifiers.hpp), [maska portów](../../src/core/port_mask.hpp) i [CMakeLists.txt](../../CMakeLists.txt).

## Wykonaj

Ustal mapowanie `NodeId` na wiersz i kolumnę oraz obliczaj połączenia z masek portów. Sprawdź wzajemność w czterech kierunkach, krawędzie siatki, brak zawijania między wierszami i brak połączenia z jednostronnym portem. Zakres obejmuje wyłącznie sąsiedztwo. Komendę obrotu dodaje P009, a wyznaczanie tras należy do osobnego zadania.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P008.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
