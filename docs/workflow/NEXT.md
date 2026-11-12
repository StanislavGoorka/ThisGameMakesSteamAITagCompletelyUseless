# NastÄ™pny maĹ‚y krok

Task: P031-01

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P031, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

Dodaj tylko wartościowy ModuleLoadout o dwóch slotach i ModuleId (rotation_range=1, terminal_rotation=2). Puste sloty i rosnący porządek ID kanoniczne, duplikat/obce ID/pełny zestaw odrzucane bez mutacji. Testy constexpr i wykonywalne Debug/Release. Nie dodawaj pola GameState, efektów modułów ani nowej wersji replay w tym kroku.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
