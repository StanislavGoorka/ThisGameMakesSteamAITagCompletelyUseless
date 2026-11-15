# Następny mały krok

Task: P088-08

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, ACHIEVEMENT_PREDICATES oraz raport P088-07 (wzór). P088-08: wyłącznie ID10 won_lean_delivery — AchievementId::won_lean_delivery w achievement_catalog i MvpAchievementEvaluator: RunEnded(won) w sektorze sector_count z difficulty.contains(lean_delivery) dodaje bit9 (0x200). Bez innych predykatów, tekstów i UI. Testy unit jak won_short_turn_limit (bit tylko dla zwycięstwa z maskami2/3/6/7), dispatch z retry tylko dla ID10 (0x32 → 0x232). Zaktualizuj testy oczekujące katalogu do ID9 (profile_model 0x1ff→0x3ff, event_delivery ≤511→≤1023 i static_assert, dispatcher pętla 10..12→11..12, invalid 512→1024, sink raw ≥10→≥11, unlock known ≤9→≤10, profile_unlock integracja {0, 10, 255}→{0, 11, 255}). windows_difficulty_run: wymagaj 0x200 dokładnie dla masek z bitem2 (2 i 7) i porównuj resztę z maską0.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
