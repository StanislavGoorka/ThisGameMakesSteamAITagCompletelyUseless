# Następny mały krok

Task: P081

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P080.
P079-06 i P080 są completed. Decyzja użytkownika z 2026-11-14 pozwala
kontynuować development bez Computer Use; nie ponawiaj starej blokady.

Zakres P081: automatyczny dowód zintegrowanego przejścia trzech sektorów
z runem, zapisem, restartem i achievementami. Najpierw porównaj istniejące
testy core_run_integration, application_checkpoint_integration,
windows_resume_window, windows_checkpoint_window, windows_achievement_window
i windows_profile_unlock_restart z kryteriami tasks.json. Sam bot core
nie dowodzi integracji usług. Jeśli brakuje kilku niezależnych kontraktów,
wykonaj wyłącznie podział P081 na dzieci i przepnij P082 na ostatnie dziecko.
Nie rozpoczynaj P082 w tym kroku.

Czytelność techniczną sprawdza P080. Obserwacja pulpitu i percepcyjna ocena
stanów pozostają jawnie niewykonane do kontroli przed wydaniem w P104;
nie przedstawiaj testów automatycznych jako obserwacji ani playtestu.
Nie generuj assetów ani finalnych napisów, nie pobieraj nowych pakietów.

Kontrole: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
Dla zmian C++ stosuj cpp-core-step, build i pełny CTest Debug/Release,
formatter src/tests/tools. Dla samego podziału dokumentacyjnego build i CTest
nie są wymagane. Dysk ma rozdzielczość czasu 1 s: po mutacji kodu odczekaj
i touch przed buildem. Komendy z katalogu repo, git z lokalnym -c safe.directory.
