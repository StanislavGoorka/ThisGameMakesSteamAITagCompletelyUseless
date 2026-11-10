# Projekt: Zamknięty Obieg

Robocza, **wewnętrzna** nazwa koncepcji; nie jest zatwierdzonym tytułem do wyświetlania w grze lub na Steam.

Taktyczny roguelike bez fabuły: gracz przełącza połączenia małej planszy. Ten sam obieg przenosi gracza, ładunki i zagrożenia. Ułatwienie dostawy może otworzyć drogę przeciwnikowi. Kolejne próby zmieniają konfiguracje i dostępne narzędzia, bez permanentnego podnoszenia statystyk.

**Windows x64 · C++20 · singleplayer · bez zewnętrznego silnika · docelowo Steam.**

[Publiczne repozytorium GitHub](https://github.com/StanislavGoorka/ThisGameMakesSteamAITagCompletelyUseless).

AI może projektować mechanikę, pisać kod i narzędzia. Nie może tworzyć treści audiowizualnych dla gracza. Treści zewnętrzne wymagają weryfikacji autorstwa, licencji i zgody przed pobraniem. Tekst roboczy zawsze zaczyna się od `[AI PLACEHOLDER]` i jest wykluczony z wydania. Nie pobrano żadnych assetów.

## Stan projektu

Ukończono planowanie P000 i etap M1 (P001–P005). CMake udostępnia cel interfejsowy `game_core` z wymaganiem C++20 oraz jeden natywny test CTest. [Workflow Windows CI](.github/workflows/windows.yml) buduje i testuje Debug oraz Release na MSVC x64 po pushu do `main` i przy pull requestach. Komendy workflow sprawdzono lokalnie w obu konfiguracjach. [Raport P005](docs/workflow/history/P005.md) podaje zakres weryfikacji. Następne zadanie to **P006 — identyfikatory domeny**. Wystarczy polecenie **„Zrealizuj następny krok”**. Agent czyta [AGENTS.md](AGENTS.md), skill next-step i aktualne przekazanie pracy.

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

Wymagane są CMake 3.26+, Visual Studio Build Tools 2022 z MSVC x64 i Windows SDK. Uruchom poniższe komendy w PowerShell z katalogu repozytorium. Generator sam wykrywa instalację VS.

```powershell
cmake --preset windows-msvc
cmake --build --preset windows-debug
ctest --preset windows-debug --output-on-failure
cmake --build --preset windows-release
ctest --preset windows-release --output-on-failure
```

Obie konfiguracje korzystają z katalogu `build/windows-msvc/`, ignorowanego przez Git. Test sprawdza kompilację, linkowanie i działanie elementów biblioteki standardowej C++20 (`span`, ranges). [Raport P002](docs/workflow/history/P002.md) dokumentuje też sprawdzenie celowego błędu testu.

## Styl C++

`.clang-format` ustawia wcięcia na cztery spacje i klamry otwierające w osobnej linii. `.editorconfig` ustawia UTF-8 i LF, z CRLF dla skryptów PowerShell zgodnie z `.gitattributes`. Nazwy funkcji i zmiennych zapisuj w `snake_case`, a typów w `PascalCase`; formatter nie sprawdza nazw.

Sprawdzenie formatowania obecnych źródeł bez ich zmiany:

```powershell
clang-format --style=file --dry-run --Werror tests/unit/build_tests.cpp
```

## Narzędzia utrzymaniowe i licencja

Projekt nie wymaga Pythona. Skrypty utrzymaniowe korzystają z Windows PowerShell 5.1+. Nie ustalono jeszcze licencji własnego kodu; publiczne repozytorium nie oznacza udzielenia licencji na wykorzystanie jego zawartości.
