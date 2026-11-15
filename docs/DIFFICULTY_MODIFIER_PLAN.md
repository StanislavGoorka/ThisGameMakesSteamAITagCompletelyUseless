# Plan modyfikatorów trudności — P087

Dokument roboczy, bez nazw ani opisów do wyświetlania w grze. P087 zamyka
wyłącznie podział. Kandydat 1.0 ma trzy modyfikatory trudności
([koncepcja](GAME_DESIGN.md)). Modyfikator zaostrza jedną istniejącą regułę;
nie dodaje waluty, akcji, modułów, przeciwników, leczenia ani permanentnych
bonusów i nie zależy od profilu ani poprzednich runów. Wszystkie są dostępne od
początku; odblokowania nie są częścią P087.

Stan: P087-01 dodało DifficultyModifiers (pusty katalog), GameState::difficulty,
start_run(seed, config, modifiers) i dziedziczenie w przejściu; state_hash v9,
replay26. P087-02: maska w bajcie31 checkpointu (run_rules17); restore/capture
ją przenoszą, a niezaimplementowane bity są odrzucane.

## Modyfikatory

Tokeny to stabilne wewnętrzne identyfikatory, nie teksty gry. Modyfikatory
są niezależnymi przełącznikami: zestaw to maska bitowa uint8, a każda
z ośmiu kombinacji jest legalna. Maska0 oznacza dokładnie obecne zachowanie.

| Bit / token | Jedyna zmieniona reguła | Bez modyfikatora |
| --- | --- | --- |
| 1 / short_turn_limit | Limit sektora 32 zatwierdzone tury (także finał): ukończenie na turze32 wygrywa, inaczej przeciążenie | 48 |
| 2 / lean_delivery | Dostawa przywraca o 1 rezerwę mniej: bazowo0, z modułem delivery_reserve1; zasilenie odbiornika i delivery_debt_relief bez zmian | 1 (2 z modułem) |
| 4 / heavy_overdrive_debt | Każdy naliczany overdrive dodaje2 długu; first_overdrive_debt nadal zwalnia pierwszy; odmowa bez mutacji, gdy dług > UINT32_MAX−2 | +1, odmowa przy UINT32_MAX |

Uzasadnienie wartości: certyfikaty korpusu (1000 seedów, `developer_corpus`
na de72292) mają 13–22 tury, więc limit32 zachowuje każdy certyfikat, a mimo to
skraca margines na błędy o jedną trzecią. Kondensatory i emitery z opóźnieniem
lub okresem >32 mogą nie zadziałać przed limitem; to akceptowany skutek, nie
osobna reguła. lean_delivery nie unieważnia modułu delivery_reserve (nadal +1
względem braku modułu). heavy_overdrive_debt nie zmienia progu3 ani reguły
spawnu na wejściu. Wartości są hipotezą projektową, nie pomiarem balansu;
korekta wymaga osobnego kroku z ADR, bez zmiany gatunku.

## Niezmienniki trwałości

- Modyfikatory żyją w `GameState::difficulty` (resolver, podgląd i
  `sector_outcome` czytają wyłącznie `GameState`), są dziedziczone przy każdym
  przejściu sektora jak loadout i ustawiane raz przez
  `start_run(seed, config, modifiers)`. `start_run(seed, config)` deleguje do
  maski0. Porównania stanu i `state_hash` obejmują pole; zmiana wersji
  hash/replay musi być jawna, z niezależnymi wektorami, bez cichej zmiany.
- Checkpoint: bajt31 (ostatni zarezerwowany) zapisuje maskę, run_rules17.
  Dopuszczalne są wyłącznie bity zaimplementowanych modyfikatorów; reguły16
  odrzucane bez nadpisania backupu. Schema1/40 bajtów zachowane. Po tym kroku
  format nie ma wolnych bajtów: każde kolejne pole wymaga schema2.
- Dług bez opłaconego impulsu nadal 0..2, bo próg3 i jedna próba spawnu się
  nie zmieniają; przy impulsie reszta 0..UINT32_MAX−3. heavy_overdrive_debt
  może przeskoczyć próg (np. dług2 → 4): granica tworzy jeden impuls i resztę1,
  co checkpoint już dopuszcza. Licznik tur checkpointu pozostaje0.
- Restart w `SessionFlow` zachowuje maskę razem z konfiguracją, chyba że
  wybrano inną. Profil nie zapisuje modyfikatorów ani ich wyboru.

## Relacja z konfiguracjami startu

Modyfikatory są ortogonalne do StartConfigId: wszystkie 3×8 kombinacji są
legalne, żadna nie zmienia topologii, seedu sektorów, ofert dla danego
loadoutu ani liczby dostaw. Znane interakcje, sprawdzane testami dzieci:

- ID2 (rezerwa0) z lean_delivery: overdrive dostępny dopiero po zdobyciu
  modułu delivery_reserve; run pozostaje wygrywalny, bo overdrive jest
  opcjonalny (certyfikaty go nie używają).
- ID3 (dług2) z heavy_overdrive_debt: pierwszy naliczany overdrive daje dług4,
  więc wejście następnego sektora tworzy jeden impuls i resztę1.
- ID3 z lean_delivery: rezerwa3 nie odnawia się bez delivery_reserve.

Modyfikatory celowo nie są równorzędne: każdy jest wyłącznie utrudnieniem.

## Achievementy

Istniejące predykaty nie zależą od modyfikatorów: run_without_overdrive nadal
wymaga braku overdrive, pochodzenie fresh_run/restored_checkpoint bez zmian.
Ewentualny predykat oparty na modyfikatorach należy do podziału P088, nie do
P087.

## Kolejność dzieci

Najpierw identyfikator i trwałość z pustym katalogiem, dopiero potem reguły,
aby żaden zapis ani restart nie zgubił po cichu wybranej maski.

| Task | Jedyna zmiana | Dowód dodatni i ujemny |
| --- | --- | --- |
| P087-01 | Silny typ `DifficultyModifiers` (maska uint8) z katalogiem zaimplementowanych bitów (pusty), pole `GameState::difficulty`, dziedziczenie w przejściu, `start_run(seed, config, modifiers)` | Maska0 daje bajtowo ten sam stan dla korpusu seedów i ID1..3; każdy bit poza katalogiem, w tym 1, 2, 4, 8 i 0xFF, odrzucony przez fabrykę; porównania i hash uwzględniają pole |
| P087-02 | Checkpoint zapisuje maskę w bajcie31; run_rules17 z niezależnymi wektorami | Round-trip maski0 dla ID1..3; nieznane bity odrzucone; reguły16 odrzucane bez nadpisania backupu; schema1/40 bajtów zachowane |
| P087-03 | Restart w `SessionFlow` używa maski runu | Restart po wyniku i po resume zachowuje maskę i konfigurację; aktywny run odmawia; maska0 bez zmian |
| P087-04 | Tylko bit1 short_turn_limit w katalogu i `sector_outcome` | Ukończenie na turze32 wygrywa, tura32 bez ukończenia to przeciążenie, 33 nigdy nie kończy; bez bitu granica48 bez zmian; podgląd zgodny z wynikiem; certyfikaty korpusu nadal ważne; checkpoint/restart zachowują bit |
| P087-05 | Tylko bit2 lean_delivery w katalogu i `deliver_packet` | Dostawa: rezerwa +0 bez modułu, +1 z delivery_reserve, limit3; zasilenie i debt_relief bez zmian; bez bitu +1/+2 jak dotąd; checkpoint/restart zachowują bit |
| P087-06 | Tylko bit4 heavy_overdrive_debt w katalogu i `overdrive_from_player` | Overdrive +2 długu; first_overdrive_debt zwalnia pierwszy; dług UINT32_MAX−1 odrzucony bez mutacji, UINT32_MAX−2 przyjęty; ID3+bit4: jeden overdrive → impuls w sektorze2 i reszta1; bez bitu +1 jak dotąd |
| P087-07 | Wybór maski w modelu sesji (application) | Na ekranie startu/wyniku cykl po maskach złożonych z zaimplementowanych bitów w rosnącym porządku, domyślnie0, niezależny od wyboru konfiguracji; w runie odmowa bez zmiany; confirm startuje wybraną konfigurację i maskę |
| P087-08 | Robocze etykiety i ekran startu | Jedna projekcja bit → ID tekstu, trzy wpisy texts.json z dokładnym prefiksem [AI PLACEHOLDER]; ekran startu/wyniku pokazuje etykiety aktywnych bitów; brak wpisu = błąd testu; public odmawia |
| P087-09 | Klawisze wyboru w rzeczywistym HWND | Strzałki góra/dół (InputKey::north/south) zmieniają maskę tylko na ekranie startu/wyniku; Enter startuje konfigurację z maską; repeat, run i strzałki lewo/prawo bez zmian |
| P087-10 | Dowód integracyjny | Dla każdego bitu i maski7 kilka root seedów kończy pełny run przez checkpoint/backup, restore każdej granicy daje identyczny stan i oferty, achievementy jak dla maski0, HWND pokazuje oznaczone etykiety; Test-PublicContent odmawia |

Każde dziecko obejmuje wyłącznie swoją zmianę. Jeśli implementacja przekracza
mikro-zakres, osobnym krokiem wolno ją dalej podzielić z zachowaniem
nadrzędnego ID i zależności konsumentów.

## Wspólne bramki

- Maska0 zachowuje dokładnie obecne zachowanie. Modyfikator nie losuje, nie
  wywołuje usług i nie zapisuje profilu. Seed sektorów nie zależy od maski.
- Każdy krok kodowy: Debug i Release, test +/−, clang-format src/tests/tools.
  Prezentacja wyłącznie przez katalog tekstów z prefiksem [AI PLACEHOLDER],
  poza publicznym buildem; stosować asset-intake. Bez nowych assetów, ikon,
  wskaźników graficznych i pobrań.
- P088 zależy od P087-10, a łańcuch obejmuje wszystkie dzieci. Sam P087 nie
  dostarcza żadnego modyfikatora.
