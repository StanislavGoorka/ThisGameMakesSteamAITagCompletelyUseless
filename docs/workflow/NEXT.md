# Następny mały krok

Task: P022-01

## Cel

Dodaj opcjonalne wyjście sektora i jeden niemutujący odczyt jego wyniku. P022 jest ukończonym podziałem planu, nie implementacją limitu ani wyjścia. P021-02 już zapewnia transakcję tury; integracja klasyfikacji z komendami i resolverem należy dopiero do P022-02.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P022](history/P022.md), [raport P021-02](history/P021-02.md), wpisy P022 i P022-01 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [dostawę](../../src/core/packet_delivery.hpp), [odbiorniki](../../src/core/packet_target.hpp), [resolver](../../src/core/turn_resolver.hpp), [testy resolvera](../../tests/unit/turn_resolver_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcje „Plansza i stan początkowy” i „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) oraz [TESTING](../TESTING.md).

## Stan zastany

GameState ma HP, pozycję gracza, licznik tur, flagi `receiver_nodes` i `powered_nodes`, opcjonalne wejście i pakiet oraz wartościową kolekcję impulsów. Nie ma wyjścia ani trwałego wyniku sektora. `deliver_packet` może zasilić cel niebędący odbiornikiem, więc sama liczba wszystkich flag zasilenia nie dowodzi dwóch dostaw do odbiorników.

`resolve_turn` zwraca `TurnResult { state, accepted }`. Nielegalna komenda nie wykonuje faz; sukces wykonuje wprowadzenie pakietu, akcję, ruch pakietu, dostawę, jedną fazę impulsów i końcowy koszt jednej tury. Wejście pozostaje niezmienione. `is_command_legal` wymaga żywego gracza, ale jeszcze nie sprawdza końca sektora. Ostatnie lokalne Debug i Release zaliczyły po 22/22 CTest w P021-02; podział P022 nie uruchamiał testów gry.

## Wykonaj

Jeden kontrakt: dla pełnego stanu zwróć wartościową klasyfikację sektora bez mutacji. Nazwy typu i funkcji wybiera wykonawca.

- Dodaj do GameState opcjonalny `NodeId` wyjścia. `GameState::initial` zachowuje sygnaturę i pozostawia go pustym; kopie niezależnie zachowują wyjście. Nie dodawaj fabryki sektora.
- Wynik ma cztery rozłączne możliwości: sektor trwa, ukończony, śmierć i przeciążenie. Nie zapisuj go jako kolejnego trwałego pola GameState; ma wynikać z danych.
- HP=0 zawsze daje śmierć, także na wyjściu po dostawach i przy limicie.
- Żywy gracz na istniejącym wyjściu z co najmniej dwoma zasilonymi odbiornikami wygrywa, gdy licznik tur nie przekroczył 48. Licz tylko indeksy z obiema flagami `receiver_nodes` i `powered_nodes`; osobny licznik dostaw jest zbędny. Na 48. turze sukces ma pierwszeństwo przed przeciążeniem.
- Bez sukcesu licznik co najmniej 48 oznacza przeciążenie; wcześniej sektor trwa. Stan syntetyczny po 48. turze nie może uzyskać spóźnionej wygranej. Brak wyjścia, inna pozycja i za mało dostaw nie dają sukcesu.
- Funkcja przyjmuje stałą referencję i odczytuje wyłącznie potrzebne dane; nie rozwiązuje zaległego kontaktu, dostawy ani fazy. Integracja P022-02 uruchomi ją po zagrożeniach i końcowym koszcie oraz zablokuje dalsze komendy. W tym kroku resolver i legalność komend zachowują swoje kontrakty.

Nowe pole uwzględnij w inicjalizacji, dwóch kolejnych niezależnych kopiach i wszystkich helperach `same_state`. Znajdź je przez `rg -l 'bool same_state' tests` — obecnie jest 14 helperów. W fabrykach testowych ustaw niepuste wyjście, aby dotychczasowe operacje nie mogły go po cichu zerować. Nowy test dodaj do trzech miejsc CMake wzorem testu resolvera.

## Dowód sukcesu i test błędu

Sprawdź wyjścia NodeId 0 i 48, brak wyjścia, inną pozycję, zero/jedną/dwie dostawy, dodatkowe zasilone węzły poza odbiornikami oraz granice 47/48/49 tur. Żywy gracz z dwiema dostawami na wyjściu wygrywa na 48. turze; identyczny stan z HP=0 daje śmierć, a bez sukcesu 48 i 49 dają przeciążenie. Na 47. turze bez sukcesu sektor trwa. Wygrana wymaga co najmniej dwóch odbiorników również wtedy, gdy wszystkie flagi istniejącego jednego odbiornika są zasilone.

Każde wywołanie porównuje pełny stan przed/po, z niepustymi impulsami, pakietem i pozostałymi polami. Sama obecność impulsu na graczu nie zmienia klasyfikacji, dopóki faza nie zaktualizowała HP; funkcja tylko odczytuje stan.

Celowa mutacja produkcyjna ma wykazać istotny przypadek negatywny, np. zaliczenie zasilonego węzła niebędącego odbiornikiem. Wymuś `/t:Rebuild` właściwego celu, potwierdź rekompilację i oczekiwany błąd CTest. Przywróć kod przed końcowymi testami Debug/Release.

## Nie rozszerzaj

Nie zmieniaj TurnResult, `resolve_turn`, `is_command_legal` ani niskopoziomowych helperów. Integracja wyniku i odmowa komend terminalnych należą do P022-02; nie deklaruj po tym kroku, że resolver już egzekwuje limit. Nie implementuj preview P023, generatora, runu, overdrive, rezerwy, długu, zapisu ani prezentacji. Nie generuj assetów i treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1`.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatter według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z faktem. Zapisz `history/P022-01.md`, a STATE i NEXT przestaw na P022-02 dopiero po spełnieniu kryteriów. Uruchom walidator projektu i kontrole diff, wykonaj lokalny commit własnych zmian, sprawdź hash, zakres i czystość. Zatrzymaj się przed kolejnym ID.
