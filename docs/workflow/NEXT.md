# Następny mały krok

Task: P086-07

Czytaj AGENTS, next-step, cpp-core-step, asset-intake, CONTENT_POLICY, WORKFLOW, STATE, TESTING, SESSION_PRESENTATION, TEXT_PRESENTATION, START_CONFIGURATION_PLAN i raport P086-06. P086-07: jedna projekcja presentation start_config_text_id(StartConfigId) (wzór module_text.hpp) i trzy wpisy content/texts.json o statusie ai_placeholder z wartością zaczynającą się dokładnie od [AI PLACEHOLDER] (np. start-config-baseline/-range/-charged). RunWindow::show_live na ekranie startu i wyniku dodaje linię etykiety flow_.selected(run) do listy linii set_board (obok session_message). Test +: każde zaimplementowane ID ma unikalny oznaczony wpis w rzeczywistym rejestrze developer, renderer offscreen/HWND pokazuje linię wybranej konfiguracji. Test −: ID spoza katalogu bez tekstu (bez fallbacku), brak wpisu lub zły prefiks wykrywa test, Test-PublicContent/Validate-PublicContent odmawia placeholderów. Bez klawiszy (P086-08), assetów i finalnych napisów.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools, Test-PublicContent.ps1. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
