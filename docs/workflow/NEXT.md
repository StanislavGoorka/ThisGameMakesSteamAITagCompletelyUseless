# Następny mały krok

Task: P079-06

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P079-05.
Zależności: P079-05; muszą być completed.

Dodaj test nowych procesów korzystających z tego samego pliku ustawienia i rzeczywistego HWND. Pierwszy zmienia klawisz przez F2, kolejny wczytuje i wykonuje obrót.

Kryteria: Restart zachowuje nowe przypisanie i dokładne bajty; stary klawisz nie obraca. Konflikt, focus i odmowa zapisu nie zmieniają ustawienia, profilu ani checkpointu.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
