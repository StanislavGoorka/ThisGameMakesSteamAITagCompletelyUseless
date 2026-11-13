# Następny mały krok

Task: P064-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P064-01 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i walidator na początku.

Resize i odtworzenie celu Direct2D. Rozszerz istniejący adapter o resize, zawieszenie przy zerowym rozmiarze i odtworzenie celu po D2DERR_RECREATE_TARGET. Fabryka pozostaje żywa. Bez rysowania i bez sesji produktu.

Kryteria: Rzeczywiste resize i recreate, zerowy rozmiar bez celu do rysowania, powrót z zawieszenia, kontrolowana odmowa po zniszczeniu HWND oraz retry. Symulowany status utraty urządzenia odróżnij od fizycznego resetu GPU; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure/build/CTest Debug i Release; formatter src/tests/tools.
Dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha i zakresu. Bez nowych assetów,
pobrań i pushu.
