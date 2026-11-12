# Następny mały krok

Task: P033-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P033-01, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P033-02: aktywuj terminal_rotation dla nieruchomej maski z dokładnie jednym portem. Wspólny limit rotate/overdrive; zużycie dopiero po sukcesie, bez wpływu na zwykłe obroty. Użyj wspólnego helpera, przetestuj pełne stany odmów i podgląd. Zmień replay na8 ze względu na reguły, zachowaj hashv4. Reset dopiero P033-03.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
