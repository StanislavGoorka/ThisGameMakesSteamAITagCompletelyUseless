# Następny mały krok

Task: P069-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P069-02.
Zależności: P069-02. Sprawdź git status i walidator.

Podłącz istniejący developer-window-title do DirectWrite w osobnym górnym obszarze BoardView. Renderer i hit-test wspólnie uwzględniają ten obszar. Bez HUD, nowych tekstów, menu i mechanik.

Kryteria: Paint/resize/recreate i focus z rzeczywistym HWND; obszar napisu nie jest polem, wybór i preview/commit nadal zgodne. Każdy pokazany tekst zachowuje marker, public gate odrzuca placeholder; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakty BOARD_PRESENTATION.md
i ACTION_PREVIEW_PRESENTATION.md. Za szeroki zakres rozbij bez implementacji.
Configure/build/CTest Debug i Release, formatter src/tests/tools, walidator
i diff --check. Raport, STATE/NEXT/README i osobny lokalny commit z kontrolą
hasha i zakresu. Bez nowych assetów i pobrań. Push wymaga osobnego polecenia.
