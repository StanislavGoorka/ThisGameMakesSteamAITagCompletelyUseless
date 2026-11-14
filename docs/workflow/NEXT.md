# Następny mały krok

Task: P079-04

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P079-03.
Zależności: P079-03; muszą być completed.

Podłącz F2, wybór litery i port ustawienia do RunWindow, z oznaczonymi tekstami katalogu. Zmieniaj aktywne mapowanie dopiero po udanym zapisie; Escape i focus anulują.

Kryteria: HWND: sukces, konflikt O, błąd/wyjątek store i retry, cancel/focus, brak tur podczas capture. Pełne stany, preview i pliki gry bez zmian.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
