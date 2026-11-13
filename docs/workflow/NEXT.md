# Następny mały krok

Task: P070-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P070-02.
Zależności completed: P070-02. Sprawdź git status i walidator.

Rozszerz istniejący renderer o wartościowe wiersze katalogu i atomową aktualizację layoutu.

Kryteria: Offscreen: kompletne wiersze, odmowa zbyt małego obszaru bez części markerów, ponowny cel i DPI.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake, CONTENT_POLICY, REGISTRY, HUD_PRESENTATION.md
i TEXT_PRESENTATION.md. Dla cyklu sesji: SESSION_PRESENTATION.md, jeśli istnieje.
Za szeroki zakres rozbij bez implementacji. Bez dodatkowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
