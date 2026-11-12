# Następny mały krok

Task: P037-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P037-02 i bieżący wpis z zależnościami w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed wszystkich zależności.

Dodaj wyłącznie decode_checkpoint(span<const uint8_t>) do optional RunCheckpoint. Rozmiar40 przed odczytem; magic, schema/rules, checksum, reserved, kanoniczne moduły, flaga i zakresy. Literalne wektory i round-trip, ucięcie/nadmiar oraz obce wersje z poprawioną checksumą; bez IO i bez zmiany formatu.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Jeden mały kontrakt; nadmierny zakres rozbij bez implementacji. CMake configure, build i pełne CTest Debug/Release; clang-format src/tests/tools. Pozytywny i negatywny przypadek z pełnym stanem. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/statusu. Bez pushu i assetów.
