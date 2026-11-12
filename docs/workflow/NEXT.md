# Następny mały krok

Task: P035-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P035-02, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P035-03: dodaj wyłącznie wartościowy resolve_run_turn(const RunState&, const Command&) przez istniejący resolve_turn, tylko dla RunPhase::playing. Wynik zawiera niezależny RunState, accepted i wyliczoną fazę. Zła faza/nielegalna komenda zachowuje pełny stan; śmierć/overload kończą run, sukces1/2 otwiera nagrodę,3 wygrywa. Preview deleguje do tej samej symulacji, bez efektów ubocznych. Nie wybieraj nagrody, nie generuj kolejnego sektora ani nie implementuj P035-04/05. Hashv4/replay8 nie obejmują metadanych runu; decyzję o ewentualnej zmianie reguł zapisz jawnie.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
