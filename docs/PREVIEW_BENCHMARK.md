# Pomiar podglądu P052

2026-11-13, lokalnie, Release x64 MSVC 19.44.35228, SDK 10.0.22621.0.
CPU: 12th Gen Intel(R) Core(TM) i5-12500H (rejestr CentralProcessor/0),
16 procesorów logicznych (zmienna NUMBER_OF_PROCESSORS i hardware_concurrency).
System: rejestr CurrentVersion podaje build 26300.9974, DisplayVersion 26H2.
Zapytania CIM o CPU/system odmówiły dostępu; użyto rejestru. Nie mierzono RAM,
zużycia pamięci procesu ani zegara CPU pod obciążeniem. Inne procesy nie były
wyłączane. To jeden komputer deweloperski, bez gwarancji minimum sprzętowego.

Komenda po buildzie Release, dwukrotnie:

```powershell
.\build\windows-msvc\Release\developer_preview_benchmark.exe
```

Korpus v1: te same 1000 seedów co `corpus/fixed_seeds.hpp` (0..998 i MAX).
Każdy stan powstaje przez sector_fixture. Rodzina 0: bazowa; 1: dług 3
i jeden spawn na wejściu; 2: syntetyczny stres, 49 impulsów z ID 0..48,
po jednym na węzeł, bez długu. Nie jest to rozkład stanów rzeczywistego runu.
Dla każdej rodziny komenda 0 to legalny wait, komenda 1 to nielegalny move:48
z wejścia 0. Każdy stan ma jedną rozgrzewkę oraz trzy próbki: 3000 na wiersz,
18 000 na uruchomienie. Stan wejściowy nie zmienia się między próbkami.

steady_clock mierzy pełne wywołanie preview_turn przez noinline wrapper,
w tym wartościową kopię stanu i alokacje resolvera. Hash wyniku, destrukcja
wyniku, sortowanie, tworzenie korpusu i IO są poza przedziałem pomiarowym.
Rozgrzewka sprawdza akceptację i zgodność hasha pełnego stanu z resolverem;
odmowa zachowuje stan. Wyniki trafiają do obserwowalnego digestu, identycznego
w obu uruchomieniach: 4619522390737374548. Percentyle to nearest rank po
posortowanych nanosekundach (ceil(p*N)-1). Koszt dwóch odczytów zegara nie jest
odejmowany; wartości rzędu 100 ns należy interpretować z tą rozdzielczością.

| Rodzina | Komenda | p50 ns, pomiar 1/2 | p95 ns, pomiar 1/2 |
| --- | --- | --- | --- |
| 0 bazowa | wait | 100 / 100 | 300 / 300 |
| 0 bazowa | odmowa move | 100 / 100 | 100 / 100 |
| 1 impuls długu | wait | 200 / 200 | 400 / 400 |
| 1 impuls długu | odmowa move | 100 / 100 | 200 / 200 |
| 2 stres 49 impulsów | wait | 3500 / 3500 | 4600 / 4700 |
| 2 stres 49 impulsów | odmowa move | 100 / 100 | 200 / 200 |

W tym korpusie p95 mieści się poniżej budżetu 16 ms samej symulacji.
Wynik nie obejmuje renderowania, profilu, dispatchu ani sesji UI. Nie dowodzi
grywalności i nie stanowi bramki czasowej w CTest. Test CLI sprawdza tylko
kontrakt korpusu, obecność liczb, zakończenie i odmowę nieznanych argumentów.
