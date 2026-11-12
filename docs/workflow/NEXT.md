# Następny mały krok

Task: P042-02

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P042-01 oraz bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku.

P042-02: trwała operacja Windows read/modify/save oparta o profile_read/save i profile_unlock. Brak main może utworzyć domyślny profil; invalid i IO nie upoważniają do resetu. Duplikat nie dotyka main ani backupu. Test błędów, zachowania ustawień i retry; bez procesu potomnego, który należy do P042-03.

Dla kodu cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, UI, Steam i pushu.
