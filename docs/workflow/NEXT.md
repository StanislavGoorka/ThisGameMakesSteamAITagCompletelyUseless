# Następny mały krok

Task: P087-10

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, DIFFICULTY_MODIFIER_PLAN, raport P087-09 oraz P086-09 (wzór dowodu integracyjnego, tests/integration/start_config_run_tests.cpp). P087-10: wyłącznie dowód, bez zmian produkcyjnych. Nowy test integracyjny (np. windows_difficulty_run): dla każdego bitu (1, 2, 4) i maski7, z kilkoma root seedami korpusu (0..5, 42, MAX) i konfiguracją ID1 (oraz ID3 dla bitu4), pełny run od start_run(seed, config, maska) z certyfikowanymi komendami i witness_choice; każda granica przez save_checkpoint_file/read_checkpoint_file z kontrolą .bak, checkpoint z tą samą maską, restore równy żywemu runowi, równe oferty po sektorze. Achievementy (MvpAchievementEvaluator na zdarzeniach tur) jak dla maski0 dla wspólnych seedów. HWND: VK_UP/VK_DOWN pokazują etykiety z literalnych ID rejestru, Enter startuje wybraną maskę. Test-PublicContent.ps1 odmawia placeholderów.

Granica dowodu: certyfikaty nie używają overdrive ani rezerwy, więc lean_delivery i heavy_overdrive_debt nie zmieniają ich przebiegu; limit32 przy finale ma zapas jednej tury (31). Liczy się, ile seedów nadal kończy run na maskę (wymagane minimum3); to nie jest pomiar trudności. Mutacje: restore zerujący maskę, brak mapowania VK_UP, confirm ignorujący maskę.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
