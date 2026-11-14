# Następny mały krok

Task: P084-06

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P084-05. P084-06: tylko developer witness samodzielnego finału dla korpusu 0..255/MAX. Korzystaj z build_sector_state(final), wspólnego resolvera i validate_solution. Wymagaj trzech dostaw/wyjścia <=48, powtarzalności, pełnej niezmienności i odmowy fałszywych/uciętych certyfikatów. Bez aktywowania finału w runie. Czytaj FINAL_SECTOR, baseline_witness i solution_certificate. Zależność P084-05 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
