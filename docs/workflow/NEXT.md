# Następny mały krok

Task: P083-04

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P083-03. P083-04: czytaj docs/DELAYED_CAPACITOR.md. Dodaj wyłącznie resolve_capacitor_phase(GameState&, span<DelayedCapacitor>, bool&): każdy element tick raz, tylko 1→0 może trafić według aktualnego footprint, limit wspólny. Przy HP0 i zużytym limicie wszystkie fuse nadal tykają. Testuj pełny stan i konfiguracje, rotację, kilka kolejności, brak podwójnego wybuchu. Bez resolvera i kolekcji GameState. Zależność P083-03 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
