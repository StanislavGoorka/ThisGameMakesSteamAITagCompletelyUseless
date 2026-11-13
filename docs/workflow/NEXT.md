# Następny mały krok

Task: P063-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P063 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i walidator na początku.

Filtr naciśnięć i focus klawiatury. Dodaj przenośny filtr jednego naciśnięcia do czasu key-up. Utrata focus resetuje stan; powtórzenie po odzyskaniu focus nie jest nową akcją. Bez komend i Win32.

Kryteria: Pierwsze naciśnięcie, hold/repeat, release/repress, niezależne klawisze, unknown i utrata/odzyskanie focus; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure/build/CTest Debug i Release; formatter src/tests/tools.
Dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha i zakresu. Bez nowych assetów,
pobrań i pushu.
