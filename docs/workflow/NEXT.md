# Następny mały krok

Task: P033-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P033, bieżący wpis i jego zależności w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed dla wszystkich zależności.

P033-01: dodaj terminal_rotation_used=false do GameState, pełnego porównania stanu oraz hasha v4 (bajt na końcu); replay7. Zaktualizuj niezależne wektory i rzeczywiste CLI. Bez efektu modułu i resetu.

Dla kodu przeczytaj cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Wykonaj wyłącznie jeden mały kontrakt; za duży zakres rozbij bez implementacji. Wymagane cmake --preset windows-msvc, build i pełne CTest Debug/Release, clang-format dla src/tests/tools, pozytywny i negatywny przypadek oraz pełny stan. Zweryfikuj wykrywanie celowej mutacji po wymuszonym Rebuild. Decyzję o zmianie replay/hasha zapisz jawnie. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/czystości. Bez pushu, assetów i prezentacji.
