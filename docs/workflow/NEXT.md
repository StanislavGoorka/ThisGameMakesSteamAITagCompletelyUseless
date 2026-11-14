# Następny mały krok

Task: P075-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P075-05.
Zależności: P075-05; muszą być completed.

Podłącz lifecycle urządzenia jako port audio w developer_window; brak urządzenia lub zatwierdzonego sygnału nie zmienia gry. Bez odtwarzania i nowych tekstów.

Kryteria: HWND z niedostępnym urządzeniem: tury, preview, zapis i focus jak dotąd; smoke procesu działa; port nigdy nie odtwarza bez zatwierdzonego pliku.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
