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
testem developer_strategy_cli. Następny jest P089-02 (pomiar bazowy).

P090 zależy od P089-03. Kandydaci parametru (do wyboru w P089-03 tylko przy
potwierdzonej H1): orientacja początkowa węzłów tak, by trasa pakietu na
wejściu nie była otwarta, albo liczba początkowych impulsów. Zmiana reguł
wymaga jawnej decyzji o wersji replay/hash.
