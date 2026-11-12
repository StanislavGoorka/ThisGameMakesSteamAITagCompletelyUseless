# Następny mały krok

Task: P038

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P037-04 i bieżący wpis z zależnościami w tasks.json. Rozpocznij od git status i walidatora; potwierdź completed wszystkich zależności.

P038: przeczytaj CHECKPOINT_FORMAT, application/checkpoint_encode.hpp, checkpoint_decode.hpp oraz wymagania zapisu w ARCHITECTURE/TESTING. Oceń rozmiar atomowego adaptera Windows temp+zamiana+backup i testów przerwania/niedostępnego katalogu; jeśli przekracza jeden mały kontrakt, wykonaj wyłącznie podział i przepnij P039 na ostatnie dziecko. Kodek przyjmuje dokładnie40 bajtów; przyszły reader odrzuca większy plik przed alokacją. Nie dodawaj UI, assetów ani odzyskiwania P039 w tym samym kroku.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README. Jeden mały kontrakt; nadmierny zakres rozbij bez implementacji. CMake configure, build i pełne CTest Debug/Release; clang-format src/tests/tools. Pozytywny i negatywny przypadek z pełnym stanem. Raport, STATE/NEXT, README, walidator, diff --check, lokalny commit i kontrola hash/zakresu/statusu. Bez pushu i assetów.
