# Następny mały krok

Task: P078-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P078-02.
Zależności: P078-02; muszą być completed.

Dodaj wąski RAII source voice posiadający zatwierdzony WAV i urządzenie. Fabryka API pozwala testować submit/start, błąd i jawne odtworzenie. Bez mapowania zdarzeń i zmian okna.

Kryteria: Każde play przesyła raz oryginalny bufor. Błędy nie zostawiają zaległego odtwarzania; source voice niszczony przed mastering voice i danymi, także przy odtworzeniu i krytycznym callbacku. Próba realnego odtwarzania wyłącznie zatwierdzonego pliku.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
