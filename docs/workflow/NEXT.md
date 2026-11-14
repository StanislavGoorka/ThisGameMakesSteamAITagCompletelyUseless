# NastÄ™pny maĹ‚y krok

Task: P078-06

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P078-05.
ZaleĹĽnoĹ›ci: P078-05; muszÄ… byÄ‡ completed.

Wywołaj AudioOutput raz dla PacketDelivered po publikacji zatwierdzonej tury RunWindow. Błędy portu izoluj od gry i zapisów.

Kryteria: HWND: dostawa daje jedno play, preview/odmowa/repeat/retry achievementu nie odtwarzają. Played/muted/unavailable/wyjątek zachowują te same pełne stany i pliki. Smoke procesu przechodzi.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetĂłw.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczoĹ›Ä‡ czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
