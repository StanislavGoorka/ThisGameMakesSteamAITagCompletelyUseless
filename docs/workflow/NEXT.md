# Następny mały krok

Task: P074-04

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-03.
Zależności: P074-03; muszą być completed.
Sprawdź git status i walidator.

Dodaj czystą decyzję granicy: zapis początku sektora (capture_checkpoint),
usunięcie po wyniku terminalnym, brak akcji w trakcie sektora. Bez IO.

Kryteria: start, każdy sektor po przejściu, tura w sektorze, nagroda,
won/death/overload; zapis zgodny z capture_checkpoint, wejście bez mutacji.

Czytaj RESUME_PRESENTATION.md, CHECKPOINT_FORMAT.md, run_checkpoint.hpp,
run_state.hpp, run_transition.hpp i tests/unit/run_checkpoint_tests.cpp.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
