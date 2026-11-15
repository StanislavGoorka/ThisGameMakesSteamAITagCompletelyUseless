# Następny mały krok

Task: P086-05

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, START_CONFIGURATION_PLAN, CHECKPOINT_FORMAT i raport P086-04. P086-05: tylko ID3 charged_debt w start_config_catalog i start_run. Plan zakładał dług3, ale checkpoint bez opłaconego impulsu dopuszcza wyłącznie dług0..2, więc przyjmij rezerwę3 i dług2 i zaktualizuj tabelę w START_CONFIGURATION_PLAN (koszt: już pierwszy overdrive daje dodatkowy impuls przy wejściu sektora2). Test +: start rezerwa3/dług2/HP3/pusty loadout; jeden overdrive w sektorze1, a po przejściu dokładnie jeden impuls na wejściu i reszta0; capture/restore (literalny wektor oracle z bajtem30=3) i restart zachowują ID3. Test −: bez overdrive brak impulsu; zablokowany spawn zachowuje dług; ID1/2 bez zmian; ID0/4/MAX odrzucone (przepnij testy odmowy z ID3 na ID4). Bez UI, tekstów i zmian reguły długu.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
