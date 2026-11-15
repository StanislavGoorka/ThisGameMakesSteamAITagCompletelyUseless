# Następny mały krok

Task: P087-09

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, KEYBOARD_INPUT, SESSION_PRESENTATION, DIFFICULTY_MODIFIER_PLAN, raport P087-08 oraz P086-08 (wzór klawiszy wyboru). P087-09: wyłącznie mapowanie strzałek góra/dół (InputKey::north/south) na flow_.select_next_modifiers/select_previous_modifiers w src/platform/windows/run_window.hpp — tylko na ekranie startu/wyniku, obok istniejącej obsługi InputKey::east/west (około linii 186). Bez nowych typów klawiszy, reguł i tekstów.

Testy (windows_session_window, rzeczywiste HWND): na starcie VK_UP pokazuje etykietę difficulty-short-turns obok etykiety konfiguracji, VK_DOWN wraca do maski0 bez etykiet modyfikatorów i bez zmiany runu; Enter startuje start_run(seed, konfiguracja, maska) i chowa etykiety; w runie strzałki góra/dół nadal są ruchem N/S; repeat (bit30) nie zmienia wyboru; strzałki lewo/prawo nadal zmieniają tylko konfigurację. Mutacje: brak mapowania VK_UP, mapowanie aktywne w runie, Enter ignorujący maskę.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
