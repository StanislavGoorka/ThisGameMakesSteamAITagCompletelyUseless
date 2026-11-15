# Następny mały krok

Task: P087-02

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, CHECKPOINT_FORMAT, DIFFICULTY_MODIFIER_PLAN, raport P087-01 oraz P086-02 (wzór zmiany checkpointu). P087-02: wyłącznie zapis GameState::difficulty w ostatnim zarezerwowanym bajcie31 checkpointu i run_rules17. RunCheckpoint dostaje pole maski; walidacja wymaga bitów z katalogu (dziś tylko maska0), dekoder parsuje przez DifficultyModifiers::from_value, enkoder zapisuje, restore ustawia GameState::difficulty z zapisu, capture przekazuje i nadal porównuje. Oracle tools/checkpoint/vectors.cjs przyjmuje maskę (dla reguł<17 bajt31 = 0); nowe literalne wektory reguł17 w tests/support/checkpoint_vectors.hpp. CHECKPOINT_FORMAT: tabela bajtów, rekonstrukcja i uwaga, że format nie ma już wolnych bajtów (kolejne pole wymaga schema2).

Testy: round-trip maski0 dla ID1..3; bajt31 = 1/2/4/8/0xFF odrzucony po ponownym zamknięciu checksumy; reguły16 i starsze odrzucane bez nadpisania backupu; schema1/40 bajtów zachowane; wyczerpujący korpus integracyjny z granicą bajtu31. Mutacje: enkoder bez bajtu31, dekoder ignorujący bajt31, walidacja bez katalogu. Bez reguł modyfikatorów, restartu, UI, tekstów i assetów.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
