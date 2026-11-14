# Następny mały krok

Task: P083-05

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P083-04. P083-05: dodaj wartościową kolekcję DelayedCapacitor do GameState, pełne porównania i ochronę dokładnego baseline witness. Zmień hash na v6 i wersję replay, dodaj niezależne wektory każdego pola z remaining/spent i kopie. Checkpoint odrzuca każdą niepustą kolekcję, także spent; restore generatora zostawia pustą. Bez resolvera, spawnów i UI. Czytaj docs/DELAYED_CAPACITOR.md; zależność P083-04 ukończona.

Dla kodu stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Dokumentację zapisuj bezpośrednio w UTF-8.

Computer Use nie jest wymagane do developmentu zgodnie z decyzją użytkownika 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.
