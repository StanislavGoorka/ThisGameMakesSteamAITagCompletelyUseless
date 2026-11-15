# Następny mały krok

Task: P088-05

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, ACHIEVEMENT_PREDICATES oraz raport P088-04. P088-05: wyłącznie ID7 won_preinstalled_range — AchievementId::won_preinstalled_range w achievement_catalog i MvpAchievementEvaluator: RunEnded(won) w sektorze sector_count z start_config == preinstalled_range dodaje bit6 (0x40) do dotychczasowej maski zwycięstwa (16/20). Bez innych predykatów, tekstów i UI. Testy: zwycięstwo ID2 tak (fresh_run i restored_checkpoint); ID1/ID3 i przegrana ID2 nie; wynik ID1..5/6 bez zmian; dispatch z retry tylko dla ID7. Zaktualizuj testy oczekujące katalogu do ID6 (profile_model static_assert, event_delivery ≤63, dispatcher pętla 7..12 → 8..12, invalid 64 → 128, sink raw ≥7 → ≥8, unlock known ≤6 → ≤7, profile_unlock integracja). Pełne runy ID2 (windows_start_config_run) porównują maski z ID1 — uwzględnij bit ID7 jawnie dla ID2.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
