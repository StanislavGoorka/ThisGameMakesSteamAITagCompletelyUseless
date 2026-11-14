# Następny mały krok

Task: P078-04

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P078-03.
Zależności: P078-03; muszą być completed.

Podłącz zweryfikowane odtwarzanie do AudioOutput i jednego zdarzenia dostawy zatwierdzonej tury. Dodaj stan mute przez istniejący port, bez nowych tekstów i klawiszy.

Kryteria: Jedno zdarzenie daje jedno play, preview/odmowa/powtórka klawisza nie odtwarzają. Mute, brak pliku/urządzenia i błąd portu nie zmieniają gry ani trwałych zapisów. HWND i smoke procesu przechodzą.

Czytaj dokumenty kontraktu wskazane w raporcie poprzednika.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Dla assetów: asset-intake, CONTENT_POLICY i rejestry; bez pobrania przed zgodą.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit i kontrola hasha/zakresu. Bez push.
