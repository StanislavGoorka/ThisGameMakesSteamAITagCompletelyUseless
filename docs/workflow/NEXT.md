# Następny mały krok

Task: P058

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P057 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Dodaj odczyt zatwierdzonych odwołań do assetów bez renderowania. Wpis P058 obejmuje model, parser JSON i weryfikację plików SHA-256; oceń rozmiar i w razie potrzeby wykonaj wyłącznie podział na małe dzieci z przepięciem konsumentów. Nie pisz ogólnego parsera lub renderera w jednym kroku. Nie pobieraj innych assetów; font, audio i finalne napisy pozostają poza zakresem.

Dla C++ stosuj cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Configure, build i CTest Debug/Release, formatter src/tests/tools; dla samej
dokumentacji walidator i diff --check. Raport, STATE/NEXT/README i osobny
lokalny commit z kontrolą hasha oraz zakresu. Bez assetów, pobrań i pushu.
