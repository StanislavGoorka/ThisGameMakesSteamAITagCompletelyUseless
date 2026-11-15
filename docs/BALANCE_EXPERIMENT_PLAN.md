# Eksperyment balansu — P089

Dokument roboczy, bez treści dla gracza. P089 zamyka wyłącznie podział.
Zadanie: zmierzyć jedną hipotezę dominującej strategii na stałym korpusie,
zmienić najwyżej jeden parametr i porównać przed/po. Wynik nie jest opinią
graczy ani dowodem przyjemności z gry ([GAME_DESIGN](GAME_DESIGN.md),
[DECISIONS](DECISIONS.md): „Czekanie lub overdrive dominuje”).

## Hipoteza (ustalona przed pomiarem)

Pakiet porusza się sam po najkrótszej trasie BFS, więc gracz może
spróbować nie podejmować decyzji. Strategia **wait-then-walk**: dopóki sektor
nie ma wymaganych dostaw, wykonuj wyłącznie `wait`; potem idź po najkrótszej
legalnej trasie ruchu do wyjścia (bez obrotów, overdrive i modułów, remisy
N/E/S/W). Bot jest deterministyczny i nie czyta RNG.

H1: wait-then-walk dominuje, jeśli kończy **sektor 1 w co najmniej 25%**
runów startowanych z korpusu `sector_seeds` (1000 root seedów, konfiguracja
ID1, maska0). Próg 25% oznacza, że co czwarty sektor nie wymaga żadnej
decyzji przebudowy obiegu, co przeczy założeniu gatunku. Metryki pomocnicze
(raportowane, nie decydujące): odsetek ukończonych pełnych runów (nagroda:
pierwsza pozycja oferty), rozkład przyczyn porażki (death/overload) i średnia
tura ukończenia.

## Dzieci

| Task | Jedyna zmiana | Dowód |
| --- | --- | --- |
| P089-01 | Narzędzie developer_strategy (tools/strategy, poza paczką gry) z botem wait-then-walk i wyjściem liczbowym per seed | CTest kontraktu CLI na kilku seedach: deterministyczny, powtarzalny ślad; bot nigdy nie wysyła komendy odrzuconej przez resolver; błędne argumenty bez stdout; bez progu balansu |
| P089-02 | Pomiar bazowy na pełnym korpusie, zapis w tym dokumencie | Dokładna komenda Release, liczby H1 i metryki pomocnicze; werdykt H1 według progu powyżej |
| P089-03 | Decyzja: przy potwierdzonej H1 zmiana jednego parametru generatora lub reguł (wybór uzasadniony danymi) z pomiarem po na tym samym korpusie i regresjami (hash/replay/korpusy certyfikatów); przy odrzuconej H1 brak zmiany i zapis | Porównanie przed/po w tabeli; pełny Debug/Release; brak nowej waluty, mechanik, tekstów i assetów |

Stan: P089-01 — narzędzie developer_strategy (`SEED...` lub `--corpus`) z
testem developer_strategy_cli. P089-02 — pomiar bazowy: H1 odrzucona (niżej).
P089-03 — decyzja bez zmiany parametru; P089 zamknięte.

P090 zależy od P089-03. Kandydaci parametru (do wyboru w P089-03 tylko przy
potwierdzonej H1): orientacja początkowa węzłów tak, by trasa pakietu na
wejściu nie była otwarta, albo liczba początkowych impulsów. Zmiana reguł
wymaga jawnej decyzji o wersji replay/hash.

## Pomiar bazowy (P089-02)

Data: 2026-11-15. Build Release z worktree (commit bce0db3), bez zmian kodu.

```powershell
cmake --build --preset windows-release --parallel 8
.\build\windows-msvc\Release\developer_strategy.exe --corpus
```

Dwa uruchomienia: kod 0 (żadna komenda bota nie została odrzucona),
identyczne wyjście, 1003 wiersze, `summary 1000 2 0`. Statystyki policzono
skryptem PowerShell z wierszy wyjścia.

| Metryka | Wynik |
| --- | --- |
| Ukończony sektor 1 (H1) | 2 / 1000 (0,2%) — seedy 296 i 645 |
| Średnia tura ukończenia sektora 1 | 24 (obie próby) |
| Najdalej osiągnięty sektor | 2 (żaden run nie ukończył sektora 2) |
| Wygrane pełne runy | 0 / 1000 |
| Koniec runu | overload 1000, death 0, won 0, rejected 0 |

Werdykt: **H1 odrzucona** (0,2% < 25%). Samo czekanie prawie nigdy nie daje
dwóch dostaw: w 998 seedach trasa pakietu do odbiorników nie otwiera się bez
przebudowy obiegu, więc sektor kończy się przeciążeniem po 48 turach. Brak
śmierci oznacza, że w sektorze 1 czekający gracz nie traci integralności do
zera w limicie tur; nie jest to ocena trudności zagrożeń ani opinia graczy.
Następny krok P089-03 dokumentuje decyzję bez zmiany parametru.

## Decyzja (P089-03)

H1 odrzucona, więc zgodnie z planem nie zmieniono żadnego parametru reguł ani
generatora (wpis w [DECISIONS](DECISIONS.md)). Porównanie przed/po na tym
samym korpusie:

| Metryka | Przed (P089-02) | Po (P089-03, brak zmiany) |
| --- | --- | --- |
| Ukończony sektor 1 | 2 / 1000 | 2 / 1000 |
| Wygrane pełne runy | 0 / 1000 | 0 / 1000 |
| Koniec: overload / death / won | 1000 / 0 / 0 | 1000 / 0 / 0 |

Regresję zapewnia developer_strategy_cli (literalny ślad seedów 0/1/2/296/MAX).
Nie zmierzono strategii opartej na overdrive; to nie jest część P089.
P089 zamknięte; kolejka przechodzi do P090.
