# Następny mały krok

Task: P042-01

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P042 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P042-01: odczyt i idempotentne odblokowanie jednego znanego AchievementId w LocalProfile. Test wszystkich 32 masek i ID 0..255, odmowa invalid profilu i zachowanie ustawień. Czytaj local_profile.hpp oraz PROFILE_FORMAT. Bez IO, predykatów i dispatchu.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
