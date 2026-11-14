# Następny mały krok

Task: P083-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P083-01. P083-02: czytaj docs/DELAYED_CAPACITOR.md. Dodaj tylko DelayedCapacitor: fabryka pełnych liczb, id/position/delay/range/remaining, advance_fuse sygnalizujący 1→0. Testuj granice, brak underflow i niezależność kopii w Debug/Release. Bez GameState, BFS, fazy i resolvera. Zależność P083-01 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
