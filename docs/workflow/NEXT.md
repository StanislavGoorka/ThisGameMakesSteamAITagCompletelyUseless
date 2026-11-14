# Następny mały krok

Task: P082-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P082.
Ustal wy??cznie kontrakt logiczny emitera w docs/PERIODIC_EMITTER.md. Uwzgl?dnij harmonogram bez overflow, topologi? po rotacji, zasi?g grafowy, wsp?lny limit HP i niezmienno?? przy odmowie. Nie implementuj jeszcze warto?ci ani fazy.

Computer Use nie jest wymagane do developmentu według decyzji użytkownika z 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.

Dla C++ stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Dla dokumentacyjnego podziału build/CTest niewymagane. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push.
