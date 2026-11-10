# Projekt: Zamknięty Obieg

Robocza, **wewnętrzna** nazwa koncepcji; nie jest zatwierdzonym tytułem do wyświetlania w grze lub na Steam.

Taktyczny roguelike bez fabuły: gracz przełącza połączenia małej planszy. Ten sam obieg przenosi gracza, ładunki i zagrożenia. Ułatwienie dostawy może otworzyć drogę przeciwnikowi. Kolejne próby zmieniają konfiguracje i dostępne narzędzia, bez permanentnego podnoszenia statystyk.

**Windows x64 · C++20 · singleplayer · bez zewnętrznego silnika · docelowo Steam.**

[Publiczne repozytorium GitHub](https://github.com/StanislavGoorka/ThisGameMakesSteamAITagCompletelyUseless).

AI może projektować mechanikę, pisać kod i narzędzia. Nie może tworzyć treści audiowizualnych dla gracza. Treści zewnętrzne wymagają weryfikacji autorstwa, licencji i zgody przed pobraniem. Tekst roboczy zawsze zaczyna się od `[AI PLACEHOLDER]` i jest wykluczony z wydania. Nie pobrano żadnych assetów.

## Stan projektu

Ukończono planowanie P000, inwentaryzację narzędzi P001 i minimalny build P002. CMake udostępnia cel interfejsowy `game_core` z wymaganiem C++20 oraz jeden natywny test CTest. Build i test przeszły w Debug i Release na MSVC x64; potwierdzono też wykrywanie celowego błędu testu. Nie ma jeszcze mechanik ani okna gry. Wersje narzędzi opisuje [toolchain](docs/TOOLCHAIN.md), a wyniki [raport P002](docs/workflow/history/P002.md). Następne zadanie: **P003 — konfiguracja stylu**. Wystarczy polecenie **„Zrealizuj następny krok”**. Agent czyta [AGENTS.md](AGENTS.md), skill next-step i aktualne przekazanie pracy.

## Mapa dokumentacji

| Dokument | Zastosowanie |
| --- | --- |
| [Koncepcja i reguły](docs/GAME_DESIGN.md) | Pętla gry, reguły tur, zakres MVP i 1.0 |
| [Analiza gier](docs/RESEARCH.md) | Punkty odniesienia, alternatywy i uzasadnienie wyboru |
| [Architektura](docs/ARCHITECTURE.md) | Moduły C++, Windows, zapis, achievementy |
| [Toolchain](docs/TOOLCHAIN.md) | Sprawdzone lokalne wersje i ścieżki narzędzi Windows |
| [Plan etapów](docs/ROADMAP.md) | Bramki etapów i sposób rozbijania pracy |
| [Zadania](docs/plan/tasks.json) | Kolejka mikro-kroków i zależności |
| [Workflow](docs/WORKFLOW.md) | Jedno uruchomienie = jeden mały krok |
| [Stan](docs/workflow/STATE.json), [przekazanie](docs/workflow/NEXT.md) | Wykonane zadania i instrukcja dla następcy |
| [Treści i pochodzenie](docs/CONTENT_POLICY.md) | Zakaz generowania assetów, zgody i blokady wydania |
| [Kandydaci na assety](docs/ASSET_CANDIDATES.md) | Źródła do przyszłej weryfikacji, bez zgody na pobranie |
| [Testy](docs/TESTING.md) | Determinizm, błędy, QA Windows i bramki |
| [Wydanie Steam](docs/STEAM_RELEASE.md) | Integracja, ankieta i zewnętrzne warunki wydania |
| [Decyzje i ryzyka](docs/DECISIONS.md) | Przyjęte założenia i kryteria zmiany kierunku |

## Komendy działające obecnie

Z katalogu repozytorium, w PowerShell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Test-ProjectValidator.ps1
git -c safe.directory=D:/ThisGameMakesSteamAITagCompletelyUseless diff --check
$plan = Get-Content docs/plan/tasks.json -Raw -Encoding UTF8 | ConvertFrom-Json
$state = Get-Content docs/workflow/STATE.json -Raw -Encoding UTF8 | ConvertFrom-Json
$plan.tasks | Where-Object id -eq $state.current_task | Format-List
```

Walidator kontroluje kolejkę, stan, odnośniki Markdown i rejestry treści. Druga komenda sprawdza jego zachowanie na izolowanych próbkach błędów. Nie jest to audyt autorstwa ani test gry. `ExecutionPolicy Bypass` dotyczy tylko uruchamianego procesu; nie zmienia polityki systemowej. Jest potrzebne tutaj, ponieważ lokalna polityka blokuje niepodpisane skrypty.

Lokalny dysk nie zapisuje informacji o właścicielu katalogu. Opcja `git -c safe.directory=...` ustanawia wyjątek tylko dla tego katalogu i wywołania; w razie komunikatu „dubious ownership” dodawaj ją także do innych komend Git. Nie potrzeba globalnego `safe.directory=*`.

## Build i test C++20

CMake 3.26+, Visual Studio Build Tools 2022 z MSVC x64 i Windows SDK. Z katalogu repozytorium w PowerShell; generator sam wykrywa instalację VS, bez globalnych zmian PATH:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

`build/` jest ignorowany przez Git. Test sprawdza kompilację, linkowanie i działanie elementów biblioteki standardowej C++20 (`span`, ranges); nie testuje jeszcze reguł gry. Szczegóły i dowód niezerowego kodu błędu są w raporcie P002.

## Planowane komendy — jeszcze niedostępne

P004 wprowadzi presety, P005 CI. Docelowo:

```powershell
cmake --preset windows-msvc
cmake --build --preset windows-debug
ctest --preset windows-debug --output-on-failure
cmake --build --preset windows-release
ctest --preset windows-release --output-on-failure
```

Nie przedstawiaj tych komend jako wykonanych przed wdrożeniem presetów. Projekt nie wymaga Pythona. Skrypty utrzymaniowe korzystają z Windows PowerShell 5.1+. Nie ustalono jeszcze licencji własnego kodu; publiczne repozytorium nie oznacza udzielenia licencji na wykorzystanie jego zawartości.
