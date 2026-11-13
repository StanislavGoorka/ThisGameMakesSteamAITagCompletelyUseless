# Następny mały krok

Task: P074-05

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-04.
Zależności: P074-04; muszą być completed.
Sprawdź git status i walidator.

Dodaj port zapisu/usunięcia checkpointu i adapter Windows na istniejącym
save_checkpoint_file. Usuwanie najpierw backup, potem main. Bez UI.

Kryteria: rzeczywiste pliki: zapis i backup, usunięcie obu, brak plików,
blokada, uszkodzony main bez nadpisania; powtórka idempotentna.

Czytaj RESUME_PRESENTATION.md, CHECKPOINT_FORMAT.md, checkpoint_save.hpp,
checkpoint_boundary.hpp, local_achievement_sink.hpp (wzorzec portu)
i tests/integration/checkpoint_save_tests.cpp.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
