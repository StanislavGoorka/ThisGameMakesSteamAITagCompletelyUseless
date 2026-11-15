# Następny mały krok

Task: P087-04

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, GAME_DESIGN (kolejność tury), DIFFICULTY_MODIFIER_PLAN, CHECKPOINT_FORMAT i raport P087-03. P087-04: wyłącznie bit1 short_turn_limit w difficulty_modifier_catalog oraz limit sektora w sector_outcome (src/core/sector_outcome.hpp): 32 zatwierdzone tury z bitem, 48 bez; dotyczy także finału. Ta sama funkcja obsługuje podgląd i resolver. Rozważ jedną funkcję limitu (np. sector_turn_limit(state)) zamiast stałej. Bez innych modyfikatorów, wyboru, UI, tekstów i assetów.

Testy +/−: z bitem ukończenie na turze32 wygrywa, tura32 bez ukończenia to przeciążenie, stan po32 nigdy nie kończy, błędna komenda na turze31 nie zużywa tury; bez bitu granica47→48 bez zmian; podgląd równy wynikowi; certyfikaty korpusu (13–22 tury) nadal ważne przy bicie; checkpoint round-trip maski1 (nowy literalny wektor z tools/checkpoint/vectors.cjs 17 z dziesiątym polem 1), korpus integracyjny dopuszcza bajt31 ∈ {0,1}; restore/resume i restart zachowują bit; fabryka przyjmuje 0 i 1, odrzuca 2/4/8/0xFF; hash maski1 z oracle. Mutacje: limit48 przy bicie, enkoder zapisujący 00, restore ignorujący maskę.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
