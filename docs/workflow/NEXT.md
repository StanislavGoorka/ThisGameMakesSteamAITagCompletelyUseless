# Następny mały krok

Task: P088-01

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, ACHIEVEMENT_EXPANSION_PLAN, PROFILE_FORMAT oraz raport P088. P088-01: wyłącznie profil schema2 o tym samym układzie 24 bajtów — achievement_bits 0..11 (0x0fff), odczyt schema1 (tylko bity 0..4) i schema2, zapis zawsze schema2. Bez nowych AchievementId, katalogu, predykatów i UI (to P088-02+). Testy: istniejące wektory v1 dekodują się jak dotąd; v2 round-trip dla 0x0fff; v1 z bitem5 i v2 z bitami12..15 odrzucone; checksum; save_profile_file z poprawnym poprzednim v1 publikuje go do .bak bez zmian i zapisuje v2. Zaktualizuj PROFILE_FORMAT (tabela, wersja, migracja). Uwaga: is_profile_valid jest używane także przez unlock — ID nadal 1..5 aż do P088-02.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; testy okna wymagają argumentu katalogu źródeł; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
