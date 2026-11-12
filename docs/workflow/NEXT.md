# Następny mały krok

Task: P026-02

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P026-01.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Zależność P026-01 ukończona. Rozszerz tylko src/core/packet_delivery.hpp: rzeczywista dostawa +1 rezerwy do limitu 3, bez zmiany długu, HP lub kosztu. Przeczytaj testy packet_delivery, packet_introduction, packet_target, turn_resolver, turn_preview i core_experiment oraz tools/replay i integrację CLI. Popraw jawne oczekiwania tylko przy rzeczywistych dostawach; testuj 0/1/2/3, wszystkie odmowy i ponowienie. Replay 3, hash nadal v2; odrzuć 1/2 bez wyniku/stdout. Wektory wylicz niezależnie z referencji w raporcie P026-01. Mutacja usunięcia regeneracji po /t:Rebuild musi zawieść. Bez helpera overdrive, komend, spawnu długu i runu.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
