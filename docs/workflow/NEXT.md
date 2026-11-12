# Następny mały krok

Task: P041-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P041-02 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P041-03: wyłącznie test integracyjny pełnych sekwencji runu przez resolve_run_turn_events. Wykorzystaj istniejące świadki run_tests i checkpoint restore; porównaj listy kluczy/payloadów i pełne stany po powtórzeniu od granicy sektora, sprawdź preview bez publikacji. Bez nowego kodu produkcyjnego, evaluatorów i dispatchu. Seed nie jest ID instancji próby ani gałęzi. Czytaj DOMAIN_EVENTS, run_turn_events, run_checks, testy runu i checkpointu. P042 zależy od tego dziecka.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
