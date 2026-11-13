# Następny mały krok

Task: P068-06

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P068-05.
Zależności: P068-05. Sprawdź git status i walidator.

Połącz istniejący hit-test i ActionSelection z wiadomościami okna. Tab/Enter/Escape i focus; R/O używają pola kursora. W trybie planowania dopiero Enter wykonuje istniejący resolver. Bez prezentacji preview.

Kryteria: Rzeczywiste HWND: hover, marginesy, resize, mouse leave, focus i repeat; sąsiedni cel R/O, brak mutacji przed Enter, jedno zatwierdzenie, anulowanie i pełna zgodność z core; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakty BOARD_PRESENTATION.md
i ACTION_PREVIEW_PRESENTATION.md. Za szeroki zakres rozbij bez implementacji.
Configure/build/CTest Debug i Release, formatter src/tests/tools, walidator
i diff --check. Raport, STATE/NEXT/README i osobny lokalny commit z kontrolą
hasha i zakresu. Bez nowych assetów i pobrań. Push wymaga osobnego polecenia.
