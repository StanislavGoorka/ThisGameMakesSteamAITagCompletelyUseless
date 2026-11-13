# Kontrakt cyklu okna P062

P062 jest wyłącznie podziałem trzech niezależnie ocenianych kontraktów:

- P062-01: RAII klasy i ukrytego HWND, walidowany tytuł i kontrolowane błędy.
  Klasa nie ma ikon, artworku, tła ani renderera. Tworzenie i destrukcja na
  tym samym wątku Win32; obiekt nie jest kopiowalny ani przenoszony.
- P062-02: stan focus i zakończenie obsługi komunikatów. Osobne dowody
  prawdziwych wiadomości Windows i wyjścia z pętli, bez wejścia gry.
- P062-03: developerski entrypoint i kontrolowany smoke procesu. Każdy
  roboczy tytuł ma prefiks i wpis katalogu/rejestru, public gate odmawia.

Sam plan nie implementuje dzieci. P063 zależy od P062-03. Minimalny Win32
desktop wymaga dostępnych nagłówków, kompilatora i lokalnej sesji Windows;
zostaną sprawdzone testem danego dziecka. Nie wymaga nowego pobrania ani
zewnętrznego konta. Istniejący import P057 i zgoda nie są zgodą na nowe treści.

Tytuł pobieramy wyłącznie przez lookup TextCatalog. Brak ID lub nieprawidłowe
UTF-8 odrzuca tworzenie okna; ID nie staje się tytułem. Konwersja używa
MB_ERR_INVALID_CHARS, bez zastępowania złych bajtów. Każdy błąd daje brak
HWND i kod Win32. Własność klasy kończy się po zniszczeniu okna; zewnętrzne
WM_CLOSE nie może pozostawić wiszącego uchwytu do podwójnego DestroyWindow.

Testy P062-01 pozostają ukryte i nie wprowadzają napisów do content/texts.json.
Ich roboczy tytuł jest niewyświetlanym fixture z dokładnym prefiksem. Osobno
sprawdzić Unicode, dwa współistniejące okna, powtórki oraz brak klasy po
destrukcji. Publiczny build testów nie jest publicznym binarium gry.

Nie projektować tła, ikon ani placeholderowej grafiki. Domyślna ramka systemu
nie zastępuje późniejszej prezentacji zatwierdzonymi assetami. Lokalny smoke
nie dowodzi zachowania na czystej VM ani czytelności kompletnej gry.
