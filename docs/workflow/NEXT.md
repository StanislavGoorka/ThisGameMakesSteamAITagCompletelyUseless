# Następny mały krok

Task: P087-08

Czytaj AGENTS, next-step, cpp-core-step, asset-intake, CONTENT_POLICY, WORKFLOW, STATE, TESTING, SESSION_PRESENTATION, DIFFICULTY_MODIFIER_PLAN, raport P087-07 oraz P086-07 (wzór etykiet konfiguracji). P087-08: wyłącznie projekcja bitu modyfikatora na ID tekstu (src/presentation/session_text.hpp, np. difficulty_modifier_text_id: difficulty-short-turns, difficulty-lean-delivery, difficulty-heavy-debt), trzy wpisy content/texts.json ze statusem ai_placeholder i wartością zaczynającą się dokładnie od "[AI PLACEHOLDER]" oraz linie etykiet aktywnych bitów wybranej maski na ekranie startu/wyniku (session_lines przyjmuje także maskę; run_window przekazuje selected_modifiers). Maska0 nie dodaje linii. Bez klawiszy (P087-09), assetów, ikon i wskaźników graficznych.

Testy: każdy bit katalogu ma wpis z dokładnym prefiksem w rzeczywistym rejestrze developer (brak wpisu = błąd testu); session_lines dla start/wyniku zawiera etykietę konfiguracji i etykiety aktywnych bitów w porządku bitów, dla runu nic; okno HWND na ekranie startu pokazuje etykietę konfiguracji (istniejący test). Test-PublicContent.ps1 i Validate-PublicContent.ps1 odmawiają placeholderów. Mutacje: brak wpisu dla bitu, linie dla maski0, linie na ekranie runu.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem; w testach nie dereferencjonuj pustych optional. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
