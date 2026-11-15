# Następny mały krok

Task: P086-04

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, START_CONFIGURATION_PLAN, CHECKPOINT_FORMAT i raport P086-03. P086-04: tylko ID2 preinstalled_range w start_config_catalog i start_run: start z rezerwą0 i modułem ID1 w loadoucie, reszta jak ID1 (HP3, dług0, ta sama topologia). Test +: zasięg rotate2 od pierwszej tury, reward_offer po sektorze1 bez ID1 i po jednej nagrodzie pełny loadout bez dalszych ofert; checkpoint (nowy literalny wektor oracle z bajtem30=2) i restart w SessionFlow zachowują ID2. Test −: ID1 bez zmian; ID0/3/MAX nadal odrzucone (przepnij dotychczasowe testy odmowy z ID2 na ID3). Bez UI, tekstów i zmian reguł modułów.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
