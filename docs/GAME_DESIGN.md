# Koncepcja i reguły

Status: projekt do sprawdzenia w testach, nie potwierdzona grywalność. Nazwy i opisy tutaj są wewnętrzne; nie kopiować ich do gry jako finalnych treści.

## Sedno i wyróżnik

Jedna postać, mała sieć przełączanych połączeń, niezależnie podróżujące pakiety energii. Gracz przebudowuje obieg, aby doprowadzić pakiety do odbiorników, zamknąć drogę zagrożeniom i dotrzeć do wyjścia przed przeciążeniem. Przeciwnicy i ładunki korzystają z tych samych krawędzi co gracz. Bez dialogów, narracji, losowej celności i grindu.

Przykład decyzji: obrót rozgałęzienia zapewnia dostawę za dwie tury, ale wpuszcza impuls na obecną pozycję gracza. Można się wycofać, zmienić kolejne połączenie albo zapłacić zasobem za wyjątkową akcję. Każda opcja modyfikuje jeden wspólny system. Nie kopiujemy świata ani zawartości gier opisanych w [analizie](RESEARCH.md).

## Plansza i stan początkowy

- Siatka 7×7; porty N/E/S/W. Krawędź istnieje, gdy sąsiednie porty wskazują na siebie. Bez zawijania krawędzi.
- Węzły: proste, narożniki, trójniki, końcówki oraz nieruchome przyłącza. Obrót o 90° zgodnie z ruchem wskazówek zegara; cztery obroty przywracają stan.
- Gracz ma 3 punkty integralności. Sektor wymaga 2 dostaw do dwóch odbiorników i następnie wejścia na wyjście. Dostawy nie leczą.
- Jeden aktywny pakiet. Startuje na wejściu sektora; drugi pojawia się na początku następnej tury po pierwszej dostawie. Cel to kolejny niezasilony odbiornik według identyfikatorów.
- Bazowy limit: 48 zatwierdzonych tur na sektor. Ostatnia tura pozwala wygrać; inaczej następuje porażka przez przeciążenie.
- Rezerwa: 2 na run, maksymalnie 3; dostawa dodaje 1. Dług przeciążenia startuje od 0 i przenosi się między sektorami.

## Akcje

| Id wewnętrzne | Warunek | Koszt |
| --- | --- | --- |
| `move` | Sąsiad po wzajemnej krawędzi, bez blokującej jednostki | 1 tura |
| `rotate` | Węzeł gracza lub ortogonalny sąsiad; obrotowy; połączenie niewymagane | 1 tura |
| `wait` | Zawsze legalna dla żywego gracza | 1 tura, bez regeneracji |
| `overdrive` | Co najmniej 1 rezerwy; cel jak dla rotate | 1 tura, dwa obroty celu, −1 rezerwy, +1 długu |

Nielegalna komenda nie zmienia stanu, czasu ani RNG. Co 3 długu powstaje dodatkowy impuls na wejściu następnego sektora, po czym dług maleje o 3. Przy przejściu do sektora jest jedna próba spawnu na wejściu: impuls lub blocked_nodes blokują ją, a gracz i pakiet mogą współdzielić pole. Przy braku miejsca dług pozostaje; dług 6 daje jeden impuls i resztę 3, bo wejście mieści jedną jednostkę. Brak pasywnego farmienia zasobów. Overdrive dołączamy dopiero po walidacji podstawowego obiegu.

Moduł `rotation_range` zwiększa zasięg celu `rotate` i `overdrive` z 1 do 2 kroków Manhattan na siatce 7×7. Obejmuje także przesunięcie o jedno pole w obu osiach; granice planszy nie zawijają się. Połączenie i wolne pole nadal nie są wymagane, a nieruchomy węzeł pozostaje niedozwolony. Moduł `terminal_rotation` pozwala raz na sektor obrócić nieruchomy węzeł z dokładnie jednym portem, w zwykłym zasięgu. Limit jest wspólny dla rotate i overdrive; overdrive wykonuje dwa obroty i zużywa jedną możliwość. Obrotowe węzły nie zużywają limitu. Jawny reset granicy sektora odnawia limit; zwykła tura tego nie robi. Wywołanie przy przejściu runu czeka na P035.

## Kolejność tury

1. Waliduj komendę; przy błędzie zwróć wynik bez mutacji.
2. Jeśli poprzednia tura dostarczyła pakiet, umieść następny na wejściu. Wykonaj akcję i koszty gracza.
3. Przesuń pakiet o jedną krawędź najkrótszej trasy do celu w nowej topologii. BFS i remisy: N, E, S, W. Brak trasy oznacza czekanie; pakiet nie znika i nie zmienia losowo celu.
4. Obsłuż dostawę. Następny pakiet wolno wprowadzić dopiero w następnej turze. Pakiet nie blokuje gracza i w MVP nie otrzymuje obrażeń.
5. Rozwiąż zagrożenia w stabilnej kolejności identyfikatorów; ruch do pola innego zagrożenia oznacza czekanie. Początkowy impuls podąża jedną krawędzią BFS do gracza, czeka przy braku trasy. Przy kontakcie z graczem zadaje obrażenie i znika. Kontakt obejmuje wejście gracza na jego pole przed fazą zagrożeń.
6. Obrażenia ogranicz do 1 punktu na gracza w turze, nawet przy kilku kontaktach. Integralność 0 oznacza porażkę, przed sprawdzeniem wyjścia.
7. Zwiększ licznik wykonanych tur. Jeśli żywy gracz stoi na wyjściu po wymaganych dostawach, ukończ sektor. W przeciwnym razie sprawdź limit 48 tur. Potem oblicz podgląd następnej akcji.

Podgląd wskazanej akcji to ta sama symulacja na kopii stanu, bez zapisu plików i efektów ubocznych. Nie pokazuj starych zamiarów przeciwnika jako pewnych po zmianie topologii. Nie ma ukrytego RNG podczas rozwiązywania tury.

## Pętla runu i zakres

**Headless proof:** obrót zmienia trasę pakietu i impulsu, prowadząc do dostawy lub utraty integralności. Syntetyczne plansze są niewyświetlanymi danymi testów; nie powstają grafiki prototypowe.

**MVP grywalny:** trzy sektory 7×7, jeden rodzaj zagrożeń, dwa moduły logiczne, jedna konfiguracja startowa, pięć lokalnych achievementów, zapis między sektorami i działające zamknięcie gry. Prezentacja dopiero po zgodzie na assety i weryfikacji ich pochodzenia. Każdy roboczy napis oznaczony.

**Kandydat 1.0:** sześć sektorów; trzy klasy zagrożeń (pościg, okresowy emiter, opóźniony kondensator); do dwunastu modułów; trzy równorzędne konfiguracje startowe; trzy modyfikatory trudności; dwanaście achievementów. Ostatni sektor wymaga trzech dostaw, bez osobnego animowanego bossa. Docelowy run 20–35 minut jest hipotezą do pomiaru.

Po sektorze wybiera się jeden z dwóch dostępnych modułów. Przykłady reguł: większy zasięg obrotu; obrót nieruchomej końcówki raz na sektor. Moduły nie dodają walut. Każdy moduł, konfiguracja, przeciwnik i achievement powstaje w osobnym mikro-zadaniu. Brak odpowiedniego assetu oznacza rezygnację z cechy lub blokadę, nie wygenerowanie obrazka.

Odblokowania permanentne dają warianty i wyzwania, bez zwiększania HP. Zgon kończy run. Zamknięcie procesu w sektorze wznawia początek sektora; zapis nie ma zapobiegać kopiowaniu plików przez gracza w singleplayer.

## Generowanie a zawartość

Seed, dobór modułów, połączeń i spawnów to algorytm reguł bez modeli generatywnych. Nie generujemy sprite'ów, map ilustracyjnych, prefabów artystycznych ani dźwięków. W grze wyświetlamy wyłącznie zatwierdzone płytki i oznaczenia. Granicę między stanem logicznym a odbieraną zawartością sprawdzamy ponownie przed wydaniem według aktualnych zasad Steam. Kod nie daje automatycznego zwolnienia z obowiązku ujawnienia AI.

## Weryfikacja koncepcji

Headless proof musi wykazać zgodność podglądu z wynikiem i co najmniej trzy sensowne sekwencje decyzji w ustalonym scenariuszu. Symulacje wykrywają dominację czekania oraz nieosiągalne dostawy; nie dowodzą przyjemności z grania. Po MVP osobno ocenić czytelność obrotów, różnorodność tras i rzeczywistą grywalność. Jeśli dowody są słabe, agent zmniejsza planszę lub liczbę zagrożeń i zapisuje ADR, bez zmiany gatunku i dodawania fabuły.

Poza zakresem: multiplayer, modding, live AI, mikrotransakcje, dubbing, ogólny silnik, porty na inne platformy, edytor dla graczy i domyślna telemetria sieciowa.

MVP ma dwa zarejestrowane moduły. Nagroda oferuje najwyżej dwa nieposiadane ID w kanonicznym porządku. Jeden posiadany moduł pozostawia jedną opcję; pełny zestaw daje pustą ofertę i w przyszłym runie przejście bez nagrody. Nie dodajemy duplikatów ani zastępczej waluty.
