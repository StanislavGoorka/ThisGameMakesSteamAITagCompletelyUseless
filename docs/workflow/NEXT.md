# Następny mały krok

Task: P079-05

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P079-04.
Zależności: P079-04; muszą być completed.

Rozwiąż rotation-key.bin obok profilu i podłącz bounded read, store i aktywne mapowanie do developer entrypoint. Brak ustawienia daje R; błąd nie naprawia pliku.

Kryteria: Bootstrap na prywatnym katalogu: missing, saved, corrupt, locked i brak lokalizacji. Brak zapisu przy starcie, bez zmiany profilu/checkpointu. Smoke procesu.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
