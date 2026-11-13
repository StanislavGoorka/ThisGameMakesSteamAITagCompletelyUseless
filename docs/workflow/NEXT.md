# Następny mały krok

Task: P049

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P048, bieżący wpis i jego
zależności w tasks.json. Status i walidator na początku.

Dodaj wyłącznie reconfigured_delivery: stały zbiór celów z dowodem ActionCommitted, konsumpcja przy dostawie, reset sektora i instancji. Retry po skonsumowaniu dowodu musi korzystać z cache dispatchera.

Dla kodu użyj cpp-core-step; czytaj ARCHITECTURE, GAME_DESIGN, TESTING,
ACHIEVEMENT_PREDICATES, DOMAIN_EVENTS i DOMAIN_DISPATCH. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, tekstów dla gracza, UI, Steam i pushu.
