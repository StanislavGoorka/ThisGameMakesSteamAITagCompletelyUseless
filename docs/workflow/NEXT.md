# Następny mały krok

Task: P033-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P033-02, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P033-03: dodaj wyłącznie reset_sector_module_usage(GameState&) ustawiający zużycie na false. Reset zachowuje cały pozostały stan, jest idempotentny i przywraca kolejną akcję. Bez wywołania w resolve_turn, bez runu. Hashv4/replay8 bez zmiany, ponieważ zapis i reguły istniejących tur pozostają identyczne.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
