# Następny mały krok

Task: P075

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-07.
Zależności: P074-07; muszą być completed.
Sprawdź git status i walidator.

Dodaj tylko lifecycle urządzenia audio i kontrakt odtwarzania, bez dźwięku
zastępczego, syntezy i wbudowanych nagrań.

Kryteria: brak urządzenia nie blokuje gry; brak syntezy i wbudowanych nagrań.

P075 obejmuje port aplikacji, adapter urządzenia Windows i integrację z oknem;
najpierw oceń podział. Za szeroki zakres rozbij bez implementacji.
Czytaj ARCHITECTURE (porty usług), CONTENT_POLICY (zakaz syntezy audio),
DIRECT2D_LIFECYCLE.md jako wzorzec dokumentu lifecycle i ROADMAP M6.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Bez nowych mechanik i assetów; żadnych plików audio ani generowanych próbek.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Uwaga: dysk ma 1 s rozdzielczości czasu; po przywróceniu pliku z mutacji
odczekaj i wykonaj touch przed przebudową.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
