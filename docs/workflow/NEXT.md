# Następny mały krok

Task: P086-06

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, SESSION_PRESENTATION, START_CONFIGURATION_PLAN i raport P086-05. P086-06: tylko application — SessionFlow przechowuje wybór StartConfigId. Proponowany kontrakt: select_next/select_previous cyklicznie po start_config_catalog w kanonicznym porządku, dozwolone wyłącznie na ekranie startu i wyniku (w runie i przy nagrodzie odmowa bez zmiany); selected(run) zwraca wybór lub run.start_config, gdy wyboru nie zmieniano; confirm startuje start_run(run.seed, selected(run)) i czyści wybór. Test +: cykl 1→2→3→1 i odwrotnie, confirm startuje wybrane ID, restart bez zmiany wyboru zachowuje ID runu (P086-03). Test −: odmowa w runie i przy nagrodzie bez zmiany wyboru; domyślny pierwszy start = ID1. Bez tekstów, renderingu i klawiszy (P086-07/08).

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
