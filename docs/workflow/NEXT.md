# Następny mały krok

Task: P082-05

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README, docs/PERIODIC_EMITTER.md i raport P082-04. Zależność P082-04 jest ukończona.

Dodaj wyłącznie wartościową kolekcję PeriodicEmitter do GameState. Zaktualizuj pełne porównania (tests/support/state_checks.hpp), kanoniczny hash tools/replay/state_hash.hpp z nową wersją i niezależnymi wektorami oraz wersję replay. capture_checkpoint ma jawnie odrzucić nieodtwarzalną kolekcję; restore obecnego generatora pozostaje z pustą. Testuj kopie i zmianę każdego pola, stare wersje i granice zapisu. Nie podłączaj jeszcze fazy do resolvera (P082-06), nie zmieniaj generatora ani UI.

Computer Use nie jest wymagane do developmentu według decyzji użytkownika z 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.

Dla C++ stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Zapisuj polską dokumentację bezpośrednio w UTF-8; domyślny potok PowerShell do node traci znaki.
