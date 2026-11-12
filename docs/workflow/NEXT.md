# NastÄ™pny maĹ‚y krok

Task: P031-03

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P031-02, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

Dodaj jeden czysty RotationRules/rotation_rules(const ModuleLoadout&) i podłącz helper rotate_from_player. Na tym etapie wszystkie znane zestawy zwracają bazowy zasięg1 i brak wyjątków nieruchomych. Testuj pusty/pojedyncze/pełny zestaw przez helper, is_command_legal, preview i resolver, również odmowę i pełny stan. Nie aktywuj jeszcze P032/P033; reguły i wersja replay5 bez zmiany zachowania.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
