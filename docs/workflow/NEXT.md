# Następny mały krok

Task: P079-02

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P079-01.
Zależności: P079-01; muszą być completed.

Dodaj port RotationBindingStore oraz bounded read i zapis przez staging/rename osobnego pliku Windows, bez nadpisania uszkodzonego lub niedostępnego main.

Kryteria: Rzeczywiste pliki: brak, save/read, duplikat, korupcja, blokada, błąd staging, retry. Profil i checkpoint pozostają identyczne.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
