# Następny mały krok

Task: P074-07

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-06.
Zależności: P074-06; muszą być completed.
Sprawdź git status i walidator.

Uruchom okno developer z odczytanego checkpointu (origin restored_checkpoint)
albo z ekranu startu z kontrolowanym wierszem błędu corrupt/unavailable.
Podłącz local_checkpoint_location, resume_checkpoint_file i LocalCheckpointStore
w developer_main.

Kryteria: zamknięcie i ponowne utworzenie okna wznawia pełny początek sektora;
uszkodzony lub niedostępny zapis pokazuje wyłącznie tekst z prefiksem;
proces developer działa.

Czytaj RESUME_PRESENTATION.md, run_window.hpp, developer_main.cpp,
checkpoint_resume.hpp, checkpoint_location.hpp, checkpoint_text.hpp,
tests/integration/checkpoint_window_tests.cpp i window_process_tests.cpp.
Uwaga: dysk ma 1 s rozdzielczości czasu; po przywróceniu pliku z mutacji
odczekaj i wykonaj touch przed przebudową.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
