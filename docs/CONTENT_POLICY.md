# Polityka treści i dowodów pochodzenia

## Reguła nadrzędna

Zakaz generowania przez agentów wszelkich assetów odbieranych przez gracza jest wymaganiem użytkownika, niezależnym od interpretacji Steam. Obejmuje grafikę bitmapową i wektorową, modele, animacje artystyczne, cząsteczki, proceduralne wzory, shadery tworzące oprawę, ikony, fonty, audio, muzykę, teksty, tłumaczenia, tutoriale i materiały marketingowe. Zakaz obejmuje również zlecanie tego innym agentom, generative fill i przepisywanie wyników AI do pliku „ręcznie”. Nie używaj imagegen w tym projekcie do zawartości produktu.

Dozwolone: projektowanie mechaniki, logika i kod integracyjny, testy, narzędzia, dokumentacja wewnętrzna, koncepcje niepublikowane jako treść gry. Aby nie mieszać zasobów, nie tworzymy tutaj również concept artów. Testowe liczby, maski i grafy pozostają poza paczką i poza prezentacją graczowi.

Nie tworzyć placeholderowych prostokątów, ASCII-artu, dźwięków sinusoidalnych, SVG ani „tymczasowych” sprite'ów. Renderowanie zatwierdzonych płytek i ich rozmieszczanie według stanu gry jest kodem prezentacji, nie uprawnieniem do tworzenia nowej ilustracji. Wątpliwy przypadek zablokuj i opisz; nie przemianowuj go na logikę.

## Jedyny wyjątek: tekst roboczy

Każdy osobny napis wygenerowany przez agenta i widoczny w grze zaczyna się **dokładnie** od `[AI PLACEHOLDER]`, np. `[AI PLACEHOLDER] Restart`. Dotyczy także tytułu okna, błędu, tooltipu, etykiety osiągnięcia, opcji, debug overlay i komunikatu ładowania. Nie wystarczy jeden wspólny baner dla całego ekranu. Konkatenacja ani tłumaczenie nie mogą zgubić prefiksu.

W przyszłym rejestrze tekstów każdy wpis ma `id`, `value`, `status`, `source_asset_id`; status `ai_placeholder` wymaga prefiksu i nie może wejść do wydania. Wewnętrzne identyfikatory i logi narzędzi nie są tekstem gry, dopóki nie są w niej wyświetlane. Nigdy nie pokazuj identyfikatora jako fallbacku bez prefiksu.

Build developer może zawierać oznaczone napisy. **Żaden build do publicznej dystrybucji, także demo i beta, nie może zawierać AI placeholderów.** Prefiks nie zwalnia z zasad Steam; jedynie identyfikuje dozwolony roboczy wyjątek użytkownika.

## Pozyskiwanie assetów

1. Przeczytaj stronę autora i warunki. Samo przeglądanie źródeł nie oznacza zgody na pobranie plików. Nie kopiuj obrazów, nie zapisuj preview ani archiwum do repo.
2. Przygotuj dossier: autor, strona pierwotna, konkretny pakiet/wersja, cel, dowód prawa do komercyjnej redystrybucji, wymagania atrybucji oraz oddzielny dowód braku generatywnego AI. Nie oceniaj po wyglądzie. Wczesna data publikacji pomaga, ale nie dowodzi pochodzenia współczesnej aktualizacji.
3. Gdy dowody są wystarczające, poproś o zgodę na **pobranie konkretnego pakietu i wersji**. Podaj zakres, URL, licencję, pochodzenie i lokalizację. Nie wysyłaj maili autorom bez dodatkowej autoryzacji. Zapisz odpowiedź w rejestrze zgód; brak odpowiedzi = `pending`.
4. Pobierz dopiero po zgodzie, do lokalnego katalogu ignorowanego przez Git. Zachowaj licencję, hash archiwum i listę wybranych plików. Nie wykonuj dostarczonych skryptów.
5. Zweryfikuj archiwum wobec dossier. Każdy używany plik otrzymuje SHA-256, rozmiar, ścieżkę, asset ID, źródło, licencję, dowód non-AI, zgodę i status. Brak dowodu oznacza kwarantannę lub odrzucenie, nawet po zgodzie na pobranie.
6. Do `assets/approved/` kopiuj tylko potrzebne, zatwierdzone pliki, wraz z wymaganymi notami. Zgoda na pobranie nie jest dowodem autorstwa; licencja komercyjna nie oznacza prawa publikowania źródeł assetu w publicznym GitHubie. Sprawdź oba zastosowania oddzielnie.

Nie pobieramy treści „free for personal use”, niejasnego autorstwa ani nieudokumentowanych mirrorów. CC0 oznacza warunki prawne, nie brak AI. Nie zmyślaj deklaracji autora. W razie braku źródła dla finalnego napisu zredukuj UI lub oznacz blokadę wydania, zamiast go wygenerować. Możliwy jest zweryfikowany, licencjonowany korpus ludzkich etykiet UI; nie kopiować chronionych tekstów innych gier.

## Rejestry

- [content/assets.json](../content/assets.json): `schema_version`, `assets`; P057 zawiera atlas Kenney 1.2 i dwie noty. Schemat szczegółowy: [REGISTRY.md](../content/REGISTRY.md).
- [content/approvals.json](../content/approvals.json): konkretne zgody z datą i dowodem; P056 zapisuje zgodę na ten jeden pakiet 1.2.
- [content/texts.json](../content/texts.json): napisy produktu, obecnie pusto.
- [ASSET_CANDIDATES.md](ASSET_CANDIDATES.md): propozycje bez statusu zatwierdzenia.

Zapisuj krótkie dowody i odnośniki, nie całe cudze strony. Zrzuty lub kopie dowodowe zawierające grafikę wymagają zgody na pobranie. Nowa wersja pakietu wymaga nowej oceny; poprzednia zgoda nie jest blankietowa.

## Bramka publikacji

Paczkowanie działa przez allowlistę: każdy zasób, napis i komunikat ma pochodzenie; brak niewymienionych plików, placeholderów i danych testowych. Sprawdzić także EXE icon, splash, achievementy, fonty, credits, store capsules, trailer, opis sklepu, lokalizacje i Steam community assets. Nie dołączać niniejszej dokumentacji AI jako tutoriala lub manuala gry.

Automatyczne wyszukiwanie prefiksu i rozszerzeń wykrywa tylko część błędów. Nie dowodzi non-AI i nie zastępuje przeglądu binariów, wszystkich ekranów oraz źródeł. Nierozstrzygnięte pochodzenie blokuje publikację. Nie usuwaj dowodów AI, aby osiągnąć brak oznaczenia.

## Steam: fakt, interpretacja, decyzja

Źródło sprawdzone 2026-11-10: [Steamworks Content Survey](https://partner.steamgames.com/doc/gettingstarted/contentsurvey). Valve opisuje treści tworzone z AI, które trafiają do produktu i są odbierane przez graczy; jako przykłady wymienia oprawę, audio, narrację i lokalizację. Wydajność pracy narzędzi deweloperskich nie jest głównym przedmiotem tej sekcji. Osobno opisuje generowanie w czasie działania produktu.

Nasza interpretacja projektowa: używać AI do kodu i pracy wewnętrznej, a nie do treści dla gracza. To nie jest gwarancja zwolnienia dla każdego fragmentu kodu ani zapewnienie decyzji Valve. Potoczne „tag AI” traktujemy jako wymaganie dotyczące ujawnienia AI, nie zwykły tag gatunkowy.

Przed wysłaniem gry ponownie sprawdź aktualną ankietę i cały produkt, w tym marketing. Ankietę wypełnia się zgodnie z prawdą. Jeżeli rzeczywista zawartość wymaga ujawnienia AI, wydanie bez niego jest zablokowane; usuń problematyczną treść lub przedstaw fakt właścicielowi, bez fałszywej deklaracji.

P062-03 dodaje jeden oznaczony tytuł developerski do texts.json. Public gate go odrzuca; katalog nie jest już pusty.
