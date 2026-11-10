# Następny mały krok

Task: P003

## Cel

Wyłącznie konfiguracja stylu: `.clang-format` i `.editorconfig` zgodne z architekturą. Zależność P002 jest ukończona. Bez mechanik, okna, treści gry, presetów i CI. Nie realizuj P004 w tym samym wznowieniu.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [STATE.json](STATE.json), [raport P002](history/P002.md), wpis P003 w [tasks.json](../plan/tasks.json), [architekturę](../ARCHITECTURE.md), [toolchain](../TOOLCHAIN.md), [testy](../TESTING.md) i [README](../../README.md).

## Wykonaj

Dodaj konfigurację formattera i edytora: 4 spacje w C++, styl nazw zgodny z architekturą (nie zakładaj, że sam formatter egzekwuje nazwy), spójne kodowanie i zakończenia linii. Dostępny clang-format 16.0.4. Sprawdź na małej próbce, że formatowanie poprawia rozbieżny styl, a drugie uruchomienie nie zmienia wyniku. Dane próbki trzymaj poza zawartością gry. Nie instaluj narzędzi.

P002 dodał cel interfejsowy `game_core` z wymaganiem C++20 i test `tests/unit/build_tests.cpp`. Brak jeszcze kodu reguł. CMake/CTest 3.26.4 z PATH poprawnie konfiguruje generator `Visual Studio 17 2022`, platformę `x64`, MSVC 19.44.35228 i SDK 10.0.22621.0 bez ręcznej inicjalizacji środowiska VS. Działające komendy są w README. Katalog `build/` jest ignorowany przez Git.

## Weryfikacja i przekazanie

Uruchom walidator projektu na początku i końcu. Sprawdź formatter na próbce i jego idempotencję. Jeśli przeformatujesz istniejący C++, zbuduj i uruchom CTest Debug/Release. Zapisz dokładne komendy i wyniki. Na końcu `git diff --check`, raport `history/P003.md`, aktualizacja STATE i NEXT oraz obowiązkowy lokalny commit wyłącznie własnych zmian, testów i przekazania. Sprawdź hash i zakres commitu, podaj hash w odpowiedzi. Push wymaga osobnej autoryzacji. Przy rzeczywistej blokadzie zapisz przyczynę i warunek odblokowania.
