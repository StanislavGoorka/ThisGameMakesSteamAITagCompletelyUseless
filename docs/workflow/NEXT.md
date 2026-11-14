# Następny mały krok

Task: P080

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P080.
Zależność P079-06 jest completed. P080 pozostaje nieukończone i blocked;
nie ma innego nieukończonego zadania z ukończonymi zależnościami.

Najpierw sprawdź dostęp do pulpitu zgodnie ze skillem computer-use.
Oba dostępne interfejsy zgłosiły native pipe unavailable (os error 2).
Nie buduj własnego obejścia przechwytywania pulpitu. Ponowne stwierdzenie
tej samej blokady bez nowych dowodów nie wymaga pustego commitu.

Po przywróceniu hosta wykonaj macierz 96/144/192 DPI i rozpoznawanie stanów
opisaną w raporcie P080. Zapisz rzeczywiste DPI, rozmiary, obserwacje i
identyfikację badanego builda. Testy offscreen/HWND nie zastępują obserwacji.
Pojedynczy wykryty problem napraw osobnym krokiem; nie rozpoczynaj P081
przed spełnieniem kryteriów P080. Nie generuj assetów ani finalnych napisów.

Czytaj BOARD_PRESENTATION, HUD_PRESENTATION, TESTING i README oraz kontrakty
wskazane w raporcie P079-06. Zawsze: walidator, diff --check, raport,
STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Bez push.
.claude/ pozostaw poza zmianami. Zmiana wyłącznie dokumentacyjna nie wymaga
ponownego builda i CTest. Dla zmiany C++ stosuj cpp-core-step oraz pełne
bramki Debug/Release i formatter src/tests/tools.
Dysk ma rozdzielczość czasu 1 s; po mutacji kodu odczekaj i touch przed build.
