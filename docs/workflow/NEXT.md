# Następny mały krok

Task: P087

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README, GAME_DESIGN, START_CONFIGURATION_PLAN (wzór podziału) i raport P086-09. P087: wyłącznie podział. Zaplanuj w tasks.json oddzielne dzieci dla trzech modyfikatorów trudności: jeden modyfikator na zadanie z testem +/−, wcześniej identyfikator i trwałość (checkpoint/restart, analogicznie do P086-01..03), osobno wybór, prezentacja z etykietami [AI PLACEHOLDER] i integracja; bez wymagania nowych assetów bez zgody. Uwzględnij niezmienniki checkpointu (np. dług0..2 bez opłaconego impulsu) i relację z konfiguracjami startu. Przepnij P088 na ostatnie dziecko, ustaw pierwsze dziecko jako następne. Bez kodu, tekstów i assetów; walidator i commit.

Dla kodu w dzieciach: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
