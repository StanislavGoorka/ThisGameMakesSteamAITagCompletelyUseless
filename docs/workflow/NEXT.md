# Następny mały krok

Task: P069-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P069.
Zależności: P069. Sprawdź git status i walidator.

Wydziel wspólny owning odczyt tekstu katalogu i ścisłą konwersję Win32 UTF-8/UTF-16; użyj go także dla tytułu NativeWindow. Bez DirectWrite, nowych wpisów tekstu i fallbacku ID.

Kryteria: Dokładny prefiks, polskie znaki, pary surogatów, invalid UTF-8, brak ID, niezależność kopii oraz niezmieniony katalog i tytuł okna; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakty BOARD_PRESENTATION.md
i ACTION_PREVIEW_PRESENTATION.md. Za szeroki zakres rozbij bez implementacji.
Configure/build/CTest Debug i Release, formatter src/tests/tools, walidator
i diff --check. Raport, STATE/NEXT/README i osobny lokalny commit z kontrolą
hasha i zakresu. Bez nowych assetów i pobrań. Push wymaga osobnego polecenia.
