# Następny mały krok

Task: P021-02

## Cel

Dodaj jeden wartościowy resolver zaakceptowanej tury. P021-01 dostarczyło komendy i niemutującą walidację, a P020-02 pełną fazę impulsów. P021 pozostaje ukończonym podziałem planu. Nie rozpoczynaj wyjścia/limitu P022 ani preview P023.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P021-01](history/P021-01.md), [raport podziału P021](history/P021.md), wpis P021-02 i jego zależność w [tasks.json](../plan/tasks.json), [komendy](../../src/core/command.hpp), [testy komend](../../tests/unit/command_tests.cpp), [GameState](../../src/core/game_state.hpp), [ruch gracza](../../src/core/player_movement.hpp), [obrót gracza](../../src/core/player_rotation.hpp), [wait](../../src/core/player_wait.hpp), [koszt tury](../../src/core/turn_cost.hpp), [wprowadzenie pakietu](../../src/core/packet_introduction.hpp), [ruch pakietu](../../src/core/packet_movement.hpp), [dostawę](../../src/core/packet_delivery.hpp), [fazę impulsów](../../src/core/pulse_phase.hpp), [testy fazy](../../tests/unit/pulse_phase_tests.cpp), [RNG](../../src/core/splitmix64.hpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), „Akcje” i „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) oraz [TESTING](../TESTING.md).

## Stan zastany

`Command` to `std::variant<MoveCommand, RotateCommand, WaitCommand>`. Ruch i obrót mają pole `NodeId target`; wait nie ma celu. Komendy są wartościowe, porównywalne, a `Command` nie ma przypadkowego domyślnego celu. `is_command_legal(const GameState&, const Command&)` odrzuca wszystkie warianty przy HP=0, a dla żywego gracza sprawdza ruch/obrót istniejącymi helperami na niezależnej roboczej kopii. Wait zwraca legalność bez naliczania tury. Predykat nie wprowadza pakietu ani nie uruchamia żadnej fazy. Ostatnie Debug i Release zaliczyły po 21/21 CTest; mutacja kontroli życia została wykryta i wycofana.

`move_player` i `rotate_from_player` wykonują tylko akcję, bez kontroli życia i bez kosztu. `wait_player` sprawdza życie i sam nalicza turę. `charge_turn` jest jedynym miejscem zwiększającym licznik. `resolve_pulse_phase` łączy kontakty zastane, ruch według ID i natychmiastowe kontakty ze wspólnym limitem jednego HP. Nie wolno uruchamiać dodatkowo `advance_pulses`.

Wprowadzenie, ruch i dostawa pakietu nadal są osobnymi krokami. Nie ma `TurnResult`, resolvera, wyjścia ani wyniku sektora. RNG to osobny `SplitMix64`, poza `GameState`; żadna z faz nie losuje.

## Wykonaj

Jeden kontrakt: `const GameState` i `Command` dają wartościowy `TurnResult` z niezależnym stanem oraz informacją o przyjęciu. Nazwę funkcji i pól wyniku wybiera wykonawca; bez callbacków i ogólnego systemu zdarzeń.

- Najpierw wywołaj `is_command_legal`. Odrzucenie zwraca pełny niezmieniony stan i informację o odmowie; nie wprowadza pakietu i nie uruchamia faz, kontaktu ani kosztu. Stan wejściowy pozostaje nietknięty również po sukcesie.
- Po akceptacji pracuj na własnej kopii: jedno `introduce_packet`, akcja gracza, jedno `advance_packet`, jedno `deliver_packet`, jedna pełna `resolve_pulse_phase`, końcowe `charge_turn`. Zachowaj właśnie tę kolejność.
- Move/rotate wykonuj istniejącymi helperami. Wait po walidacji nie ma efektu na planszy: nie wywołuj naliczającego koszt `wait_player` razem ze wspólnym końcowym kosztem. Jego dotychczasowy kontrakt pozostaje bez zmian.
- Każda przyjęta komenda kosztuje dokładnie jedną turę, również gdy gracz umrze w fazie impulsów. Po dostawie nie wprowadzaj następnego pakietu w tej samej turze; pojawi się na początku kolejnej zaakceptowanej tury.
- Resolver nie przyjmuje RNG i nie losuje. Nie dodawaj RNG do GameState ani nie deklaruj rollbacku generatora. Wynik domenowy wystarcza do obserwacji kolejności.

## Dowód sukcesu i test błędu

Porównuj pełny wynik z jawnym oczekiwanym stanem i sprawdzaj niezmienność wejścia dla sukcesu i odmowy. Testy muszą objąć:

- odmowę nielegalnego move/rotate i wszystkich wariantów przy HP=0, także na stanie gotowym do wprowadzenia pakietu i z impulsami, które inaczej by ruszyły lub spowodowały kontakt;
- legalne move/rotate/wait z kosztem dokładnie jednej tury i bez podwójnej fazy pakietu lub impulsów;
- obrót przed wyborem trasy pakietu i impulsu oraz kontakt po legalnym wejściu gracza na impuls;
- dostawę zachowaną także w turze śmierci od impulsu i naliczenie tej tury;
- następny pakiet dopiero na początku kolejnej zaakceptowanej tury, nie po dostawie i nie po odrzuconej komendzie;
- niezależne posiadanie kolekcji impulsów przez wynik (zmiana wyniku nie modyfikuje wejścia), także dla odmowy;
- zewnętrzny `SplitMix64`: po odrzuconych i zaakceptowanych wywołaniach dalsza sekwencja jest taka sama jak w jego kopii. To dokumentuje granicę API, nie istnienie RNG w transakcji.

Zarejestruj nowy test w trzech miejscach CMake. Celowa mutacja produkcyjna ma wykazać istotną ścieżkę negatywną, np. usunięcie odmowy lub zamianę kolejności akcji i ruchu pakietu. Wymuś `/t:Rebuild` właściwego celu i potwierdź rekompilację testu w logu. Przywróć kod przed końcowymi Debug/Release.

## Nie rozszerzaj

Nie implementuj ukończenia sektora, limitu 48 tur, wyniku runu, preview, overdrive, generatora, rezerwy, długu, zapisu i prezentacji. Nie dodawaj ogólnej magistrali zdarzeń ani nowych rodzajów zagrożeń. Nie generuj assetów lub treści dla gracza. Nie naprawiaj znanego błędu `scripts/Test-ProjectValidator.ps1` z raportów P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatter według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z faktem. Zapisz `history/P021-02.md`, a STATE/NEXT przestaw na P022 dopiero po kryteriach. Następcy pozostaw ocenę rozmiaru P022 przed implementacją i ewentualny podział, bez rozpoczynania tego zadania tutaj. Uruchom walidator i kontrole diff, wykonaj lokalny commit tylko własnych zmian, sprawdź hash, zakres i czystość. Zatrzymaj się przed P022.
