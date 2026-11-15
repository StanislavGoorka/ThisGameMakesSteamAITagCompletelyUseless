# Następny mały krok

Task: P086-08

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, KEYBOARD_INPUT, SESSION_PRESENTATION, START_CONFIGURATION_PLAN i raport P086-07. P086-08: tylko w RunWindow::key_available, w gałęzi screen() != SessionScreen::run, InputKey::west/east wywołują flow_.select_previous/select_next(run_) i show_live (wzór wyboru nagrody, który też używa west/east). Enter startuje wybraną konfigurację przez istniejące flow.confirm; repeat nie restartuje. Test HWND (session_window_tests): na starcie VK_RIGHT zmienia etykietę na start-config-range, VK_LEFT wraca, Enter startuje start_run(seed, wybór); na wyniku wybór i restart. Uwaga: obecna lista ignored zawiera VK_RIGHT na ekranie startu/wyniku — zaktualizuj test, bo klawisz zyskuje znaczenie. Test −: w runie strzałki nadal ruszają gracza, przy nagrodzie nadal kursor nagrody; inne klawisze na starcie nadal ignorowane. Bez nowych tekstów i reguł.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
