# Następny mały krok

Task: P027

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P026-05.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Przeczytaj wpis P027 i zależność P026-05, regułę długu w GAME_DESIGN, GameState, pulse_occupancy i EntityId. P027 wymaga przeliczenia długu na dodatkowy spawn przy wejściu następnego sektora (granice2/3/6, reszta długu, brak spawnu na zajętym polu). Nie ma jeszcze runu ani generatora. Oceń zakres; jeśli wymaga więcej niż jednego niezależnego kontraktu, tym krokiem jest wyłącznie podział na dzieci, jawna decyzja o zajętym wejściu/pozostałym długu/wyborze ID i przepięcie konsumentów. Nie implementuj runu lub spawnu w kroku planistycznym.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
