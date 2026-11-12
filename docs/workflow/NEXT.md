# Następny mały krok

Task: P041-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P041 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P041-01: wartościowy DomainEvent w application i specyfikacja zdarzeń. Jawne stabilne rodzaje oraz klucz seed/sektor/tura/ordinal wewnątrz próby. Akcja, dostawa, ukończenie sektora, koniec runu. Seed nie odróżnia różnych prób; globalna trwała deduplikacja pozostaje P044. Testuj ID i równość/kopie. Bez resolvera, emisji, sink i IO.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
