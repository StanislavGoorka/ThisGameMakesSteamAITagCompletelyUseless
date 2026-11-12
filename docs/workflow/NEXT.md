# Następny mały krok

Task: P030

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P029.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

P030: zapisz i sprawdź1000 stałych seedów w narzędziu testowym, z raportem odrzuceń/przyczyn i sprawdzeniem legalnych spawnów. Przeczytaj P030 oraz zależność P029, generator w sector_topology.hpp, solution_certificate.hpp i testowy konstruktor świadków w solution_certificate_tests.cpp. Nie kopiuj do produkcji testowego konstruktora jako uniwersalnego solvera. Ustal mały kontrakt korpusu; jeśli potrzebne są niezależne adaptery składania stanu/generowania świadków/raportowania, wykonaj wyłącznie podział na dzieci i przepnij P031 na ostatnie dziecko. Pełny run i przywołanie długu raz na granicy sektora nadal nie istnieją. Jawnie odróżnij bazowy sektor bez długu od scenariuszy długu/spawnów; nie pomijaj nielegalnych wyników ani nie nazywaj osiągalności wygraną. Bez nowych assetów, prezentacji lub zmiany stałego fixture replay.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
