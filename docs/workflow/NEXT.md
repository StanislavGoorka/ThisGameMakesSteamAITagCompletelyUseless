# Następny mały krok

Task: P086-03

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, SESSION_PRESENTATION, START_CONFIGURATION_PLAN i raport P086-02. P086-03: tylko SessionFlow::confirm (src/application/session_flow.hpp) startuje świeży run przez start_run(run.seed, run.start_config) zamiast domyślnego; ID spoza katalogu odmawia bez zmiany runu i ekranu. Test +: restart po wyniku i po resume z checkpointu zachowuje start_config i daje stan równy start_run(seed, config). Test −: aktywny run i oczekiwanie na nagrodę nadal odmawiają; ID1 bez zmian. Bez wyboru konfiguracji (P086-06), UI, tekstów i nowych konfiguracji.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
