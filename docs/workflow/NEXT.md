# Następny mały krok

Task: P081-01

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P081.
Zaimplementuj tylko pełny run HWND z lokalnymi usługami. Użyj start_run bez podmiany stanu, numerycznego świadka tras i istniejących wejść klawiatury; sprawdź preview/odmowę, trzy checkpointy i trwały wynik profilu. Restart procesu pozostaw P081-02.

Computer Use nie jest wymagane do developmentu według decyzji użytkownika z 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.

Dla C++ stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Dla dokumentacyjnego podziału build/CTest niewymagane. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push.
