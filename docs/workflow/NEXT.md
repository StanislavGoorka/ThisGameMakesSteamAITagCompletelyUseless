# Następny mały krok

Task: P044-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P044-02 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P044-03: tylko test integracyjny resolvera i lokalnego sinka. Pełny run ze świadkiem, duplikaty, porównanie kluczy/payloadów i pełnego stanu po restore sektora2 z nową gałęzią, nowa próba o tym samym seedzie, restart procesu i dokładne bajty main/bak. Preview zachowuje liczniki usług i pliki. Użyj stałego fake evaluatora, bez predykatów P045 i zmian produkcyjnych.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
