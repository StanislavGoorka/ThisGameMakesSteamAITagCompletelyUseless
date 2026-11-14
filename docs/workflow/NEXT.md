# Następny mały krok

Task: P084-08

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P084-04. P084-08: tylko ograniczony wymóg dostaw 2/3, pole stanu, porównania, hashv7/replay13 i ochrona checkpointu/baseline witness. Nie zmieniaj jeszcze sector_outcome; produkcyjne granice normalne. Czytaj FINAL_SECTOR, game_state, state_hash/hash_vectors, run_checkpoint, baseline_witness i testy replay. Zależność P084-04 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
