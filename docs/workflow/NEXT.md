# Następny mały krok

Task: P038-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P038 i bieżący wpis z zależnościami w tasks.json. Status i walidator na początku; wszystkie zależności completed.

Dodaj wyłącznie ograniczony reader Windows pojedynczego checkpointu i RAII uchwytu. Sprawdź rozmiar GetFileSizeEx przed odczytem do stałej tablicy40 bajtów; waliduj decode_checkpoint. Rozróżnij brak, niepoprawny plik i błąd IO. Bez backup fallback, staging i zapisu. Testuj literalne wektory, pełny restore, ucięcia0..39, nadmiar/duży plik, uszkodzenie, brak, katalog i zablokowany plik; brak mutacji.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING, CHECKPOINT_FORMAT i README. Configure/build/CTest Debug i Release, clang-format src/tests/tools, walidator, diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą hash/zakresu/statusu. Bez assetów i pushu.
