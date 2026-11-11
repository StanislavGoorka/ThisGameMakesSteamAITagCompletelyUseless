# Następny mały krok

Task: P023

## Cel i lektura

Dodaj podgląd jednej komendy przez ten sam wartościowy resolver. P022-02 jest zaimplementowane: TurnResult podaje state/accepted/outcome, a terminalne wejście odrzuca komendę przed fazami. Ostatnie Debug i Release zaliczyły po 23/23 CTest; walidator ma 127 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P022-02](history/P022-02.md), wpis P023 i jego zależność w [kolejce](../plan/tasks.json), [resolver](../../src/core/turn_resolver.hpp), [komendy](../../src/core/command.hpp), [GameState](../../src/core/game_state.hpp), [klasyfikator](../../src/core/sector_outcome.hpp), [testy resolvera](../../tests/unit/turn_resolver_tests.cpp), [testy komend](../../tests/unit/command_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md) i komendy w [README](../../README.md).

## Wykonaj

Jeden kontrakt: preview przyjmuje const GameState i const Command, zwraca wartościowy wynik identyczny z resolve_turn i pozostawia live state oraz komendę nietknięte. Wybierz nazwę funkcji i mały nagłówek core. Deleguj do istniejącego resolvera; nie kopiuj kolejności faz, reguł ani klasyfikacji. Resolver już pracuje na własnej kopii, więc nie dodawaj warstw sesji, commit API, callbacków ani zbędnej drugiej kopii.

Podgląd nie zapisuje plików, nie odblokowuje achievementów i nie przyjmuje RNG. Brak tych usług w API jest właściwym ograniczeniem; test zewnętrznego RNG względem kopii dokumentuje granicę API, a nie rollback.

## Dowód sukcesu i błędu

Zbuduj niewyświetlany korpus move/rotate/wait: legalne i błędne akcje, wprowadzenie i dostawa pakietu, obrót zmieniający trasy, ruch oraz kontakt impulsu, śmierć, wygrana i przeciążenie 47→48. Obejmij odmowę wszystkich komend dla completed/death/overload. Porównuj pełne state, accepted i outcome podglądu z wynikiem resolve_turn dla tego samego wejścia; zachowaj też jawne oczekiwania istotnych rezultatów, żeby zgodność dwóch wywołań nie ukrywała błędnego scenariusza.

Po każdym wywołaniu porównaj wszystkie pola live state i komendę. Powtarzany podgląd nie nalicza tury w live state, nie wprowadza pakietu ani nie przesuwa lub usuwa impulsów. Mutacja wyniku podglądu nie może zmienić live state, osobnego wyniku ani kolejnego podglądu; chroń pakiet, kolekcję impulsów i sector_exit. Nowy same_state musi obejmować wszystkie pola, w tym sector_exit. CMake wymaga rejestracji celu, opcji MSVC i add_test, wzorem testu resolvera.

Wykaż czułość testu celową mutacją produkcyjnej funkcji preview, np. dodatkowym charge_turn w zwracanym stanie. Wymuś /t:Rebuild właściwego celu, potwierdź rekompilację i oczekiwany błąd CTest; przywróć kod i przebuduj przed końcowymi testami.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter, walidator projektu i kontrole diff. Zaktualizuj README/ARCHITECTURE, zapisz history/P023.md, a STATE/NEXT przestaw na P024 po kryteriach. Następca ocenia zakres P024 przed implementacją replay: obecnie nie ma fabryki stanów z seeda, hasha pełnego stanu ani formatu/narzędzia replay. Nie pozoruj tych braków samym testem powtarzanego resolvera.

Lokalny commit własnych zmian oraz sprawdzenie hasha, zakresu i czystości są obowiązkowe; bez pushu. Nie implementuj replay P024, generatora, runu, overdrive, rezerwy, długu, zapisu ani prezentacji. Nie generuj assetów i treści dla gracza. Nie naprawiaj przy okazji znanego błędu scripts/Test-ProjectValidator.ps1. Zatrzymaj się przed kolejnym ID.
