# Następny mały krok

Task: P072-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P072.
Zależności completed: P072. Sprawdź git status i walidator.

Dodaj czysty model wskazania jednego modułu z istniejącej oferty runu; nawigacja lewo/prawo i odczyt kandydata, bez zmiany RunState.

Kryteria: Oferty 0/1/2, oba posiadane moduły, brak oferty poza właściwą fazą; zmiana kontekstu usuwa stary wybór, brak mutacji runu.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake, CONTENT_POLICY, REGISTRY, HUD_PRESENTATION.md
i TEXT_PRESENTATION.md. Dla cyklu sesji: SESSION_PRESENTATION.md, jeśli istnieje.
Czytaj też REWARD_PRESENTATION.md oraz core/module_offer.hpp i core/run_reward.hpp.
Za szeroki zakres rozbij bez implementacji. Bez dodatkowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
