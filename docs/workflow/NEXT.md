# Następny mały krok

Task: P073-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P073.
Zależności: P073; muszą być completed.
Sprawdź git status i walidator.

Dodaj dekorator istniejącego AchievementSink, zapamiętujący wyłącznie nowe trwałe odblokowanie first_delivery. Bez IO i renderowania.

Kryteria: unlocked tworzy jeden pending; already_unlocked, unavailable, invalid, wyjątek i inne ID nie tworzą powiadomienia; dismiss i powtórka nie dublują.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake, CONTENT_POLICY, REGISTRY, TEXT_PRESENTATION.md.
Czytaj REWARD_PRESENTATION.md, SESSION_PRESENTATION.md i pliki bieżącego kontraktu.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
