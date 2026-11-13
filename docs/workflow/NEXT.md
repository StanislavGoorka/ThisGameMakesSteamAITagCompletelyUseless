# Następny mały krok

Task: P062-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P062-01, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Komunikaty zamknięcia i focus okna. Dodaj wąski stan focus i kontrakt zakończenia pętli Win32 dla istniejącego okna. Bez wejścia gry, renderera i entrypointu produktu.

Kryteria: WM_SETFOCUS/KILLFOCUS, WM_CLOSE/DESTROY i zakończenie GetMessage, brak busy loop i kontrolowany błąd odczytu komunikatu; rzeczywiste wiadomości Windows, D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
