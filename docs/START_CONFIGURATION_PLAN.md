# Plan konfiguracji startu — P086

Dokument roboczy, bez nazw ani opisów do wyświetlania w grze. P086 zamyka
wyłącznie podział. Kandydat 1.0 ma trzy równorzędne konfiguracje startowe
([koncepcja](GAME_DESIGN.md)); obecnie istnieje jedna, niejawna w
`start_run(RunSeed)`. Nie dodajemy waluty, leczenia, nowych akcji, nowych
modułów ani permanentnych bonusów: konfiguracja zmienia wyłącznie wartości
początkowe istniejących pól runu i nie zależy od profilu ani poprzednich runów.
Wszystkie trzy są dostępne od początku; odblokowania nie są częścią P086.

Stan: P086-01 dodało StartConfigId, katalog {ID1}, RunState::start_config
i start_run(seed, config). Capture checkpointu odmawia ID≠1 do P086-02.

## Konfiguracje

Tokeny to stabilne wewnętrzne identyfikatory, nie teksty gry.

| ID / token | HP | Rezerwa | Dług | Loadout | Koszt względem ID1 |
| --- | --- | --- | --- | --- | --- |
| 1 / baseline | 3 | 2 | 0 | pusty | — (dokładnie obecny start) |
| 2 / preinstalled_range | 3 | 0 | 0 | ID1 rotation_range | Jeden slot zajęty od startu, więc najwyżej jedna nagroda w runie; brak rezerwy na pierwsze overdrive |
| 3 / charged_debt | 3 | 3 | 3 | pusty | Pełna rezerwa kosztem długu3, który daje dodatkowy impuls na wejściu sektora2 według zwykłej reguły długu |

Równorzędność oznacza tu: żadna konfiguracja nie dominuje innej w każdym
parametrze, każda ma jawny koszt i każda zachowuje osiągalność zwycięstwa na
korpusie seedów. To hipoteza projektowa, nie pomiar balansu ani przyjemności.
Wartości wolno skorygować osobnym krokiem z ADR, bez zmiany gatunku.
Konfiguracja nie zmienia topologii sektorów, ofert dla danego loadoutu, limitu
tur, liczby dostaw ani reguł modułów. ID2 korzysta z istniejącej reguły ID1.

## Kolejność dzieci

Najpierw trwałość identyfikatora, dopiero potem konfiguracje inne niż ID1, aby
żaden zapis ani restart nie zgubił po cichu wybranej konfiguracji.

| Task | Jedyna zmiana | Dowód dodatni i ujemny |
| --- | --- | --- |
| P086-01 | Silny typ `StartConfigId`, katalog zaimplementowanych ID (tylko1), pole `RunState::start_config` i `start_run(seed, config)`; `start_run(seed)` deleguje do ID1 | ID1 daje bajtowo ten sam stan co dotąd dla korpusu seedów; 0, 2, 3, MAX odrzucone przez fabrykę/katalog; porównania runu uwzględniają pole |
| P086-02 | Checkpoint zapisuje ID konfiguracji w dotąd zarezerwowanym bajcie; nowa wersja run_rules | Round-trip ID1, nieznane/niezaimplementowane ID i niezerowy ostatni bajt odrzucone; stare reguły odrzucane bez nadpisania backupu; schema1/40 bajtów zachowane |
| P086-03 | Restart w `SessionFlow` używa `run.start_config` | Restart po wyniku i po resume zachowuje ID; aktywny run nadal odmawia; ID1 bez zmian |
| P086-04 | Tylko ID2 preinstalled_range w katalogu i `start_run` | Start: rezerwa0, loadout {ID1}, zasięg2 od pierwszej tury; pierwsza oferta bez ID1 i najwyżej jedna nagroda; ID1 bez zmian; checkpoint/restart zachowują ID2 |
| P086-05 | Tylko ID3 charged_debt w katalogu i `start_run` | Start: rezerwa3, dług3; wejście sektora2 tworzy jeden impuls i reszta0; zablokowany spawn zachowuje dług; ID1/2 bez zmian |
| P086-06 | Wybór konfiguracji w modelu sesji (application) | Na ekranie startu/wyniku cykl po zaimplementowanych ID w kanonicznym porządku, domyślnie ID1; w trakcie runu odmowa bez zmiany; confirm startuje wybrane ID |
| P086-07 | Robocze etykiety i ekran startu | Jedna projekcja StartConfigId->ID tekstu, trzy wpisy texts.json z dokładnym prefiksem [AI PLACEHOLDER]; ekran startu pokazuje etykietę wybranej konfiguracji; brak wpisu = błąd testu; public odmawia |
| P086-08 | Klawisze wyboru w rzeczywistym HWND | Istniejące klawisze nawigacji zmieniają wybór tylko na ekranie startu/wyniku; Enter startuje wybraną konfigurację; repeat i run bez zmian |
| P086-09 | Dowód integracyjny | Dla każdego ID kilka root seedów kończy pełny run przez checkpoint/backup, restore każdej granicy daje identyczny stan i oferty, achievementy jak dla ID1, HWND pokazuje oznaczone etykiety; Test-PublicContent odmawia |

Każde dziecko obejmuje wyłącznie swoją zmianę. Jeśli implementacja przekracza
mikro-zakres, osobnym krokiem wolno ją dalej podzielić z zachowaniem
nadrzędnego ID i zależności konsumentów.

## Wspólne bramki

- Brak wyboru zachowuje dokładnie obecne zachowanie (ID1). Konfiguracja nie
  losuje, nie wywołuje usług i nie zapisuje profilu. Seed sektorów nie zależy
  od konfiguracji.
- Checkpoint sektora1 odtwarza wartości z zapisu, a nie z domyślnych stałych
  `GameState`; walidacja zakresów (HP1..3, rezerwa0..3, znane moduły) zostaje.
  Zmiana danych/reguł ma jawne wersje i niezależne literalne wektory.
- Achievementy nie zależą od konfiguracji: run_without_overdrive nadal
  wymaga braku overdrive, a pochodzenie fresh_run/restored_checkpoint
  pozostaje bez zmian. Profil nie zapisuje konfiguracji.
- Każdy krok kodowy: Debug i Release, test +/−, clang-format. Prezentacja
  wyłącznie przez katalog tekstów z prefiksem [AI PLACEHOLDER], poza publicznym
  buildem; stosować asset-intake. Bez nowych assetów, ikon i pobrań.
- P087 zależy od P086-09, a łańcuch obejmuje wszystkie dzieci. Sam P086 nie
  dostarcza żadnej nowej konfiguracji.
