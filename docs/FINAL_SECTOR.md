# Kontrakt finału — P084-02

Stan po P084-01: zaimplementowany run sześciu sektorów, jedyny finał w 6,
checkpoint schema1/rules3, hashv7/replay14. Poniższe rozdzielenie P084-05/07/01
opisuje kolejność wdrożenia; wcześniejsze wersje reguł nie są akceptowane.

Dokument roboczy, bez tekstów ani assetów gry. Implementacje P084-03..07
powstają oddzielnie; samo przyjęcie kontraktu nie aktywuje finału.

## Wariant i granica

`SectorVariant` rozróżni normal i final. Generator normalny zachowuje dokładnie
dotychczasowe wektory i dwa odbiorniki. Samodzielny generator finału korzysta
z tej samej rodziny dwóch ścieżek obwodu 0..48, dodając trzeci nieruchomy
odbiornik na indeksie 10 ścieżki (pola 34 lub 46). Odbiorniki na indeksach
4 i 8 pozostają. Trzeci otrzymuje porty obu sąsiadów ścieżki; nie dodajemy
zagrożeń, grafiki ani osobnego bossa. Pozostałe orientacje są identyczne
z normalnym generatorem tego seedu. Wzajemność krawędzi nadal wynika
z portów obu końców; początkowo obrócone segmenty mogą nie być połączone.

`GameState::delivery_requirement` będzie ograniczoną wartością 2 lub 3,
domyślnie 2. Wariant jest wyborem budowy granicy, próg jest regułą aktywnego
stanu. Nie przechowujemy drugiego wariantu w RunState ani osobnego licznika
dostaw. Liczą się wyłącznie pola mające jednocześnie receiver i powered.
Pełne porównania, hash, witness i capture muszą uwzględnić nowy próg.

Wspólny builder granicy dostaje seed sektora i jawny wariant. Start, przejście
i restore używają tego samego buildera. Dziedziczone HP/rezerwa/moduły/dług
są nakładane przez istniejących callerów. Przejście wykonuje jedną próbę
spawnu z długu; restore odtwarza już opłacony impuls bez ponownego kosztu.
P084-05 pozostawia wszystkie produkcyjne wywołania normalne.

P084-07 wprowadzi jedną stałą liczby sektorów i wybór wariantu przez
`sector_number == sector_count`, używany także przez run_phase. Obecnie
count=3; P084-01 zmieni go na 6. Nie wolno rozpoznać finału literalnym 3
w start/przejściu/restore. Zwykłe sektory zachowują wymóg dwóch dostaw.

## Wynik i determinizm

HP=0 daje death przed sukcesem. Żywy gracz na wyjściu po wymaganych dostawach
kończy sektor do tury 48 włącznie. W turze 48 sukces wyprzedza overload;
w 49 sukces jest niemożliwy. Dwie dostawy nie kończą finału, trzecia bez
wejścia na wyjście też nie. Resolver i preview nadal dzielą implementację,
a terminalna lub nielegalna akcja nie wykonuje faz ani nie pobiera tury.

Próg dopisujemy jako jeden bajt na końcu pełnego hasha v7; pozostałe pola
zachowują kolejność v6. Replay v13 wiąże migrację danych P084-08, a v14
wiąże użycie progu przez wynik sektora P084-09. Niezależny bajtowy oracle
aktualizuje literalne wektory również zwykłych stanów. Żadna nowa losowość
nie trafia do resolvera.

## Zapis

Przed aktywacją finału checkpoint schema1/run_rules1 pozostaje normalny;
capture odrzuca próg niezgodny z odtworzonym początkiem. Aktywacja finału
w P084-07 zmienia wyłącznie run_rules na 2. Układ 40 bajtów i schema1
pozostają, wariant i próg wynikają z numeru sektora oraz wersji reguł.
Rules1 jest jawnie odrzucane nawet z poprawną checksumą, bez migracji.
Odczyt poprawnego backupu rules2 po niezgodnym main musi działać bez zapisu.
Przyszłe rozszerzenie liczby sektorów do 6 wymaga kolejnego run_rules=3,
bo zmieni znaczenie sektora 3 i zakres numerów. Stare zapisy nie mogą
po cichu odtworzyć innego finału. Literalne bajty i checksumy zmieniamy
niezależnie od produkcyjnego encodera.

## Korpus dowodów

Stały niewyświetlany korpus: seedy sektorów 0..255 oraz UINT64_MAX,
obejmujący obie rodziny obwodu i granice RNG. Normalne wektory 0/1/2/MAX
pozostają zamrożone. Kopie topologii i stanów muszą być niezależne.
Developer witness obsługuje tylko dokładny baseline finału, bez solvera
ogólnego zastosowania. Każdy certyfikat przechodzi przez resolve_turn:
trzy dostawy i wyjście w <=48 zaakceptowanych akcjach. Ucięty, zmieniony,
nadmiarowy lub pochodzący z obcego stanu certyfikat ma być odrzucony.
Korpus nie dowodzi balansu, czytelności ani opinii graczy.

Integracja obejmie pełny run, wygraną/przegraną, preview, achievementy,
checkpoint/backup i istniejące testy HWND. Wszystkie kroki kodowe wymagają
pełnych CTest Debug i Release. Obserwacja pulpitu pozostaje kontrolą P104.
