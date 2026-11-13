# Następny mały krok

Task: P074-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P074-02.
Zależności: P074-02; muszą być completed.
Sprawdź git status i walidator.

Dodaj katalogowe placeholdery uszkodzonego, niedostępnego i niezapisanego
checkpointu oraz czystą projekcję wierszy z ResumeStatus. Bez renderowania.

Kryteria: każdy status ma pełny marker lub brak wiersza; brak ID daje odmowę;
public gate blokuje robocze wpisy; bez kodów błędów w tekście.

Czytaj RESUME_PRESENTATION.md, session_resume.hpp, achievement_text.hpp,
tests/unit/achievement_text_tests.cpp, content/REGISTRY.md i CONTENT_POLICY.
Dla treści: asset-intake, CONTENT_POLICY, REGISTRY, TEXT_PRESENTATION.md.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools,
scripts/Test-PublicContent.ps1.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
