# NastÄ™pny maĹ‚y krok

Task: P030-03

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P030-02, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

Dodaj tylko deweloperską kontrolę legalności rozstawienia na bazowych sektorach i osobne testy spawn_debt_pulse. Sprawdź duplicate ID/pozycje, blokady i aktywne pola; współdzielenie pola z graczem/pakietem jest legalne. Scenariusze debt0/2/3/6, zajęte i brak wejścia. Nie implementuj raportu korpusu ani runu.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
