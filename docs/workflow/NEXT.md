# Następny mały krok

Task: P024-01

## Cel i lektura

Dodaj jeden niemutujący hash pełnego GameState jako narzędzie deweloperskie. P024 jest ukończonym podziałem planu, bez implementacji dzieci. P023 dostarcza preview; ostatnie Debug i Release zaliczyły po 24/24 CTest. Walidator po podziale ma 131 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P024](history/P024.md), wpisy P024/P024-01 i zależność P023 w [kolejce](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [PortMask](../../src/core/port_mask.hpp), [testy podglądu](../../tests/unit/turn_preview_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md) i komendy w [README](../../README.md).

## Wykonaj

Jeden kontrakt: const GameState → uint64_t, bez mutacji. Umieść mały nagłówek pod tools/; core nie zależy od narzędzi. CMake udostępnia go tylko właściwemu celowi testowemu. Nazwy funkcji i pliku wybiera wykonawca.

Użyj FNV-1a 64-bit: początek 14695981039346656037, dla każdego bajtu XOR, następnie mnożenie przez 1099511628211 modulo 2^64. To hash niekryptograficzny do regresji, bez gwarancji braku kolizji. Nie haszuj surowej pamięci, paddingu, adresów ani std::hash; nie dodawaj ogólnego serializatora lub plików.

Jawny format bajtów v1, w tej kolejności:

1. Wersja kodowania 1 jako uint32 little-endian.
2. ports: 49 bajtów bits() w kolejności NodeId 0…48.
3. rotatable_nodes i potem blocked_nodes: po 49 bajtów 0/1.
4. player_position jako uint8, player_integrity jako uint8, turns_taken jako uint32 little-endian.
5. packet: tag 0/1; przy 1 position i target jako po jednym uint8.
6. powered_nodes i potem receiver_nodes: po 49 bajtów 0/1.
7. sector_entry i potem sector_exit: osobny tag 0/1; przy 1 jeden uint8 NodeId. Brak i NodeId 0 muszą dawać różne kodowanie.
8. Liczba pulses jako uint64 little-endian, następnie dla każdego elementu w kolejności przechowywania EntityId jako uint32 little-endian i position jako uint8.

Kolejność impulsów jest częścią dokładnego wartościowego stanu, jak w helperach same_state. Nie sortuj kolekcji w hashu ani w stanie; identyczne zachowanie symulacji po permutacji nie oznacza identycznej reprezentacji. Wynik sektora jest wyliczany, więc nie dodawaj jego kopii do strumienia. Nie dokładaj seeda/RNG do GameState.

## Dowód sukcesu i błędu

Dodaj test do trzech miejsc CMake wzorem testu preview. Ustal stałe oczekiwane hashe co najmniej dla minimalnego i pełnego stanu. Oblicz wektory niezależnie od produkcyjnej funkcji i zapisz odtwarzalny sposób w raporcie; samo porównanie funkcji z nią samą nie dowodzi formatu.

Sprawdź każde pole i każdy indeks tablic przez kontrolowaną zmianę względem kopii. Obejmij tagi optional (brak/NodeId 0/48), pola pakietu, ID impulsów 0/UINT32_MAX, pozycje 0/48, zmianę liczby i kolejności impulsów, HP 0/3/255, licznik 0/48/UINT32_MAX. Korpus ma wykrywać pominięcie pola; nie dowodzi braku wszystkich kolizji. Każde wywołanie porównuje cały stan przed/po z niepustym pakietem i impulsami w pełnym scenariuszu. Powtórzenie i niezależna kopia muszą dawać ten sam hash.

Celowo pomiń pole w produkcyjnym hashu i wykaż oczekiwany błąd CTest po /t:Rebuild właściwego celu z potwierdzoną rekompilacją. Przywróć kod i przebuduj przed końcowymi testami.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter, walidator projektu i kontrole diff. Zaktualizuj README/ARCHITECTURE zgodnie z faktem, zapisz history/P024-01.md, STATE/NEXT przestaw na P024-02 po kryteriach. Lokalny commit własnych zmian i sprawdzenie hasha, zakresu oraz czystości są obowiązkowe; bez pushu. Zatrzymaj się przed kolejnym ID.

P024-02 ma później zbudować jeden stały syntetyczny fixture headless: pola 0/1/7/8 tworzą kwadrat, z maskami odpowiednio E|S, W|S, N|E, N|W; pozostałe maski 0. Gracz startuje na 0 z HP3 i turą0, wejście0, odbiorniki1/8, wyjście8, impuls EntityId1 na7; brak pakietu, dostaw i blocked_nodes. Tylko węzeł1 jest obrotowy, a lokalny SplitMix64 z seeda wybierze przez next_in_range(0,3) liczbę jego początkowych obrotów. Te dane nie są planszą do wyświetlania ani generatorem sektorów P028.

Nie implementuj teraz fabryki, runnera, CLI, P025, generatora, runu, overdrive, zapisu ani prezentacji. Nie generuj assetów lub treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem.
