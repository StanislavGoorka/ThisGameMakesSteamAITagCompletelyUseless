# Następny mały krok

Task: P035-04

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P035-03 i bieżący wpis z zależnościami w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed wszystkich zależności.

Dodaj wyłącznie atomowy choose_run_module(RunState&, ModuleId) w choosing_reward. Użyj choose_module; po sukcesie reward_claimed=true. Drugi wybór, duplikat, obcy ID i zła faza bez zmiany pełnego runu; pełny zestaw pomija nagrodę. Bez przejścia sektora.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Jeden mały kontrakt; nadmierny zakres rozbij bez implementacji. CMake configure, build i pełne CTest Debug/Release; clang-format src/tests/tools. Pozytywny i negatywny przypadek z pełnym stanem. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/statusu. Bez pushu i assetów.
