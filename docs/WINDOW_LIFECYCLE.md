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

P062-01 wdraża `windows::NativeWindow::create(catalog, title_id)`: wynik
zawiera unique_ptr albo kod Win32. Brak ID daje ERROR_NOT_FOUND, złe UTF-8
ERROR_NO_UNICODE_TRANSLATION. Błędy rejestracji i CreateWindowExW zachowują
kod systemowy; alokacja może propagować wyjątek bez opublikowania HWND.
Każdy obiekt ma własny atom klasy; obiekt nie może zmienić adresu.
WM_NCDESTROY czyści zapisany uchwyt również po wcześniejszym WM_CLOSE.
Destruktor niszczy okno przed wyrejestrowaniem klasy, zgodnie z kontraktami
[WM_NCDESTROY](https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-ncdestroy)
i [UnregisterClassW](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-unregisterclassw).
P062-03 pozostaje następnym krokiem: entrypoint developer.

P062-02: focused() śledzi WM_SETFOCUS/KILLFOCUS i zeruje się przy WM_DESTROY. run_message_loop() używa blokującego GetMessageW bez filtra HWND; tylko zniszczenie okna z aktywną pętlą wysyła PostQuitMessage. Pozostałe okna i destruktory nie zanieczyszczają kolejki WM_QUIT. dispatch_messages rozróżnia wynik -1, 0 i dodatni; błąd zachowuje kod Win32, WM_QUIT zachowuje kod wyjścia. Brak HWND lub rekurencyjna pętla daje kontrolowaną odmowę. Kontrakt GetMessage: https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessagew .

P062-03: developer_window.exe jest celem WIN32 wyłącznie developer. CMake odczytuje oznaczony developer-window-title z content/texts.json i przekazuje go do TextCatalog; brak poprawnego wpisu kończy start niezerowo. Program pokazuje wyłącznie ramkę systemową i kończy się po WM_CLOSE. Nie zawiera renderera ani logiki. windows_window_process uruchamia trzy procesy z timeoutem, kontroluje tytuł i zamknięcie; test API focus pozostaje w P062-02. Bez oceny wizualnej i VM.
