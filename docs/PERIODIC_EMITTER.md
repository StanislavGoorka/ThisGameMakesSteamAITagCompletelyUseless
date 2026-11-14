# Logiczny emiter okresowy

Kontrakt P082-01. Dokument roboczy, bez treści do wyświetlania graczowi.
Nie wprowadza nowego assetu, emisji dźwięku, spawnu ani zmiany generatora.
P082 jest podziałem planu; implementacje mają osobne ID.

## Wartość i harmonogram

`PeriodicEmitter` posiada własne wartości: `EntityId id`, `NodeId position`,
okres `period` 2–48, offset `phase` 0–period−1 i zasięg `range` 1–48.
Fabryka sprawdza pełne wejściowe liczby przed zawężeniem, zwraca brak wartości
przy błędzie i niczego nie zmienia. Typ nie ma publicznego niepoprawnego
konstruktora. Kopia nie dzieli mutowalnych danych. ID są unikalne w kolekcji
emiterów; przestrzeń ID impulsów jest osobna. Emiter nie przemieszcza się,
nie zajmuje pola blokującego ruch i nie znika po trafieniu.

Harmonogram jest odczytem z liczby już wykonanych tur `t = turns_taken`.
W najbliższej zaakceptowanej akcji emiter odpala, gdy
`((t % period) + 1) % period == phase`. To numer nadchodzącej akcji modulo
okres. Nie wykonuj `t + 1` w uint32: dla UINT32_MAX byłby overflow.
Pochodna faza nie potrzebuje licznika w stanie ani resetu w środku sektora.
Okres co najmniej 2 zapewnia turę przerwy; limit 48 odpowiada limitowi sektora.
Zasięg 48 pokrywa najdłuższą prostą ścieżkę grafu 49 pól.

| period | phase | t=0 | t=1 | t=2 | t=3 | t=4 | t=5 |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 3 | 0 | nie | nie | tak | nie | nie | tak |
| 3 | 1 | tak | nie | nie | tak | nie | nie |
| 3 | 2 | nie | tak | nie | nie | tak | nie |
| 2 | 0 | nie | tak | nie | tak | nie | tak |

Predykat działa także dla UINT32_MAX jako czysty odczyt. Resolver nadal
odrzuca zakończony sektor przed fazami; predykat nie daje dodatkowej tury.

## Zasięg i przewidywany wynik

Footprint to zbiór pól osiągalnych od pozycji emitera w najwyżej `range`
wzajemnych krawędziach N/E/S/W. Obejmuje źródło w odległości 0. BFS zwraca
49 flag według NodeId; cykle i rozgałęzienia nie duplikują pól. Jednostronny
port, brak portu i krawędź siatki zatrzymują połączenie; brak zawijania.
`blocked_nodes`, impulsy, pakiet i gracz nie zatrzymują propagacji. Zasięg
nie jest odległością Manhattan przez niepołączone pola ani prostym promieniem.

Predykat footprint czyta wyłącznie porty i wartość emitera, bez mutacji.
Odczyt nie zależy od harmonogramu: można go osobno badać w turze przerwy.
Trafienie wymaga jednocześnie aktywnej fazy i gracza w footprint.
Po obrocie używana jest nowa topologia, jak dla pakietu i pościgu.
Przykład: źródło 0, połączenia 0–1–2, range=1 obejmuje dokładnie 0/1;
range=2 obejmuje 0/1/2. Odłączenie wzajemnego portu 1–2 usuwa pole 2.

## Faza obrażeń i integracja

Docelowy porządek zaakceptowanej tury: akcja, ruch/dostawa pakietu,
kontakty i ruch impulsów, emitery, jeden koszt tury, wynik sektora.
Wszystkie zagrożenia współdzielą jeden `bool damage_applied` utworzony
przez resolver. Emiter odbiera 1 HP tylko przy HP>0 i świeżym limicie,
następnie ustawia limit. Gdy limit jest wykorzystany lub HP=0, nie odejmuje
ani nie zmienia limitu. Pozostałe emitery nie zmieniają wyniku. Kolejność
przechowywania emiterów nie wpływa na efekt, ponieważ nie poruszają się
ani nie zmieniają grafu. Pakiet i inne jednostki nie otrzymują obrażeń.

Helper fazy przyjmuje jawny span emiterów i zewnętrzny limit. Zmienia
wyłącznie HP i ewentualnie limit, bez zwiększania tury i zmiany konfiguracji.
Osobny krok włączy go do resolvera. Dotychczasowy jednoargumentowy helper
impulsów zachowa swój kontrakt przez wrapper z lokalnym limitem.

Preview używa tego samego resolvera na kopii. Przewidywane trafienie po
akcji jest dokładnie wynikiem commit tej akcji, nie starym zamiarem sprzed
rotacji. Odmowa i terminalna komenda zachowują cały stan bez faz i kosztu.
Śmierć od emitera ma pierwszeństwo przed sukcesem na wyjściu.

## Granice wdrożenia i dowody

P082-02 testuje wartość, odrzucone liczby i harmonogram z powyższej tabeli,
period=2/48, phase=0/period−1 i UINT32_MAX. P082-03 testuje dokładne zbiory
pól, zakres 1/48, cykle, rozgałęzienia, jednostronne porty i obrót. P082-04
testuje aktywną/przerwaną fazę, zasięg, wiele emiterów, HP0 i wspólny limit,
chroniąc pełny pozostały stan oraz dane wejściowe.

P082-05 dodał kolekcję do GameState, pełnych porównań i kanonicznego hasha
v5. Checkpoint z obecnego generatora odtwarza pustą kolekcję;
capture odrzuca obcy nieodtwarzalny stan, zamiast cicho gubić emiter.
Format zapisu nie przechowuje arbitralnej planszy ani mid-sector countdown.
Wersję replay zmieniamy jawnie przy zmianie stanu/reguł i chronimy wektorami.
P082-06 podłączył resolver, wspólny limit i regresje preview/replay/odmów.
Replay v10 wiąże nowe reguły; dotychczasowy helper impulsów zachowuje wrapper.
P083-05 rozszerzył stan o kondensatory (hash v6), a P083-06 podłączył ich fazę:
aktualnie obowiązuje replay v12.
Generator i prezentacja nadal nie zawierają emiterów.
Rozmieszczenie i prezentacja wymagają przyszłego osobnego zakresu i dowodów;
te kroki nie mogą wyświetlić niezatwierdzonego zastępczego symbolu.
