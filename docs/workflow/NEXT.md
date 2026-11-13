# Następny mały krok

Task: P068-02

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P068-01.
Zależności: P068-01. Sprawdź git status i walidator.

Dodaj czysty model trybu planowania: Tab przełącza, siedem klawiszy wybiera kandydata, Enter oddaje go do wykonania, Escape/focus loss czyści. Opcjonalny wskazany NodeId jest celem R/O; bez Win32 i wykonywania tur.

Kryteria: Wybór i anulowanie nie zmieniają RunState; Enter najwyżej raz, brak starego kandydata po zmianie kontekstu/focus; ruch bez zawijania i fallback R/O do własnego pola przy braku wskazania; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakty BOARD_PRESENTATION.md
i ACTION_PREVIEW_PRESENTATION.md. Za szeroki zakres rozbij bez implementacji.
Configure/build/CTest Debug i Release, formatter src/tests/tools, walidator
i diff --check. Raport, STATE/NEXT/README i osobny lokalny commit z kontrolą
hasha i zakresu. Bez nowych assetów i pobrań. Push wymaga osobnego polecenia.
