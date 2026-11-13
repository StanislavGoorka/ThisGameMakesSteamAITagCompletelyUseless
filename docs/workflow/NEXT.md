# Następny mały krok

Task: P058-03

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P058-02 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Zaimplementuj wyłącznie P058-03 zgodnie z ASSET_MANIFEST.md: bounded read jednego AssetReference przez chroniony uchwyt i SHA-256 BCrypt. Sprawdź dokładny rozmiar i kanoniczną lokalizację pod zaufanym approved przed danymi, odmów katalogów/reparse/ucieczki. Zwracaj wyłącznie cały zweryfikowany bufor i kontrolowane błędy. Testy dokładnych bajtów, braku pliku, IO/blokady, złego rozmiaru/hash i Unicode katalogu bazowego w D/R. Bez PNG decode, renderowania i nowych assetów; zatwierdzonych plików nie mutuj w testach.

Dla C++ stosuj cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Configure, build i CTest Debug/Release, formatter src/tests/tools; dla samej
dokumentacji walidator i diff --check. Raport, STATE/NEXT/README i osobny
lokalny commit z kontrolą hasha oraz zakresu. Bez assetów, pobrań i pushu.
