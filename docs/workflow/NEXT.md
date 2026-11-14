# Następny mały krok

Task: P082-06

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P082-05. Dodaj P082-06: fazę emitera do resolvera po impulsach, ze wspólnym limitem obrażeń i zachowaniem starego helpera impulsów. Testuj preview/commit po rotacji, odmowy, śmierć przed sukcesem, koszt 1 oraz deterministyczny replay. Bez generatora i UI. Czytaj docs/PERIODIC_EMITTER.md; zależność P082-05 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
