# Następny mały krok

Task: P028

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P027-02.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Przeczytaj P028 i zależność P027-02, odpowiednie granice P029 (certyfikat rozwiązania) i P030 (spawny), GAME_DESIGN, ARCHITECTURE i SplitMix64. P028 dotyczy wyłącznie seedowanej topologii7×7: rozmiar, dozwolone porty, nieruchome przyłącza i powtarzalność, dane bez prezentacji. Najpierw ustal wąski kontrakt; jeśli potrzeba więcej niż jednego niezależnego rezultatu, wykonaj wyłącznie podział. Nie dodawaj solvera, przeciwników, runu lub wizualizacji. Fixture replay pozostaje osobne; nie zmieniaj jego danych ani replay4/hashv2.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
