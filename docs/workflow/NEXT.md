# Następny mały krok

Task: P035-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P035-01, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P035-02: dodaj wyłącznie start_run(RunSeed). Lokalny SplitMix64(root seed), pierwszy next_u64 jako seed generate_sector_topology; zbuduj nowy GameState na wejściu, ustaw odbiorniki i wyjście. RunState sektor1, reward_claimed=false, HP3/reserve2/debt0/puste moduły, brak pakietu/impulsów i zużycia. Bez tur i przejść runu, bez pobrań i prezentacji. Test pełnych niezależnych wektorów seed0/1/MAX, powtórzeń, kopii i zewnętrznego RNG. Hashv4/replay8 bez zmiany (dotychczasowe fixture i tury identyczne).

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
