# Następny mały krok

Task: P038-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P038-01 i bieżący wpis z zależnościami w tasks.json. Status i walidator na początku; wszystkie zależności completed.

Dodaj wyłącznie stage_checkpoint_file(path, checkpoint): encode przed otwarciem, zapis40 bajtów do wskazanego staging, kontrola liczby bajtów, FlushFileBuffers i jawne zamknięcie. Błędy zwracają odmowę. Testuj wszystkie literalne wektory, powtórzenie, resztki staging, niepoprawną wartość, sharing violation, katalog i brak rodzica. Nie dotykaj main/backup i nie dodawaj publikacji.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING, CHECKPOINT_FORMAT i README. Configure/build/CTest Debug i Release, clang-format src/tests/tools, walidator, diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą hash/zakresu/statusu. Bez assetów i pushu.
