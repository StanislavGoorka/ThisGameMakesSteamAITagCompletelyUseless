# Następny mały krok

Task: P041-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P041-01 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P041-02: wartościowy wrapper resolve_run_turn zwracający RunResult i listę DomainEvent. Wywołaj wspólny resolver raz. Po akceptacji: akcja, nowa dostawa (delta powered_nodes), sukces sektora dopiero po zagrożeniach, końcowy wynik runu. Odmowa i terminalne wejście bez zdarzeń. Testuj wszystkie komendy, śmierć przy dostawie/wyjściu, turę48, pełną zgodność stanu, powtórzenia i preview. Bez dispatchu, sink i IO.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
