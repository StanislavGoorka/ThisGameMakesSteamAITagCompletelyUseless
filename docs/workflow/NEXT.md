# Następny mały krok

Task: P088-04

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, ACHIEVEMENT_PREDICATES oraz raport P088-03. P088-04: wyłącznie ID6 swift_sector — dodaj AchievementId::swift_sector do achievement_catalog (local_profile.hpp) i do MvpAchievementEvaluator: SectorCompleted z key.turn ≤ 16 daje bit5 (0x20) obok first_sector. Bez innych predykatów, tekstów i UI. Testy: tura16 kwalifikuje, tura17 nie; brak ukończenia nie; każdy sektor 1..sector_count; origin restored_checkpoint i fresh_run oba kwalifikują; dispatch z retry do sinka (RecordingSink, fail_id = swift_sector) i LocalAchievementSink zapisuje bit5 w profilu v2. Zaktualizuj oczekiwania testów, które zakładały katalog 1..5 (profile_model static_assert, event_delivery, dispatcher dla ID6, sink dla raw 6) — ID6 staje się prawidłowe, 7..12 nadal nie. ACHIEVEMENT_PREDICATES: wiersz ID6.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
