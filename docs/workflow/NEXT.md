# Następny mały krok

Task: P088-03

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, DOMAIN_EVENTS oraz raport P088-02. P088-03: wyłącznie pola start_config (domyślnie ID1) i difficulty (domyślnie maska0) w RunEnded (src/application/domain_event.hpp), wypełniane w resolve_run_turn_events (run_turn_events.hpp) z runu dla won/death/overload. Bez predykatów i zmian symulacji, checkpointu, hasha i replay. Testy: ID1..3 × maski0/1/7: wygrana, śmierć i przeciążenie niosą wartości runu; pozostałe zdarzenia bez zmian; deduplikacja dispatchera porównuje cały payload (konflikt przy innej masce dla tego samego klucza); run po restore checkpointu niesie wartości z checkpointu. Sprawdź istniejące testy tworzące RunEnded{...} (agregaty) i DOMAIN_EVENTS.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
