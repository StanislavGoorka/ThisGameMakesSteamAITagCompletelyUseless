# Następny mały krok

Task: P087-01

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ARCHITECTURE, DIFFICULTY_MODIFIER_PLAN, raport P087 oraz P086-01 (wzór identyfikatora). P087-01: wyłącznie silny typ DifficultyModifiers (maska uint8, fabryka z wartości jak start_config_from_value) z pustym katalogiem zaimplementowanych bitów, pole GameState::difficulty (domyślnie maska0), dziedziczenie w run_transition jak loadout, start_run(seed, config, modifiers) i delegacja start_run(seed, config) do maski0. Porównania stanu w testach (tests/support) i tools/replay/state_hash.hpp obejmują pole; zapisz jawną decyzję wersji hash/replay w raporcie i ARCHITECTURE (nowa wersja wymaga niezależnych wektorów, bez cichej zmiany oczekiwań).

Testy: maska0 daje identyczny stan dla korpusu seedów i ID1..3; bity 1, 2, 4, 8 i 0xFF odrzucone; przejście sektora zachowuje pole; mutacja (np. przejście zerujące maskę lub fabryka przyjmująca bit) musi dać czerwony test. Bez checkpointu (P087-02), restartu, reguł modyfikatorów, UI, tekstów i assetów.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
