# Następny mały krok

Task: P082-04

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P082-03.
Zaimplementuj wy??cznie helper resolve_emitter_phase(GameState&, span<const PeriodicEmitter>, bool& damage_applied), bez dodawania kolekcji do stanu i pod??czenia resolvera. Testuj aktywno??, footprint, kilka emiter?w/r??ne kolejno?ci, HP0 i wsp?lny limit; chro? pe?ny pozosta?y stan i konfiguracje.

Computer Use nie jest wymagane do developmentu według decyzji użytkownika z 2026-11-14. Obserwacja pulpitu i percepcyjna czytelność pozostają niewykonane do P104. Nie generuj assetów ani finalnych napisów, nie pobieraj pakietów.

Dla C++ stosuj cpp-core-step, pełny build/CTest Debug i Release oraz clang-format src/tests/tools. Po mutacji kodu odczekaj ponad 1 s i touch przed buildem (rozdzielczość dysku). Dla dokumentacyjnego podziału build/CTest niewymagane. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push.
