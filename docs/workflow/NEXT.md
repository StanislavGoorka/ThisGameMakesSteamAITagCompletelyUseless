# Następny mały krok

Task: P087-07

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, SESSION_PRESENTATION, DIFFICULTY_MODIFIER_PLAN, raport P087-06 oraz P086-06 (wzór wyboru konfiguracji). P087-07: wyłącznie wybór maski modyfikatorów w SessionFlow (src/application/session_flow.hpp): selected_modifiers(run) zwraca wybór albo maskę runu (świeża sesja = maska0, restart zachowuje maskę runu), select_next_modifiers/select_previous_modifiers cyklicznie po wszystkich maskach złożonych z zaimplementowanych bitów w rosnącym porządku (0..7) wyłącznie na ekranie startu/wyniku; confirm startuje start_run(seed, selected(run), selected_modifiers(run)) i czyści oba wybory. Wybór maski jest niezależny od wyboru konfiguracji. Bez tekstów, klawiszy i HWND.

Testy (application_session_flow, seedy0/1/MAX): domyślnie0; cykl 0→1→…→7→0 i wstecz 0→7; w runie i przy nagrodzie odmowa bez zmiany; confirm startuje wybraną konfigurację z wybraną maską i czyści wybór; zmiana konfiguracji nie zmienia maski i odwrotnie; restart bez wyboru zachowuje maskę runu. Mutacje: confirm ignorujący wybór maski, cykl bez zawinięcia, wybór dozwolony w runie.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; w testach nie dereferencjonuj pustych optional (okno asercji Debug blokuje proces). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
