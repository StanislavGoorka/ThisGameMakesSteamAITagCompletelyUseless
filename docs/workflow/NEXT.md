# Następny mały krok

Task: P068-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P068.
Czytaj także [kontrakt podglądu](../ACTION_PREVIEW_PRESENTATION.md).
Sprawdź git status, walidator i zależności: P068.

Dodaj wartościową projekcję wskazanej Command: wynik z preview_run_turn oraz trasa pakietu z istniejącego BFS na topologii wyniku. Bez wejścia, usług i rysowania.

Kryteria: Pełny wynik równy resolve_run_turn; obrót zmienia trasę, brak trasy i dostawa, nowy pakiet, odmowy i terminalne wejścia; powtórki nie zmieniają live state, usług ani RNG; D/R.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake i CONTENT_POLICY. Kontrakt BOARD_PRESENTATION.md.
Za szeroki zakres rozbij bez implementacji. Configure/build/CTest Debug i Release;
formatter src/tests/tools, walidator i diff --check. Raport, STATE/NEXT/README
i osobny commit z kontrolą hasha i zakresu. Bez nowych assetów i pobrań.
Push wyłącznie na polecenie użytkownika; bieżąca seria 10 kroków ma zgodę na push na końcu.
