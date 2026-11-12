# Następny mały krok

Task: P040-06

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P040-05 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P040-06: wyłącznie dowód integracyjny niezależności profilu i runu. Uruchom rzeczywisty writer i reader w oddzielnych procesach, porównaj pełne wartości i literalne bajty. Usuń tylko cztery jawne nazwy plików runu w prywatnym katalogu testowym; profil/main/bak/temp pozostają niezmienione. Bez produkcyjnej operacji delete i zmian checkpointu.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
