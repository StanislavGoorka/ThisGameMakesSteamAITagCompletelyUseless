# Następny mały krok

Task: P025

## Cel i lektura

Przeprowadź jeden niewyświetlany eksperyment rdzenia: ten sam stan początkowy i trzy sekwencje decyzji, z dowodem wpływu wspólnej topologii na pakiet i impuls. P024-04 kończy adapter CLI replay. Debug i Release zaliczyły po 28/28 CTest; walidator ma 131 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P024-04](history/P024-04.md), P025 i zależność w [kolejce](../plan/tasks.json), [GAME_DESIGN](../GAME_DESIGN.md), [ARCHITECTURE](../ARCHITECTURE.md), [TESTING](../TESTING.md), [resolver](../../src/core/turn_resolver.hpp), [podgląd](../../src/core/turn_preview.hpp), [pakiet](../../src/core/packet_movement.hpp), [pościg](../../src/core/pulse_chase.hpp), [trasy](../../src/core/grid_pathfinding.hpp), [testy resolvera](../../tests/unit/turn_resolver_tests.cpp), [testy podglądu](../../tests/unit/turn_preview_tests.cpp), [runner replay](../../tools/replay/replay_runner.hpp) i komendy [README](../../README.md).

## Wykonaj

Jeden obserwowalny dowód wyróżnika, bez zmiany reguł gry. Dobierz mały syntetyczny stan numeryczny i trzy krótkie legalne sekwencje. Co najmniej jedna decyzja obrotu musi zmienić trasy zarówno pakietu, jak i impulsu przez te same krawędzie. Porównaj dostawy, ruch zagrożenia, HP i koszt tur; podaj pełne dane wejściowe oraz wyniki pozwalające powtórzyć eksperyment. Używaj istniejącego resolvera i wspólnego preview, nie kopiuj faz.

Można dodać jeden wykonywalny test eksperymentu w CTest z jawnymi oczekiwaniami; nie jest to nowa funkcja produkcyjna. Trzy sekwencje zaczynają się od niezależnych kopii tego samego stanu. Istniejący fixture replay pozostaje scenariuszem deweloperskim, nie musi spełniać hipotezy eksperymentu. Nie zmieniaj go lub wersji replay dla wygody; osobny niewyświetlany stan testu jest dozwolony.

## Dowód sukcesu i ograniczenia

Zapisz hipotezę, konkretne trasy przed/po, trzy sekwencje i rzeczywiste wyniki oraz wniosek o wspólnym obiegu. Sprawdź zgodność preview z wynikiem oraz niezmienność wejścia. Wykryj celową istotną mutację (np. pominięcie akcji obrotu) po wymuszonym /t:Rebuild właściwego celu, z potwierdzoną rekompilacją i oczekiwanym błędem CTest. Przywróć kod i przebuduj przed końcową weryfikacją.

Raport nie może udawać playtestu, opinii graczy, pomiaru przyjemności lub dowodu całego balansu. To jeden test headless, bez grafiki, audio lub interfejsu. Jeśli hipoteza nie jest potwierdzona, opisz fakt i mały następny krok; nie zmyślaj pozytywnego wyniku.

## Weryfikacja i przekazanie

Dla zmian C++ wykonaj konfigurację, build i pełne CTest Debug/Release oraz formatter src/tests/tools. Zawsze wykonaj walidator i kontrole diff, zapisz history/P025.md i zgodną dokumentację. Po kryteriach STATE/NEXT wskazują P026; następca musi ocenić zakres overdrive przed implementacją, ponieważ GameState nie ma jeszcze rezerwy ani długu. Nie implementuj ich w tym kroku.

Obowiązkowy lokalny commit wyłącznie własnych zmian i sprawdzenie hasha/zakresu/czystości; bez pushu. Zatrzymaj się przed kolejnym ID. Nie implementuj overdrive, generatora P028, runu, zapisu ani prezentacji. Nie generuj assetów lub treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem.
