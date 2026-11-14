# Następny mały krok

Task: P084-05

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P084-09. P084-05: jeden wspólny builder początkowego GameState dla jawnego SectorVariant, startu, przejścia i restore. Produkcyjni callerzy nadal normalni; finał tylko jawnie w testach. Testuj pełną równoważność normalnych granic i pojedynczy koszt długu. Czytaj FINAL_SECTOR, run_start, run_transition, run_checkpoint i testy. Zależność P084-09 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
