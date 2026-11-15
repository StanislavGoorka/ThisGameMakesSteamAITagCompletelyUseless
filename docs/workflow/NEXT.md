# Następny mały krok

Task: P091

Status: blocked (STATE.json). Czytaj AGENTS, next-step, skill asset-intake, WORKFLOW (blokady), STATE, CONTENT_POLICY, content/REGISTRY.md, docs/CONTENT_GAPS.md, docs/assets/TEXT_SOURCE_DOSSIER.md i raport P091.

Przed jakąkolwiek pracą sprawdź, czy właściciel udzielił odpowiedzi z sekcji „Decyzja wymagana od właściciela” w dossier. Bez niej nie twórz commita (ponowne sprawdzenie blokady bez zmian nie jest krokiem) i zakończ z tą samą potrzebą. Gdy właściciel dostarczy ludzkie teksty z dowodami 1–5: zapisz zgodę w approvals.json z wiernym cytatem, przyjmij plik do assets/approved z SHA-256 i wpisem assets.json, usuń blokadę i zamknij P091; wymianę wpisów texts.json zaplanuj w P092. Gdy wskaże konkretny korpus: wykonaj pełne dossier (autor, wersja, licencja, non-AI wersji) bez pobierania i przedstaw konkretny wniosek. Nie pisz tekstów, nie redaguj cudzych i nie fabrykuj zgody.

Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push.
