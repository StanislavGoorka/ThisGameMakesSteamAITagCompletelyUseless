# Następny mały krok

Task: P074-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074.
Zależności: P074; muszą być completed.
Sprawdź git status i walidator.

Dodaj ścieżkę checkpoint.bin w tym samym prywatnym katalogu LocalAppData
co profil. Bez odczytu, zapisu i zmian UI.

Kryteria: rzeczywisty i prywatny testowy katalog, wspólny katalog z profilem,
powtórka, Unicode, kolizja z plikiem i ścieżka względna; kontrolowane błędy.

Czytaj RESUME_PRESENTATION.md, CHECKPOINT_FORMAT.md, profile_location.hpp
i tests/integration/profile_location_tests.cpp.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
