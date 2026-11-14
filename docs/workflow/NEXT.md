# Następny mały krok

Task: P076

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P075-03.
Zależności: P075-03; muszą być completed.

Sprawdź jeden pakiet audio i wystąp o konkretną zgodę bez pobierania.

Kryteria: Dokumentacja wersji, non-AI, licencji i faktycznej odpowiedzi.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
