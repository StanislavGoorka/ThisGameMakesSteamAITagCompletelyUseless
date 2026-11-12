# NastÄ™pny maĹ‚y krok

Task: P033

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P032, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

P033: tylko jednorazowy obrót nieruchomej końcówki w sektorze. Przeczytaj ModuleLoadout, rotation_rules, rotate_from_player/overdrive, GameState, hash/replay i dotychczasowe testy. Ustal dokładny warunek końcówki i zużycia, uwzględnij pierwszy sukces, drugą odmowę i reset. Run nie istnieje: nie deklaruj automatycznej granicy sektora. Jeśli wymagane są oddzielnie flaga/hash, akcja i reset, w tym kroku wykonaj wyłącznie podział na dzieci i przepnij P034. Nie implementuj P034/P035.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
