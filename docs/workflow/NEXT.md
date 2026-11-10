# Następny mały krok

Task: P001

## Cel

Wyłącznie inwentaryzacja toolchainu Windows do `docs/TOOLCHAIN.md`. Nie zaczynaj CMake, kodu gry, okna, grafiki ani pobierania assetów. P000 jest jednorazowym pakietem planowania; następne kroki są znacznie mniejsze.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [STATE.json](STATE.json), [raport P000](history/P000.md), wpis P001 w [tasks.json](../plan/tasks.json) oraz [architekturę](../ARCHITECTURE.md).

## Wykonaj

Sprawdź rzeczywiste wersje i lokalizacje CMake, CTest, clang-format, Visual Studio, MSVC i Windows SDK, w tym `vswhere`. Ustal, czy brak polecenia wynika tylko z niezaładowanego środowiska VS. Nie instaluj i nie zmieniaj globalnego PATH. Gdy czegoś brakuje, zapisz to z konkretnym warunkiem umożliwiającym P002.

Wstępna obserwacja P000: CMake/CTest są w `C:/mingw64/bin`, CMake 3.26.4, clang-format 16.0.4, widoczny katalog Visual Studio 2022. To nie potwierdza działającego kompilatora MSVC ani SDK. Python launcher zgłosił odmowę dostępu; workflow nie wymaga Pythona. Sprawdź aktualny stan zamiast kopiować te wyniki jako własny test.

## Weryfikacja i przekazanie

Uruchom `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1` oraz rzeczywiste komendy wersji/wykrywania. Raportuj dokładne wyniki bez sekretów. Zapisz `history/P001.md`, zaktualizuj STATE i NEXT. Jeżeli narzędzia nie umożliwiają P002, wpisz blokadę; nie oznaczaj przyszłego builda jako wykonanego. Nie realizuj P002 w tym samym wznowieniu.

## Obowiązkowy commit każdego kroku

Każdy agent obowiązkowo kończy swój krok lokalnym commitem obejmującym wyłącznie własne zmiany, testy i przekazanie. Dotyczy to również kroków dokumentacyjnych, podziału zadania oraz zapisania nowej blokady. Najpierw wykonaj wymagane kontrole, potem commit, następnie sprawdź jego hash i zakres. W odpowiedzi końcowej podaj hash commitu. Nie uznawaj kroku za zakończony, jeśli commit się nie udał; podaj konkretny powód. Nie commituj cudzych zmian i nie twórz pustych commitów, gdy ponowne sprawdzenie blokady niczego nie zmieniło. Push jest osobną operacją wymagającą autoryzacji.
