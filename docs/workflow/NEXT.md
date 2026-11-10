# Następny mały krok

Task: P002

## Cel

Wyłącznie minimalny build core: `CMakeLists.txt` i jeden test C++20 wykonywany przez CTest. Zależność P001 jest ukończona. Bez okna, treści gry, presetów, CI i pobierania assetów. Nie realizuj P003 w tym samym wznowieniu.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [STATE.json](STATE.json), [raport P001](history/P001.md), wpis P002 w [tasks.json](../plan/tasks.json), [toolchain](../TOOLCHAIN.md), [architekturę](../ARCHITECTURE.md) i [testy](../TESTING.md).

## Wykonaj

Dodaj najmniejszy sensowny cel core i jeden natywny test C++20 z jawnym kodem błędu, zarejestrowany w CTest. Test ma działać także w Release, bez polegania na `assert`. Wybierz generator `Visual Studio 17 2022` i platformę `x64`. Nie dodawaj mechanik ani kolejnych modułów.

P001 potwierdził CMake/CTest 3.26.4 z `C:/mingw64/bin`, clang-format 16.0.4, VS Build Tools 2022 17.14.40, MSVC x64 19.44.35228 i SDK 10.0.22621.0. CMake zgłasza generator VS 2022 i x64. Dodatkowy CMake/CTest 3.31.6-msvc6 jest w instalacji VS. Brak `cl` w zwykłym PATH wynika z niezaładowanego środowiska VS; komendy inicjalizacji procesu potomnego są w TOOLCHAIN.md. Nie zmieniaj globalnego PATH i niczego nie instaluj. P001 nie sprawdzał kompilacji ani linkowania.

## Weryfikacja i przekazanie

Uruchom walidator projektu na początku i końcu. Skonfiguruj CMake, zbuduj Debug i Release oraz uruchom CTest dla obu konfiguracji. Sprawdź celowy błąd testu: runner i CTest muszą zwrócić niezerowy kod również w Release; przywróć poprawny stan i potwierdź PASS. Zapisz dokładne komendy i kody wyjścia, nie deklaruj niewykonanych buildów. Na końcu `git diff --check`, raport `history/P002.md`, aktualizacja STATE i NEXT oraz lokalny commit tylko własnych zmian. Przy rzeczywistej blokadzie zapisz przyczynę i warunek odblokowania.

## Obowiązkowy commit każdego kroku

Każdy agent obowiązkowo kończy swój krok lokalnym commitem obejmującym wyłącznie własne zmiany, testy i przekazanie. Dotyczy to również kroków dokumentacyjnych, podziału zadania oraz zapisania nowej blokady. Najpierw wykonaj wymagane kontrole, potem commit, następnie sprawdź jego hash i zakres. W odpowiedzi końcowej podaj hash commitu. Nie uznawaj kroku za zakończony, jeśli commit się nie udał; podaj konkretny powód. Nie commituj cudzych zmian i nie twórz pustych commitów, gdy ponowne sprawdzenie blokady niczego nie zmieniło. Push jest osobną operacją wymagającą autoryzacji.
