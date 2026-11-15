# Następny mały krok

Task: P088-06

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, ACHIEVEMENT_PREDICATES oraz raport P088-05 (wzór). P088-06: wyłącznie ID8 won_charged_debt — AchievementId::won_charged_debt w achievement_catalog i MvpAchievementEvaluator: RunEnded(won) w sektorze sector_count z start_config == charged_debt dodaje bit7 (0x80). Bez innych predykatów, tekstów i UI. Testy jak w P088-05 (unit: konfiguracje × powody × sektory × pochodzenia, dispatch z retry tylko dla ID8). Zaktualizuj testy oczekujące katalogu do ID7 (profile_model 0x7f→0xff, event_delivery ≤127→≤255 i static_assert, dispatcher pętla 8..12→9..12, invalid 128→256, sink raw ≥8→≥9, unlock known ≤7→≤8, profile_unlock integracja). Pełne runy: windows_start_config_run (ID3 dostaje 0x80) i windows_difficulty_run (ID3 dla masek0/4/7 dostaje 0x80; porównanie z maską0 w tej samej konfiguracji pozostaje równe).

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
