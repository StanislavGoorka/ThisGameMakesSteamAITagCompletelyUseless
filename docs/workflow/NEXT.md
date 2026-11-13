# Następny mały krok

Task: P061

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P060, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Blokada placeholderów. Dodaj rozdzielenie developer/public build i bramkę placeholderów.

Kryteria: Developer dopuszcza oznaczone teksty; public package je odrzuca.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
