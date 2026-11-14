# Następny mały krok

Task: P079-01

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P079.
Zależności: P079; muszą być completed.

Dodaj niezależną wartość RotationBinding dla jednej litery A-Z z odmową O oraz ścisły pamięciowy kodek ustawienia według ROTATION_BINDING.

Kryteria: Wszystkie wartości bajtu, konflikt O, kanoniczny wektor, ucięcia, nadmiar i korupcja; brak IO i zmian gry.

Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN, TESTING i README.
Czytaj kontrakty wskazane w raporcie poprzednika. Zbyt szerokie zadanie
rozbij bez implementacji. Bez nowych mechanik i generowanych assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji odczekaj i touch przed build.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README, lokalny commit
i kontrola hasha/zakresu. Bez push. .claude/ pozostaw poza zmianami.
