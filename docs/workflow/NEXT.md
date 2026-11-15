# Następny mały krok

Task: P088

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README, GAME_DESIGN, ACHIEVEMENT_PREDICATES, PROFILE_FORMAT, DIFFICULTY_MODIFIER_PLAN i START_CONFIGURATION_PLAN (wzór podziału) oraz raport P087-10. P088: wyłącznie podział. Zaplanuj w tasks.json dzieci dla siedmiu dodatkowych predykatów (ID6..12, do dwunastu): najpierw format profilu z 12 bitami (jawna wersja, migracja v1) i katalog zaimplementowanych ID zamiast stałych 1..5/0x1f w profilu, unlock, masce kandydatów i dispatcherze; potem dane potrzebne predykatom; następnie jeden predykat na zadanie z testem +/−, resetem/pochodzeniem historii i stabilnym ID; na końcu dowód integracyjny. Tekst i ikona pozyskiwane osobno — bez nowych tekstów i assetów. Przepnij P089 na ostatnie dziecko, ustaw pierwsze dziecko jako następne. Bez kodu; walidator i commit.

Dla kodu w dzieciach: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
