# Następny mały krok

Task: P074-02

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-01.
Zależności: P074-01; muszą być completed.
Sprawdź git status i walidator.

Dodaj odczyt istniejącego recover_checkpoint_file z odtworzeniem RunState
i statusem none/resumed/corrupt/unavailable. Bez UI i zapisu.

Kryteria: rzeczywiste pliki main/bak: brak, poprawny main, fallback backupu,
uszkodzenie, blokada odczytu; pełny stan równy restore, brak modyfikacji plików.

Czytaj RESUME_PRESENTATION.md, CHECKPOINT_FORMAT.md, checkpoint_recover.hpp,
run_checkpoint.hpp i tests/integration/checkpoint_recover_tests.cpp.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
