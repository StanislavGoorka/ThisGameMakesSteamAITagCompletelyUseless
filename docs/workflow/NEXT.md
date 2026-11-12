# Następny mały krok

Task: P034

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P033-03, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P034: logiczny wybór nagrody z dostępnych modułów. Przeczytaj ModuleLoadout i reguły. Oferta ma zawierać najwyżej dwa niezainstalowane moduły w kanonicznym porządku (przy dwóch ID:2/1/0 ofert). Run jeszcze nie istnieje; bramkowanie po sektorze dopiero P035. Jeśli oferta i atomowy wybór wymagają osobnych kontraktów, wykonaj wyłącznie podział na dzieci i przepnij P035.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
