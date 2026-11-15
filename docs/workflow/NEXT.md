# Następny mały krok

Task: P087-06

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, GAME_DESIGN (dług i spawn), DIFFICULTY_MODIFIER_PLAN, CHECKPOINT_FORMAT i raport P087-05 (wzór modyfikatora). P087-06: wyłącznie bit4 heavy_overdrive_debt w difficulty_modifier_catalog i w overdrive_from_player (src/core/player_overdrive.hpp): każdy naliczany overdrive dodaje2 długu; first_overdrive_debt nadal zwalnia pierwszy; odmowa bez mutacji, gdy dług > UINT32_MAX−2. Próg3, spawn długu i format checkpointu bez zmian. Bez wyboru, UI, tekstów i assetów.

Testy +/−: +2 długu z bitem, +1 bez; first_overdrive_debt z bitem: pierwszy 0, drugi +2; dług UINT32_MAX−1 odrzucony bez mutacji, UINT32_MAX−2 przyjęty (wynik UINT32_MAX); legalność komendy (command/preview) zgodna z resolverem przy granicy; ID3 z bitem: jeden overdrive w sektorze1 → dług4 → przejście tworzy jeden impuls na wejściu i resztę1, checkpoint tej granicy round-trip; bez overdrive brak impulsu. Checkpoint: wektor z maską4 lub7 (vectors.cjs), korpus integracyjny dopuszcza bajt31 ≤ 7 (cały zakres masek), dekoder odrzuca 8/255; fabryka przyjmuje 0..7; restart zachowuje bit. Podrobione bity w testach (run_checkpoint pole17, encode pole9) przenieś na 8. Mutacje: +1 przy bicie, brak granicy UINT32_MAX−2, bit naliczany mimo first_overdrive_debt.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; w testach nie dereferencjonuj pustych optional (okno asercji Debug blokuje proces). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
