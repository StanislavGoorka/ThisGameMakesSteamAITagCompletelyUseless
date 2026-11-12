# Następny mały krok

Task: P044-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P044-01 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P044-02: EventDispatcher według DOMAIN_DISPATCH. Cache kopii zdarzenia i maski przed sinkiem, jednokrotna ocena na zakres, retry tylko pending ID. Waliduj kształt/kontekst/kolejność i konflikt payloadu; błąd maski zamraża zakres. Test fake sinka, częściowego potwierdzenia i wyjątku po trwałym skutku. Bez IO i predykatów.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
