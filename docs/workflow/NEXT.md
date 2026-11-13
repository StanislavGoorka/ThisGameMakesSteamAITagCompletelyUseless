# Następny mały krok

Task: P058-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P058-01 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Zaimplementuj wyłącznie pamięciowy parser P058-02 zgodnie z ASSET_MANIFEST.md. UTF-8 bez BOM do 1 MiB, głębokość32 i najwyżej256 odwołań, schema1/status approved i ścisłe typy wymaganych pól. Użyj systemowego Windows.Data.Json/C++WinRT, caller zapewnia apartment. Zwracaj tylko pełny zwalidowany manifest. Testy składni, UTF-8, typów, pól, limitów i duplikatów w D/R. Bez IO, SHA liczonego z plików, renderera i pobrań.

Dla C++ stosuj cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Configure, build i CTest Debug/Release, formatter src/tests/tools; dla samej
dokumentacji walidator i diff --check. Raport, STATE/NEXT/README i osobny
lokalny commit z kontrolą hasha oraz zakresu. Bez assetów, pobrań i pushu.
