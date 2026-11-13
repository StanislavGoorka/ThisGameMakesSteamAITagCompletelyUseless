# Następny mały krok

Task: P068-05

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P068-03.
Zależności: P068-03. Sprawdź git status i walidator.

Dodaj czysty hit-test pozycji kursora przez istniejący fit_board. Zwracaj opcjonalny NodeId; bez Win32, zmiany układu i rysowania.

Kryteria: Wszystkie 49 pól, marginesy, granice półotwarte, brak zawijania, resize i niepoprawne liczby. Dokładna zgodność z geometrią board_draws; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakty BOARD_PRESENTATION.md
i ACTION_PREVIEW_PRESENTATION.md. Za szeroki zakres rozbij bez implementacji.
Configure/build/CTest Debug i Release, formatter src/tests/tools, walidator
i diff --check. Raport, STATE/NEXT/README i osobny lokalny commit z kontrolą
hasha i zakresu. Bez nowych assetów i pobrań. Push wymaga osobnego polecenia.
