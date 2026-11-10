# Następny mały krok

Task: P011

## Cel

Dodaj mały deterministyczny generator liczb pseudolosowych dla core. Zależność P010 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P010](history/P010.md), wpis P011 w [tasks.json](../plan/tasks.json), [GAME_DESIGN.md](../GAME_DESIGN.md), [ARCHITECTURE.md](../ARCHITECTURE.md), [TESTING.md](../TESTING.md) i [CMakeLists.txt](../../CMakeLists.txt).

## Wykonaj

Wybierz algorytm i opisz inicjalizację z seeda oraz przejście stanu na typach całkowitych o ustalonej szerokości. Zweryfikuj kilka kolejnych wyników względem znanych wektorów i powtarzalność dwóch niezależnych instancji z tym samym seedem. Mapowanie wyników na przedział należy do P012.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P011.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
