# Następny mały krok

Task: P037-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P037 i bieżący wpis z zależnościami w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed wszystkich zależności.

Dodaj wyłącznie RunCheckpoint i capture_checkpoint/restore_checkpoint w application. Czytaj CHECKPOINT_FORMAT. Capture porównuje każde pole pełnego RunState z kanoniczną rekonstrukcją; restore przenosi zasoby i odtwarza jeden już opłacony impuls bez spawnu. Zakresy HP/rezerwy/długu, pełne stany1/2/3, odmowa każdej lokalnej zmiany. Bez kodowania bajtów i IO.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Jeden mały kontrakt; nadmierny zakres rozbij bez implementacji. CMake configure, build i pełne CTest Debug/Release; clang-format src/tests/tools. Pozytywny i negatywny przypadek z pełnym stanem. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/statusu. Bez pushu i assetów.
