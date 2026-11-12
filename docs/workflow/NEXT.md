# Następny mały krok

Task: P030-01

Przeczytaj AGENTS.md, next-step i cpp-core-step, WORKFLOW, STATE, raport P030, bieżący wpis i zależności, ARCHITECTURE, GAME_DESIGN, TESTING i README. Zacznij od git status i walidatora; sprawdź ukończenie P030 (tylko plan).

Zapisz 1000 unikalnych stałych seedów (w tym 0/MAX) i deweloperski adapter generate_sector_topology → bazowy GameState: gracz na wejściu, wejście/wyjście i dwa odbiorniki z topologii, HP3, rezerwa2, dług0, brak pakietu/impulsów. Pełny stan dla obu rodzin, niezależność kopii i zewnętrznego RNG. Bez świadka, kontroli spawnów i raportu korpusu w tym kroku.

Configure, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Wymuszony Rebuild i wykryta mutacja adaptera. Raport, STATE/NEXT, README i osobny lokalny commit; sprawdź hash, zakres i czystość. Bez pushu, assetów, prezentacji i zmian replay4/hashv2.
