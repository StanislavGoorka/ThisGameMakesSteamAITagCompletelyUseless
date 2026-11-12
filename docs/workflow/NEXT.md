# Następny mały krok

Task: P034-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P034, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P034-01: dodaj module_offer(const ModuleLoadout&) zwracające dwie opcjonalne wartości, kompaktowo w porządku ID1/2. Bez RNG, wyboru, GameState i tekstów. Sprawdź wszystkie cztery zestawy oraz przeciwne kolejności dodawania. Hashv4/replay8 bez zmiany.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
