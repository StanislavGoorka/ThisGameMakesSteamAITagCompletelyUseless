# Następny mały krok

Task: P088-02

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, PROFILE_FORMAT oraz raport P088-01. P088-02: wyłącznie AchievementId 6..12 (tokeny z planu) i katalog zaimplementowanych ID (na razie 1..5) zamiast stałych 1..5/0x1f w is_achievement_id_valid (profile_unlock.hpp), is_candidate_mask_valid (event_delivery.hpp) i pętli deliver w event_dispatcher.hpp. Bez predykatów. Testy: zachowanie ID1..5 bez zmian; ID0, 6..12 i 13 odrzucone przez unlock i LocalAchievementSink; maska kandydatów z bitem5 nieprawidłowa (dispatcher invalid_candidates); profil v2 z bitem6 poprawny, ale is_achievement_unlocked(ID6) fałsz. Mutacja: katalog z ID6 musi zostać wykryta.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
