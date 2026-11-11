# Następny mały krok

Task: P022-02

## Cel i lektura

Podepnij istniejącą klasyfikację sektora do transakcji tury i blokady dalszych komend. P022-01 jest zaimplementowane, lecz resolver nadal nie egzekwuje końca sektora.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P022-01](history/P022-01.md), [raport P021-02](history/P021-02.md), wpis P022-02 i zależność w [kolejce](../plan/tasks.json), [klasyfikator](../../src/core/sector_outcome.hpp), [GameState](../../src/core/game_state.hpp), [komendy](../../src/core/command.hpp), [resolver](../../src/core/turn_resolver.hpp), [testy klasyfikacji](../../tests/unit/sector_outcome_tests.cpp), [testy komend](../../tests/unit/command_tests.cpp), [testy resolvera](../../tests/unit/turn_resolver_tests.cpp), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md) i komendy w [README](../../README.md).

## Wykonaj

Jeden kontrakt: wynik tury podaje aktualny wynik sektora i odrzuca terminalne wejście przed fazami.

- Dodaj wartościowy SectorOutcome do TurnResult, bez trwałego pola w GameState. Używaj istniejącego sector_outcome, nie powielaj reguł.
- is_command_legal odrzuca każdy wariant komendy, gdy wejściowy wynik nie jest ongoing. resolve_turn zwraca accepted=false, istniejącą klasyfikację i niezależną pełną kopię stanu, bez faz ani kosztu.
- Po przyjęciu komendy zachowaj kolejność: wprowadzenie pakietu, akcja, ruch pakietu, dostawa, jedna faza impulsów, końcowy koszt jednej tury. Dopiero wtedy odczytaj wynik. Dostawa i koszt obowiązują również przy śmierci.
- Zwykła błędna akcja podaje klasyfikację wejścia i zachowuje stan bez kosztu. Wejściowy stan i komenda pozostają nietknięte. RNG jest poza API.
- Zachowaj kontrakty niskopoziomowych helperów, w tym wait_player; nie dodawaj im limitu.

## Dowód sukcesu i błędu

Tura 47→48: druga dostawa i żywy gracz na wyjściu dają completed; kontakt zabierający ostatni HP daje death mimo dostawy i wyjścia; bez sukcesu następuje overload. Dostawa i koszt pozostają zapisane przy śmierci. Błędna akcja na turze 47 nie zużywa ostatniej tury.

Dla completed/death/overload sprawdź wszystkie warianty komendy w legalności i resolverze, także przy gotowym do wprowadzenia pakiecie, impulsach z trasą i zaległym kontakcie. Odmowa nie wykonuje faz ani tury 49. Porównuj pełny stan, komendę i niezależność wynikowych kopii, w tym sector_exit. Potwierdź dalszą sekwencję zewnętrznego RNG względem kopii; nie deklaruj rollbacku RNG.

Fabryki dotychczasowych testów mają niepuste wyjście 48. Jawnie dobierz wyjście dla sekwencji, które mają dalej trwać. Nie osłabiaj oczekiwań kolejności/liczby faz. Wszystkie 15 helperów same_state porównuje sector_exit.

Wykonaj celową mutację produkcyjną, np. pominięcie blokady terminalnego wejścia albo odczyt wyniku przed zagrożeniami/kosztem. Wymuś /t:Rebuild właściwego celu, potwierdź rekompilację i oczekiwany błąd CTest. Przywróć kod przed pełnymi Debug/Release.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter, walidator projektu i kontrole diff. P022-01 zaliczył po 23/23 CTest i walidator 127 zadań. Zaktualizuj README/ARCHITECTURE, zapisz history/P022-02.md, a STATE/NEXT przestaw na P023 po kryteriach. Lokalny commit własnych zmian i sprawdzenie hasha, zakresu oraz czystości są obowiązkowe; bez pushu.

Nie implementuj preview P023, generatora, runu, overdrive, rezerwy, długu, zapisu ani prezentacji. Nie generuj assetów i treści dla gracza. Nie naprawiaj przy okazji znanego błędu scripts/Test-ProjectValidator.ps1. Zatrzymaj się przed kolejnym ID.
