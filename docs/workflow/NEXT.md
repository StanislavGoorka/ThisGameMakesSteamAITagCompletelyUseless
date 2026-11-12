# Następny mały krok

Task: P040-03

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P040-02 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P040-03: decode_profile ze span const bajtów. Dokładna długość24 przed odczytem pól, wersja, checksum, reserved, bity i zakresy. Testuj round-trip całej poprawnej domeny, wszystkie ucięcia i systematyczne uszkodzenia także z odnowioną checksumą.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
