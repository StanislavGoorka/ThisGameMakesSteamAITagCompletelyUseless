# Następny mały krok

Task: P078-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P078.
Zależności: P078; muszą być completed.

Dodaj niemutujący dekoder RIFF/WAVE PCM16 z pożyczonym widokiem próbek. Bez IO, odtwarzania i wytwarzania danych audio.

Kryteria: Zatwierdzony WAV daje poprawny format i dokładny podzbiór bajtów. Odmowa uciętych chunków, długości, duplikatów i nieobsługiwanego formatu bez odczytu poza buforem. Testy zmieniają tylko metadane istniejącego nagrania.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
