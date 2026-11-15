# Następny mały krok

Task: P089-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, BALANCE_EXPERIMENT_PLAN oraz raport P089-01. P089-02: wyłącznie pomiar bazowy, bez zmian kodu gry i narzędzia. Zbuduj Release (cmake --build --preset windows-release --parallel 8), uruchom `.\build\windows-msvc\Release\developer_strategy.exe --corpus`, sprawdź kod 0 (żadnej odrzuconej komendy) i powtarzalność drugiego uruchomienia. W BALANCE_EXPERIMENT_PLAN dopisz sekcję „Pomiar bazowy (P089-02)”: dokładna komenda, data, liczba ukończonych sektorów 1 / 1000 i odsetek, wygrane pełne runy, rozkład końców (won/death/overload) i średnia tura ukończenia sektora 1 (z wierszy wyjścia, policzone skryptem PowerShell bez zmian narzędzia), a następnie werdykt H1 względem progu 25% ustalonego przed pomiarem. Bez interpretacji jako opinii graczy. Jeśli H1 odrzucona, zapisz to wprost — P089-03 wtedy dokumentuje brak zmiany parametru.

Walidator, diff --check, raport z komendami i wynikami, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push.
