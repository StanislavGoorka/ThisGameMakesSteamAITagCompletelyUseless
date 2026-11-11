# Następny mały krok

Task: P024-02

## Cel i lektura

Dodaj jedną czystą fabrykę syntetycznego niewyświetlanego GameState z RunSeed jako narzędzie deweloperskie. P024-01 dostarcza pełny kanoniczny hash v1; Debug i Release zaliczyły po25/25 CTest, walidator ma131 zadań. P024 to ukończony podział, nie działający replay.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P024-01](history/P024-01.md), [raport P024](history/P024.md), P024-02 i jego zależność w [kolejce](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [porty](../../src/core/port_mask.hpp), [RNG](../../src/core/splitmix64.hpp), [hash](../../tools/replay/state_hash.hpp), [test hasha](../../tests/unit/state_hash_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md) i komendy [README](../../README.md).

## Wykonaj

Jeden kontrakt: RunSeed → niezależny wartościowy GameState, bez usług lub efektów ubocznych. Dodaj mały nagłówek pod tools/replay/, poza core i paczką gry. Nazwę wybierz sam. Nie zmieniaj resolvera ani GameState.

Stały scenariusz numeryczny:

- Węzły0/1/7/8 mają maski odpowiednio E|S, W|S, N|E, N|W; wszystkie inne maski0.
- Gracz0, HP3, tura0; wejście0, odbiorniki1/8, wyjście8.
- Impuls EntityId1 na7; brak pakietu, dostaw i blocked_nodes.
- Tylko węzeł1 jest obrotowy; wszystkie inne flagi rotatable_nodes fałszywe.
- Lokalny SplitMix64 z wejściowego seeda wywołuje next_in_range(0,3); wynik0–3 wyznacza liczbę początkowych obrotów węzła1. Żadne inne pole nie jest seedowane.

To pojedynczy fixture headless do replay, nie plansza do wyświetlania, generator sektorów P028, solver ani certyfikat wygrywalności. Różne seedy mogą dać identyczny stan. RNG nie wchodzi do API resolvera ani GameState.

## Dowód sukcesu i błędu

Test zarejestruj w CMake z prywatnym include tools, opcjami MSVC oraz add_test wzorem developer_state_hash_tests. Sprawdź pełne jawne oczekiwane stany dla seeda0 i UINT64_MAX oraz dobranych znanych seedów dających różne orientacje. Wektory orientacji ustal niezależnie lub oprzyj o istniejące znane wektory RNG; oczekiwania nie mogą być obliczane przez fabrykę pod testem. Ustal również stałe oczekiwane hashe przez state_hash v1, zapisując sposób obliczenia w raporcie.

Porównuj wszystkie pola stanu, nie sam hash lub obrócony węzeł. Ten sam seed powtórzony daje ten sam stan i hash; mutacja jednej zwróconej wartości nie wpływa na kolejną lub wcześniejszą. Zewnętrzny RNG względem kopii pozostaje nietknięty, bo fabryka używa tylko własnego generatora.

Celowo zignoruj seed albo wyzeruj orientację w produkcyjnej fabryce. Wymuś /t:Rebuild właściwego celu, potwierdź rekompilację i oczekiwany błąd CTest. Przywróć kod oraz przebuduj przed końcowymi testami.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator projektu oraz kontrole diff. Zaktualizuj README/ARCHITECTURE, zapisz history/P024-02.md, a STATE/NEXT przestaw na P024-03 po kryteriach. P024-03 będzie wartościowym runnerem: wersja1 + seed + komendy, początkowy hash i accepted/outcome/hash po każdej próbie, niezależny stan końcowy, jawna odmowa obcej wersji bez częściowego wyniku. Nie implementuj go w tym kroku.

Obowiązkowy lokalny commit wyłącznie własnych zmian, sprawdź hash, zakres i czystość; bez pushu. Nie dodawaj CLI, eksperymentu P025, generatora, runu, overdrive, zapisu ani prezentacji. Nie generuj assetów lub treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem. Zatrzymaj się przed kolejnym ID.
