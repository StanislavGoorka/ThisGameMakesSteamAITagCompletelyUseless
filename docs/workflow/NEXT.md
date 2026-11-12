# Następny mały krok

Task: P039

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P038-04 i bieżący wpis z zależnościami w tasks.json. Status i walidator na początku; wszystkie zależności completed.

P039: dodaj wyłącznie odczyt poprawnego backupu, gdy main jest uszkodzony lub go brakuje. Czytaj platform/windows/checkpoint_read.hpp i checkpoint_save.hpp; jawnie rozróżnij brak/invalid/IO i zachowaj diagnostykę. Reader nie zapisuje, nie naprawia plików i nie promuje temp. Testuj poprawny main przed backupem, uszkodzony main + poprawny bak, oba uszkodzone, brak obu i dokładną niezmienność dobrego backupu; porównaj pełne restore z opłaconym impulsem. Jeśli zakres polityki błędów i napraw przekracza jeden kontrakt, wyłącznie go podziel i przepnij konsumentów. Bez profilu P040, podłączenia UI, nowych formatów i assetów.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING, CHECKPOINT_FORMAT i README. Configure/build/CTest Debug i Release, clang-format src/tests/tools, walidator, diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą hash/zakresu/statusu. Bez assetów i pushu.
