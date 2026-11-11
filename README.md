# Projekt: Zamknięty Obieg

Robocza, **wewnętrzna** nazwa koncepcji; nie jest zatwierdzonym tytułem do wyświetlania w grze lub na Steam.

Taktyczny roguelike bez fabuły: gracz przełącza połączenia małej planszy. Ten sam obieg przenosi gracza, ładunki i zagrożenia. Ułatwienie dostawy może otworzyć drogę przeciwnikowi. Kolejne próby zmieniają konfiguracje i dostępne narzędzia, bez permanentnego podnoszenia statystyk.

**Windows x64 · C++20 · singleplayer · bez zewnętrznego silnika · docelowo Steam.**

[Publiczne repozytorium GitHub](https://github.com/StanislavGoorka/ThisGameMakesSteamAITagCompletelyUseless).

AI może projektować mechanikę, pisać kod i narzędzia. Nie może tworzyć treści audiowizualnych dla gracza. Treści zewnętrzne wymagają weryfikacji autorstwa, licencji i zgody przed pobraniem. Tekst roboczy zawsze zaczyna się od `[AI PLACEHOLDER]` i jest wykluczony z wydania. Nie pobrano żadnych assetów.

## Stan projektu

Ukończono planowanie P000, etap M1 (P001–P005), identyfikatory domeny `NodeId` i `EntityId` w P006, maskę portów z obrotem w P007, sąsiedztwo na siatce 7×7 w P008, komendę obrotu pojedynczego węzła w P009, najkrótszą trasę BFS w P010, deterministyczny generator SplitMix64 w P011, losowanie z przedziału bez obciążenia modulo w P012, wartościowy `GameState` w P013, walidację ruchu gracza w P014, akcję obrotu w zasięgu gracza w P015, akcję `wait` z integralnością gracza oraz naliczaniem tury w P016, krok pakietu po najkrótszej trasie w P017, krok dostawy pakietu stojącego na celu w P018-01, odbiorniki sektora z predykatem celu następnego pakietu w P018-02 oraz wejście sektora i osobny krok wprowadzenia pakietu na początku tury w P018-03. P019-01 dodało wartościową kolekcję impulsów i predykat zajęcia ich pola, P019-02 wybór kroku pościgu bez mutacji stanu, a P019-03 ruch impulsów w rosnącej kolejności ID. P020-01 dodało pojedynczy kontakt impulsu z limitem obrażeń. CMake udostępnia cel interfejsowy `game_core` z wymaganiem C++20 i dziewiętnaście testów CTest. Obie konfiguracje MSVC x64 sprawdzono lokalnie. [Raport P020-01](docs/workflow/history/P020-01.md)

P018 zamknięto jako wpis planistyczny: dostawę pakietu rozbito na zadania P018-01, P018-02 i P018-03. Wszystkie trzy dzieci są zaimplementowane. Kroki wprowadzenia, ruchu i dostawy pozostają osobnymi funkcjami; ich kolejność ustali resolver w P021. [Raport P018](docs/workflow/history/P018.md)

P019 zamknięto wyłącznie jako podział planu. Wszystkie trzy dzieci są zaimplementowane: `GameState::pulses` przechowuje impulsy z `EntityId` i pozycją `NodeId`, `pulse_occupies` odczytuje zajęcie, a `next_pulse_step` wybiera jedną krawędź BFS do bieżącej pozycji gracza bez mutacji stanu. Zajęty pierwszy krok oznacza czekanie, bez objazdu. Impuls pozwala graczowi wejść na swoje pole; osobna flaga `blocked_nodes` nadal blokuje ruch gracza. `advance_pulses` przesuwa każdy impuls najwyżej raz w rosnącej kolejności ID, zachowuje kolejność kolekcji i czyta zajęcie po wcześniejszych ruchach. Jednostka, która czekała, nie ponawia próby. Kontakt działa osobno w P020-01; pełna faza pozostaje do P020-02. [Raport P019](docs/workflow/history/P019.md) · [Raport P019-03](docs/workflow/history/P019-03.md)

P020 zamknięto wyłącznie jako podział planu. P020-01 implementuje `resolve_pulse_contact(GameState&, EntityId, bool&)`: kontakt usuwa wskazany impuls i odbiera jeden HP tylko przy świeżej blokadzie i dodatniej integralności. Wspólna blokada pozostaje poza stanem, a następne kontakty nadal usuwają impulsy bez obrażeń. P020-02 połączy kontakt przed ruchem i po każdym ruchu z fazą impulsów oraz jednym limitem na fazę; P021 zależy od tego dziecka. [Raport P020](docs/workflow/history/P020.md) · [Raport P020-01](docs/workflow/history/P020-01.md)

Następne zadanie to **P020-02 — faza impulsów z natychmiastowym kontaktem**. Polecenie **„Zrealizuj następny krok”** uruchamia pracę zgodnie z instrukcjami projektu. [AGENTS.md](AGENTS.md) · [Przekazanie pracy](docs/workflow/NEXT.md)

## Mapa dokumentacji

| Dokument | Zastosowanie |
| --- | --- |
| [Koncepcja i reguły](docs/GAME_DESIGN.md) | Pętla gry, reguły tur, zakres MVP i 1.0 |
| [Analiza gier](docs/RESEARCH.md) | Punkty odniesienia, alternatywy i uzasadnienie wyboru |
| [Architektura](docs/ARCHITECTURE.md) | Moduły C++, Windows, zapis, achievementy |
| [Toolchain](docs/TOOLCHAIN.md) | Sprawdzone lokalne wersje i ścieżki narzędzi Windows |
| [Plan etapów](docs/ROADMAP.md) | Bramki etapów i sposób rozbijania pracy |
| [Zadania](docs/plan/tasks.json) | Kolejka mikro-kroków i zależności |
| [Workflow](docs/WORKFLOW.md) | Jedno uruchomienie = jeden mały krok |
| [Stan](docs/workflow/STATE.json), [przekazanie](docs/workflow/NEXT.md) | Wykonane zadania i instrukcja dla następcy |
| [Treści i pochodzenie](docs/CONTENT_POLICY.md) | Zakaz generowania assetów, zgody i blokady wydania |
| [Kandydaci na assety](docs/ASSET_CANDIDATES.md) | Źródła do przyszłej weryfikacji, bez zgody na pobranie |
| [Testy](docs/TESTING.md) | Determinizm, błędy, QA Windows i bramki |
| [Wydanie Steam](docs/STEAM_RELEASE.md) | Integracja, ankieta i zewnętrzne warunki wydania |
| [Decyzje i ryzyka](docs/DECISIONS.md) | Przyjęte założenia i kryteria zmiany kierunku |

## Komendy działające obecnie

Z katalogu repozytorium, w PowerShell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1
powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Test-ProjectValidator.ps1
git -c safe.directory=D:/ThisGameMakesSteamAITagCompletelyUseless diff --check
$plan = Get-Content docs/plan/tasks.json -Raw -Encoding UTF8 | ConvertFrom-Json
$state = Get-Content docs/workflow/STATE.json -Raw -Encoding UTF8 | ConvertFrom-Json
$plan.tasks | Where-Object id -eq $state.current_task | Format-List
```

Walidator kontroluje kolejkę, stan, odnośniki Markdown i rejestry treści. Druga komenda sprawdza jego zachowanie na izolowanych próbkach błędów. Nie jest to audyt autorstwa ani test gry. `ExecutionPolicy Bypass` dotyczy tylko uruchamianego procesu; nie zmienia polityki systemowej. Jest potrzebne tutaj, ponieważ lokalna polityka blokuje niepodpisane skrypty.

Lokalny dysk nie zapisuje informacji o właścicielu katalogu. Opcja `git -c safe.directory=...` ustanawia wyjątek tylko dla tego katalogu i wywołania; w razie komunikatu „dubious ownership” dodawaj ją także do innych komend Git. Nie potrzeba globalnego `safe.directory=*`.

## Build i test C++20

Wymagane są CMake 3.26+, Visual Studio Build Tools 2022 z MSVC x64 i Windows SDK. Uruchom poniższe komendy w PowerShell z katalogu repozytorium. Generator sam wykrywa instalację VS.

```powershell
cmake --preset windows-msvc
cmake --build --preset windows-debug
ctest --preset windows-debug --output-on-failure
cmake --build --preset windows-release
ctest --preset windows-release --output-on-failure
```

Obie konfiguracje korzystają z katalogu `build/windows-msvc/`, ignorowanego przez Git. `core_build_smoke` sprawdza kompilację, linkowanie i działanie elementów biblioteki standardowej C++20 (`span`, ranges). `core_identifier_ranges` sprawdza granice identyfikatorów i ich porządek, a podczas kompilacji także niedozwolone konwersje. `core_port_rotation` sprawdza obrót wszystkich 16 masek i odrzucanie wartości spoza zakresu. `core_grid_adjacency` sprawdza wzajemne porty i granice planszy. `core_node_rotation` sprawdza zmianę połączeń, odmowę obrotu węzła nieruchomego i powrót planszy do stanu początkowego po czterech obrotach. `core_grid_pathfinding` sprawdza najkrótsze trasy, remisy i nieosiągalne cele. `core_splitmix64` sprawdza wektory surowego generatora, powtarzalność losowania z przedziału, jego granice i ponawianie odrzuconych losowań. Sprawdzenie celowego błędu testu kompilacji i uruchomienia opisano w raporcie. [Raport P002](docs/workflow/history/P002.md)

`core_game_state` sprawdza inicjalizację planszy i gracza z pełną integralnością, odrzucanie błędnych danych wejściowych oraz niezależność kopii stanu, także wejścia sektora oraz ID, pozycji i kolekcji impulsów w dwóch kolejnych kopiach. Stan początkowy nie ma pakietu, zasilonego celu, odbiornika, wejścia sektora ani impulsów.

`core_player_movement` sprawdza ruch po wzajemnej krawędzi do wolnego sąsiada, odrzucanie braku połączenia, zawijania planszy i blokującej jednostki oraz zachowanie stanu przy błędzie. Test obejmuje też wejście na impuls bez obrażeń i jego usuwania oraz odmowę, gdy to samo pole ma osobną flagę blokowania. Sam ruch zmienia wyłącznie pozycję; rozliczanie tury powstanie osobno.

`core_player_rotation` sprawdza obrót własnego węzła i ortogonalnych sąsiadów, również bez połączenia i na zajętym polu, oraz odrzucanie celów dalekich, po przekątnej, przez zawinięcie planszy i nieruchomych. Sukces zmienia tylko porty celu; błąd zachowuje cały stan. Rozliczanie tury pozostaje osobnym krokiem.

`core_player_wait` sprawdza, że `wait` żywego gracza dolicza dokładnie jedną turę i nie zmienia planszy, pozycji ani integralności, także na polu bez żadnej innej legalnej akcji i przy stu kolejnych turach. Martwy gracz odrzuca akcję bez zmiany stanu. Licznik tur zwiększa wyłącznie `charge_turn`; ruch i obrót nadal go nie naliczają.

`core_packet_movement` sprawdza, że pakiet przesuwa się o dokładnie jedną krawędź najkrótszej trasy do celu, z remisami w kolejności N, E, S, W, i zmienia wyłącznie własną pozycję. Obrót węzła wykonany przez gracza zmienia wybraną trasę. Pakiet bez trasy czeka z tym samym celem i rusza po przywróceniu połączenia. Pakiet i gracz mogą zajmować to samo pole. Krok ruchu nie obsługuje dostawy.

`core_packet_delivery` sprawdza, że pakiet stojący na celu znika z planszy, a stan zapisuje dokładnie ten cel jako zasilony, bez naliczenia tury i bez zmiany reszty stanu. Powtórzony krok, pakiet poza celem, stan bez pakietu i pakiet na celu już zasilonym niczego nie zmieniają. Pakiet doprowadzony krokami ruchu zostaje dostarczony, po czym plansza pozostaje bez pakietu. Druga dostawa do innego celu zachowuje zapis pierwszej. Dostawa nie zależy od gracza ani jednostek blokujących.

`core_packet_target` sprawdza, że celem następnego pakietu jest niezasilony odbiornik o najniższym `NodeId`, niezależnie od układu odbiorników na planszy, także dla identyfikatorów 0 i 48. Zasilone odbiorniki są pomijane; bez odbiorników i po zasileniu wszystkich celu nie ma. Zasilony węzeł niebędący odbiornikiem, aktywny pakiet, gracz, jednostki blokujące i porty nie zmieniają wyniku, a predykat nie zmienia stanu. Kolejne dostawy przesuwają cel na następny odbiornik, aż żaden nie pozostanie. Predykat nie umieszcza pakietu na planszy.

`core_packet_introduction` sprawdza, że krok początku tury tworzy pakiet wyłącznie na wejściu, z celem wskazanym przez predykat, i zachowuje resztę stanu. Aktywny pakiet, brak wejścia, brak odbiorników i zasilenie wszystkich odbiorników blokują wprowadzenie. Gracz, jego integralność, jednostki blokujące i porty nie wpływają na wynik. Test obejmuje wejście na celu, wejścia 0 i 48 oraz sekwencję dwóch dostaw: następny pakiet powstaje na początku kolejnej tury i rusza w niej tylko o jedną krawędź. Granice tur w teście wyznacza ręczna kolejność wywołań; resolver jeszcze nie istnieje.

`core_pulse_occupancy` sprawdza zajęte i wolne pola dla pustej i niepustej kolekcji, w tym granice 0 i 48, po odwróceniu kolejności jednostek i przy zmianie pozostałych danych planszy. Powtarzany predykat zachowuje cały stan, w tym licznik tur. Poprawny stan ma unikalne ID impulsów i najwyżej jeden impuls na polu; gracz i pakiet mogą z nim współdzielić pole.

`core_pulse_movement` sprawdza ruch o jedną krawędź, czekanie bez trasy, pustą kolekcję i kontakt bez usuwania jednostki lub obrażeń. Wszystkie sześć permutacji trzech impulsów daje te same pozycje według ID przy zachowanej kolejności kolekcji. Scenariusze obejmują zwolnienie pola dla późniejszej jednostki, brak ponownej próby po późniejszym zwolnieniu, pierwszeństwo niższego ID przy wspólnym celu i czekanie mimo dłuższego objazdu. Porównanie pełnego stanu chroni pakiet, flagi, integralność i licznik tur.

`core_pulse_contact` sprawdza brak ID i kontaktu, ponowienie po usunięciu, graniczne ID i pozycje, HP 3/1/0 i wykorzystany limit w każdej permutacji trzech impulsów. Wszystkie kontakty z jedną blokadą usuwają jednostki, ale HP spada tylko raz; świeża blokada pozwala na następne obrażenie. Test wejścia gracza na impuls sprawdza pełny stan, w tym pakiet i licznik tur.

CI buduje i testuje Debug oraz Release na MSVC x64 po pushu do `main` i przy pull requestach. [Workflow Windows CI](.github/workflows/windows.yml)

## Styl C++

`.clang-format` ustawia wcięcia na cztery spacje i klamry otwierające w osobnej linii. `.editorconfig` ustawia UTF-8 i LF, z CRLF dla skryptów PowerShell zgodnie z `.gitattributes`. Nazwy funkcji i zmiennych zapisuj w `snake_case`, a typów w `PascalCase`; formatter nie sprawdza nazw.

Sprawdzenie formatowania obecnych źródeł bez ich zmiany:

```powershell
clang-format --style=file --dry-run --Werror src/core/identifiers.hpp src/core/port_mask.hpp src/core/grid_adjacency.hpp src/core/node_rotation.hpp src/core/grid_pathfinding.hpp src/core/splitmix64.hpp src/core/game_state.hpp src/core/player_movement.hpp src/core/player_rotation.hpp src/core/turn_cost.hpp src/core/player_wait.hpp src/core/packet_movement.hpp src/core/packet_delivery.hpp src/core/packet_target.hpp src/core/packet_introduction.hpp src/core/pulse_occupancy.hpp src/core/pulse_chase.hpp src/core/pulse_movement.hpp src/core/pulse_contact.hpp tests/unit/build_tests.cpp tests/unit/identifier_tests.cpp tests/unit/port_mask_tests.cpp tests/unit/grid_adjacency_tests.cpp tests/unit/node_rotation_tests.cpp tests/unit/grid_pathfinding_tests.cpp tests/unit/splitmix64_tests.cpp tests/unit/game_state_tests.cpp tests/unit/player_movement_tests.cpp tests/unit/player_rotation_tests.cpp tests/unit/player_wait_tests.cpp tests/unit/packet_movement_tests.cpp tests/unit/packet_delivery_tests.cpp tests/unit/packet_target_tests.cpp tests/unit/packet_introduction_tests.cpp tests/unit/pulse_occupancy_tests.cpp tests/unit/pulse_chase_tests.cpp tests/unit/pulse_movement_tests.cpp tests/unit/pulse_contact_tests.cpp
```

## Narzędzia utrzymaniowe i licencja

Projekt nie wymaga Pythona. Skrypty utrzymaniowe korzystają z Windows PowerShell 5.1+. Nie ustalono jeszcze licencji własnego kodu; publiczne repozytorium nie oznacza udzielenia licencji na wykorzystanie jego zawartości.
