# Następny mały krok

Task: P015

## Cel

Dodaj akcję obrotu węzła gracza lub jego ortogonalnego sąsiada. Zależność P014 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P014](history/P014.md), wpis P015 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [obrót węzła](../../src/core/node_rotation.hpp), [ruch gracza](../../src/core/player_movement.hpp), [sąsiedztwo](../../src/core/grid_adjacency.hpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Wykonaj

Użyj `GameState` i istniejącego `rotate_node`. Połączenie portów nie jest warunkiem obrotu: zakres wynika z położenia na siatce. Przetestuj własny węzeł i czterech ortogonalnych sąsiadów, także bez połączenia, oraz odrzucenie celu dalekiego, po przekątnej, przez zawinięcie wiersza i nieruchomego. Przy odrzuceniu cały stan i licznik tur mają pozostać bez zmian. Tak jak `move_player`, akcja wykonuje tylko swoją zmianę; rozliczanie pełnej tury pozostaje osobnym krokiem. Nie dodawaj overdrive, zagrożeń ani prezentacji.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P015.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
