# Następny mały krok

Task: P073-03

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P073-02.
Zależności: P073-02; muszą być completed.
Sprawdź git status i walidator.

Dodaj wąski właściciel istniejącego evaluatora i EventDispatcher z przygotowaniem kolejki przed publikacją stanu i ponowieniem po błędzie sinka. Bez Win32 i UI.

Kryteria: Istniejące predykaty bez zmian; pełna kolejność, retry/wyjątek bez ponownej oceny lub utraty zdarzeń, duplikaty idempotentne, fresh/restore i nowy scope niezależne.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Dla treści: asset-intake, CONTENT_POLICY, REGISTRY, TEXT_PRESENTATION.md.
Czytaj REWARD_PRESENTATION.md, SESSION_PRESENTATION.md i pliki bieżącego kontraktu.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
