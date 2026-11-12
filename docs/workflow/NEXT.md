# Następny mały krok

Task: P038-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P038-02 i bieżący wpis z zależnościami w tasks.json. Status i walidator na początku; wszystkie zależności completed.

Dodaj wyłącznie publikację staging w tym samym katalogu przez MoveFileExW(REPLACE_EXISTING|WRITE_THROUGH) i save_checkpoint_file. Nowy checkpoint do main.tmp, zwalidowany poprzedni main do bak.tmp, publikacja bak, potem main. Nigdy nie przenoś main do bak. Odrzuć uszkodzony/niedostępny main, zachowaj dobry backup. Testuj pierwszy i kolejne zapisy, dokładne bajty backupu, odmowy staging/backup/main i retry. Jeden zapisujący, prywatny katalog; bez fallbacku P039 i bez gwarancji zaniku zasilania.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING, CHECKPOINT_FORMAT i README. Configure/build/CTest Debug i Release, clang-format src/tests/tools, walidator, diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą hash/zakresu/statusu. Bez assetów i pushu.
