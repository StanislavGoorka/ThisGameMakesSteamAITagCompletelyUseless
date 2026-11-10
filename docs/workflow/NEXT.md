# Następny mały krok

Task: P004

## Cel

Dodaj presety CMake dla Windows x64 i udokumentuj sprawdzone komendy w README. Zależność P003 jest ukończona. Nie zmieniaj kodu C++, mechanik ani zawartości gry. CI należy do P005.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [STATE.json](STATE.json), [raport P003](history/P003.md), wpis P004 w [tasks.json](../plan/tasks.json), [CMakeLists.txt](../../CMakeLists.txt), [toolchain](../TOOLCHAIN.md) i [README](../../README.md).

## Wykonaj

Dodaj `CMakePresets.json` z presetem konfiguracji `windows-msvc` i presetami builda oraz testów `windows-debug` i `windows-release`. Użyj generatora `Visual Studio 17 2022`, platformy `x64` i katalogu pod ignorowanym `build/`. CMake/CTest 3.26.4 z PATH obsługuje istniejący build z tym generatorem bez ręcznej inicjalizacji środowiska VS.

## Weryfikacja i przekazanie

Uruchom walidator projektu na początku i końcu. Z katalogu repozytorium sprawdź `cmake --preset windows-msvc`, `cmake --build --preset windows-debug`, `ctest --preset windows-debug --output-on-failure` i odpowiednie komendy dla `windows-release`. Dopiero po sprawdzeniu przenieś działające komendy do bieżących instrukcji README. Zapisz wyniki w `history/P004.md`, zaktualizuj STATE i NEXT, uruchom `git diff --check` i wykonaj lokalny commit wyłącznie własnych zmian. Sprawdź hash i zakres commitu. Przy rzeczywistej blokadzie zapisz przyczynę i warunek odblokowania.
