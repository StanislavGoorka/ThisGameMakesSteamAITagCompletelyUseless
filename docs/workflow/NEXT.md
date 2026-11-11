# Następny mały krok

Task: P022

## Cel i ocena zakresu

Oceń zadanie „Limit i wyjście” przed implementacją. Resolver P021-02 już łączy zaakceptowaną turę, ale nie ma reprezentacji wyjścia ani wyniku sektora. Jeżeli dodanie danych, ocena zakończenia i integracja z transakcją stanowią osobne kontrakty, tym krokiem ma być wyłącznie podział P022 na małe dzieci, z kryteriami i testami, bez rozpoczynania pierwszego dziecka. Przepnij P023 na ostatnie wymagane dziecko i zachowaj porządek topologiczny. Zamknięcie rodzica planistycznego nie oznacza implementacji.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P021-02](history/P021-02.md), [podział P021](history/P021.md), wpis P022 i jego zależność w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [komendy](../../src/core/command.hpp), [resolver](../../src/core/turn_resolver.hpp), [testy resolvera](../../tests/unit/turn_resolver_tests.cpp), [dostawę](../../src/core/packet_delivery.hpp), [odbiorniki](../../src/core/packet_target.hpp), [fazę impulsów](../../src/core/pulse_phase.hpp), [koszt tury](../../src/core/turn_cost.hpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), „Plansza i stan początkowy” oraz „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`resolve_turn(const GameState&, const Command&)` zwraca wartościowy `TurnResult { GameState state; bool accepted; }`. Najpierw wywołuje `is_command_legal`; odmowa zwraca niezależną, niezmienioną kopię, bez faz i kosztu. Sukces na kopii wykonuje jedno wprowadzenie pakietu, akcję gracza, ruch pakietu, dostawę, jedną pełną fazę impulsów i końcowy `charge_turn`, także po śmierci. Wejście i komenda pozostają nietknięte. Wait nie uruchamia dodatkowo kosztującego `wait_player`. Kolejny pakiet czeka do następnej zaakceptowanej tury. Resolver nie przyjmuje RNG i nie losuje.

GameState przechowuje życie, licznik tur, flagi odbiorników i dostaw oraz wejście sektora, lecz nie wyjście ani trwały wynik sektora. `is_command_legal` wymaga żywego gracza; nie rozpoznaje ukończenia sektora i limitu. `deliver_packet` zapisuje cel jako zasilony bez kontroli flagi odbiornika; testy mają także zasilone węzły niebędące odbiornikami. Nie dodawaj kolejnego licznika dostaw bez potrzeby. Przyszłą reprezentację wyniku i warunek zatrzymania dalszych komend trzeba jasno ustalić w małym kontrakcie lub podziale, bez zmiany wizji gry.

Ostatnie lokalne Debug i Release zaliczyły po **22/22 CTest**. Mutacja usuwająca wczesne odrzucenie komendy została wykryta na stanie gotowym do wprowadzenia pakietu i z kontaktami; kod przywrócono i przebudowano.

## Kontrakt do pokrycia w implementacji lub dzieciach

- Bazowy sektor wymaga dwóch dostaw do odbiorników i żywego gracza na wyjściu. Brak wyjścia, za mało dostaw lub inna pozycja nie mogą dawać sukcesu. Dowolna flaga zasilonego węzła poza odbiornikami nie zastępuje wymaganej dostawy.
- Wynik oceniaj po zagrożeniach i końcowym naliczeniu zaakceptowanej tury. HP=0 ma pierwszeństwo przed wyjściem. Żywy gracz z wymaganymi dostawami wygrywa również na ostatniej, 48. turze; w przeciwnym razie limit oznacza przeciążenie. Samo osiągnięcie wyjścia nie pomija kontaktu.
- Ustal i przetestuj zachowanie kolejnej komendy po terminalnym wyniku. Odmowa nie uruchamia faz, nie zmienia pakietu, impulsów ani kosztu; zachowaj wartościowy wynik i niezmienność wejścia.
- Ewentualne nowe pola GameState uwzględnij w inicjalizacji, kopiowaniu i wszystkich porównaniach pełnego stanu. Wyszukaj je przez `rg -l 'bool same_state' tests` — obecnie jest 14 helperów, w tym nowe testy komend i resolvera. Nie ograniczaj aktualizacji do pierwotnych siedmiu helperów.

## Dowód sukcesu i test błędu

Kryteria muszą pokrywać wygraną na 48. zaakceptowanej turze, brak wygranej na wyjściu bez dwóch dostaw, śmierć na wyjściu mimo dostaw i limit bez dostaw. Sprawdź też granicę przed limitem, zasilony węzeł poza odbiornikami oraz zachowanie pełnego stanu po odmowie dalszej komendy. Wyniki nie mogą zależeć od przypadkowego naliczania wait dwa razy.

Przy samym podziale wystarczy walidator, kontrola dokładnego grafu i negatywne próby pominięcia dzieci na izolowanej kopii; nie pozoruj testów gry. Przy implementacji wykonaj Debug/Release i celową mutację istotnej reguły z potwierdzeniem rekompilacji właściwego testu, a potem przywróć kod.

## Nie rozszerzaj

Nie implementuj preview P023, generatora, całego runu, overdrive, rezerwy, długu, zapisu ani prezentacji. Nie twórz ogólnej magistrali zdarzeń i nowych zagrożeń. Nie generuj assetów ani treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` z raportów P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj odpowiednie kontrole według [README](../../README.md) i [TESTING](../TESTING.md), zaktualizuj README i ARCHITECTURE zgodnie z faktem. Zapisz `history/P022.md` lub raport bieżącego dziecka, jeżeli podział nastąpi osobnym krokiem; STATE i NEXT muszą wskazywać ten sam następny Task ID i rzeczywiście gotowe zależności. Po samym podziale ustaw pierwsze dziecko jako następne. Uruchom walidator i kontrole diff, wykonaj lokalny commit wyłącznie własnych zmian, sprawdź hash, zakres oraz czystość. Zatrzymaj się przed kolejnym ID.
