# Następny mały krok

Task: P062-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P062, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Własność ukrytego okna Win32. Dodaj RAII klasy i jednego ukrytego HWND. Tytuł wyłącznie z TextCatalog, ścisła konwersja UTF-8, kontrolowane błędy bez fallbacku. Bez pętli komunikatów, focus, pokazywania okna i rysowania.

Kryteria: Rzeczywiste create/destroy i unregister, dwa okna, powtórki, dokładny tytuł Unicode, unknown ID i invalid UTF-8 bez HWND. WM_CLOSE przed destruktorem nie powoduje podwójnego zwolnienia; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
