# Następny mały krok

Task: P086-09

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, START_CONFIGURATION_PLAN, raport P085-20 (wzór tests/integration/module_run_tests.cpp i tests/support/run_witness.hpp) i raport P086-08. P086-09: tylko dowód, bez zmian produkcyjnych. Dla ID1..3 kilka root seedów kończy pełny run od start_run(seed, config): każda granica przez save/read_checkpoint_file z kontrolą .bak, restore równy żywemu runowi (z start_config) i te same oferty; achievementy (dispatcher) działają jak dla ID1; HWND pokazuje oznaczone etykiety start-config-* i startuje wybór. Uwaga: certyfikaty sektorów nie uwzględniają modułów ani dodatkowego impulsu długu, więc dla ID2/ID3 dobierz seedy, które nadal kończą run, i zapisz to jako granicę dowodu, nie pomiar balansu. Test-PublicContent odmawia placeholderów. Debug/Release. Zapisz sporadyczną awarię windows_run_window z P086-08, jeśli wystąpi ponownie.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
