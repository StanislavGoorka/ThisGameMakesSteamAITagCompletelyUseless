# Następny mały krok

Task: P016

## Cel

Dodaj akcję `wait` bez regeneracji zasobów. Zależność P015 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P015](history/P015.md), wpis P016 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [ruch gracza](../../src/core/player_movement.hpp), [obrót gracza](../../src/core/player_rotation.hpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Wykonaj

Udowodnij, że `wait` żywego gracza kosztuje dokładnie jedną turę, nie regeneruje zasobów i zachowuje planszę oraz pozycję. Martwy gracz musi odrzucić akcję bez żadnej zmiany stanu. Obecny `GameState` nie zawiera jeszcze integralności gracza: dodaj tylko minimalny kontrakt potrzebny temu warunkowi, zgodnie z regułami i testami inicjalizacji oraz kopiowania stanu. Ustal jawnie odpowiedzialność za koszt tury; istniejące `move_player` i `rotate_from_player` wykonują tylko akcję. Nie dodawaj pełnej symulacji pakietów, zagrożeń, overdrive ani prezentacji.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P016.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
