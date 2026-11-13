# Następny mały krok

Task: P075-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P075.
Zależności: P075; muszą być completed.
Sprawdź git status i walidator.

Dodaj identyfikator sygnału, port odtwarzania z wynikiem
played/muted/unavailable/unknown oraz cichą implementację.
Bez Windows, danych audio i mapowania zdarzeń.

Kryteria: cicha implementacja zawsze zwraca unavailable bez skutków; nieznany
sygnał odrzucony; fake portu potwierdza kolejność wywołań; brak próbek i plików.

Czytaj AUDIO_LIFECYCLE.md, CONTENT_POLICY, achievement_sink.hpp
i checkpoint_store.hpp (wzorce portów).
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Uwaga: dysk ma 1 s rozdzielczości czasu; po przywróceniu pliku z mutacji
odczekaj i wykonaj touch przed przebudową.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
