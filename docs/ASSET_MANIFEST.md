# Kontrakt odczytu odwołań P058

P058 jest podziałem planu. P058-01 dostarcza model, walidację i lookup,
a P058-02 pamięciowy parser Windows. P058-03/04 pozostają kolejnymi kontraktami.
P059 zależy od ostatniego dziecka. Nie dodajemy renderera, nowych assetów,
formatu zapisu gry ani ogólnej biblioteki JSON.

## Wartości application — P058-01

AssetReference jest niezależną wartością: wewnętrzny ID, względna ścieżka,
32 bajty SHA-256 i oczekiwana długość. Powstaje wyłącznie po walidacji pól;
bez komunikatów dla gracza. Digest odczytuje dokładnie 64 znaki hex,
wielkość liter nie zmienia jego bajtów. Długość 0..64 MiB.

ID: 1..64 znaków ASCII, litery a-z, cyfry, podkreślenie i myślnik; pierwszy
znak jest literą lub cyfrą. Nie interpretujemy ID jako ścieżki lub etykiety UI.
Ścieżka: 1..240 bajtów, dokładny prefiks `assets/approved/`, potem niepuste
segmenty oddzielone `/`. Segmenty składają się z ASCII A-Z/a-z/0-9/`_`/`-`/`.`;
bez `.`/`..`, końcowej kropki, zastrzeżonych nazw urządzeń Windows (także przed
rozszerzeniem), backslash, ADS/dwukropka, absolutnych ścieżek, UNC i NUL.
Nazwa `CONIN$` także nie przejdzie alfabetu. W przyszłości Unicode może wystąpić
w zaufanym katalogu bazowym, nie w kanonicznym odwołaniu tego kontraktu.

Manifest zachowuje kolejność i zawiera najwyżej 256 odwołań, także zero.
Duplikat ID albo ścieżki porównanej bez rozróżniania liter ASCII jest błędem;
Windows nie może otworzyć tego samego pliku pod dwiema takimi nazwami.
Lookup nieznanego ID daje brak wyniku. Model nie sprawdza istnienia pliku ani
zgodności jego zawartości z digestem — to osobny kontrakt P058-03.
AssetReference posiada własne stringi i porównanie pełnej wartości.
Walidacja manifestu odrzuca również niekanoniczne wartości pozostawione
po przeniesieniu obiektu; lookup nie publikuje częściowo poprawnej listy.

## Parser pamięciowy Windows — P058-02

Wejście: UTF-8 `content/assets.json` jako string_view, najwyżej 1 MiB,
bez BOM, z ograniczeniem zagnieżdżenia do 32 (nawiasy poza stringami).
Wyjście: optional całego wektora AssetReference; odmowa nie zwraca częściowej
listy. Root jest obiektem, schema_version jest liczbą 1, assets jest tablicą.
Każdy rekord jest obiektem; id/path/sha256/status są stringami, status=approved,
bytes jest skończoną liczbą całkowitą w zakresie modelu. Nie ma konwersji
string→number/bool ani wartości domyślnych dla brakujących pól.
Pozostałe metadane rejestru mogą być obecne; pochodzenie i rzeczywistą zgodę
sprawdza walidator oraz bramka paczki, nie samo słowo approved w JSON.

Składnię odczyta systemowy
[Windows.Data.Json](https://learn.microsoft.com/en-us/uwp/api/windows.data.json.jsonobject.tryparse),
przez C++/WinRT dostarczone w SDK. Lokalne nagłówki Windows.Data.Json.h/base.h
SDK 10.0.22621.0 są obecne (Test-Path=True); konfiguracja docelowa jest sprawdzana
buildem dziecka. Nie pobieramy nowej zależności. Wywołujący zapewnia apartment
WinRT; parser nie przejmuje życia wątku/COM i nie wykonuje IO ani wywołań sieci.
Nieprawidłowe UTF-8, dane/typy, nieobsługiwana wersja i limity dają brak wyniku;
błąd alokacji może propagować, bez publikacji częściowej listy.

P058-02 wdraża `windows::parse_asset_manifest`. Kontrola Win32
[MultiByteToWideChar](https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar)
z MB_ERR_INVALID_CHARS poprzedza konwersję do hstring: domyślna konwersja
C++/WinRT zastępuje złe sekwencje. Test negatywny wykrył tę różnicę przed
ukończeniem kroku. Przyjęcie JSON nie oznacza jeszcze sprawdzenia pliku ani
realnej zgody; pełna kontrola pochodzenia pozostaje w rejestrze i bramce paczki.

## Weryfikacja pliku — P058-03

Adapter Windows otwiera raz regularny plik spod zaufanego katalogu approved.
Chroniony uchwyt nie pozwala na zmianę/usunięcie podczas odczytu. Kontroluje
kanoniczną końcową lokalizację przed odczytaniem danych, odrzuca katalog,
reparse i ucieczkę do innego katalogu. Sam leksykalny prefiks nie jest dowodem
lokalizacji na dysku. Katalog bazowy jest zaufany i stabilny podczas operacji.
Rozmiar i SHA-256 muszą odpowiadać odwołaniu; tylko wtedy zwraca cały bufor.
Brak pliku, błąd IO, invalid reference, rozmiar i hash są kontrolowanymi
odmowami, nigdy fallbackiem do niezarejestrowanej grafiki.

SHA-256 przez systemowy
[BCrypt](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcryptcreatehash),
z RAII i bez własnej implementacji kryptografii. Nie dekoduje PNG i nie
zmienia treści. Sprawdzenie hasha potwierdza bajty, nie autorstwo lub licencję.

P058-03 wdraża `windows::read_asset_file(trusted_root, reference)`.
Porównuje znormalizowaną ścieżkę uchwytu z kanonicznym approved i pełnym
względnym odwołaniem; również junction do innego miejsca wewnątrz approved
nie jest aliasem dopuszczonej ścieżki. Uchwyt odmawia współdzielenia zapisu
i usuwania. Brak wyniku zawiera kod Win32: invalid reference=INVALID_DATA,
lokalizacja/katalog/reparse=ACCESS_DENIED, długość=BAD_LENGTH, hash=CRC;
błędy otwarcia/IO zachowują kod systemu. Błąd BCrypt daje GEN_FAILURE.
Alokacja może zgłosić wyjątek bez publikacji bajtów. Wykorzystano systemowy
[BCryptHash](https://learn.microsoft.com/en-us/windows/win32/api/bcrypt/nf-bcrypt-bcrypthash)
z RAII providera oraz
[GetFinalPathNameByHandleW](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfinalpathnamebyhandlew).

## Dowód integracji — P058-04

Rzeczywisty rejestr i wszystkie trzy pliki P057 przechodzą parse→lookup→read.
Oczekiwane bajty porównujemy z niezmienionymi plikami. Złe ID/hash/ścieżka,
brak pliku i uszkodzenie występują wyłącznie w izolowanym katalogu testowym.
Nigdy nie mutować approved w repo dla negatywnej próbki. Debug/Release,
formatter, walidator i diff --check są wymagane w każdym dziecku C++.

P058-04 wdraża `windows_asset_manifest_integration`: rzeczywisty rejestr
P057 i trzy literalne ID przechodzą parse→lookup→read, z porównaniem pełnych
bajtów. Korupcja i brak pliku dotyczą tylko kopii w katalogu testowym;
zła ścieżka zostaje odrzucona w parserze, a zły digest przed publikacją bufora.
To dowód kontraktu czytnika, bez dekodowania lub renderowania atlasu.
