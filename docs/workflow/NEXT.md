# NastÄ™pny maĹ‚y krok

Task: P031-02

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P031-01, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

Dodaj GameState::modules domyślnie puste, chronione pełnymi porównaniami i bazowym kontraktem świadka. Hashv3: dotychczasowe pola z tagiem3, następnie dwa sloty po jednym bajcie (0 pusty,1/2 ID); bez osobnego licznika, kanoniczny porządek. Replay5 odrzuca wcześniejsze wersje. Wylicz niezależne wektory pustego/pojedynczych/pełnego zestawu i odśwież wszystkie stałe ślady. Nie aktywuj efektów modułów. Wyniki korpusu mają pozostać takie same.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
