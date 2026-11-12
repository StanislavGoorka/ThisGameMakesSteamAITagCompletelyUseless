# NastÄ™pny maĹ‚y krok

Task: P031

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P030-04, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

P031: jeden port modyfikacji reguł, stabilne ID i limit slotów, bez implementowania modułów P032/P033. Przeanalizuj GameState, player_rotation/overdrive, command/turn_resolver, state_hash i replay. Jeśli niezależne zmiany modelu, stanu/hasha oraz portu przekraczają mikro-krok, wykonaj wyłącznie podział P031 na dzieci i przepnij P032 na ostatnie dziecko. Nie implementuj dzieci przy podziale.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
