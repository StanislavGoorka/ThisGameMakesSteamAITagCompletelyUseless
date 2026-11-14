# Następny mały krok

Task: P083-06

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P083-05. P083-06: czytaj docs/DELAYED_CAPACITOR.md. Podłącz fazę kondensatorów po emiterach przed kosztem i wynikiem, ze wspólnym bool. Testuj preview/commit po rotacji między tickami i podczas detonacji, odmowy/terminalne bez ticka, koszt 1, kilka zagrożeń z limitem 1 HP, brak podwójnego wybuchu i śmierć przed sukcesem. Zaktualizuj wersję replay dla nowych reguł; bez generatora i UI. Zależność P083-05 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
