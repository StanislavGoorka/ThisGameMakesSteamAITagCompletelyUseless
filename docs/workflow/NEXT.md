# Następny mały krok

Task: P063-03

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P063-02 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i walidator na początku.

Adapter komunikatów klawiatury Win32. Połącz WM_KEYDOWN/UP i focus z filtrem oraz mapowaniem. Okno udostępnia co najwyżej jednego kandydata do pobrania, bez kolejki tur i bez podłączenia sesji gry.

Kryteria: Rzeczywiste wiadomości Windows: pierwszy down, bit repeat, keyup/repress, focus loss z pending, odzyskanie focus i unknown. Brak komendy po zamknięciu, brak zmiany GameState; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure/build/CTest Debug i Release; formatter src/tests/tools.
Dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha i zakresu. Bez nowych assetów,
pobrań i pushu.
