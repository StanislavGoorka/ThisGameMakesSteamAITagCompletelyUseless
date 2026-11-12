# Następny mały krok

Task: P044

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P043 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P044: rozbij idempotentny dispatch. Kontrakt musi odróżnić próbę oraz gałąź restore od samego seeda. Ustal pamięć evaluatora, retry po błędzie sinka, konflikty payloadu i granicę gwarancji po restarcie; istniejący profil ma tylko trwały zbiór ID, nie outbox. Czytaj DOMAIN_EVENTS, achievement_sink i checkpoint. Bez predykatów P045.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
