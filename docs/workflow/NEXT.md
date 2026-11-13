# Następny mały krok

Task: P061-03

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P061-02, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Tryb public w konfiguracji CMake. Dodaj jawne rozdzielenie developer/public niezależne od Debug/Release. Public build uruchamia bramkę P061-02 przed celami produktu i wystawia politykę do przyszłego entrypointu; developer pozostaje domyślny.

Kryteria: Configure/build D/R developer i public bez placeholderów. Izolowany developer z oznaczonym wpisem przechodzi, public build odmawia z niezerowym kodem. Bez deklaracji gotowego depotu i bez publikacji.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
