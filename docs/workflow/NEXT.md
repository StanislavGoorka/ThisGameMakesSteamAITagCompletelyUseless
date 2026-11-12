# NastÄ™pny maĹ‚y krok

Task: P032

Przeczytaj AGENTS, next-step i cpp-core-step, WORKFLOW, STATE, raport P031-03, bieĹĽÄ…cy wpis i zaleĹĽnoĹ›ci w tasks.json, ARCHITECTURE, GAME_DESIGN, TESTING oraz README. Rozpocznij od git status i walidatora; potwierdĹş zaleĹĽnoĹ›ci completed.

P032: aktywuj tylko ModuleId::rotation_range, zwiększając maximum_distance do2 (Manhattan na7×7, także przekątna1+1, bez zawijania). Pozostałe zestawy pozostają przy1; nieruchome odrzucane. Overdrive korzysta z tego samego zasięgu zgodnie z kontraktem celu rotate. Sprawdź nowy legalny cel, granicę3, brak modułu, fixed i pełne stany preview/resolve z otwarciem wspólnej trasy pakietu/impulsu. Zmiana reguł: replay6, hashv3 bez zmian; odśwież obsługiwane wersje testów i README. Nie implementuj P033 ani wyboru nagrody/runu.

Wykonaj wyĹ‚Ä…cznie ten maĹ‚y kontrakt. Dla kodu: cmake --preset windows-msvc; build i peĹ‚ne CTest Debug/Release; formatter src/tests/tools. Test pozytywny, wĹ‚aĹ›ciwa odmowa, peĹ‚ny stan i mutacja po wymuszonym Rebuild. Walidator i git diff --check. Raport, STATE/NEXT, README, lokalny commit i kontrola hash/zakresu/czystoĹ›ci. Bez pushu, assetĂłw i prezentacji. Zmiana reguĹ‚ lub peĹ‚nego stanu wymaga jawnej decyzji o wersji replay/hasha.
