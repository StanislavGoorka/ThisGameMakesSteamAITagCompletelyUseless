# Następny mały krok

Task: P087-03

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, SESSION_PRESENTATION, DIFFICULTY_MODIFIER_PLAN, raport P087-02 oraz P086-03 (wzór restartu). P087-03: wyłącznie restart w SessionFlow (src/application/session_flow.hpp), który startuje świeży run przez start_run(seed, wybrana konfiguracja, run.sector.difficulty), więc zachowuje maskę modyfikatorów runu razem z jego konfiguracją. Bez wyboru maski (P087-07), reguł modyfikatorów, UI, tekstów i assetów.

Testy (application_session_flow): restart po wyniku i po resume zachowuje maskę i StartConfigId; aktywny run i oczekiwanie na nagrodę nadal odmawiają bez zmiany; maska0 zachowuje dokładnie stare zachowanie. Niepustą maskę w teście tworzy std::bit_cast (katalog pusty) — resume z checkpointu odrzuca ją, więc wariant po resume sprawdza maskę0, a po wyniku podrobiony bit. Mutacja: restart z maską0 musi dać czerwony test.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
