# Następny mały krok

Task: P018-03

## Cel

Dodaj wejście sektora do stanu oraz osobny krok początku tury, który umieszcza następny pakiet na wejściu z celem wskazanym przez `next_packet_target`. Zależność P018-02 jest ukończona i zaimplementowana. To zadanie jest ostatnim z trzech dzieci P018; P019 zależy od niego.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P018-02](history/P018-02.md), [raport P018-01](history/P018-01.md), [raport P018](history/P018.md), wpis P018-03 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [predykat celu](../../src/core/packet_target.hpp), [krok dostawy](../../src/core/packet_delivery.hpp), [krok ruchu pakietu](../../src/core/packet_movement.hpp), [testy predykatu celu](../../tests/unit/packet_target_tests.cpp), [testy kroku dostawy](../../tests/unit/packet_delivery_tests.cpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [ARCHITECTURE.md](../ARCHITECTURE.md), sekcje „Plansza i stan początkowy” oraz „Kolejność tury” w [GAME_DESIGN.md](../GAME_DESIGN.md) i [TESTING.md](../TESTING.md).

## Stan zastany

`GameState::packet` to `std::optional<Packet>` z pozycją i celem. `GameState::powered_nodes` to zapis dostaw, a `GameState::receiver_nodes` wskazuje odbiorniki; oba pola są tablicami flag indeksowanymi `NodeId`. `next_packet_target` zwraca najniższy `NodeId`, który jest odbiornikiem bez flagi zasilenia, albo pusty wynik bez odbiorników i po zasileniu wszystkich. Predykat nie czyta pakietu: aktywny, niedostarczony pakiet nie rezerwuje celu, więc wynik pozostaje ten sam aż do dostawy. `deliver_packet` zdejmuje pakiet stojący na własnym, niezasilonym celu, ustawia flagę i zwraca `true`; nie sprawdza, czy cel jest odbiornikiem. `advance_packet` przesuwa pakiet o jedną krawędź trasy. Żaden z tych kroków nie nalicza tury.

Stan nie zawiera wejścia sektora ani znacznika oczekującego pakietu i nic nie umieszcza pakietu na planszy; testy ustawiają `packet` bezpośrednio. `GameState::initial` tworzy stan bez pakietu, zasilonego celu i odbiorników. `NodeId` nie ma konstruktora domyślnego, a testy tworzą stan przez `GameState{.player_position = ...}`. CTest ma czternaście testów.

## Wykonaj

Dodaj do `GameState` wyłącznie wejście sektora. Zalecana reprezentacja to `std::optional<NodeId>`, puste w stanie początkowym: pole typu `NodeId` bez wartości domyślnej zepsułoby kompilację wszystkich testów tworzących stan inicjalizatorem desygnowanym. Inną reprezentację uzasadnij w raporcie. Zachowaj sygnaturę `GameState::initial`; testy ustawiają pole bezpośrednio, a dane wejścia dostarczy później generator.

Dodaj jeden krok w nowym pliku `src/core/`, obok kroku dostawy. Przyjmuje `GameState&` i zwraca `bool`. Gdy na planszy nie ma pakietu, stan ma wejście, a `next_packet_target` zwraca cel, krok tworzy pakiet z pozycją na wejściu i tym celem, po czym zwraca `true`. W pozostałych przypadkach zwraca `false` i nie zmienia żadnego pola stanu. Krok nie przesuwa pakietu, nie nalicza tury i nie czyta gracza, portów ani `blocked_nodes`: pakiet nie blokuje pola i może stanąć na polu gracza lub jednostki blokującej.

Decyzja o znaczniku oczekującego pakietu należy do tego zadania. [Raport P018](history/P018.md) wskazuje, że warunek „brak pakietu i istnieje niezasilony odbiornik” może wystarczyć bez dodatkowego pola. Ten sam warunek wprowadza wtedy także pierwszy pakiet sektora na początku pierwszej tury, zgodnie z regułą „startuje na wejściu sektora”. Jeśli mimo to dodasz znacznik, musi go ustawiać `deliver_packet`; tylko w tym wypadku wolno zmienić krok dostawy. Zapisz wybór i uzasadnienie w raporcie.

Rozstrzygnij i opisz w raporcie przypadek, w którym wejście jest zarazem wskazanym odbiornikiem: pakiet powstaje wtedy od razu na własnym celu.

Testy w nowym pliku `tests/unit/`, zarejestrowanym w [CMakeLists](../../CMakeLists.txt) w trzech miejscach, wzorem `core_packet_target`. Każda odmowa potwierdza, że pełny stan jest równy stanowi sprzed wywołania; każde wprowadzenie porównuje pełny stan z oczekiwanym, różniącym się wyłącznie pakietem. Sprawdź co najmniej:

- stan bez pakietu, z wejściem i niezasilonym odbiornikiem: pakiet powstaje na wejściu z celem równym najniższemu niezasilonemu odbiornikowi, bez zmiany licznika tur i reszty stanu;
- powtórzony krok oraz krok przy aktywnym pakiecie niczego nie zmieniają;
- brak wprowadzenia bez wejścia, bez odbiorników i po zasileniu wszystkich odbiorników;
- wprowadzenie nie zależy od gracza, także martwego i stojącego na wejściu, ani od jednostki blokującej na wejściu;
- sekwencję kroków w kolejności z reguł, czyli wprowadzenie, `advance_packet`, `deliver_packet` w każdej turze, dla dwóch odbiorników: w turze dostawy nie powstaje nowy pakiet i nic się nie rusza po dostawie; w następnej turze krok wprowadza pakiet z celem drugiego odbiornika, a pakiet wykonuje w niej najwyżej jeden ruch; po drugiej dostawie krok odmawia;
- wejście na skrajnych identyfikatorach 0 i 48.

Rozszerz o nowe pole testy inicjalizacji i kopiowania w `game_state_tests.cpp` oraz sześć helperów `same_state`: w testach `player_movement`, `player_rotation`, `player_wait`, `packet_movement`, `packet_delivery` i `packet_target`. Nowy plik testów potrzebuje własnego porównania pełnego stanu. Potwierdź nowe testy celowym błędem w kodzie produkcyjnym, każdy osobno: wprowadzenie mimo aktywnego pakietu, cel inny niż wynik predykatu, pozycja inna niż wejście oraz wprowadzenie bez niezasilonego odbiornika. Sprawdzaj w wyjściu builda, że plik testu został przekompilowany; [raport P018-01](history/P018-01.md) opisuje przypadek uruchomienia nieaktualnej binarki. Zapisz wynik w raporcie.

Zaktualizuj [README](../../README.md): stan projektu, opis nowego testu i listę plików w komendzie formattera. Dopisz kontrakt kroku i pola wejścia do [ARCHITECTURE.md](../ARCHITECTURE.md).

## Nie rozszerzaj

Nie zmieniaj `next_packet_target`, `advance_packet` ani reguły trasy. Nie zmieniaj `deliver_packet`, chyba że dodajesz znacznik oczekującego pakietu; w żadnym wypadku nie dodawaj do dostawy kontroli, czy cel jest odbiornikiem. Wniosek w tej sprawie zapisuje [raport P018-02](history/P018-02.md). Nie dodawaj rezerwy ani jej przyrostu za dostawę, długu, wymaganej liczby dostaw, ukończenia sektora, wyjścia, zagrożeń, impulsów na wejściu, resolvera tury, zdarzeń domenowych, achievementów, generatora ani prezentacji. Nie łącz kroków w funkcję całej tury: kolejność wywołań ustali resolver w P021, a testy składają ją ręcznie. Nie naprawiaj `scripts/Test-ProjectValidator.ps1`. Nie zaczynaj P019.

## Weryfikacja i przekazanie

Uruchom konfigurację, build i CTest w Debug i Release zgodnie z [README](../../README.md); po dodaniu nowego testu CTest powinien mieć ich piętnaście. Sprawdź formatowanie zmienionych plików C++, wykonaj walidator projektu i `git diff --check`. Zapisz wyniki w `history/P018-03.md`, zaktualizuj STATE i NEXT na P019 po przeczytaniu jego wpisu w kolejce, a następnie sprawdź hash i zakres lokalnego commitu.
