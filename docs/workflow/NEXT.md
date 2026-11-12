# Następny mały krok

Task: P037-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P037-01 i bieżący wpis z zależnościami w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed wszystkich zależności.

Dodaj wyłącznie encode_checkpoint(RunCheckpoint) do optional tablicy40 uint8_t. Jawne offsety LE, schema1/run_rules1, kanoniczne moduły i FNV-1a64 bajtów0..31. Niezależny skrypt i literalne wektory bajtów; odrzuć błędne wartości bez mutacji. Bez dekodowania i IO.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Jeden mały kontrakt; nadmierny zakres rozbij bez implementacji. CMake configure, build i pełne CTest Debug/Release; clang-format src/tests/tools. Pozytywny i negatywny przypadek z pełnym stanem. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/statusu. Bez pushu i assetów.
