# Następny mały krok

Task: P018

## Cel

Dodaj dostawę pakietu do celu i odroczone wprowadzenie następnego pakietu. Zależność P017 jest ukończona.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P017](history/P017.md), wpis P018 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [krok pakietu](../../src/core/packet_movement.hpp), [testy kroku pakietu](../../tests/unit/packet_movement_tests.cpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Stan zastany

`GameState::packet` to `std::optional<Packet>` z pozycją i celem; puste pole oznacza brak pakietu. `advance_packet` przesuwa pakiet o jedną krawędź i zwraca `false`, gdy pakietu nie ma, nie ma trasy albo pakiet stoi na celu. Pakiet na celu pozostaje na planszy: nic go jeszcze nie dostarcza. Stan nie zawiera wejścia sektora, listy odbiorników, licznika dostaw ani znacznika oczekującego pakietu. `GameState::initial` tworzy stan bez pakietu.

## Wykonaj

Udowodnij dwa warunki z wpisu P018. Dostawa następuje dokładnie raz: pakiet stojący na celu zostaje zdjęty z planszy, dostawa zostaje zapisana, a ponowne wywołanie kroku dostawy niczego nie zmienia. Następny pakiet pojawia się na wejściu dopiero w osobnym kroku przeznaczonym na początek następnej tury, więc w turze dostawy nie wykonuje żadnego ruchu, a w turze wprowadzenia najwyżej jeden. Cel nowego pakietu to kolejny niezasilony odbiornik według identyfikatorów, zgodnie z [regułami](../GAME_DESIGN.md).

Dodaj do `GameState` tylko pola potrzebne tym warunkom i rozszerz o nie testy inicjalizacji i kopiowania oraz cztery helpery `same_state` w testach `player_movement`, `player_rotation`, `player_wait` i `packet_movement`. Kroki dostawy i wprowadzenia nie naliczają tury; licznik zmienia wyłącznie `charge_turn`. Potwierdź nowe testy celowym błędem w kodzie produkcyjnym i zapisz wynik w raporcie.

Jeżeli dostawa razem z wprowadzeniem następnego pakietu przekracza limit jednego kroku z [WORKFLOW.md](../WORKFLOW.md), tym krokiem jest wyłącznie rozbicie P018 na dzieci, na przykład dostawę i osobno odroczone wprowadzenie, z przepięciem zależności P019 na ostatnie dziecko.

Nie dodawaj rezerwy ani jej przyrostu za dostawę, długu, ukończenia sektora, wyjścia, zagrożeń, resolvera tury, zdarzeń domenowych, achievementów ani prezentacji. Nie zmieniaj reguły trasy pakietu.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md). Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P018.md`, zaktualizuj STATE i NEXT, a następnie sprawdź hash i zakres lokalnego commitu.
