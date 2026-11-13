# Następny mały krok

Task: P058-04

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P058-03, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Integracja manifestu z zatwierdzonym importem. Połącz parser, lookup i weryfikację pliku w jednym teście rzeczywistego rejestru P057 oraz izolowanych odmów. Nie zmieniaj zatwierdzonych bajtów i nie dodawaj funkcji prezentacji.

Kryteria: Wszystkie trzy rekordy zwracają dokładne bajty; nieznane ID, zła ścieżka/hash i brak pliku są kontrolowane bez partial result. Własne kopie do negatywnych próbek, D/R i walidator.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
