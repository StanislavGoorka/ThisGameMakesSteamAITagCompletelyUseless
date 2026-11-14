# Następny mały krok

Task: P079-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P079-02.
Zależności: P079-02; muszą być completed.

NativeWindow mapuje zatwierdzoną literę obrotu i udostępnia opcjonalny callback surowego pierwszego naciśnięcia do konfiguracji. Zachowaj filtr powtórek i focusu.

Kryteria: HWND: nowa litera działa, stara nie działa, O zachowuje overdrive; przechwycenie nie wydaje komendy, hold/repeat/focus i zmiana podczas hold nie powodują akcji.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
