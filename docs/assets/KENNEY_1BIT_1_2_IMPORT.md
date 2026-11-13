# Import P057: Kenney 1-Bit Pack 1.2

2026-11-13. Zgoda `kenney-1bit-1.2-2026-11-13` ma status approved.
[Wniosek i odpowiedź](REQUEST_KENNEY_1BIT_1_2.md), [dossier](KENNEY_1BIT_1_2.md).

Pobrano raz `1bitpack_kenney_1.2.zip` przez darmową ścieżkę źródłowej strony
[pakietu](https://kenney-assets.itch.io/1-bit-pack), upload ID 4803115.
Adres transportowy HTTPS dostarczył endpoint pliku itch.io (podpisany URL CDN
Cloudflare R2). Nie użyto innego katalogu assetów ani mirrora oferującego inny
pakiet. Nie wykonano płatności, darowizny i skryptów dostawcy.

- Lokalnie: `.local/asset-intake/kenney-1bit-1.2/1bitpack_kenney_1.2.zip`;
  katalog jest ignorowany przez Git.
- Rzeczywisty rozmiar archiwum: **657579 bajtów** (strona pokazuje 642 kB).
- SHA-256: `129A0E74E1DC9091A769D5118BE5C88780484C7A3771EE33AC9EF0DF02583A9A`.
- License.txt potwierdza **1-Bit Pack (1.2)**, Kenney, datę 09-11-2021,
  CC0 i zastosowania komercyjne; bez wyjątku dotyczącego wybranych plików.
- Strona tego samego pliku nadal deklaruje **No generative AI was used**.
  To źródłowa deklaracja pochodzenia, nie niezależny audyt pracy autora.
- Inne nazwy/rozmiary w starym devlogu nie zostały uznane za ten sam ZIP.

## Minimalny przyjęty zestaw

Każdy plik jest bajtowo identyczny z wpisem ZIP; bez wycinania, kolorowania,
kompozycji lub generowania grafiki. Przyjęto jeden gotowy atlas i dwie noty,
łącznie **20259 bajtów**. Rejestr zawiera ich SHA-256, długości, źródło,
wersję, licencję, deklarację non-AI i rzeczywistą zgodę.

| Ścieżka pod assets/approved/kenney-1bit-1.2/ | Bajty | SHA-256 |
| --- | --- | --- |
| Tilesheet/monochrome-transparent.png | 19451 | 888B96CEF0777CE03E9A9F3EB2C7AFC02B2EED9C8F4E9E36E9871F8B624D2D00 |
| License.txt | 569 | D861F208CD550A54E6E59BE553EF6AC93A84F4A2966D0371D21331B70E54F33D |
| Tilesheet.txt | 239 | DAAF1EF167014D100E49780E9C09841A96858C01B284EB5D67AF17AFA0482F73 |

Atlas obejrzano lokalnie, ma 832×373 px, bez prezentacji gry. Tilesheet.txt
opisuje siatkę 49×22 = 1078 płytek, 16×16 px i przerwę 1 px; wymiary odpowiadają
49*17-1 i 22*17-1. Nie sprawdzono jeszcze mapowania wszystkich masek portów,
ostrzeżeń ani czytelności. Liczba ikon nie dowodzi kompletności prezentacji.
Nie wybierać symbolu brandingu jako zastępczego oznaczenia gry.
Noty są dowodami i metadanymi importu, nie finalnymi tekstami UI.

## Pełna lista ZIP-a

Lista obejmuje także trzy wybrane pliki. Pozostałych wpisów nie rozpakowano
do approved ani nie wczytano jako grafiki. Nazwy i rozmiary pochodzą
z centralnego katalogu ZIP.

```text
Preview.png 99808
Sample_fantasy.png 129343
Sample_interior.png 58609
Sample_platformer.png 15489
Sample_urban.png 145462
Tilesheet.txt 239
Visit Kenney.url 45
Visit Patreon.url 55
Tilemap/ 0
Tilemap/sample_fantasy.tmx 4241
Tilemap/sample_interior.tmx 2995
Tilemap/sample_platformer.tmx 2965
Tilemap/sample_urban.tmx 7412
Tilemap/tileset_colored.tsx 255
Tilemap/tileset_legacy.png 26124
Tilesheet/ 0
Tilesheet/colored-transparent.png 25199
Tilesheet/colored-transparent_packed.png 23516
Tilesheet/colored.png 27443
Tilesheet/colored_packed.png 23606
Tilesheet/monochrome-transparent.png 19451
Tilesheet/monochrome-transparent_packed.png 17497
Tilesheet/monochrome.png 22467
Tilesheet/monochrome_packed.png 17561
Instructions.url 152
License.txt 569
```

Wybrane pliki mają atrybut Git `binary` (-text, -diff, -merge), także noty,
aby normalizacja i automatyczny merge nie zmieniały zatwierdzonych bajtów/hashów.
Oryginalna nota zawiera CRLF i białe znaki na pustych liniach; zachowano je.
Testy walidatora kopiują
rzeczywiste approved do izolowanych fixture i dodają syntetyczne wpisy do
istniejących rejestrów; nie zastępują ich i nie tworzą fikcyjnych zgód do gry.
Brak fontu, audio, finalnych napisów i materiałów sklepu pozostaje poza importem.
