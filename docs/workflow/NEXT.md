# Następny mały krok

Task: P058-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P058 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Zaimplementuj wyłącznie P058-01: wartościowy AssetReference, walidowane ID i kanoniczna ścieżka approved, hex SHA-256, długość, unikalność ograniczonego manifestu i lookup nieznanego ID. Stosuj ASSET_MANIFEST.md i cpp-core-step. Bez JSON, IO, renderowania i nowych assetów. Testy pól/granic/duplikatów oraz kopii w Debug/Release.

Dla C++ stosuj cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Configure, build i CTest Debug/Release, formatter src/tests/tools; dla samej
dokumentacji walidator i diff --check. Raport, STATE/NEXT/README i osobny
lokalny commit z kontrolą hasha oraz zakresu. Bez assetów, pobrań i pushu.
