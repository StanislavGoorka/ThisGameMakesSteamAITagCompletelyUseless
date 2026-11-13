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
