# Następny mały krok

Task: P027-02

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P027-01.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Dodaj wyłącznie spawn_debt_pulse(GameState&) w src/core/debt_spawn.hpp z użyciem available_pulse_id i pulse_occupies. Wymagane debt>=3, sector_entry, brak blocked_nodes na wejściu i brak impulsu. Gracz/pakiet nie blokują. Dopisz jeden impuls na wejściu, zachowując starą kolejność; po udanym push_back odejmij3. Brak miejsca/wejścia/zasobu nie zmienia stanu. Jeden helper nie rozpoznaje granicy sektora: przyszły run wywoła go raz po złożeniu nowego sektora. Debt6 daje jeden impuls i3, ponowienie odmawia na zajętym wejściu. Testuj0/1/2/3/4/5/6/MAX, wejścia0/48, lukiID/graniczneID, brak podwójnego zajęcia, pełny stan, kopie i mutację kosztu po Rebuild. Bez wywołania w resolverze, CLI, fixture lub runie; replay4/hashv2 bez zmian.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
