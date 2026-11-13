# Następny mały krok

Task: P064-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P064 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i walidator na początku.

Własność fabryki i celu Direct2D. Dodaj RAII single-threaded ID2D1Factory i ID2D1HwndRenderTarget dla istniejącego ukrytego HWND. Kontrolowane błędy i brak częściowego wyniku; bez BeginDraw, artworku i podłączenia entrypointu.

Kryteria: Rzeczywiste API: dwa niezależne cele, dokładny HWND i rozmiar pikseli, powtórki create/release, odmowa null/zniszczonego HWND bez obiektu; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure/build/CTest Debug i Release; formatter src/tests/tools.
Dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha i zakresu. Bez nowych assetów,
pobrań i pushu.
