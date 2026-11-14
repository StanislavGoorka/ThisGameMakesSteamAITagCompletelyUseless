# Następny mały krok

Task: P078

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P077.
Zależności: P077; muszą być completed.

Podepnij jeden zatwierdzony sygnał do dostawy.

Kryteria: Jedno zdarzenie daje jedno odtworzenie; mute i brak pliku obsłużone.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
