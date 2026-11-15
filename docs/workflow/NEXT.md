# Następny mały krok

Task: P086-02

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, CHECKPOINT_FORMAT, START_CONFIGURATION_PLAN i raport P086-01. P086-02: tylko zapis StartConfigId w dotąd zarezerwowanym bajcie30 checkpointu (bajt31 nadal 00) i jawna nowa wersja run_rules16 z niezależnymi wektorami (tools/checkpoint/vectors.cjs). RunCheckpoint otrzymuje pole konfiguracji; restore_checkpoint ustawia RunState::start_config z zapisu, capture nadal porównuje. Test +: round-trip ID1 przez kodek i plik. Test −: ID0/2/3/MAX, niezerowy bajt31 i reguły15 odrzucone bez nadpisania backupu; schema1/40 bajtów zachowane. Bez restartu (P086-03), nowych konfiguracji, UI i tekstów.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
