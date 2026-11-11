# Następny mały krok

Task: P018-01

## Cel

Dodaj krok dostawy pakietu stojącego na celu. Zależność P018 jest ukończona jako wpis planistyczny: rozbił zakres na dzieci i niczego nie zaimplementował. To zadanie jest pierwszym z trzech dzieci.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P018](history/P018.md), [raport P017](history/P017.md), wpis P018-01 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [krok pakietu](../../src/core/packet_movement.hpp), [testy kroku pakietu](../../tests/unit/packet_movement_tests.cpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [ARCHITECTURE.md](../ARCHITECTURE.md), [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Stan zastany

`GameState::packet` to `std::optional<Packet>` z pozycją i celem; puste pole oznacza brak pakietu. `advance_packet` przesuwa pakiet o jedną krawędź i zwraca `false`, gdy pakietu nie ma, nie ma trasy albo pakiet stoi na celu. Pakiet na celu pozostaje na planszy: nic go jeszcze nie dostarcza. Stan nie zawiera zapisu dostaw, listy odbiorników, wejścia sektora ani znacznika oczekującego pakietu. `GameState::initial` tworzy stan bez pakietu. CTest ma dwanaście testów.

## Wykonaj

Dodaj jedną funkcję kroku dostawy w nowym pliku `src/core/`, obok kroku pakietu. Gdy pakiet stoi na swoim celu, funkcja zdejmuje go z planszy, zapisuje w stanie cel jako zasilony i zwraca `true`. W każdym innym przypadku zwraca `false` i nie zmienia żadnego pola stanu: gdy pakietu nie ma oraz gdy pakiet stoi poza celem. Ponowne wywołanie po dostawie niczego nie zmienia, bo pakietu już nie ma. Krok nie nalicza tury, nie sprawdza życia gracza i nie czyta `blocked_nodes`.

Dodaj do `GameState` wyłącznie zapis dostaw. Zapis musi wskazywać, który cel został zasilony, a nie tylko liczbę dostaw: P018-02 wybierze cel następnego pakietu jako najniższy niezasilony odbiornik. Wystarcza tablica flag indeksowana `NodeId`, wzorem `blocked_nodes`; liczba dostaw wynika z liczby flag i nie potrzebuje osobnego pola. Stan początkowy nie ma żadnego zasilonego celu.

Testy w nowym pliku `tests/unit/`, zarejestrowanym w [CMakeLists](../../CMakeLists.txt) w trzech miejscach, wzorem `core_packet_movement`. Każdy przypadek porównuje pełny stan z oczekiwanym, w tym licznik tur. Sprawdź co najmniej:

- dostawę pakietu na celu: brak pakietu, jedna nowa flaga celu, reszta stanu bez zmian;
- powtórzony krok po dostawie, pakiet poza celem i stan bez pakietu: wynik `false`, stan bez zmian;
- przejście z `advance_packet`: pakiet dochodzi korytarzem do celu, dostawa go zdejmuje, a kolejne `advance_packet` zwraca `false` i plansza zostaje bez pakietu;
- drugą dostawę do innego celu: flaga pierwszego celu pozostaje.

Zachowanie dla pakietu stojącego na celu, który jest już zasilony, wybierz sam, przetestuj i zapisz w raporcie.

Rozszerz o nowe pole testy inicjalizacji i kopiowania w `game_state_tests.cpp` oraz cztery helpery `same_state` w testach `player_movement`, `player_rotation`, `player_wait` i `packet_movement`. Nowy plik testów potrzebuje własnego porównania pełnego stanu. Potwierdź nowe testy celowym błędem w kodzie produkcyjnym, na przykład pozostawieniem pakietu na planszy po dostawie i osobno dostawą pakietu spoza celu. Zapisz wynik w raporcie.

Zaktualizuj [README](../../README.md): stan projektu, opis nowego testu i listę plików w komendzie formattera. Dopisz kontrakt kroku do [ARCHITECTURE.md](../ARCHITECTURE.md).

## Nie rozszerzaj

Nie dodawaj listy odbiorników ani wyboru celu; należą do P018-02. Nie dodawaj wejścia sektora, znacznika oczekującego pakietu ani wprowadzania następnego pakietu; należą do P018-03. Nie dodawaj rezerwy ani jej przyrostu za dostawę, długu, ukończenia sektora, wyjścia, zagrożeń, resolvera tury, zdarzeń domenowych, achievementów ani prezentacji. Nie zmieniaj `advance_packet` ani reguły trasy. Nie zaczynaj P018-02.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md); po dodaniu nowego testu CTest powinien mieć ich trzynaście. Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P018-01.md`, zaktualizuj STATE i NEXT na P018-02, a następnie sprawdź hash i zakres lokalnego commitu.
