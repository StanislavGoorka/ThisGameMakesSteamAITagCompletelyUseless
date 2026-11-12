# Następny mały krok

Task: P026-04

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P026-03.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Dodaj OverdriveCommand{NodeId target} do Command i obsłuż przez overdrive_from_player w legalności i resolverze, przed ruchem/dostawą/zagrożeniami, z jednym wspólnym charge_turn. Preview deleguje bez zmiany. Replay 4, hash v2. Testy muszą wykazać koszt przed regeneracją (reserve3→2→3), topologię po obu obrotach, jedną turę, odmowy zasobów/celu/terminalności przed fazami, preview=resolver dla sukcesu/odmowy/śmierci/47→48 oraz pełną niezmienność stanu, komendy i zewnętrznego RNG. Czytaj command.hpp, turn_resolver.hpp, turn_preview.hpp, odpowiednie testy oraz replay_runner i CLI. Dodaj jawny korpus replay nowej komendy, odrzuć 1/2/3 bez wyniku/stdout. CLI w tym kroku nadal bez tokenu overdrive. Mutacja kosztu lub kolejności po Rebuild wymagana. Bez spawnu długu/runu.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
