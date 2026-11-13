# Następny mały krok

Task: P069-02

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P069-01.
Zależności: P069-01. Sprawdź git status i walidator.

Dodaj RAII fabryki/formatu/layoutu DirectWrite dla tekstu wyłącznie z katalogu. Rysuj istniejący tekst na przekazanym celu D2D, bez własnego BeginDraw/EndDraw i bez nowego fontu. Podłącz opcjonalny napis do pojedynczej klatki AtlasRenderer, bez okna UI.

Kryteria: Rzeczywisty offscreen: pełny UTF-16 i prefiks, mierzalne piksele tekstu, brak rysowania gdy cały napis nie mieści się w obszarze, invalid target/bounds i ponowne użycie na innym celu; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakty BOARD_PRESENTATION.md
i ACTION_PREVIEW_PRESENTATION.md. Za szeroki zakres rozbij bez implementacji.
Configure/build/CTest Debug i Release, formatter src/tests/tools, walidator
i diff --check. Raport, STATE/NEXT/README i osobny lokalny commit z kontrolą
hasha i zakresu. Bez nowych assetów i pobrań. Push wymaga osobnego polecenia.
