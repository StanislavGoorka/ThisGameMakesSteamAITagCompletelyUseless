# Następny mały krok

Task: P084

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P083-06. P084: wyłącznie rozbij końcowy sektor na małe dzieci, ponieważ wariant trzech dostaw dotyka generatora, warunku sukcesu, runu i checkpointu oraz certyfikatu. Zachowaj istniejące P084-01 (run sześciu sektorów); nowe dzieci mogą mieć ID P084-02..06. Przepnij P084-01 na ostatnią wymaganą implementację. Czytaj GAME_DESIGN, sector_topology, sector_outcome, run_start/run_transition i run_checkpoint. Zależność P083-06 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
