# Następny mały krok

Task: P061-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P061, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Polityka developer/public katalogu tekstów. Dodaj jawny TextAudience do walidacji TextCatalog: developer przyjmuje oznaczone placeholdery, public odrzuca całą listę z placeholderem. Bez IO, JSON i CMake.

Kryteria: Puste i approved-only listy, mixed/placeholder listy, invalid audience, atomowa odmowa i brak fallbacku ID; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
