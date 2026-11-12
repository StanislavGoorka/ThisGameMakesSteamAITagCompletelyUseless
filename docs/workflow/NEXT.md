# Następny mały krok

Task: P035-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P035, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P035-01: dodaj silny SectorNumber z zakresem1..3, RunState{seed,sector_number,sector,reward_claimed} oraz czysty run_phase. Ongoing→playing, death/overload→lost, completed3→won, completed1/2→choosing_reward albo ready_for_next przy odebranej nagrodzie/pełnych slotach. Bez start_run, tur, nagród i przejścia. Przetestuj wszystkie numery i fazy oraz pełny stan. Hashv4/replay8 nadal dotyczą GameState; run nie ma jeszcze formatu replay/checkpointu.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
