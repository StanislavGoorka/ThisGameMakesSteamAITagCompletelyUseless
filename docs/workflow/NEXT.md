# Następny mały krok

Task: P063-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P063-01 oraz bieżący wpis
i jego zależności w tasks.json. Sprawdź status i walidator na początku.

Czyste mapowanie klawisza na komendę. Przełóż siedem logicznych klawiszy na kandydatów Command: strzałki move, R rotate bieżącego pola, O overdrive bieżącego pola, Space wait. Bez wykonania tury i usług.

Kryteria: Każde pole 7x7 i cztery kierunki bez zawijania; pełne payloady czterech komend, unknown bez wyniku, legalność pozostaje w resolverze; D/R.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure/build/CTest Debug i Release; formatter src/tests/tools.
Dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha i zakresu. Bez nowych assetów,
pobrań i pushu.
