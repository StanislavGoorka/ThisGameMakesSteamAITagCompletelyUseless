# Następny mały krok

Task: P078-02

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P078-01.
Zależności: P078-01; muszą być completed.

Dodaj przypięte AssetReference i właściciela zatwierdzonego WAV, tworzonego wyłącznie przez read_asset_file i dekoder. Bez source voice i odtwarzania.

Kryteria: Rzeczywisty plik ma dokładne bajty/format; brak, korupcja, blokada i zły root odrzucone. Kopie i przeniesienia właściciela nie pozostawiają wiszących widoków.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
