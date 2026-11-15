# Następny mały krok

Task: P088-11

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, PROFILE_FORMAT, ACHIEVEMENT_EXPANSION_PLAN oraz raporty P088-12, P088-13 i P088-11-split. P088-11 (zawężony): wyłącznie dowód restartu procesu, bez nowej logiki produkcyjnej, tekstów i assetów. Rozszerz tests/integration/achievement_profile_run_tests.cpp o tryby procesu potomnego (wzór uruchamiania samego siebie przez CreateProcessW, CREATE_NO_WINDOW i timeout: profile_unlock_restart_tests.cpp): np. `--run <ścieżka> <config> <mask> <seed> <gałąź>` wykonuje istniejący full_run i zwraca 0 tylko po udanym flush, `--read <ścieżka> <bity>` sprawdza read_profile_file. Scenariusz w procesie głównym: plik bajtów schema1 (wektor v1 {0x16, 2, 37, muted} z P088-13 albo test::profile_v1_vectors[2]) → proces potomny kończy run ID3 z maską7 (seed kończony przez certyfikaty) → nowy proces czyta bity v1 ∪ maska evaluatora (ID8 i 0xf00 obecne, ID7 nie), wersja pliku 2; trzeci proces powtarza run (inna gałąź) i bajty pliku oraz .bak się nie zmieniają; kontrolny run ID1 maska0 na osobnym pliku po restarcie nie ma bitów 0xfc0. Wynik procesów sprawdzaj kodem wyjścia; nie dereferencjonuj pustych optional. Mutacja: np. sink bez zapisu albo potomek kończący przed flush ma dać FAIL.

Po ukończeniu P088-11 zamyka łańcuch achievementów 1.0 (P088-01..13); następny w kolejce P089 (jeden eksperyment balansu).

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
