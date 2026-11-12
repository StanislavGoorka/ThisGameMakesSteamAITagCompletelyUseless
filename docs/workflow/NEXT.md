# NastÄ™pny maĹ‚y krok

Task: P030-02

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P030-01, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

Dodaj tylko deweloperski konstruktor świadka dla bazowego sector_fixture rodziny P028. Odmawiaj poza ścisłym kontraktem bazowego stanu, nie zmieniaj danych wejściowych i nie filtruj seedów. Waliduj przez rzeczywisty validate_solution, sprawdź ucięcie, błędny stan i pełny stan końcowy. P030-03/04 pozostają osobnymi krokami.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
