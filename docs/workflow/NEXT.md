# Następny mały krok

Task: P026-01

## Cel i lektura

Dodaj wyłącznie dane rezerwy/długu i zachowaj pełny wersjonowany hash. P026 jest ukończonym podziałem pięciu kontraktów, bez implementacji dzieci. GameState nadal nie ma zasobów, overdrive nie działa. Ostatni kod z P025 ma po 29/29 CTest Debug/Release; plan po podziale ma 136 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P026](history/P026.md), P026/P026-01 i zależność P025 w [kolejce](../plan/tasks.json), [GAME_DESIGN](../GAME_DESIGN.md), [ARCHITECTURE](../ARCHITECTURE.md), [TESTING](../TESTING.md), [GameState](../../src/core/game_state.hpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [hash](../../tools/replay/state_hash.hpp), [format v1](history/P024-01.md), [test hasha](../../tests/unit/state_hash_tests.cpp), [fixture](../../tools/replay/seeded_fixture.hpp), [runner](../../tools/replay/replay_runner.hpp), [test runnera](../../tests/unit/replay_runner_tests.cpp), [CLI](../../tools/replay/main.cpp), [test CLI](../../tests/integration/replay_cli_tests.cmake), [dostawę](../../src/core/packet_delivery.hpp), [komendy](../../src/core/command.hpp), [resolver](../../src/core/turn_resolver.hpp), [CMake](../../CMakeLists.txt) i komendy [README](../../README.md).

## Jeden kontrakt

GameState dostaje wartościowe `std::uint8_t reserve` ze startem 2 oraz `std::uint32_t overload_debt` ze startem 0. Limit prawidłowej rezerwy w grze to 3; dodaj nazwaną stałą. Zachowaj sygnaturę `GameState::initial`; nie dodawaj runu ani fabryki sektorów. Zasoby są przechowywane w stanie i kopiowane niezależnie. Samo ich dodanie nie regeneruje po dostawie, nie pobiera kosztu i nie zmienia legalności istniejących komend.

Pełny `state_hash` przechodzi na jawne kodowanie **v2**: format v1, z prefiksem uint32 LE równym 2, a po całej kolekcji impulsów dopisane reserve uint8 i overload_debt uint32 LE. Pozostałe szerokości/tagi/kolejność nie zmieniają się. Wynik nadal FNV-1a64, bez surowej pamięci i bez gwarancji braku kolizji. Testy haszują również syntetyczne granice reprezentacji, nawet jeśli rezerwa 255 nie jest prawidłowym zasobem gry.

Przełącz `replay_version` na **2**. Runner przyjmuje tylko 2; 1 i pozostałe obce wersje odrzuca bez częściowego wyniku, CLI bez stdout. Nie emuluj starych reguł i nie zachowuj starej funkcji jako rzekomo pełnego hasha. Fixture ma tę samą topologię/seed, ale startowe zasoby 2/0 i nowe pełne hashe. Zaktualizuj wszystkie stałe wektory w testach runnera/fixture/CLI, przypadki wersji i działające komendy README. Parser i reguły CLI nie wymagają nowej funkcji.

## Dowód sukcesu i błędu

- Initial daje 2/0. Dwie kolejne kopie zachowują oba pola; zmiana każdej kopii i zwracanych wyników nie zmienia innych stanów.
- Znajdź `rg -l 'bool same_state' tests`: obecnie 20 helperów. Każdy porównuje oba pola; fabryki testowe dostają niezerowy dług i różne poprawne wartości rezerwy, gdzie nie przeczy to testowi inicjalizacji/fixture. Istniejące operacje muszą zachować nowe dane.
- Niezależnie wylicz co najmniej minimalny/pełny wektor hasha v2 i znane fixture; zapisz odtwarzalny sposób. Zmiana samej rezerwy (0/1/2/3/255) lub długu (0/1/UINT32_MAX) zmienia oczekiwane hashe w korpusie i nie mutuje stanu. Nie zastępuj jawnych oczekiwań porównaniem funkcji z nią samą.
- Replay 2 daje pełny oczekiwany stan i ślad; replay 1/0/3/UINT32_MAX odrzucone. Test CLI uruchamia rzeczywiste exe z nową wersją i sprawdza pusty stdout dla starej wersji.
- Celowo pomiń jedno nowe pole w produkcyjnym hashu. Wymuś `/t:Rebuild` właściwego celu, potwierdź rekompilację i oczekiwany CTest błąd. Przywróć kod oraz przebuduj przed końcowymi testami.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i kontrole diff. Zaktualizuj README/ARCHITECTURE, zapisz history/P026-01.md, a STATE/NEXT przestaw na P026-02 po kryteriach. Następne dziecko regeneruje rezerwę przy rzeczywistej dostawie i przełącza replay na 3; nie wykonuj go teraz.

Obowiązkowy lokalny commit własnych zmian, sprawdź hash/zakres/czystość; bez pushu. Nie zmieniaj deliver_packet, resolvera ani Command, nie implementuj helpera overdrive, regeneracji, spawnu długu P027, runu, generatora lub prezentacji. Nie generuj assetów i treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem. Zatrzymaj się przed kolejnym ID.
