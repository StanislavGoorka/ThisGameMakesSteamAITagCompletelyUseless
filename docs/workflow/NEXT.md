# Następny mały krok

Task: P088-07

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, ACHIEVEMENT_PREDICATES oraz raporty P088-05/06 (wzór). P088-07: wyłącznie ID9 won_short_turn_limit — AchievementId::won_short_turn_limit w achievement_catalog i MvpAchievementEvaluator: RunEnded(won) w sektorze sector_count z difficulty.contains(short_turn_limit) dodaje bit8 (0x100). Bez innych predykatów, tekstów i UI. Testy unit: maski0..7 × powody × sektory × pochodzenia (bit tylko dla zwycięstwa z maskami1/3/5/7), dispatch z retry tylko dla ID9. Zaktualizuj testy oczekujące katalogu do ID8 (profile_model 0xff→0x1ff, event_delivery ≤255→≤511 i static_assert, dispatcher pętla 9..12→10..12, invalid 256→512, sink raw ≥9→≥10, unlock known ≤8→≤9, profile_unlock integracja). windows_difficulty_run porównuje maski z maską0 tej samej konfiguracji — dopuść i wymagaj 0x100 dokładnie dla masek z bitem1 (1 i 7).

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
