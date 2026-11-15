# Następny mały krok

Task: P085-18

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P085-17. P085-18: tylko testy i wsparcie testów. W run_tests, reward_window_tests, event_dispatcher_tests, checkpoint_window_tests i support/mvp_window.hpp wybieraj nagrodę z offered_run_modules dla bieżącego runu zamiast module_offer w domyślnym kontekście lub stałych ID1/2 (poza jawnymi fixture slotów). Te same pełne stany, pliki i zdarzenia przy puli1/2. Bez zmian produkcyjnych, puli i wersji; oba buildy.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Bez nowych assetów i finalnych napisów. Obserwacja pulpitu i percepcyjna czytelność w P104.
