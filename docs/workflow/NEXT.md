# Następny mały krok

Task: P035

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P034-02, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P035: trzysektorowy run jest za duży na jeden mikro-krok. Przeczytaj generator topologii, GameState, resolver, module_choice, reset_sector_module_usage i spawn_debt_pulse. Wykonaj wyłącznie podział na wartościowy stan/fazy, start, tury, wybór nagrody, przejście i integracyjny dowód trzech sektorów. Przepnij P036 na ostatnie dziecko. Bez kodu w kroku planistycznym.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
