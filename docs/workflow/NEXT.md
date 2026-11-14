# Następny mały krok

Task: P075-05

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P075-04.
Zależności: P075-04; muszą być completed.

Dodaj atomowy stan błędu krytycznego przez IXAudio2EngineCallback i jawne odtworzenie zasobów poza callbackiem.

Kryteria: Błąd rejestracji, callback z innego wątku, powtórki odtworzenia i porządek unregister/voice/engine przetestowane; callback nie zwalnia zasobów.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
