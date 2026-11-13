# Następny mały krok

Task: P074-06

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-05.
Zależności: P074-05; muszą być completed.
Sprawdź git status i walidator.

Połącz checkpoint_boundary z CheckpointStore w RunWindow: start, restart,
przejście sektora i wynik terminalny. Błąd pokazuje wiersz checkpoint-unsaved.

Kryteria: HWND: dokładne bajty po starcie i przejściu, brak zapisu po zwykłej
turze i preview, usunięcie po wyniku, błąd zapisu z markerem bez blokady gry.

Czytaj RESUME_PRESENTATION.md, run_window.hpp, checkpoint_boundary.hpp,
checkpoint_store.hpp, local_checkpoint_store.hpp, checkpoint_text.hpp,
tests/integration/achievement_window_tests.cpp i session_window_tests.cpp.
Uwaga: dysk ma 1 s rozdzielczości czasu; po przywróceniu pliku z mutacji
odczekaj i wykonaj touch przed przebudową.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
