# Następny mały krok

Task: P062-03

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P062-02 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i walidator na początku.

Developer entrypoint okna i smoke procesu. Połącz okno i pętlę w minimalnym celu WIN32 wyłącznie developer. Każdy widoczny tytuł pochodzi z katalogu z oznaczonym wpisem odnotowanym w texts.json. Bez ikon, grafiki, audio i logiki gry.

Kryteria: Start, focus, zamknięcie i zwolnienie zasobów; proces smoke z timeoutem i kilkoma startami. Public gate odrzuca roboczy wpis, bez publicznego binarium z placeholderem. D/R; oddziel brak VM i ocenę wizualną.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure/build/CTest Debug i Release; formatter src/tests/tools.
Dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha i zakresu. Bez nowych assetów,
pobrań i pushu.
