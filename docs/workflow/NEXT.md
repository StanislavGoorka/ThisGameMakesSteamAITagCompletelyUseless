# Następny mały krok

Task: P074

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P073-05.
Zależności: P073-05; muszą być completed.
Sprawdź git status i walidator.

Dodaj w UI odczyt istniejącego checkpointu i kontrolowane błędy.

Kryteria: Zamknięcie i restart; zły zapis nie pokazuje wewnętrznych tekstów bez prefiksu.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake, CONTENT_POLICY, REGISTRY, TEXT_PRESENTATION.md.
Czytaj REWARD_PRESENTATION.md, SESSION_PRESENTATION.md i pliki bieżącego kontraktu.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.

Czytaj również ACHIEVEMENT_PRESENTATION.md, run_achievements.hpp, profile_location.hpp, run_window.hpp oraz checkpoint_recover.hpp i run_checkpoint.hpp. P074 obejmuje kilka kontraktów; najpierw oceń podział na odczyt checkpointu, katalogowe błędy, granice zapisu i integrację wznowienia. Nie implementuj całego łańcucha w jednym kroku.
