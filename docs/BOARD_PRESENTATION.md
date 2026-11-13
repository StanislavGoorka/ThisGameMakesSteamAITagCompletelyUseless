# Prezentacja zatwierdzonej planszy

P066 jest podziałem na cztery małe kontrakty. Nie zamyka implementacji.

1. P066-01: wybór istniejącego kafla dla każdej z 16 masek portów.
   Atlas Kenney 1-Bit Pack 1.2 ma komórki 16x16, krok 17, 49x22.
   Korytarze (kolumna, wiersz; indeks od zera): (8,0) NS,
   (9,0) ES, (10,0) NES, (11,0) NESW, (12,0) S.
   Zamknięty symbol (9,3) reprezentuje maskę zero. Pozostałe orientacje
   uzyskuje wyłącznie obrót całego gotowego kafla o wielokrotność 90 stopni.
   Test porówna porty na krawędziach rzeczywistych pikseli oraz wszystkie maski.
2. P066-02: czysta projekcja 49 pól GameState na kafle i współrzędne 7x7.
   Dopasowanie kwadratu do obszaru klienta, jednakowy rozmiar komórek,
   brak mutacji stanu, brak wyniku dla pustego lub niepoprawnego viewportu.
3. P066-03: upload zweryfikowanego PNG do Direct2D i rysowanie listy kafli.
   Bitmapa należy do konkretnego celu; brak pliku, zły atlas lub hash
   dają kontrolowany błąd bez zastępczej grafiki. Test offscreen porówna
   rzeczywiste piksele z oryginalnym atlasem po obrotach.
4. P066-04: podłączenie planszy z istniejącego start_run do developer_window,
   WM_PAINT, resize i odtworzenia bitmapy po utracie celu. Bez jednostek,
   wykonywania komend, HUD i nowych treści. Smoke procesu oraz test repaint.

P067 wymaga P066-04. Plan nie oznacza gotowego renderera ani grywalnej sesji.
Pochodzenie i zgoda: [import](assets/KENNEY_1BIT_1_2_IMPORT.md).
Bez zmian zatwierdzonych plików i bez pobierania.

API: [DrawBitmap](https://learn.microsoft.com/en-us/windows/win32/direct2d/how-to-draw-a-bitmap)
wymaga bitmapy z tego samego celu i sprawdzenia EndDraw;
[WM_PAINT](https://learn.microsoft.com/en-us/windows/win32/gdi/wm-paint)
zamyka BeginPaint przez EndPaint. Clear służy wyłącznie wyczyszczeniu bufora.
Nie rysujemy linii, figur, proceduralnych oznaczeń ani nowych ilustracji.

## Jednostki: podział P067

P067-01 projektuje wartościową listę gotowych symboli: gracza, aktywnego
pakietu i impulsów (rosnąco po ID), a także wejścia, wyjścia i odbiorników.
Brak pakietu daje brak symbolu, HP zero ukrywa żywego gracza. Nakładające się
jednostki zajmują osobne narożniki komórki. Porty nie mogą zostać zasłonięte.
Role pól i zasilenie nie są nowymi mechanikami.

P067-02 składa jedną listę dla AtlasRenderer i BoardView. Centralny kafel
zajmuje 50% boku komórki; symbole zajmują rozłączne narożniki po 25%.
Nie ma tła/obrysu generowanego w kodzie. Test kontroluje rozdzielność obszarów
i porównuje klatkę offscreen z oryginalnymi pikselami gotowych symboli.

P067-03 dodaje wąski adapter sesji okna: przyjęty klawisz tworzy istniejącą
Command i trafia dokładnie raz do resolve_run_turn. Po przyjęciu cały stan
i scena pochodzą z tego samego wyniku. Odmowa, repeat i utrata focus nie
wykonują dodatkowej tury. Callbacki nie wypuszczają wyjątków przez Win32.
Brak automatycznego przejścia sektora, zapisu, achievementów, HUD i menu.

P068 wymaga P067-03, nie samego ukończenia planu P067.

P067-01 przypina gotowe symbole do komórek atlasu (indeksy od zera):
gracz (24,0), pakiet (45,8), impuls (25,7), wejście (19,20), wyjście (20,20),
odbiornik (39,14), zasilony odbiornik (39,15), stałe przyłącze (38,13).
Test PNG potwierdza, że wszystkie osiem komórek istnieje i jest niepustych.
Pierwszeństwo jednej roli pola: odbiornik/zasilenie, wyjście, wejście, stałe
przyłącze. Symbole jednostek pozostają niezależne od tego pierwszeństwa.

Kolejny zakres P068 rozbito na cztery dzieci; kontrakt i negatywne przypadki
opisuje [ACTION_PREVIEW_PRESENTATION](ACTION_PREVIEW_PRESENTATION.md).
P069 wymaga ostatniego dziecka, nie samego planu.
