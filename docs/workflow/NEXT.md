# Następny mały krok

Task: P026-03

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P026-02.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Dodaj jeden helper overdrive_from_player(GameState&, NodeId) w src/core/player_overdrive.hpp. Cel/zasięg dokładnie jak rotate_from_player, reserve>=1, dług<UINT32_MAX. Dwa obroty zgodnie z zegarem oraz -1 reserve/+1 debt atomowo, bez kosztu tury i bez kontroli HP/terminalności. Można wykonać pierwszy obrót dopiero po kontroli zasobów; drugi nie może zawieść po pierwszym, bo nie zmieniają się zasięg i rotatable. Bez Command/resolvera/CLI i spawnu; replay pozostaje 3. Dodaj test celu, wszystkie 16 masek, granice rezerwy 0/1/3 oraz długu 0/MAX-1/MAX, pełny stan i mutację pojedynczego obrotu po Rebuild.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
