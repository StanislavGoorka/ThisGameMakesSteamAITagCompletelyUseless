# Jeden tekst developerski: P069

P069 jest podziałem, nie implementacją. Łańcuch P068-04 -> P069 ->
P069-01 -> P069-02 -> P069-03 -> P070.

P069-01 wydziela ścisłą konwersję istniejących wartości TextCatalog do
owning UTF-16. Nieznane ID i niepoprawne UTF-8 dają błąd, bez tekstowego
fallbacku. NativeWindow używa tego samego odczytu tytułu. Pełny prefiks
pozostaje częścią wartości; konwersja nie tłumaczy i nie skraca tekstu.

P069-02 posiada fabrykę, format i layout DirectWrite. Korzysta z fontu
systemowego, bez pobierania lub projektowania fontu. Rysowanie glifów
jest składem tekstu katalogu; nie dodaje ilustracji ani tekstur. Nie zaczyna
i nie zamyka klatki: AtlasRenderer wykonuje tekst przed swoim EndDraw.
Zasoby tekstu są niezależne od celu; brush pochodzi z aktualnego celu.
Nie rysować części napisu, gdy cały tekst nie mieści się w obszarze.
Test offscreen sprawdza tekst, rzeczywiste piksele, odmowy i drugi cel.

P069-03 używa wyłącznie istniejącego developer-window-title z rejestru.
Osobny górny obszar nie zasłania kafli. BoardView podaje oknu wysokość
tego obszaru; hit-test odejmuje ją przed tym samym fit_board co rysowanie.
Brak napisu zachowuje dotychczasową geometrię. Resize i utrata celu nie
mogą udostępnić starego brush ani trasy preview. Bez HUD i nowych etykiet.

Public gate nadal odrzuca istniejący placeholder. Nie tworzymy finalnych
tekstów ani nowej zgody na assety. Brak obserwacji pulpitu należy raportować
oddzielnie od testów API i pikseli; Computer Use ostatnio zgłaszał brak pipe.

P069-02 wdraża TextRenderer: Segoe UI z kolekcji systemowej, 16 DIP,
pełny UTF-16 z katalogu, bez zawijania i bez skracania. Extent uwzględnia
metryki i overhang glifów. Zbyt mały obszar zwraca S_FALSE bez rysowania;
niepoprawny cel/obszar zwraca E_INVALIDARG. Brush jest lokalny dla każdego
draw, a layout/fabryka/format żyją w RAII bez przypisania do celu D2D.
AtlasRenderer opcjonalnie składa napis w tej samej klatce przed EndDraw.

Zweryfikowane API: [CreateTextLayout](https://learn.microsoft.com/en-us/windows/win32/api/dwrite/nf-dwrite-idwritefactory-createtextlayout)
i [DrawTextLayout](https://learn.microsoft.com/en-us/windows/win32/api/d2d1/nf-d2d1-id2d1rendertarget-drawtextlayout).
DrawTextLayout nie zwraca błędu operacji rysowania; caller sprawdza EndDraw.
Test używa dwóch niezależnych programowych celów WIC, porównuje wynikowe
piksele i sprawdza czarny bufor po odmowie, bez zapisywania obrazu jako asset.

P069-03 podłącza istniejący tytuł developerski bez zmian texts.json.
BoardView rezerwuje wysokość napisu plus 8 DIP, przesuwa całą scenę i daje
oknu wspólny hit-test przez WindowPainter::node_at. Hit-test przelicza piksele
klienta na DIP według DPI aktualnego celu, odejmuje obszar tekstu i używa
tego samego fit_board. Bez renderera pozostaje wcześniejszy hit-test.
Zbyt małe okno może pominąć cały napis, nigdy nie wycina samego markera.

Test rzeczywistego HWND przechodzi cztery rozmiary, wszystkie 49 pól i obszar
tekstu poza planszą, preview/anulowanie/focus/commit. Wstrzyknięty status
D2DERR_RECREATE_TARGET sprawdza odtworzenie zasobów i zachowanie napisu/trasy;
nie jest fizycznym resetem GPU. Smoke procesu sprawdza również developer_window.
Debug i Release: po 110/110 CTest. Ocena wzrokowa pozostaje niewykonana.
