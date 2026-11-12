# Następny mały krok

Task: P038-04

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P038-03 i bieżący wpis z zależnościami w tasks.json. Status i walidator na początku; wszystkie zależności completed.

Dodaj wyłącznie integracyjny test faktycznego save_checkpoint_file w procesie potomnym, zatrzymanym po częściowym WriteFile, flush staging oraz publikacji bak/main. Zdarzenie Win32 synchronizuje gotowość; rodzic kończy proces i sprawdza dokładne main/bak/temp, pełny restore i retry. Testuj nazwy Unicode i niedostępny katalog. Nie duplikuj protokołu zapisu w workerze; ogranicz ewentualne przechwycenie wywołań Win32 do testu. Bez odzyskiwania P039, UI i gwarancji zaniku zasilania.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING, CHECKPOINT_FORMAT i README. Configure/build/CTest Debug i Release, clang-format src/tests/tools, walidator, diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą hash/zakresu/statusu. Bez assetów i pushu.
