# Następny mały krok

Task: P018-02

## Cel

Dodaj odbiorniki sektora do stanu oraz predykat wskazujący cel następnego pakietu: najniższy niezasilony odbiornik według identyfikatorów. Zależność P018-01 jest ukończona i zaimplementowana. To zadanie jest drugim z trzech dzieci P018; niczego nie umieszcza na planszy.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P018-01](history/P018-01.md), [raport P018](history/P018.md), wpis P018-02 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [krok dostawy](../../src/core/packet_delivery.hpp), [testy kroku dostawy](../../tests/unit/packet_delivery_tests.cpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Stan zastany

`GameState::packet` to `std::optional<Packet>` z pozycją i celem. `GameState::powered_nodes` to tablica flag indeksowana `NodeId`: flaga oznacza cel, który otrzymał pakiet. `deliver_packet` zdejmuje pakiet stojący na własnym, niezasilonym celu, ustawia flagę i zwraca `true`; bez pakietu, poza celem i na celu już zasilonym zwraca `false` bez zmiany stanu. Krok zasila dowolny węzeł wskazany jako cel, bo stan nie zna jeszcze odbiorników. Stan nie zawiera listy odbiorników, wejścia sektora ani znacznika oczekującego pakietu. `GameState::initial` tworzy stan bez pakietu i bez zasilonego celu. CTest ma trzynaście testów.

## Wykonaj

Dodaj do `GameState` wyłącznie odbiorniki sektora. Wystarcza tablica flag indeksowana `NodeId`, wzorem `powered_nodes` i `blocked_nodes`; kolejność „według identyfikatorów” wyznacza `NodeId` pola odbiornika, zgodnie z decyzją z raportu P018. Zmiana tej reprezentacji wymaga uzasadnienia w raporcie. Zachowaj sygnaturę `GameState::initial`: stan początkowy nie ma odbiorników, a testy ustawiają pole bezpośrednio, tak jak pakiet. Dane odbiorników dostarczy później generator.

Dodaj jeden predykat w nowym pliku `src/core/`, obok kroku dostawy. Przyjmuje stan przez stałą referencję i zwraca `std::optional<NodeId>`: najniższy `NodeId`, który jest odbiornikiem i nie ma flagi w `powered_nodes`. Zwraca pusty wynik, gdy stan nie ma odbiorników oraz gdy wszystkie są zasilone. Zasilony węzeł, który nie jest odbiornikiem, nie wpływa na wynik. Predykat nie czyta pakietu, portów, `blocked_nodes` ani gracza i nie zmienia stanu.

Testy w nowym pliku `tests/unit/`, zarejestrowanym w [CMakeLists](../../CMakeLists.txt) w trzech miejscach, wzorem `core_packet_delivery`. Każdy przypadek potwierdza, że stan po wywołaniu jest równy stanowi sprzed wywołania. Sprawdź co najmniej:

- dwa odbiorniki podane w stanie w dowolnym układzie: wynikiem jest niższy `NodeId`, także gdy niższy leży na końcu wiersza, a wyższy na początku następnego;
- zasilony niższy odbiornik jest pominięty i wynikiem jest następny niezasilony;
- brak wyniku bez odbiorników i po zasileniu wszystkich;
- zasilony węzeł niebędący odbiornikiem oraz aktywny pakiet nie zmieniają wyniku;
- sekwencję z `deliver_packet`: pakiet z celem wskazanym przez predykat zostaje dostarczony, po czym predykat wskazuje drugi odbiornik, a po drugiej dostawie zwraca pusty wynik;
- skrajne identyfikatory 0 i 48.

Rozszerz o nowe pole testy inicjalizacji i kopiowania w `game_state_tests.cpp` oraz pięć helperów `same_state`: w testach `player_movement`, `player_rotation`, `player_wait`, `packet_movement` i `packet_delivery`. Nowy plik testów potrzebuje własnego porównania pełnego stanu. Potwierdź nowe testy celowym błędem w kodzie produkcyjnym, na przykład zwracaniem najwyższego odbiornika zamiast najniższego i osobno pominięciem kontroli `powered_nodes`. Mutacje wykonuj pojedynczo i sprawdzaj w wyjściu builda, że plik testu został przekompilowany: raport P018-01 opisuje przypadek uruchomienia nieaktualnej binarki. Zapisz wynik w raporcie.

Zaktualizuj [README](../../README.md): stan projektu, opis nowego testu i listę plików w komendzie formattera. Dopisz kontrakt predykatu do [ARCHITECTURE.md](../ARCHITECTURE.md).

## Nie rozszerzaj

Nie dodawaj wejścia sektora, znacznika oczekującego pakietu ani wprowadzania następnego pakietu; należą do P018-03. Nie zmieniaj `deliver_packet`, `advance_packet` ani reguły trasy; w szczególności nie dodawaj do dostawy kontroli, czy cel jest odbiornikiem. Jeśli uznasz taką kontrolę za potrzebną, zapisz to w raporcie jako wniosek dla następcy. Nie dodawaj rezerwy ani jej przyrostu za dostawę, długu, wymaganej liczby dostaw, ukończenia sektora, wyjścia, zagrożeń, resolvera tury, zdarzeń domenowych, achievementów ani prezentacji. Nie naprawiaj `scripts/Test-ProjectValidator.ps1`. Nie zaczynaj P018-03.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md); po dodaniu nowego testu CTest powinien mieć ich czternaście. Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P018-02.md`, zaktualizuj STATE i NEXT na P018-03, a następnie sprawdź hash i zakres lokalnego commitu.
