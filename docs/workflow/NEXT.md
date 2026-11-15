# Następny mały krok

Task: P086-01

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, README, START_CONFIGURATION_PLAN i raport P086. P086-01: tylko silny typ StartConfigId z katalogiem zaimplementowanych ID (wyłącznie ID1 baseline), pole RunState::start_config i start_run(seed, config); start_run(seed) deleguje do ID1. Test +: ID1 daje identyczny stan jak dotąd dla korpusu seedów. Test −: 0, 2, 3 i MAX odrzucone; porównania runu uwzględniają pole. Bez checkpointu (P086-02), restartu (P086-03), nowych konfiguracji, UI, tekstów i assetów.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
