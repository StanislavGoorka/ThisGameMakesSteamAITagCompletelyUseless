# Następny mały krok

Task: P044-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P044 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P044-01: specyfikacja i małe typy DeliveryScope (seed, token próby, token gałęzi, sektor początkowej granicy), wyniki oraz port evaluatora z fixed maską. Nowy zakres i świeży evaluator po nowej próbie lub restore; cache tylko w pamięci, trwała idempotencja przez zbiór profilu. Test wartości, granic i fake. Bez dispatchu i predykatów.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
