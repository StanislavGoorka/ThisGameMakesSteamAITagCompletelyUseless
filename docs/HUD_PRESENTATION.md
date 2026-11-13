# Minimalny HUD

P070 jest podziałem, nie implementacją. Łańcuch: P069-03 -> P070 ->
P070-01 -> P070-02 -> P070-03 -> P070-04 -> P071.

Projekcja odczytuje HP, wykonane tury i liczbę zasilonych odbiorników
z tego samego GameState, który jest rysowany. Nie liczy innych zasilonych pól
jako dostaw. Nie zmienia core, limitów ani zasad preview.

Trzy osobne etykiety pochodzą z TextCatalog. Liczby są danymi stanu;
każdy wygenerowany wiersz zachowuje pełny początek `[AI PLACEHOLDER]`.
Brak etykiety oznacza odmowę całej operacji, bez wyświetlania ID.
Public gate nadal blokuje wszystkie robocze etykiety.

Renderer składa wiersze pionowo w DIP. Aktualizacja layoutu jest atomowa;
zbyt mała szerokość lub wysokość nie pozwala wyświetlić części markera.
BoardView rezerwuje wspólny obszar tekstów i przesuwa całą scenę oraz
hit-test. Preview, anulowanie i focus pokazują te same dane co plansza.
Bez nowych grafik, fontów, pobrań i usług zapisu.

Testy oddzielają projekcję, formatowanie, rzeczywiste piksele DirectWrite
i integrację HWND. DPI celu i resize sprawdzamy przez API; brak obserwacji
pulpitu nie jest oceną czytelności przez gracza.
