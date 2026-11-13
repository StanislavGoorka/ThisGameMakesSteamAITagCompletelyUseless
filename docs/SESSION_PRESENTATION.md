# Start, wynik i restart

P071 rozbito na P071-01 -> P071-02 -> P071-03. P072 zależy od P071-03.
Podział nie oznacza implementacji dzieci ani wyboru nagrody.

Model application wylicza ekran start/run/wynik. Wynik dotyczy wyłącznie
RunPhase::won lub lost; sukces sektora pośredniego nadal czeka na istniejącą
nagrodę i P072. Rozpoczęcie i restart używają start_run z tym samym root seedem.
Aktywny run i oczekiwanie na nagrodę odmawiają restartu. Model nie posiada
profilu, ścieżek ani usług IO, więc nie zeruje ustawień ani achievementów.

Komunikaty startu, sukcesu, śmierci i przeciążenia są osobnymi wpisami katalogu
z pełnym prefiksem `[AI PLACEHOLDER]`. Zawierają instrukcję Enter. Żadnych
niezarejestrowanych fallbacków, nowych grafik i pobrań.

Okno developer zaczyna od komunikatu startu, bez rysowania planszy i hit-testu
pól. Enter rozpoczyna run bez tury. Dopiero zatwierdzony terminalny wynik
pokazuje ekran wyniku; preview pozostaje przewidywanym stanem planszy z HUD.
Enter na wyniku tworzy świeży run. Po przejściu nie może przetrwać kandydat,
trasa ani tryb planowania. Filtr klawiatury nadal odpowiada za hold i focus.

Adapter przyjmujący gotowy RunState może zaczynać od aktywnego runu.
Dotychczasowy widok bez katalogu sesji zachowuje wąski kontrakt planszy.
Testy modelu obejmują wszystkie fazy; test HWND obejmuje klawiaturę,
odmalowanie, pełny reset i dokładne bajty istniejącego pliku profilu.
Zapis sesji i rzeczywiste przejście między sektorami należą do dalszych ID.
