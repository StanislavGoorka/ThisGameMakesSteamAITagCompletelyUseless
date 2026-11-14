# NastÄ™pny maĹ‚y krok

Task: P078-05

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P078-04.
ZaleĹĽnoĹ›ci: P078-04; muszÄ… byÄ‡ completed.

Podłącz XAudio2Playback do XAudio2Output, dodaj stan mute przez AudioOutput i wczytaj tylko zatwierdzony WAV w entrypoint. Bez mapowania zdarzeń.

Kryteria: Port odtwarza zatwierdzony bufor; mute i unknown nie submitują. Brak pliku, błędy urządzenia i odtwarzania są unavailable bez retry. Debug/Release i smoke procesu przechodzą.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetĂłw.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczoĹ›Ä‡ czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
