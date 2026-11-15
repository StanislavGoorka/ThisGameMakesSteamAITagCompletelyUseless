# Następny mały krok

Task: P088-09

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, ACHIEVEMENT_PREDICATES oraz raport P088-08 (wzór). P088-09: wyłącznie ID11 won_heavy_overdrive_debt — AchievementId::won_heavy_overdrive_debt w achievement_catalog i MvpAchievementEvaluator: RunEnded(won) w sektorze sector_count z difficulty.contains(heavy_overdrive_debt) dodaje bit10 (0x400). Bez innych predykatów, tekstów i UI. Testy unit jak won_lean_delivery (bit tylko dla zwycięstwa z maskami4/5/6/7), dispatch z retry tylko dla ID11 (0x32 → 0x432). Zaktualizuj testy oczekujące katalogu do ID10 (profile_model 0x3ff→0x7ff, event_delivery ≤1023→≤2047 i static_assert, dispatcher pętla 11..12→12..12, invalid 1024→2048, sink raw ≥11→≥12, unlock known ≤10→≤11, profile_unlock integracja {0, 11, 255}→{0, 12, 255}). windows_difficulty_run: wymagaj 0x400 dokładnie dla masek z bitem4 (4 i 7) i porównuj resztę (bez 0x700) z maską0.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
