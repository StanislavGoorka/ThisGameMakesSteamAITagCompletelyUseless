# Następny mały krok

Task: P026-05

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P026-04.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Rozszerz wyłącznie tools/replay/main.cpp o token overdrive:NodeId mapowany na OverdriveCommand. Replay pozostaje 4, hash v2; bez zmiany reguł core. Test integracyjny CMake na rzeczywistym exe: pełny ślad 5×overdrive:1 seed0 (referencja i hashe w raporcie P026-04), odmowa bez rezerwy poprawną linią, powtarzalność; błędne/brakujące cele,49,znaki,overflow i trailing junk odrzucone bez częściowego stdout także po wait. Sprawdź cele0/48 i liczbę z zerami wiodącymi. Mutacja parsera wykryta po Rebuild developer_replay. README z rzeczywiście uruchomioną komendą. Bez spawnu długu, runu i generatora.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
