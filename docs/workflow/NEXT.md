# Następny mały krok

Task: P017

## Cel

Dodaj jeden pakiet przemieszczany po BFS o jedną krawędź. Zależność P016 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P016](history/P016.md), wpis P017 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [najkrótszą trasę](../../src/core/grid_pathfinding.hpp), [obrót węzła](../../src/core/node_rotation.hpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Wykonaj

Udowodnij, że pakiet przesuwa się o dokładnie jedną krawędź najkrótszej trasy do celu w bieżącej topologii, z remisami N, E, S, W z istniejącego BFS. Obrót węzła musi zmienić wybraną trasę. Pakiet bez trasy czeka: nie znika, nie zmienia celu i nie zmienia reszty stanu. Pakiet nie blokuje gracza, więc nie zapisuj go w `blocked_nodes`. Obecny `GameState` nie zawiera pakietu ani celu: dodaj tylko minimalny kontrakt potrzebny temu warunkowi, zgodnie z testami inicjalizacji i kopiowania stanu, oraz rozszerz porównania pełnego stanu w testach akcji o nowe pola. Krok pakietu nie nalicza tury; licznik zmienia wyłącznie `charge_turn`. Nie dodawaj dostawy, drugiego pakietu, zagrożeń, resolvera tury ani prezentacji.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P017.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
