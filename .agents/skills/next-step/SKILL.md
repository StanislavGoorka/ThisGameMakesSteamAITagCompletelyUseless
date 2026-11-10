---
name: next-step
description: Zrealizuj jeden maly krok tego projektu gry Windows C++ i zostaw przetestowane przekazanie. Uzywaj dla polecenia Zrealizuj nastepny krok lub kontynuacji kolejki projektu.
---

# Jeden krok projektu

Czytaj root [AGENTS](../../../AGENTS.md), [workflow](../../../docs/WORKFLOW.md), [stan](../../../docs/workflow/STATE.json), [NEXT](../../../docs/workflow/NEXT.md) i wskazany raport poprzednika. Uruchom `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1` z katalogu repo. Sprawdź cudze zmiany przez `git status`.

W [kolejce](../../../docs/plan/tasks.json) znajdź tylko `current_task` i jego zależności. Zależności muszą być completed; zamknięty wpis planistyczny nie zastępuje dzieci implementujących. Wybierz jedno gotowe zadanie. Nie realizuj całej fazy, nie łącz kolejnych ID. Jeśli zakres jest za duży, wykonaj wyłącznie rozbicie na dzieci i ustaw pierwsze dziecko jako następne; przepnij zależności konsumentów.

Ustal jeden obserwowalny rezultat i odpowiedni test błędu. Dla kodu czytaj [architekturę](../../../docs/ARCHITECTURE.md) i stosuj skill `cpp-core-step`. Gdy dotykasz assetów lub napisów, czytaj skill `asset-intake` i [politykę treści](../../../docs/CONTENT_POLICY.md). Dla paczki, ankiety lub Steam czytaj `release-audit`. Te skills znajdują się w sąsiednich folderach `.agents/skills/`; w razie braku automatycznego wykrycia odczytaj plik bezpośrednio.

Nie generuj treści dla gracza. Wyjątek to osobne napisy robocze zaczynające się od `[AI PLACEHOLDER]`, nigdy do publicznego builda. Brak assetu nie upoważnia do narysowania zamiennika w kodzie. Pobranie wymaga rzeczywistej zgody na pakiet i wersję.

Wykonaj minimalną zmianę i testy. Nie pytaj o rutynowe wybory projektowe, nie rozbudowuj zakresu. Nie uruchamiaj równoległych agentów implementujących ten krok. Jeśli wymagany warunek jest niedostępny, zapisz blokadę i nie fikcyjne ukończenie; możesz wybrać jedno niezależne gotowe zadanie.

Przy zakończeniu zapisz raport `docs/workflow/history/<ID>.md` z komendami, wynikami i ograniczeniami. Zaktualizuj STATE i NEXT, utrzymując to samo Task ID; wykonaj walidator oraz `git diff --check`. Oznacz ukończenie dopiero po kryteriach. Mały lokalny commit jest częścią kroku, jeśli można bezpiecznie wyodrębnić własne zmiany; push wymaga odpowiedniej autoryzacji. Zakończ krótkim raportem po polsku i zatrzymaj się przed kolejnym ID.
