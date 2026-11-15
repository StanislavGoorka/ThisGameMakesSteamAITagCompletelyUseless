# Plan rozszerzenia modułów — P085

Dokument roboczy, bez nazw ani opisów do wyświetlania w grze. P085 zamyka
wyłącznie podział. Do P085-09 działają ID1..8; nagrody nadal1/2.
Dalsze dzieci czekają w kolejce.
Docelowy katalog ma najwyżej 12 typów i nadal dwa kanoniczne sloty na run.
Nie dodajemy waluty, leczenia, permanentnych bonusów ani nowych akcji/UI.

## Kolejność i osiągalność

P085-01 ustala kontrakt modeli, reguł, resetów, wersjonowania i dowodów.
P085-02 przygotowuje jeden katalog znanych ID, walidację loadoutu i kodeków.
ID1/2 zachowują znaczenie; ID3..12 otrzymają pojedyncze dzieci poniżej.
Nie wolno zaakceptować niezaimplementowanego ID przez decoder ani add.
Nowe mechaniki najpierw są samodzielną logiką core: pula nagród zachowuje
ID1/2 do P085-14. Rejestr odróżnia zaimplementowane ID od dopuszczonych ofert.

Obecna oferta najniższych nieposiadanych ID po zapełnieniu dwóch slotów
nie pozwoliłaby zdobyć ID4..12. Dlatego P085-03 przygotuje deterministyczny
wybór najwyżej dwóch różnych, nieposiadanych ID z dopuszczonej puli,
zależny od root seedu i numeru ukończonego sektora. Bez trwałego RNG;
wynik w kanonicznym porządku. Pełny loadout daje pustą ofertę. UI, wybór
i walidacja nagrody muszą używać tej samej oferty. Dla obecnej puli dwóch
typów zachowujemy stare oferty i istniejące regresje. Syntetyczny korpus
indeksów puli1..12 sprawdzi osiągalność każdego indeksu, niezależnie od
tego, czy przyszłe ID są już zaimplementowane. Nie są to assety gry.

P085-14 doda osobno projekcję/oznaczone etykiety dziesięciu gotowych reguł
i dopiero wtedy dopuści je do nagród. Żadnych finalnych napisów, ikon,
pobrań ani nowych assetów. Robocze etykiety wyłącznie przez katalog,
z dokładnym prefiksem [AI PLACEHOLDER], poza publicznym buildem.
Stosować asset-intake dla tego zadania. Całość pozostaje developerska.

## Jedna reguła na dziecko

Tokeny poniżej to stabilne wewnętrzne identyfikatory, nie teksty gry.

| Task | ID / token | Jedyny efekt | Dowód dodatni i ujemny |
| --- | --- | --- | --- |
| P085-04 | 3 / delivery_reserve | Rzeczywista dostawa odnawia 2 rezerwy zamiast 1, nadal maksymalnie3 | Rezerwa0/1/2/3; brak i duplikat dostawy niczego nie odnawia; bez modułu +1 |
| P085-05 | 4 / first_overdrive_debt | Pierwszy zaakceptowany overdrive sektora dodaje0 długu; następne +1 | Pierwszy/kolejny, MAX, odmowa, preview i reset na granicy; rezerwa i tura nadal kosztują |
| P085-06 | 5 / packet_stride | Aktywny pakiet może przejść do2 krawędzi na zaakceptowaną turę | Trasa0/1/2/3 krawędzi, brak trasy, dostawa tylko raz; drugi pakiet dopiero następna tura |
| P085-07 | 6 / pulse_cadence | Ruch impulsów tylko w parzystej zaakceptowanej turze; kontakty nadal w każdej | Tury1/2/47/48, kontakt na graczu także w nieparzystej, odmowa bez faz, wspólny limit1HP |
| P085-08 | 7 / powered_pulse_barrier | Impuls czeka, gdy wybrany krok BFS jest zasilonym odbiornikiem | Powered+receiver blokuje krok; każda flaga osobno nie blokuje; bez szukania alternatywnej trasy |
| P085-09 | 8 / entry_insulation | W pierwszej zaakceptowanej turze sektora kontakt impulsu na wejściu nie zadaje HP | Impuls usunięty, dług nie zwrócony; poza wejściem/druga tura bez ochrony; emiter/kondensator nadal może zranić |
| P085-10 | 9 / delivery_debt_relief | Rzeczywista dostawa odejmuje1 istniejącego długu, do minimum0 | Dług0/1/MAX, duplikat bez ulgi; overdrive i dostawa tej samej tury zachowują kolejność |
| P085-11 | 10 / overdrive_range | Tylko overdrive ma zasięg Manhattan większy o1 | Samodzielnie2, z rotation_range3; rotate bez zmiany, granice i nieruchome cele chronione |
| P085-12 | 11 / reverse_rotation | Zwykły rotate obraca przeciwnie do wskazówek; overdrive nadal o180 stopni | Wszystkie16 masek, cykl4, jedna zgoda terminal_rotation; odmowa zachowuje stan i koszt |
| P085-13 | 12 / packet_priority | Następny cel to najwyższy niezasilony odbiornik zamiast najniższego | Kilka celów, puste/zasilone, niecelowe powered; aktywny pakiet zachowuje cel |

Każde dziecko obejmuje wyłącznie swój ID i jedną regułę; bez innych modułów
i bez prezentacji. P085-01 uszczegółowi granice API, w tym darmowy pierwszy
overdrive przy MAX długu (brak inkrementacji), definicję parzystej tury jako
turns_taken+1 i jeden koszt uprawnienia przy odwrotnym obrocie terminala.
Jeśli dana implementacja przekracza mikro-zakres, osobnym krokiem wolno ją
dalej podzielić z zachowaniem nadrzędnego ID i zależności konsumentów.

## Wspólne bramki

P085-05 rozbito dalej: P085-15 dodaje wyłącznie domyślnie pusty znacznik,
reset i ścisłą rekonstrukcję/hash; nie ustawia go jeszcze podczas akcji.
P085-16 aktywuje ID4 i ustawianie znacznika po każdym udanym overdrive,
również bez modułu. Rozdział oddziela zmianę kontraktu wartościowego od reguły
kosztu. P085-06 zależy od P085-16. Zamknięty podział nie dostarcza ID4.

- Dwa sloty, stabilne ID i porządek pozostają. Pula ofert nie zawiera duplikatu,
  posiadanego ani niedopuszczonego ID. Pełna pula nie jest pełnym loadoutem.
- Brak modułu zachowuje dokładne stare zachowanie. Każda odmowa/preview chroni
  pełny stan; zaakceptowana akcja nadal pobiera jedną turę. Moduły nie losują
  podczas resolvera, nie resetują kosztu i nie uruchamiają usług.
- Dla nowego trwałego pola: niezależne kopie, pełne porównania/hash, jawny reset
  i ścisłe capture/restore. Zmiana danych/reguł ma jawne wersje hash/replay/
  checkpoint i niezależne literalne wektory. Schema1 jest zachowana tylko,
  jeśli wszystkie potrzebne informacje da się nadal odtworzyć bez straty.
- Każdy krok kodowy: Debug i Release, test +/− oraz interakcja z istniejącym
  modułem w dwóch slotach. Końcowa integracja sprawdza seed/offers, pełny run,
  checkpoint/backup, achievementy, HWND i publiczną odmowę placeholderów.
- P086 zależy od P085-14, a łańcuch obejmuje wszystkie wymagane dzieci.
  Sam P085 nie oznacza gotowego katalogu12. Balans i przyjemność pozostają
  hipotezami do osobnych pomiarów; nie wykonano playtestu.
