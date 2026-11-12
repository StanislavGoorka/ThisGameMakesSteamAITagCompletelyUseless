# Następny mały krok

Task: P027-01

Przeczytaj AGENTS.md, .agents/skills/next-step/SKILL.md, .agents/skills/cpp-core-step/SKILL.md, docs/WORKFLOW.md, STATE.json, history/P027.md, bieżący wpis i zależności w docs/plan/tasks.json, docs/ARCHITECTURE.md, docs/GAME_DESIGN.md, docs/TESTING.md i README.md. Rozpocznij od git status i walidatora.

Dodaj wyłącznie available_pulse_id(const GameState&) -> optional<EntityId> w src/core/pulse_identity.hpp. Zwracaj najmniejszy niewykorzystany ID, nie max+1; zachowaj kolejność i pełny stan. Przeczytaj GameState, EntityId i testy pulse_occupancy. Testuj pustą kolekcję, luki, ciągły prefiks0..48 (wynik49), granice0/MAX i permutacje. Nie korzystaj z RNG. Arytmetyka musi nie zawijać przy MAX; nullopt dla pełnej reprezentacji (tak ogromnej kolekcji nie alokuj w teście). Mutacja pominięcia zajętego0 po Rebuild musi zawieść. Bez spawnu/zużycia długu/zmian replay4/hashv2.

Wykonaj wyłącznie ten kontrakt. Konfiguracja CMake, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i git diff --check. Przy zmianie zachowania potwierdź właściwy przypadek odmowy, pełny stan i negatywną mutację po wymuszonym Rebuild. Zapisz raport, zaktualizuj STATE/NEXT i README, wykonaj lokalny commit własnych zmian, sprawdź hash/zakres/czystość. Bez pushu, prezentacji, assetów i treści dla gracza. Nie realizuj kolejnego ID w tym samym kroku.
