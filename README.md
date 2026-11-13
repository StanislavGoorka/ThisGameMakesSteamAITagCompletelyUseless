# Projekt: Zamknięty Obieg

Robocza, **wewnętrzna** nazwa koncepcji; nie jest zatwierdzonym tytułem do wyświetlania w grze lub na Steam.

Taktyczny roguelike bez fabuły: gracz przełącza połączenia małej planszy. Ten sam obieg przenosi gracza, ładunki i zagrożenia. Ułatwienie dostawy może otworzyć drogę przeciwnikowi. Kolejne próby zmieniają konfiguracje i dostępne narzędzia, bez permanentnego podnoszenia statystyk.

**Windows x64 · C++20 · singleplayer · bez zewnętrznego silnika · docelowo Steam.**

[Publiczne repozytorium GitHub](https://github.com/StanislavGoorka/ThisGameMakesSteamAITagCompletelyUseless).

AI może projektować mechanikę, pisać kod i narzędzia. Nie może tworzyć treści audiowizualnych dla gracza. Treści zewnętrzne wymagają weryfikacji autorstwa, licencji i zgody przed pobraniem. Tekst roboczy zawsze zaczyna się od `[AI PLACEHOLDER]` i jest wykluczony z wydania. Po zgodzie P056 przyjęto w P057 jeden niezmieniony atlas Kenney 1.2 i dwie noty; [dowody importu](docs/assets/KENNEY_1BIT_1_2_IMPORT.md).

## Stan projektu

Tryb treści jest niezależny od Debug/Release. Domyślny preset windows-msvc
buduje wariant developerski. Public: `cmake --preset windows-public`, następnie
`cmake --build --preset windows-public-debug` lub `windows-public-release`
i odpowiedni `ctest --preset ... --output-on-failure`. Każdy build sprawdza
payload; public dodatkowo odrzuca placeholdery w rejestrze napisów.
To kontrola rejestru, a nie gotowa paczka wydania. [Kontrakt](docs/PUBLIC_CONTENT.md)

Obecnie public build celowo odmawia z powodu oznaczonego tytułu developer.
`build/windows-msvc/Debug/developer_window.exe` (także Release) uruchamia
planszę start_run z jednostkami i rolami pól z zatwierdzonego atlasu,
z obsługą odmalowania, resize i tur core z klawiatury. HUD nie jest podłączony.
Sterowanie developer: strzałki — ruch, R — obrót, O — overdrive, Space — wait.
R/O używają pola pod kursorem, a bez wskazania własnego pola. Tab przełącza
planowanie, klawisz akcji wybiera komendę, Enter wykonuje ją raz, Escape
anuluje. Focus loss czyści wybór. Wybór pokazuje wynik core i trasę pakietu
gotowymi symbolami; anulowanie przywraca stan sesji. Przejścia sektorów
i zapis z UI pozostają kolejnymi zadaniami.
Ostatnia pełna pula: Debug i Release po 107/107 CTest. Brak testów VM,
fizycznego resetu GPU i obserwacji pulpitu (niedostępny Computer Use).

Ukończono planowanie P000, etap M1 (P001–P005), identyfikatory domeny `NodeId` i `EntityId` w P006, maskę portów z obrotem w P007, sąsiedztwo na siatce 7×7 w P008, komendę obrotu pojedynczego węzła w P009, najkrótszą trasę BFS w P010, deterministyczny generator SplitMix64 w P011, losowanie z przedziału bez obciążenia modulo w P012, wartościowy `GameState` w P013, walidację ruchu gracza w P014, akcję obrotu w zasięgu gracza w P015, akcję `wait` z integralnością gracza oraz naliczaniem tury w P016, krok pakietu po najkrótszej trasie w P017, krok dostawy pakietu stojącego na celu w P018-01, odbiorniki sektora z predykatem celu następnego pakietu w P018-02 oraz wejście sektora i osobny krok wprowadzenia pakietu na początku tury w P018-03. P019-01 dodało wartościową kolekcję impulsów i predykat zajęcia ich pola, P019-02 wybór kroku pościgu bez mutacji stanu, a P019-03 ruch impulsów w rosnącej kolejności ID. P020-01 dodało pojedynczy kontakt impulsu z limitem obrażeń, a P020-02 pełną fazę impulsów z natychmiastowym kontaktem. P021-01 dodało wartościowe komendy move/rotate/wait i niemutujący predykat legalności, a P021-02 połączyło kroki w wartościowy resolver tury. CMake udostępnia cel interfejsowy `game_core` z wymaganiem C++20 i dwadzieścia dziewięć testów CTest. Obie konfiguracje MSVC x64 sprawdzono lokalnie. [Raport P021-02](docs/workflow/history/P021-02.md)

P018 zamknięto jako wpis planistyczny: dostawę pakietu rozbito na zadania P018-01, P018-02 i P018-03. Wszystkie trzy dzieci są zaimplementowane. Kroki wprowadzenia, ruchu i dostawy pozostają osobnymi funkcjami; ich kolejność ustala resolver P021-02. [Raport P018](docs/workflow/history/P018.md)

P019 zamknięto wyłącznie jako podział planu. Wszystkie trzy dzieci są zaimplementowane: `GameState::pulses` przechowuje impulsy z `EntityId` i pozycją `NodeId`, `pulse_occupies` odczytuje zajęcie, a `next_pulse_step` wybiera jedną krawędź BFS do bieżącej pozycji gracza bez mutacji stanu. Zajęty pierwszy krok oznacza czekanie, bez objazdu. Impuls pozwala graczowi wejść na swoje pole; osobna flaga `blocked_nodes` nadal blokuje ruch gracza. `advance_pulses` przesuwa każdy impuls najwyżej raz w rosnącej kolejności ID, zachowuje kolejność kolekcji i czyta zajęcie po wcześniejszych ruchach. Jednostka, która czekała, nie ponawia próby. Sam ruch nadal nie obsługuje kontaktów; robi to osobna faza P020-02. [Raport P019](docs/workflow/history/P019.md) · [Raport P019-03](docs/workflow/history/P019-03.md)

P020 zamknięto wyłącznie jako podział planu; wszystkie dzieci są zaimplementowane. P020-01 implementuje `resolve_pulse_contact(GameState&, EntityId, bool&)`: kontakt usuwa wskazany impuls i odbiera jeden HP tylko przy świeżej blokadzie i dodatniej integralności. P020-02 dodaje `resolve_pulse_phase(GameState&)`: najpierw usuwa wszystkie kontakty zastane, potem wykonuje po jednym ruchu ocalałych według ID i natychmiast usuwa nowe kontakty. Jedna lokalna blokada ogranicza wszystkie obrażenia do jednego HP na fazę, także po śmierci impulsy nadal znikają. Kolejna faza ma świeży limit; resolver wywołuje ją raz na przyjętą turę. [Raport P020](docs/workflow/history/P020.md) · [Raport P020-02](docs/workflow/history/P020-02.md)

P021 zamknięto jako podział planu; oba zadania potomne są zaimplementowane. P021-01 implementuje `Command` jako wartościowe warianty `MoveCommand`, `RotateCommand` i `WaitCommand`, z silnym `NodeId` dla celów. `is_command_legal` sprawdza reguły ruchu/obrotu na niezależnej kopii; wait nie nalicza tury podczas odczytu. Od P022-02 każdy wariant wymaga trwającego sektora. P021-02 implementuje `resolve_turn(const GameState&, const Command&)` zwracające `TurnResult` z niezależnym `state` i `accepted`, rozszerzone w P022-02 o `outcome`. Odmowa zachowuje cały stan bez faz. Sukces wykonuje wprowadzenie pakietu, akcję, ruch pakietu, dostawę, jedną fazę impulsów i końcowy koszt jednej tury, również po śmierci. Dopiero wtedy podaje wynik sektora. Wejściowy stan i komenda pozostają nietknięte. Resolver nie przyjmuje RNG i nie losuje. Ostatnie Debug i Release po P022-02 zaliczyły po 23/23 CTest. [Raport P021](docs/workflow/history/P021.md) · [Raport P021-02](docs/workflow/history/P021-02.md)

P022 zamknięto jako podział planu; oba dzieci są zaimplementowane. P022-01 implementuje opcjonalne `GameState::sector_exit` oraz niemutujące `sector_outcome(const GameState&)` z wynikiem `SectorOutcome`: ongoing, completed, death lub overload. HP=0 ma pierwszeństwo; żywy gracz na wyjściu z co najmniej dwoma zasilonymi odbiornikami wygrywa do tury 48 włącznie. Bez sukcesu co najmniej 48 tur oznacza przeciążenie; po limicie nie ma spóźnionej wygranej. Wynik jest wyliczany bez trwałej kopii i bez wykonywania faz. P022-02 podaje go w `TurnResult::outcome` po zagrożeniach i koszcie. Każda komenda terminalnego sektora jest odrzucana przed fazami i nie nalicza tury 49; odmowa podaje dotychczasowy wynik. Niskopoziomowe helpery zachowały swoje kontrakty. [Raport P022-01](docs/workflow/history/P022-01.md) · [Raport P022-02](docs/workflow/history/P022-02.md)

P023 implementuje `preview_turn(const GameState&, const Command&)`, które deleguje do wartościowego resolvera bez dodatkowej kopii i bez dostępu do zapisu, achievementów oraz RNG. Podgląd i wykonanie tury zwracają identyczne state/accepted/outcome; wyniki posiadają niezależne kopie. Korpus testów chroni pełny live state i komendę przy powtórzeniach, legalnych akcjach, odmowach oraz końcu sektora. Debug i Release po P023 zaliczyły po 24/24 CTest. [Raport P023](docs/workflow/history/P023.md)

P024 zamknięto jako podział planu. P024-01 implementuje deweloperskie `state_hash(const GameState&)`: FNV-1a 64-bit jawnego kodowania v4 wszystkich pól, ze stałymi szerokościami little-endian, tagami optional i zachowaniem kolejności impulsów. Hash służy regresji, jest niekryptograficzny i nie gwarantuje braku kolizji. P024-02 implementuje `seeded_fixture(RunSeed)`: niewyświetlany stały kwadrat węzłów 0/1/7/8, z lokalnym SplitMix64 wybierającym wyłącznie orientację węzła 1. P024-03 implementuje `run_replay(version, seed, commands)`: wersja 8 zwraca początkowy hash, accepted/outcome/hash po każdej próbie i niezależny stan końcowy; obca wersja nie zwraca wyniku. Runner używa tego samego resolvera i nie przerywa po odmowie. P024-04 implementuje `developer_replay.exe`: pełne parsowanie argumentów przed runnerem oraz stabilny numeryczny ślad. Błędne argumenty lub obca wersja dają niezerowy kod i pusty stdout. Narzędzie jest budowane przy BUILD_TESTING i pozostaje poza paczką gry. Fixture jest odrębny od generatora sektorów P028 i nie obiecuje wygrywalności. Debug i Release po P024-04 zaliczyły po 28/28 CTest. [Raport P024-04](docs/workflow/history/P024-04.md) [Raport P024-03](docs/workflow/history/P024-03.md) [Raport P024-02](docs/workflow/history/P024-02.md) · [Raport P024](docs/workflow/history/P024.md) · [Raport P024-01](docs/workflow/history/P024-01.md)

P025 potwierdza wspólny obieg w jednym niewyświetlanym eksperymencie z trzema sekwencjami po trzy tury. Obrót węzła 7 otwiera krawędź dla pakietu i impulsu: pakiet zostaje dostarczony, a zagrożenie rusza do gracza. Kolejny obrót zamyka pościg i zachowuje HP. Pełne dane, trasy i ograniczenia zawiera [raport P025](docs/workflow/history/P025.md). To dowód działania reguł w syntetycznym scenariuszu, bez oceny grywalności lub balansu. Debug i Release zaliczyły po 29/29 CTest.

P026 zamknięto wyłącznie jako podział planu na pięć dzieci: dane rezerwy/długu i pełny hash, regenerację dostawy, atomowy helper, integrację komendy z turą oraz CLI. P026-01 jest zaimplementowane: reserve 2, limit 3, debt 0; pełny hash v2. P026-02 regeneruje rezerwę po dostawie, replay 4. P026-03 dodaje helper overdrive, a P026-04 integruje OverdriveCommand z turą i podglądem. P026-05 udostępnia token overdrive:NodeId w CLI. [Raport P026-01](docs/workflow/history/P026-01.md)

Następne zadanie to **P069-02**. [Przekazanie pracy](docs/workflow/NEXT.md)

P056 otrzymało rzeczywistą zgodę na
[jedno darmowe pobranie Kenney 1-Bit Pack 1.2](docs/assets/REQUEST_KENNEY_1BIT_1_2.md).
Zakres zapisano w rejestrze, a P057 przyjął minimalny zestaw z kontrolą bajtów.
P058 jest podziałem planu; wszystkie cztery dzieci dostarczają model, parser,
chroniony odczyt z SHA-256 i dowód integracji na trzech plikach P057.
P059 dostarcza bramkę zawartości paczki, P060 katalog tekstów, a dzieci P061
politykę developer/public i jej integrację CMake. Wszystkie dzieci P062,
P063 i P064 są ukończone; P065 dekoduje zatwierdzony atlas bez renderowania.

P045: Specyfikacja pięciu predykatów z przykładami, resetem i konserwatywną historią po restore; zaplanowano dwa dzieci dowodu zmiany trasy przed P046. [Raport](docs/workflow/history/P045.md)

P045-01: ActionCommitted przechowuje opcjonalny cel zmienionej trasy; równość i konflikt dispatchera obejmują dowód, a wait/move z dowodem są odrzucane przed evaluatorem. [Raport](docs/workflow/history/P045-01.md)

P045-02: Wrapper porównuje pełne trasy BFS tego samego pakietu przed i po rotacji. Dodano dosłowne testy otwarcia, zmiany, negatywnych przypadków, zgodności preview/core i odtworzenia checkpointu. [Raport](docs/workflow/history/P045-02.md)

P046: MvpAchievementEvaluator zgłasza first_delivery wyłącznie dla PacketDelivered. Testy obejmują wszystkie cele/sektory, odmowę zapisu i retry, dostawę przed śmiercią oraz niezmienne pliki po powtórce w nowej gałęzi. [Raport](docs/workflow/history/P046.md)

P047: Evaluator zgłasza first_sector wyłącznie dla SectorCompleted. Scenariusze wszystkich sektorów sprawdzają turę 48, śmierć na wyjściu, brak drugiej dostawy i brak sukcesu poza wyjściem; powtórka zachowuje profil i backup. [Raport](docs/workflow/history/P047.md)

P048: Dodano jawne RunHistoryOrigin i tracker overdrive przez wszystkie sektory. Tylko pełna nowa próba bez użycia i zwycięstwo kwalifikują; restore, porażka i niepoprawny początek strumienia nie. Testy resetu instancji i trwałości PASS. [Raport](docs/workflow/history/P048.md)

P049: Dodano tracker 49 celów rekonfiguracji: dowód z akcji, konsumpcja przy dostawie, reset sektora i instancji. Testy wszystkich celów, obu rotacji, innego celu i retry po konsumpcji oraz rzeczywista dostawa po otwarciu trasy PASS. [Raport](docs/workflow/history/P049.md)

P050: Ukończono pięć predykatów MVP. RunEnded(won) sektora 3 zgłasza completed_run, wraz z run_without_overdrive przy znanej czystej historii. Testy negatywne, częściowego potwierdzenia dwóch ID, retry i trwałości rzeczywistych zdarzeń końcowej tury PASS. [Raport](docs/workflow/history/P050.md)

P051: Rozbito integrację na dwa niezależnie oceniane dowody: pełny run i restore w procesie oraz osobny restart procesu. P052 zależy od P051-02; bez implementacji dzieci. [Raport](docs/workflow/history/P051.md)

P051-01: Pełny run root seedu 1 przez rzeczywiste predykaty i lokalny sink odblokowuje maskę 31 z backupem 15. Zapisany/odczytany checkpoint sektora 2 odtwarza wszystkie stany i payloady; pusty profil z nieznaną historią dostaje 27 i backup 11. Preview, duplikaty i świeża gałąź zachowują pliki. Bez zmian produkcyjnych. [Raport](docs/workflow/history/P051-01.md)

P051-02: Dwa nowe procesy odczytują checkpoint sektora 2 i porównują każdy pełny stan oraz payload sektorów 2–3 z niezależną kontynuacją pełnego runu. Rzeczywisty evaluator zgłasza 27; pusty profil otrzymuje 27/backup 11, profil 31/backup 15 zachowuje dokładne bajty. Drugi proces zachowuje oba main/bak. Preview i duplikaty nie wywołują usług. [Raport](docs/workflow/history/P051-02.md)

P052: Dodano developer_preview_benchmark: 1000 stałych seedów, trzy rodziny obciążenia, legalny wait i odmowa ruchu, po 3000 próbek na przypadek. Raport sprzętu i dwóch pomiarów Release w docs/PREVIEW_BENCHMARK.md: p95 maksymalnie 4700 ns w syntetycznym stresie 49 impulsów. CTest kontroluje kontrakt CLI bez progu czasu; wynik nie ocenia grywalności. [Raport](docs/workflow/history/P052.md)

P053: Dodano deterministyczny test 16384 komend: 64 seedy, dwie rodziny zagrożeń, cztery zestawy modułów, sukcesy i odmowy wszystkich akcji. Pełne stany preview i dwóch wykonań są identyczne; atomowe odmowy, HP, tury, porty, zasoby i unikalne ID/pola impulsów spełniają niezmienniki. Błąd wypisuje pełny reproducer. [Raport](docs/workflow/history/P053.md)

P054: Przejrzano i przyjęto bramkę headless M4: przejście do dossier treści M5, bez nowych reguł. docs/HEADLESS_GATE.md wiąże dowody core, seedów, replayów, checkpointów, profilu i pięciu predykatów z ich ograniczeniami. Powtórzone Debug/Release i osobny Release AddressSanitizer: po 83/83 testy PASS; 11 testów walidatora PASS. Brak QA wizualnego i oceny grywalności. [Raport](docs/workflow/history/P054.md)

P055: Dossier Kenney 1-Bit Pack 1.2 dokumentuje CC0 dla gry i publicznego Git, deklarację non-AI przy konkretnym pliku i devlog autora. Zapisano różnice nazw/rozmiarów archiwów i brakujące kontrole po zgodzie. Nie pobrano plików ani preview; rejestry assetów i zgód pozostają puste. [Raport](docs/workflow/history/P055.md)

P056: Zapisano rzeczywistą odpowiedź użytkownika: „Tak, zgadzam się na ten zakres”. Zgoda kenney-1bit-1.2-2026-11-13 ma status approved; usunięto blokadę. Dotyczy wyłącznie jednego darmowego pobrania wersji 1.2 z itch.io i minimalnego importu niezmienionych PNG/not po weryfikacji. Bez pobrania w kroku P056. [Raport](docs/workflow/history/P056.md)

P057: Pobrano raz zatwierdzony 1bitpack_kenney_1.2.zip: 657579 bajtów, wersja 1.2 i CC0 potwierdzone w License.txt. Przyjęto jeden niezmieniony atlas PNG 832x373 i dwie noty, 20259 bajtów, z kompletnym rejestrem i SHA-256. Archiwum pozostało ignorowane, pozostałych grafik/scen nie rozpakowano. Git zachowuje bajty approved bez normalizacji. Testy walidatora obsługują teraz niepusty rejestr. [Raport](docs/workflow/history/P057.md)

P058: P058 zamknięto wyłącznie jako podział na cztery kontrakty: wartościowe odwołania, pamięciowy parser JSON Windows, bounded read z SHA-256 oraz test integracji rzeczywistego importu. P059 zależy od P058-04; samo zamknięcie planu nie dostarcza czytnika. ASSET_MANIFEST.md ustala limity, kontrolowane odmowy i granice dowodów. [Raport](docs/workflow/history/P058.md)

P058-01: Dodano owning AssetReference tworzony po walidacji ID, kanonicznej ścieżki Windows pod approved, dokładnego hex SHA-256 i długości 0..64 MiB. Manifest do 256 odwołań odrzuca duplikaty ID i ścieżek bez rozróżniania ASCII case; lookup nieznanego ID lub invalid listy zwraca brak wyniku. Kopie oraz wartości po move są chronione; bez JSON i IO. [Raport](docs/workflow/history/P058-01.md)

P058-02: Dodano bounded parser UTF-8 manifestu przez systemowy Windows.Data.Json/C++WinRT. Zwraca cały zwalidowany model albo brak wyniku dla błędów JSON, wersji, pól, statusu, hasha/ID/ścieżki, duplikatów i limitów 1 MiB/256 rekordów/głębokości32. Ścisły MultiByteToWideChar odrzuca invalid UTF-8 przed konwersją. Bez IO i renderowania; zakończono zamówione 10 kroków. [Raport](docs/workflow/history/P058-02.md)

## Mapa dokumentacji

Pomiar podglądu: `.\build\windows-msvc\Release\developer_preview_benchmark.exe`.
[Korpus, sprzęt i p50/p95](docs/PREVIEW_BENCHMARK.md).

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

`core_game_state` sprawdza inicjalizację planszy i gracza z pełną integralnością, odrzucanie błędnych danych wejściowych oraz niezależność kopii stanu, także wejścia i wyjścia sektora oraz ID, pozycji i kolekcji impulsów w dwóch kolejnych kopiach. Stan początkowy nie ma pakietu, zasilonego celu, odbiornika, wejścia lub wyjścia sektora ani impulsów.

`core_player_movement` sprawdza ruch po wzajemnej krawędzi do wolnego sąsiada, odrzucanie braku połączenia, zawijania planszy i blokującej jednostki oraz zachowanie stanu przy błędzie. Test obejmuje też wejście na impuls bez obrażeń i jego usuwania oraz odmowę, gdy to samo pole ma osobną flagę blokowania. Sam ruch zmienia wyłącznie pozycję; rozliczanie tury powstanie osobno.

`core_player_rotation` sprawdza obrót własnego węzła i ortogonalnych sąsiadów, również bez połączenia i na zajętym polu, oraz odrzucanie celów dalekich, po przekątnej, przez zawinięcie planszy i nieruchomych. Sukces zmienia tylko porty celu; błąd zachowuje cały stan. Rozliczanie tury pozostaje osobnym krokiem.

`core_player_wait` sprawdza, że `wait` żywego gracza dolicza dokładnie jedną turę i nie zmienia planszy, pozycji ani integralności, także na polu bez żadnej innej legalnej akcji i przy stu kolejnych turach. Martwy gracz odrzuca akcję bez zmiany stanu. Licznik tur zwiększa wyłącznie `charge_turn`; ruch i obrót nadal go nie naliczają.

`core_packet_movement` sprawdza, że pakiet przesuwa się o dokładnie jedną krawędź najkrótszej trasy do celu, z remisami w kolejności N, E, S, W, i zmienia wyłącznie własną pozycję. Obrót węzła wykonany przez gracza zmienia wybraną trasę. Pakiet bez trasy czeka z tym samym celem i rusza po przywróceniu połączenia. Pakiet i gracz mogą zajmować to samo pole. Krok ruchu nie obsługuje dostawy.

`core_packet_delivery` sprawdza, że pakiet stojący na celu znika z planszy, a stan zapisuje dokładnie ten cel jako zasilony, bez naliczenia tury oraz regeneruje 1 rezerwy do limitu 3. Powtórzony krok, pakiet poza celem, stan bez pakietu i pakiet na celu już zasilonym niczego nie zmieniają. Pakiet doprowadzony krokami ruchu zostaje dostarczony, po czym plansza pozostaje bez pakietu. Druga dostawa do innego celu zachowuje zapis pierwszej. Dostawa nie zależy od gracza ani jednostek blokujących.

`core_packet_target` sprawdza, że celem następnego pakietu jest niezasilony odbiornik o najniższym `NodeId`, niezależnie od układu odbiorników na planszy, także dla identyfikatorów 0 i 48. Zasilone odbiorniki są pomijane; bez odbiorników i po zasileniu wszystkich celu nie ma. Zasilony węzeł niebędący odbiornikiem, aktywny pakiet, gracz, jednostki blokujące i porty nie zmieniają wyniku, a predykat nie zmienia stanu. Kolejne dostawy przesuwają cel na następny odbiornik, aż żaden nie pozostanie. Predykat nie umieszcza pakietu na planszy.

`core_packet_introduction` sprawdza, że krok początku tury tworzy pakiet wyłącznie na wejściu, z celem wskazanym przez predykat, i zachowuje resztę stanu. Aktywny pakiet, brak wejścia, brak odbiorników i zasilenie wszystkich odbiorników blokują wprowadzenie. Gracz, jego integralność, jednostki blokujące i porty nie wpływają na wynik. Test obejmuje wejście na celu, wejścia 0 i 48 oraz sekwencję dwóch dostaw: następny pakiet powstaje na początku kolejnej tury i rusza w niej tylko o jedną krawędź. Granice tur w tym teście wyznacza ręczna kolejność wywołań; rzeczywistą transakcję sprawdza osobny test resolvera.

`core_pulse_occupancy` sprawdza zajęte i wolne pola dla pustej i niepustej kolekcji, w tym granice 0 i 48, po odwróceniu kolejności jednostek i przy zmianie pozostałych danych planszy. Powtarzany predykat zachowuje cały stan, w tym licznik tur. Poprawny stan ma unikalne ID impulsów i najwyżej jeden impuls na polu; gracz i pakiet mogą z nim współdzielić pole.

`core_pulse_movement` sprawdza ruch o jedną krawędź, czekanie bez trasy, pustą kolekcję i kontakt bez usuwania jednostki lub obrażeń. Wszystkie sześć permutacji trzech impulsów daje te same pozycje według ID przy zachowanej kolejności kolekcji. Scenariusze obejmują zwolnienie pola dla późniejszej jednostki, brak ponownej próby po późniejszym zwolnieniu, pierwszeństwo niższego ID przy wspólnym celu i czekanie mimo dłuższego objazdu. Porównanie pełnego stanu chroni pakiet, flagi, integralność i licznik tur.

`core_pulse_contact` sprawdza brak ID i kontaktu, ponowienie po usunięciu, graniczne ID i pozycje, HP 3/1/0 i wykorzystany limit w każdej permutacji trzech impulsów. Wszystkie kontakty z jedną blokadą usuwają jednostki, ale HP spada tylko raz; świeża blokada pozwala na następne obrażenie. Test wejścia gracza na impuls sprawdza pełny stan, w tym pakiet i licznik tur.

`core_pulse_phase` sprawdza kontakt wysokiego ID przed jakimkolwiek ruchem, natychmiastowe usunięcie kolejnych impulsów wchodzących na gracza i wspólny limit jednego HP. Obejmuje HP 3/1/0, dalsze usuwanie po śmierci, świeży limit kolejnej fazy, legalne wejście gracza na impuls i wszystkie permutacje do czterech jednostek. Pełny stan i kolejność ocalałych są porównywane również przy czekaniu bez trasy, blokadzie bez objazdu i braku ponownej próby.

`core_command_legality` sprawdza podstawowe komendy, wymaganie trwającego sektora, wzajemne krawędzie i granice planszy, osobne flagi blokujące ruch oraz legalne wejście na impuls. Obrót wymaga zasięgu i obrotowego celu, ale nie połączenia ani wolnego pola; wait w trwającym sektorze nie ma dodatkowych warunków. Każde wywołanie zachowuje pełny stan i komendę oraz zgadza się z istniejącym helperem na osobnej kopii po uwzględnieniu końca sektora. Powtarzana walidacja nie wprowadza gotowego pakietu. Każdy wariant komendy odrzuca completed/death/overload, także z gotowym pakietem i zaległym kontaktem.

`core_turn_resolver` porównuje pełny wynik i jawną klasyfikację z oczekiwaniami oraz chroni wejściowy stan i komendę. Obejmuje odrzucenie przed fazami, dokładnie jedną turę dla każdej akcji, pojedynczy krok pakietu i impulsów, obrót przed trasami, kontakt po wejściu gracza, dostawę i koszt w turze śmierci oraz następny pakiet dopiero w kolejnej zaakceptowanej turze. Granica 47→48 obejmuje drugą dostawę i wejście na wyjście, śmierć mimo dostawy oraz przeciążenie bez sukcesu; błędny ruch na turze 47 nie zużywa ostatniej tury. Wszystkie komendy terminalnego stanu zachowują cały stan bez faz i kosztu. Wyniki sukcesu oraz zwykłej i terminalnej odmowy mają niezależne kopie. Zewnętrzny SplitMix64 zachowuje dalszą sekwencję; RNG nie jest częścią wejścia resolvera.

`core_sector_outcome` sprawdza wyjścia 0/48, brak wyjścia i inną pozycję, zero/jedną/dwie dostawy, zasilone nieodbiorniki oraz granice 47/48/49 tur i maksymalny licznik. Śmierć wyprzedza sukces, a sukces na 48. turze wyprzedza przeciążenie. Powtarzane odczyty zachowują pełny stan z pakietem i impulsami, także przy zaległym kontakcie lub dostawie.

`core_turn_preview` porównuje pełny wynik podglądu z resolverem i jawnymi oczekiwaniami dla move/rotate/wait, odmów, pakietu, impulsów, obrotu tras oraz zwycięstwa, śmierci i przeciążenia 47→48. Powtórzenia zachowują live state i komendę, a zmiany zwróconego stanu nie dotykają innych wyników. Zewnętrzny RNG pozostaje poza API.

`core_experiment` wykonuje sekwencje wait/wait/wait, rotate:7/wait/wait i rotate:7/rotate:7/wait z niezależnych kopii tego samego syntetycznego stanu. Sprawdza jawne pełne stany po każdej turze, wspólne trasy oraz zgodność powtarzanego preview z resolverem i niezmienność wejścia. Sam eksperyment można powtórzyć przez `.\build\windows-msvc\Debug\core_experiment_tests.exe`.

`developer_state_hash` sprawdza dwa niezależne wektory kodowania, zmiany każdego pola i indeksu tablic, granice identyfikatorów, optional oraz liczbę i kolejność impulsów. Każdy odczyt chroni pełny stan; powtórzenia i kopie dają zgodne hashe. Nagłówek pod `tools/replay/` jest dostępny tylko właściwemu testowi i pozostaje poza core oraz paczką gry.

`developer_seeded_fixture` porównuje pełne stany i stałe niezależne hashe dla seedów 0/1/2/UINT64_MAX, obejmujących wszystkie cztery orientacje. Powtórzenia, zmiany wyników i kopii oraz zewnętrzny RNG potwierdzają niezależność wartości. Fixture pozostaje narzędziem deweloperskim poza core i paczką gry.

`developer_replay_runner` porównuje ślad i pełny stan z jawnym korpusem oraz bezpośrednim resolverem. Obejmuje nieobsługiwane wersje, pustą sekwencję, błędne i legalne move/rotate/wait, dwie dostawy, ukończenie, przeciążenie i dalsze odmowy. Powtórzenia oraz zmiany komend i wyników sprawdzają niezależność wartości.

`developer_replay_cli` uruchamia rzeczywisty plik exe przez CMake: sprawdza dokładny ślad znanego korpusu, powtórzenie, legalny obrót, seedy graniczne i błędne argumenty bez częściowego stdout.

Deweloperskie CLI jest dostępne w buildzie z `BUILD_TESTING=ON` (domyślnie). Uruchomiona lokalnie komenda:

```powershell
.\build\windows-msvc\Debug\developer_replay.exe 8 0 rotate:1
```

Argumenty to dziesiętne `version seed`, potem dowolna liczba `wait`, `move:NodeId` `rotate:NodeId` lub `overdrive:NodeId`. Wersja 8, seed 0–18446744073709551615, NodeId 0–48; liczby wymagają wyłącznie cyfr, bez znaku lub białych znaków. Początkowa linia ma format `0 initial_hash`; każda następna `numer_próby accepted outcome hash` (próby od 1, accepted 0/1). Jawne numery outcome: 0=ongoing, 1=completed, 2=death, 3=overload. Każda linia kończy się nową linią. Dla powyższej komendy wynik to:

```text
0 1868949118029829820
1 1 0 8456215732236435769
```

Nielegalna komenda gry daje poprawny wpis z accepted=0 i kod procesu 0. Błąd argumentów albo nieobsługiwana wersja daje kod 1 i pusty stdout. Diagnostyka jest deweloperska; CLI nie wyświetla planszy i nie stanowi formatu zapisu gry.

CI buduje i testuje Debug oraz Release na MSVC x64 po pushu do `main` i przy pull requestach. [Workflow Windows CI](.github/workflows/windows.yml)

## Styl C++

`.clang-format` ustawia wcięcia na cztery spacje i klamry otwierające w osobnej linii. `.editorconfig` ustawia UTF-8 i LF, z CRLF dla skryptów PowerShell zgodnie z `.gitattributes`. Nazwy funkcji i zmiennych zapisuj w `snake_case`, a typów w `PascalCase`; formatter nie sprawdza nazw.

Sprawdzenie formatowania obecnych źródeł bez ich zmiany:

```powershell
clang-format --style=file --dry-run --Werror src/core/identifiers.hpp src/core/port_mask.hpp src/core/grid_adjacency.hpp src/core/node_rotation.hpp src/core/grid_pathfinding.hpp src/core/splitmix64.hpp src/core/game_state.hpp src/core/player_movement.hpp src/core/player_rotation.hpp src/core/turn_cost.hpp src/core/player_wait.hpp src/core/packet_movement.hpp src/core/packet_delivery.hpp src/core/packet_target.hpp src/core/packet_introduction.hpp src/core/pulse_occupancy.hpp src/core/pulse_chase.hpp src/core/pulse_movement.hpp src/core/pulse_contact.hpp src/core/pulse_phase.hpp src/core/command.hpp src/core/turn_resolver.hpp src/core/sector_outcome.hpp src/core/turn_preview.hpp tests/unit/build_tests.cpp tests/unit/identifier_tests.cpp tests/unit/port_mask_tests.cpp tests/unit/grid_adjacency_tests.cpp tests/unit/node_rotation_tests.cpp tests/unit/grid_pathfinding_tests.cpp tests/unit/splitmix64_tests.cpp tests/unit/game_state_tests.cpp tests/unit/player_movement_tests.cpp tests/unit/player_rotation_tests.cpp tests/unit/player_wait_tests.cpp tests/unit/packet_movement_tests.cpp tests/unit/packet_delivery_tests.cpp tests/unit/packet_target_tests.cpp tests/unit/packet_introduction_tests.cpp tests/unit/pulse_occupancy_tests.cpp tests/unit/pulse_chase_tests.cpp tests/unit/pulse_movement_tests.cpp tests/unit/pulse_contact_tests.cpp tests/unit/pulse_phase_tests.cpp tests/unit/command_tests.cpp tests/unit/turn_resolver_tests.cpp tests/unit/sector_outcome_tests.cpp tests/unit/turn_preview_tests.cpp tools/replay/state_hash.hpp tests/unit/state_hash_tests.cpp tools/replay/seeded_fixture.hpp tests/unit/seeded_fixture_tests.cpp tools/replay/replay_runner.hpp tests/unit/replay_runner_tests.cpp tools/replay/main.cpp tests/unit/core_experiment_tests.cpp
```

## Narzędzia utrzymaniowe i licencja

Projekt nie wymaga Pythona. Skrypty utrzymaniowe korzystają z Windows PowerShell 5.1+. Nie ustalono jeszcze licencji własnego kodu; publiczne repozytorium nie oznacza udzielenia licencji na wykorzystanie jego zawartości.

Dodatkowa uruchomiona komenda replay overdrive:

```powershell
.\build\windows-msvc\Debug\developer_replay.exe 8 0 overdrive:1 overdrive:1 overdrive:1 overdrive:1 overdrive:1
```

Cztery przyjęte akcje kończą się reserve0/debt4/tura4; piąta jest odmową i zachowuje hash 1910320260738461074.

P027 jest podziałem planu: wolny ID impulsu (P027-01), następnie pojedynczy spawn długu na wejściu sektora (P027-02). P027-01 implementuje najmniejszy wolny EntityId; P027-02 implementuje pojedynczy spawn na wolnym wejściu za3 długu. Wywołanie granicy sektora czeka na run.

P028 implementuje ograniczony generator topologii7×7: dwie trasy po bokach siatki,13 aktywnych pól, dwa nieruchome odbiorniki, seedowane obroty9 węzłów. To dane headless; P029 potwierdza podane sekwencje rozwiązania przez rzeczywisty resolver. Brak oceny grywalności. [Raport P028](docs/workflow/history/P028.md)

P029 dodaje validate_solution do walidacji certyfikatów. Cztery wygenerowane seedy0/1/2/MAX mają sprawdzone sekwencje18/17/17/17 tur; nie jest to jeszcze korpus1000 seedów ani pełny run. Końcowy zestaw:35/35 CTest Debug i Release. [Raport P029](docs/workflow/history/P029.md)

P030 zamknięto wyłącznie jako podział korpusu na cztery dzieci: stałe seedy i stan bazowy, świadek ograniczonej rodziny, kontrola spawnów oraz wykonywalny raport. P031 czeka na P030-04. [Raport P030](docs/workflow/history/P030.md)

P030-01 zapisuje korpus v1 (0…998 i MAX) i bazowy adapter sektora bez długu/impulsów. Dwa niezależne pełne wektory i unikalność seedów:36/36 CTest Debug/Release. [Raport P030-01](docs/workflow/history/P030-01.md)

P030-02 dodaje deweloperskiego świadka wyłącznie bazowej rodziny P028. Cztery certyfikaty i pełne stany końcowe, odmowy spoza kontraktu:37/37 CTest Debug/Release. [Raport P030-02](docs/workflow/history/P030-02.md)

P030-03 dodaje kontrolę początkowych spawnów oraz odrębne scenariusze długu. Duplikaty i niedozwolone pola odrzucane; pełne stany sprawdzone.38/38 CTest Debug/Release. [Raport P030-03](docs/workflow/history/P030-03.md)

P030-04 kończy korpus:1000 certyfikatów bazowych i7000 testów spawnów, zero odrzuceń; raport Debug/Release identyczny. Narzędzie: `.\build\windows-msvc\Debug\developer_corpus.exe` (bez argumentów). Wiersz:seed baseline_reason turns debt0 debt2 debt3 debt6 no_entry blocked occupied; 0=PASS,1…6=SpawnIssue,7=unsupported witness,8=invalid certificate,9=debt mismatch. Końcowe `summary 1000 0 0`. Pełne dane:[raport korpusu](docs/workflow/history/P030-04-corpus.txt).40/40 CTest w obu konfiguracjach. [Raport P030-04](docs/workflow/history/P030-04.md)

P031 zamknięto jako plan trzech dzieci: stabilne ID i dwa sloty; GameState i hashv3/replay5; port reguł obrotu bez aktywacji efektów. P032 zależy od P031-03. [Raport P031](docs/workflow/history/P031.md)

P031-01 implementuje ModuleLoadout: dwa kanoniczne sloty i stabilne ModuleId1/2, odrzucenie duplikatów i obcych wartości bez mutacji.41/41 CTest Debug/Release. Efekty jeszcze nieaktywne. [Raport P031-01](docs/workflow/history/P031-01.md)

P031-02 dodaje pusty GameState::modules. Pełny hashv3 koduje dwa kanoniczne bajty slotów; bieżąca wersja replay5 odrzuca poprzednie. Cztery niezależne wektory modułów i niezmieniony korpus;42/42 CTest Debug/Release. [Raport P031-02](docs/workflow/history/P031-02.md)

P031-03 podłącza wspólny czysty port RotationRules do helpera obrotu. Wszystkie zestawy nadal mają bazowy zasięg1.19208 przypadków geometrii oraz pełne stany komendy/podglądu/wykonania:43/43 CTest Debug/Release. [Raport P031-03](docs/workflow/history/P031-03.md)

P032 aktywuje rotation_range: rotate i overdrive w odległości Manhattan2; bez modułu1, nieruchome odrzucane. Pakiet i impuls korzystają z nowo otwartej trasy w teście pełnego stanu. Bieżący replay6/hashv3;44/44 CTest Debug/Release. Korpus1000+7000 bez zmian. Zakończono10 osobno commitowanych kroków; następny P033. [Raport P032](docs/workflow/history/P032.md)


P033: P033 zamknięto jako podział planu na flagę/hash, akcję i reset. Nieruchoma końcówka oznacza maskę z dokładnie jednym portem; wspólny limit dla rotate i overdrive. P034 zależy od P033-03. Nie dodano kodu ani efektu modułu. [Raport P033](docs/workflow/history/P033.md)

P033-01: GameState ma terminal_rotation_used=false. Pełne porównania stanu i świadek korpusu uwzględniają flagę. Hash v4 koduje tag4 oraz bajt flagi po slotach; replay7 odrzuca wcześniejsze wersje. Brak efektu modułu. [Raport P033-01](docs/workflow/history/P033-01.md)

P033-02: Moduł terminal_rotation pozwala obrócić nieruchomą maskę z dokładnie jednym portem. Pierwszy sukces zużywa wspólny limit rotate/overdrive, drugi jest atomową odmową. Zwykły obrotowy węzeł nie zużywa limitu. RotationRules udostępnia uprawnienie, wspólny helper egzekwuje limit i zasięg; obydwa moduły działają razem. Replay8 z powodu zmiany reguł; hashv4 i wektory fixture bez zmian. [Raport P033-02](docs/workflow/history/P033-02.md)

P033-03: reset_sector_module_usage(GameState&) odnawia wyłącznie jednorazowy limit modułu. Reset jest idempotentny i nie zmienia HP, zasobów, topologii, pakietu, impulsów ani tur. Zwykła tura nie odnawia limitu. Integracja granicy sektora czeka na run P035. Hashv4/replay8 pozostają bez zmian: istniejące tury i kodowanie identyczne. [Raport P033-03](docs/workflow/history/P033-03.md)

P034: P034 zamknięto wyłącznie jako podział planu na deterministyczną ofertę i atomowy wybór. Oferta zawiera najwyżej dwa nieposiadane moduły; po wyczerpaniu katalogu jest pusta. P035 zależy od P034-02. Bez kodu i bez bramkowania runu. [Raport P034](docs/workflow/history/P034.md)

P034-01: module_offer(const ModuleLoadout&) zwraca wartościową, kompaktową tablicę dwóch optional ModuleId w porządku ID1/2. Zawiera wyłącznie niezainstalowane moduły; pełny zestaw daje pustą ofertę. Odczyt nie zmienia stanu ani RNG. Hashv4/replay8 pozostają bez zmian, bo nie dodano pól ani reguł tury. [Raport P034-01](docs/workflow/history/P034-01.md)

P034-02: choose_module(GameState&, ModuleId) przelicza ofertę z bieżącego zestawu i dodaje dokładnie jeden wskazany moduł. Duplikat, obcy ID i pełny zestaw są odrzucane bez mutacji. Sukces zmienia wyłącznie modules; bramkowanie okna nagrody pozostaje odpowiedzialnością przyszłego runu. Hashv4/replay8 bez zmiany: format GameState i istniejący resolver nie zmieniły się. [Raport P034-02](docs/workflow/history/P034-02.md)

P035: P035 zamknięto wyłącznie jako podział runu na sześć kontraktów. P036 zależy od P035-06. Jeden aktywny GameState jest źródłem HP/rezerwy/długu/modułów; brak leczenia między sektorami. Seed sektoraN to N-ty next_u64 z lokalnego SplitMix64(root seed), bez trwałego RNG. Bez implementacji dzieci i bez deklaracji ukończenia runu. [Raport P035](docs/workflow/history/P035.md)

P035-01: RunState posiada root seed, silny SectorNumber1..3, aktywny GameState i reward_claimed. RunPhase jest czystym odczytem: ongoing→playing, death/overload→lost, completed3→won, completed1/2→nagroda albo gotowość przejścia, jeśli odebrana lub zestaw pełny. Zasoby nie są dublowane. Brak startu, akcji runu i przejść. GameState/hashv4/replay8 pozostają bez zmian; metadane runu nie mają jeszcze kodeka ani replay. [Raport P035-01](docs/workflow/history/P035-01.md)

P035-02: start_run(RunSeed) tworzy pierwszy sektor runu z pierwszego next_u64 lokalnego SplitMix64(root seed). Ustawia wejście, wyjście i dwa odbiorniki z topologii; świeży stan ma HP3/reserve2/debt0/turę0, puste moduły, pakiet i impulsy oraz niezużyty limit. Run zaczyna na sektorze1 z nieodebraną nagrodą. Hashv4/replay8 bez zmiany, bo dotychczasowe fixture, reguły tury i format GameState pozostają identyczne. Run nie ma jeszcze resolvera, przejść ani zapisu. [Raport P035-02](docs/workflow/history/P035-02.md)

P035-03: resolve_run_turn i preview_run_turn zwracają niezależny RunResult; tylko playing uruchamia istniejący resolver. Odmowy zachowują pełny run, sukcesy wyliczają nagrodę/gotowość/wygraną, śmierć i overload kończą run. Hashv4/replay8 bez zmiany: GameState i reguły istniejących tur identyczne; metadane runu nie mają replay ani zapisu. [Raport P035-03](docs/workflow/history/P035-03.md)

P035-04: choose_run_module bramkuje choose_module fazą choosing_reward i po sukcesie ustawia reward_claimed. Drugi wybór, duplikaty, obce ID i pozostałe fazy nie zmieniają pełnego runu; HP nie jest odnawiane. Hashv4/replay8 bez zmiany: ten sam GameState i resolver. [Raport P035-04](docs/workflow/history/P035-04.md)

P035-05: advance_run zwraca wartościowe przejście wyłącznie z ready_for_next. SektorN bierze N-ty draw root RNG; przenosi HP/rezerwę/dług/moduły bez leczenia. Odtwarza topologię, wejście/wyjście/odbiorniki, zeruje turę/dostawy/pakiet/impulsy/blokady/użycie/nagrodę i raz wywołuje spawn_debt_pulse. Nie ma sektora4. Hashv4/replay8 bez zmiany; metadane runu poza istniejącym replay. [Raport P035-05](docs/workflow/history/P035-05.md)

P035-06: Test integracyjny kończy trzy sektory dla root0/1/2/MAX i obu kolejności modułów, dwukrotnie. Dług3 powstaje przez trzy legalne overdrive w sektorze1; sektor2 ma impuls, który po wait zmniejsza HP3 do2. HP2 przechodzi do trzeciego sektora. Brak sektora4 i dalszych akcji po końcu. Lethal fixture jawnie ustawia HP1 na rzeczywistej granicy z impulsem; 48 wait prowadzi do overload. Bez zmian produkcyjnych i bez oceny grywalności. Hashv4/replay8 bez zmiany. [Raport P035-06](docs/workflow/history/P035-06.md)

P036: Specyfikacja checkpointu granicy sektora v1: dokładnie40 bajtów, wersje schema1/run_rules1, pola zasobów i modułów, seed/numer sektora, flaga opłaconego impulsu i checksum FNV-1a64. Tabela mapuje wszystkie19 pól RunState/GameState; capture musi odrzucić każdą lokalną zmianę, restore nie powtarza spawnu. Nieznane wersje, długość, zakresy i niekanoniczne rekordy są odrzucane. Bez kodeka i IO; hashv4/replay8 bez zmiany. [Raport P036](docs/workflow/history/P036.md)

P037: P037 zamknięto wyłącznie jako podział planu na cztery dzieci: projekcja/rekonstrukcja, encoder, decoder i korpus odporności. P038 zależy od P037-04, nie od planu. Każde dziecko ma własny obserwowalny kontrakt; nie dodano kodu ani IO. Hashv4/replay8 i specyfikacja checkpointu bez zmian. [Raport P037](docs/workflow/history/P037.md)

P037-01: RunCheckpoint, is_checkpoint_valid, capture_checkpoint i restore_checkpoint działają w application bez IO. Capture odrzuca każdą różnicę lokalnego stanu względem początku wygenerowanego sektora. Restore przenosi HP/rezerwę/dług/moduły i odtwarza opłacony impuls ID0 bez ponownego pobrania3 długu. Hashv4/replay8 bez zmiany; format bajtowy jeszcze niezaimplementowany. [Raport P037-01](docs/workflow/history/P037-01.md)

P037-02: encode_checkpoint zwraca optional kanonicznej tablicy40 bajtów. Jawne LE, magic, schema1/run_rules1, dwa kanoniczne sloty i checksum FNV-1a64. Walidacja wartości poprzedza kodowanie; brak serializacji pamięci C++, brak decoder i IO. Hashv4/replay8 bez zmiany. [Raport P037-02](docs/workflow/history/P037-02.md)

P037-03: decode_checkpoint przyjmuje const span bajtów i po dokładnej kontroli długości40 sprawdza magic, schema1/run_rules1, checksum, reserved, flagę, kanoniczne moduły i zakresy. Zwraca optional RunCheckpoint, bez częściowego wyniku, IO i alokacji zależnych od danych. Hashv4/replay8 i format bez zmiany. [Raport P037-03](docs/workflow/history/P037-03.md)

P037-04: Korpus integracyjny potwierdza pamięciowy kodek checkpointu v1 dla 11000 pełnych granic i systematycznie uszkodzonych danych. Dokumentacja formatu, architektury i testów wskazuje wdrożony kodek. Bez zmian produkcyjnych, formatu, hashv4/replay8 i IO. Gotowy run headless ma nagrody i przejścia; zapis plików pozostaje kolejnym etapem. [Raport P037-04](docs/workflow/history/P037-04.md)

P038 zamknięto wyłącznie jako podział adaptera Windows na ograniczony odczyt, staging, publikację z backupem i testy przerwania. P039 zależy od P038-04; nie dodano IO. [Raport P038](docs/workflow/history/P038.md)

P038-01: Ograniczony reader Win32 i RAII uchwytu: dokładnie40 bajtów, walidacja, rozróżnienie brak/uszkodzenie/IO. 58/58 CTest Debug/Release. Bez fallbacku i zapisu. [Raport P038-01](docs/workflow/history/P038-01.md)

P038-02: Staging wartości checkpointu: pełny WriteFile, flush i zamknięcie; retry po resztkach/błędzie. 59/59 CTest Debug/Release. Bez publikacji. [Raport P038-02](docs/workflow/history/P038-02.md)

P038-03: save_checkpoint_file publikuje zwalidowany poprzedni main jako backup, potem nowy main; błędy i retry zachowują poprawne dane. 60/60 CTest Debug/Release. Bez fallbacku. [Raport P038-03](docs/workflow/history/P038-03.md)

P038-04: Test faktycznego save w procesie potomnym: 16 przerwań, dokładne pliki i pełny restore/retry, ścieżka Unicode. Mutacja utraty main wykryta. 61/61 CTest Debug/Release. Pięć kroków zakończono; odzyskiwanie z backupu pozostaje P039. [Raport P038-04](docs/workflow/history/P038-04.md)

P039: recover_checkpoint_file wybiera poprawny main lub backup po braku/uszkodzeniu main, zachowując diagnostykę. Błędy IO main zatrzymują wybór; odczyt nie zmienia plików. Testy obejmują pełny restore i niezmienność backupu. [Raport P039](docs/workflow/history/P039.md)

P040: Rozbito profil na sześć małych dzieci: model/format, encoder, decoder, odczyt, zapis z backupem i test niezależności runu. P041 zależy od P040-06. Nie zaimplementowano dzieci. [Raport P040](docs/workflow/history/P040.md)

P040-01: Dodano LocalProfile i walidację zakresów, stabilne wewnętrzne AchievementId 1..5 oraz specyfikację profilu v1 (24 bajty). Test sprawdza wszystkie 65536 masek achievementów, 256 masek wariantów i 256 głośności oraz niezależność kopii. Bez kodeka i IO. [Raport P040-01](docs/workflow/history/P040-01.md)

P040-02: encode_profile zwraca optional kanonicznej tablicy24 bajtów z jawnym LE, magic CCPR, schema1 i FNV-1a64. Cztery niezależne literalne wektory oraz pełne zakresy masek i ustawień potwierdzają wynik i brak mutacji wejścia. Bez decodera i IO. [Raport P040-02](docs/workflow/history/P040-02.md)

P040-03: decode_profile sprawdza długość24, magic, schema1, checksum, reserved i zakresy przed zwróceniem całego profilu. Testy obejmują 25856 poprawnych profili, cztery literalne wektory, wszystkie ucięcia i nadmiar1..64, zmiany wszystkich bajtów i pól z odnowioną checksumą. Kodek pamięciowy gotowy; bez IO. [Raport P040-03](docs/workflow/history/P040-03.md)

P040-04: read_profile_file czyta przez jeden chroniony uchwyt dokładnie24 bajty i zwraca optional profilu oraz DWORD. Cztery literalne pliki, ucięcia, nadmiar do1MiB, checksum, brak pliku/rodzica, katalog i blokada przechodzą testy bez zmiany bajtów main i backupu; ścieżka Unicode. Bez fallbacku i zapisu. [Raport P040-04](docs/workflow/history/P040-04.md)

P040-05: save_profile_file publikuje poprzedni poprawny main jako backup, potem nowy main; staging jest flushowany i zamykany. Testy potwierdzają dokładne bajty, blokady każdej ścieżki, odmowę invalid i retry. Dodatkowo16 rzeczywistych przerwań procesu: po9 bajtach, flush24, rename backupu i main; każdy przypadek zachowuje poprawne dane i pozwala retry. Nowy test początkowo miał błędną ścieżkę include; poprawiono przed końcowymi buildami. Bez automatycznego fallbacku profilu. [Raport P040-05](docs/workflow/history/P040-05.md)

P040-06: Test integracyjny uruchamia12 osobnych procesów dla4 wektorów: writer i reader przed/po usunięciu runu. Po usunięciu4 jawnych nazw checkpointu wszystkie bajty profilu, backupu i obu staging pozostają identyczne. Pełne wartości i Unicode potwierdzone. Cały zakres dzieci profilu zakończony; bez produkcyjnego delete i integracji sesji. [Raport P040-06](docs/workflow/history/P040-06.md)

P041: P041 rozbito wyłącznie planistycznie na trzy dzieci: model/specyfikacja, wartościowe zdarzenia zaakceptowanej tury oraz korpus sekwencji/restartu/preview. P042 zależy od P041-03. Nie zmieniono resolvera ani nie dodano usług. [Raport P041](docs/workflow/history/P041.md)

P041-01: Dodano wartościowe DomainEvent, payloady i klucz wystąpienia seed/sektor/tura/ordinal. Stałe ID rodzajów1..4 i powodów końca1..3 są niezależne od indeksów variant. Specyfikacja ustala kolejność i ograniczenia klucza; testy chronią ID, równość, porządek kluczy i niezależność kopii pełnych komend. Bez emisji i usług. [Raport P041-01](docs/workflow/history/P041-01.md)

P041-02: resolve_run_turn_events wywołuje wspólny resolver runu raz i zwraca listę wartości: akcja, dostawa, sukces sektora, koniec runu. Brak usług i zmian core/preview. Testy każdej komendy, odmów i terminalnych wejść sprawdzają pełne stany, klucze, payloady, kolejność i niezależność kopii przy3 powtórzeniach. Trzy sektory obejmują sukces w turze48, dostawę z przeciążeniem i śmierć na wyjściu po dostawie bez sukcesu sektora. Korpus pełnych sekwencji i restore pozostaje P041-03. [Raport P041-02](docs/workflow/history/P041-02.md)

P041-03: Korpus pełnych runów porównuje każdą listę kluczy i payloadów oraz pełny stan po dwóch odtworzeniach checkpointu granicy. Cztery seedy, obie kolejności modułów, opłacony impuls, śmierć i overload; powtarzane preview bez kanału zdarzeń. Bez kodu produkcyjnego. [Raport P041-03](docs/workflow/history/P041-03.md)

P042: P042 zamknięto wyłącznie jako podział na trzy kontrakty: czysty zbiór, trwałą operację Windows oraz restart procesu. P043 zależy od P042-03. Nie dodano implementacji dzieci. [Raport P042](docs/workflow/history/P042.md)

P042-01: Dodano is_achievement_unlocked i unlock_achievement: znane ID 1..5, walidacja profilu, osobny wynik duplikatu, zmiana wyłącznie jednego bitu. Test wszystkich 32 masek, ID 0..255 i odmów invalid profilu zachowuje ustawienia. Bez IO i predykatów. [Raport P042-01](docs/workflow/history/P042-01.md)

P042-02: unlock_profile_file wykonuje read/modify/save bieżącego profilu z zachowaniem ustawień. Brak main tworzy profil domyślny; invalid lub IO blokuje zapis. Duplikat zachowuje dokładne bajty wszystkich czterech nazw. Testy blokad każdej ścieżki, odmowy rename i retry przechodzą. Obowiązuje jeden zapisujący. [Raport P042-02](docs/workflow/history/P042-02.md)

P042-03: Test trwałych odblokowań uruchamia 24 osobne procesy dla pięciu ID. Porównuje pełny profil i dokładne bajty main/bak po nowym odblokowaniu, duplikacie i restarcie readera. Uszkodzony i zablokowany main nie jest resetowany. Bez zmian produkcyjnych; dzieci P042 ukończone. [Raport P042-03](docs/workflow/history/P042-03.md)

P043: Dodano port AchievementSink z wynikami unlocked/already_unlocked/invalid/unavailable i lokalny adapter profilu Windows bez cache. Fake testuje kontrakt, a dwa lokalne obiekty potwierdzają trwały zbiór, duplikaty, odmowę i retry offline. Symulacja i preview nie otrzymują usług. [Raport P043](docs/workflow/history/P043.md)

P044: P044 zamknięto wyłącznie jako podział na kontrakt próby i gałęzi, dispatcher z retry oraz dowód restore/restart. P045 zależy od P044-03. Profil utrwala zbiór ID, nie outbox; seed nie identyfikuje próby. Bez implementacji dzieci. [Raport P044](docs/workflow/history/P044.md)

P044-01: Dodano DeliveryScope z odrębnymi tokenami próby i gałęzi oraz granicą sektora, wyniki dispatchu i port AchievementEvaluator zwracający fixed maskę. Specyfikacja określa kolejność, retry, konflikty i granice gwarancji po restore. Testuje tokeny, kopie, fake i wszystkie 65536 masek. Bez dispatchu i predykatów. [Raport P044-01](docs/workflow/history/P044-01.md)

P044-02: EventDispatcher zachowuje kopie zdarzeń i masek, ocenia raz na zakres i ponawia tylko pending ID. Odrzuca konflikty, zły kontekst, kształt i kolejność. Testy chronią częściowe potwierdzenia, invalid maskę i wyjątek sinka po skutku; syntetyczny strumień trzech sektorów powtarza się bez dodatkowych ocen. Bez IO i predykatów. [Raport P044-02](docs/workflow/history/P044-02.md)

P044-03: Test integracyjny przechodzi trzy pełne runy dla seedów 0/1/MAX przez resolver, baseline witness i lokalny sink. Duplikaty nie oceniają ponownie; restore sektora2 porównuje każdą listę kluczy/payloadów i pełny stan. Nowa gałąź, nowa próba z tym samym seedem i sześć nowych procesów oceniają ponownie bez zmiany main/bak. Preview zachowuje stan, pliki i liczniki usług. Stały fake nie implementuje predykatów MVP; bez zmian produkcyjnych. [Raport P044-03](docs/workflow/history/P044-03.md)

P058-03: Dodano read_asset_file: jeden chroniony uchwyt, kanoniczna lokalizacja, dokładna długość i SHA-256 BCrypt przed publikacją całego bufora. Testy obejmują Unicode, puste i dokładne bajty, brak, blokadę, katalog, junction, ucieczkę oraz złe rozmiary/hash i invalid reference. [Raport](docs/workflow/history/P058-03.md)

P058-04: Integracja rzeczywistego rejestru P057 potwierdza parse/lookup/read i dokładne bajty trzech zatwierdzonych plików. Nieznane ID, traversal, zły digest, korupcja i brak izolowanych kopii dają odmowę bez częściowych danych. [Raport](docs/workflow/history/P058-04.md)

P059: Dodano bramkę payloadu approved w checkout Git, z kontrolą rejestru, zgód, hashy, śledzenia, hidden i reparse. CI uruchamia bramkę i jej 7 izolowanych prób; istniejące 11 testów walidatora również PASS. [Raport](docs/workflow/history/P059.md)

P060: Dodano owning TextCatalog z atomową walidacją listy, oznaczonymi placeholderami i źródłem zatwierdzonych wpisów. Lookup nieznanego ID nie publikuje fallbacku. Testy chronią prefiks, status/źródło, duplikaty, granice i niezależne wartości. [Raport](docs/workflow/history/P060.md)

P061: Rozbito zakres na P061-01 (polityka katalogu), P061-02 (public gate rejestru) i P061-03 (tryb CMake i dowód builda). P062 wymaga P061-03; utworzono PUBLIC_CONTENT.md. Bez implementacji dzieci. [Raport](docs/workflow/history/P061.md)

P061-01: TextCatalog wymaga jawnego TextAudience: developer przyjmuje oznaczone placeholdery, public_release odrzuca atomowo każdą listę z placeholderem. Nieznany audience jest błędem również dla pustej listy. Testy obu polityk i źródeł PASS. [Raport](docs/workflow/history/P061-01.md)

P061-02: Public content gate wymaga poprawnego approved payloadu i katalogu tekstów wyłącznie approved bez markera. Dziewięć izolowanych wyników procesu potwierdza m.in. developer acceptance oraz public rejection tego samego oznaczonego wpisu. [Raport](docs/workflow/history/P061-02.md)

P061-03: Dodano jawny tryb CMake CLOSED_CIRCUIT_PUBLIC_BUILD, presety public i build_text_audience. Każdy konsument game_core wymaga bramki treści; developer/public działa niezależnie od D/R. Wszystkie cztery warianty zaliczyły po 88/88 CTest. Izolowana próba 9 rejestrów oraz 4 wyników builda PASS; CI rozszerzono o oba tryby. [Raport](docs/workflow/history/P061-03.md)

P062: Rozbito cykl Win32 na P062-01 (RAII ukrytego okna), P062-02 (komunikaty/focus) i P062-03 (developer entrypoint i smoke procesu). P063 wymaga ostatniego dziecka. WINDOW_LIFECYCLE.md określa własność, tytuł z katalogu i zakaz rysowania. [Raport](docs/workflow/history/P062.md)

P062-01: Dodano RAII NativeWindow: ukryte HWND, własna klasa, tytuł wyłącznie z TextCatalog i ścisła konwersja UTF-8. Cztery powtórzenia dwóch okien potwierdzają dokładny tytuł Unicode, create/destroy/unregister i wcześniejsze WM_CLOSE; brak ID i invalid UTF-8 dają kontrolowane błędy. Wszystkie cztery warianty developer/public D/R zaliczyły po 89/89 CTest. Zakończono zamówione 10 kroków, w tym dwa podziały planu. [Raport](docs/workflow/history/P062-01.md)

P062-02: Dodano stan focus oraz blokującą pętlę Win32 z rozróżnieniem WM_QUIT i błędu GetMessage. Cztery rzeczywiste zamknięcia, utrata focus, invalid HWND i kod wyjścia 37 są chronione testem. [Raport](docs/workflow/history/P062-02.md)

P062-03: Dodano developer_window.exe: tytuł wyłącznie z rejestru przez TextCatalog, ramka Win32, blokująca pętla i kontrolowane kody błędów. Trzy osobne uruchomienia smoke kontrolują tytuł i zamknięcie. Public gate i build odrzucają roboczy wpis; cel okna jest wykluczony z public. CI sprawdza odmowę przed testami infrastruktury z pustym katalogiem. [Raport](docs/workflow/history/P062-03.md)

P063: Rozbito wejście klawiatury na trzy kontrakty: filtr naciśnięć/focus, czyste mapowanie i adapter Win32. P064 wymaga ostatniego dziecka. KEYBOARD_INPUT.md ustala brak kolejki tur i brak mutacji stanu przez wejście. [Raport](docs/workflow/history/P063.md)

P063-01: Dodano przenośny KeyboardInput: jedna akcja do key-up, niezależne klawisze, reset przy focus loss i odmowa repeat po odzyskaniu focus. Redundantny focus=true zachowuje held; unknown jest ignorowany. [Raport](docs/workflow/history/P063-01.md)

P063-02: Dodano map_key_command: strzałki tworzą kandydatów MoveCommand bez zawijania, R i O wskazują pole gracza, Space tworzy WaitCommand. Test wszystkich 49 pól i pełnych payloadów oraz rzeczywistej odmowy resolvera chroni granice i niezmienność stanu. [Raport](docs/workflow/history/P063-02.md)

P063-03: NativeWindow tłumaczy WM_KEYDOWN/UP na filtr i mapowanie; bit30 oraz stan held blokują repeat. take_command konsumuje jedyną akcję, focus loss i zamknięcie czyszczą pending. Test siedmiu komend, powtórek, repress, unknown i krawędzi nie wymaga GameState. [Raport](docs/workflow/history/P063-03.md)

P064: Rozbito lifecycle Direct2D na własność fabryki/celu HWND oraz resize, zawieszenie i recreate. P065 zależy od P064-02. Dokument ustala granice dowodów i zakaz rysowania w tych krokach. [Raport](docs/workflow/history/P064.md)

P064-01: Dodano Direct2DTarget z RAII ComPtr fabryki single-threaded i celu HWND. Create publikuje cały adapter albo HRESULT. Rzeczywiste dwa cele w czterech powtórzeniach mają dokładny HWND i rozmiar klienta; null/zniszczony HWND dają odmowę. [Raport](docs/workflow/history/P064-01.md)

P064-02: Direct2DTarget obsługuje resize, zawieszenie przy zerowym wymiarze i odtworzenie po D2DERR_RECREATE_TARGET z tą samą fabryką. Test czterech sekwencji, prawdziwego SetWindowPos i zamkniętego HWND potwierdza rozmiary, nowy cel i kontrolowane odmowy. [Raport](docs/workflow/history/P064-02.md)

P065: Dodano read_png_asset: chroniony odczyt i SHA-256 przed systemowym WIC PNG, owning bufor PBGRA z limitem 4096x4096. Pełne piksele zatwierdzonego atlasu są zgodne z niezależnym GDI+. Testuje brak COM, Unicode, kopie, korupcję przed hashem i kodekiem, brak pliku, retry, non-PNG i niezmienność źródeł. Zakończono zamówione 10 kolejnych kroków: osiem implementacyjnych i dwa podziały planu. [Raport](docs/workflow/history/P065.md)

P066: Rozbito prezentację planszy na cztery kontrakty: kafle portów, projekcja 7x7, bitmapa Direct2D i okno developerskie. P067 wymaga ostatniego dziecka; plan nie implementuje renderowania. [Raport](docs/workflow/history/P066.md)

P066-01: Dodano mapowanie wszystkich 16 masek portów na sześć istniejących kafli atlasu 1.2 i obroty całych bitmap. Rzeczywiste piksele potwierdzają dokładnie 12 nieprzezroczystych pikseli każdego otwartego brzegu i zero na zamkniętych. [Raport](docs/workflow/history/P066-01.md)

P066-02: Dodano czystą projekcję planszy 7x7 z centrowaniem i dopasowaniem do viewportu. Każdy kafel zachowuje współrzędne NodeId i maskę portów. Poprawiono kodowanie polskich opisów nowych wpisów P066 po wykryciu stratnej konwersji potoku PowerShell. [Raport](docs/workflow/history/P066-02.md)

P066-03: Dodano AtlasRenderer z uploadem wyłącznie zweryfikowanego atlasu 1.2, własnością celu i bitmapy oraz rysowaniem gotowych kafli przez DrawBitmap. Test offscreen porównuje wszystkie piksele 16 masek po obrotach i skalowaniu 2x; błędna lista nie zmienia poprzedniej klatki. [Raport](docs/workflow/history/P066-03.md)

P066-04: Developer_window wyświetla planszę z istniejącego start_run i zatwierdzonego atlasu. Dodano callback WM_PAINT z BeginPaint/EndPaint, resize i odtwarzanie bitmapy po utracie celu. COM i zasoby mają jawny cykl życia; start z dowolnego cwd używa ścieżki checkoutu z CMake. [Raport](docs/workflow/history/P066-04.md)

P067: Rozbito prezentację jednostek na projekcję gotowych symboli, złożenie sceny z rozłącznymi obszarami oraz adapter komend okna do core. P068 wymaga ostatniego dziecka. Plan nie wdraża jednostek ani sesji. [Raport](docs/workflow/history/P067.md)

P067-01: Dodano czystą projekcję gracza, pakietu, impulsów i ról pól na osiem gotowych symboli atlasu. Trzy typy jednostek mogą dzielić pole w osobnych narożnikach, a impulsy mają stabilną kolejność ID. Tura dostawy i kontaktu usuwa właściwe symbole bez zmiany źródłowego stanu. [Raport](docs/workflow/history/P067-01.md)

P067-02: Dodano jedną projekcję sceny z planszą, jednostkami i rolami pól oraz podłączono ją do BoardView. Centralne kafle zajmują połowę komórki, symbole rozłączne narożniki. Zmieniono wcześniejsze planowane 60% na 50%, aby prostokąty nie zachodziły na narożniki. Pełny nowy stan odmalowuje jedną spójną scenę. [Raport](docs/workflow/history/P067-02.md)

P067-03: Podłączono odfiltrowane klawisze okna do istniejącego resolve_run_turn przez RunWindow. Po przyjęciu komendy stan sesji i widok otrzymują ten sam pełny wynik; odmowa, repeat i brak focusu nie wykonują dodatkowej tury. Sesja i callback mają jawny cykl życia. [Raport](docs/workflow/history/P067-03.md)

P068: Rozbito podgląd akcji na wartościowy wynik i trasę, czysty model wyboru, adapter wskazania Win32 i prezentację gotowych znaczników. P069 wymaga ostatniego dziecka. Zakończono zamówione 10 kroków: siedem implementacji i trzy podziały planu, każdy osobnym commitem. [Raport](docs/workflow/history/P068.md)

P068-01: Dodano wartościowy ActionPreview: pełny wynik istniejącego resolvera oraz trasę BFS pakietu po akcji. Trasa obejmuje nowo wprowadzony pakiet i pozostaje dostępna po dostawie; odmowy nie publikują trasy. [Raport](docs/workflow/history/P068-01.md)

P068-02: Dodano czysty ActionSelection: przełączanie planowania, wybór siedmiu akcji, jednokrotne pobranie Enter, anulowanie, reset focusu i kontekstu oraz opcjonalny cel R/O z powrotem do pola gracza. [Raport](docs/workflow/history/P068-02.md)

P068-03: Rozbito adapter wskazania na P068-05 (czysty hit-test istniejącej geometrii) i P068-06 (wiadomości Win32 oraz ActionSelection). P068-04 wymaga ostatniego dziecka; ustalono półotwarte granice i reset hover po resize. [Raport](docs/workflow/history/P068-03.md)

P068-05: Dodano board_node_at: opcjonalny NodeId z tej samej geometrii fit_board co renderer. Półotwarte granice rozstrzygane przez porównanie współrzędnych, bez zaokrąglania dzielenia na wspólnej krawędzi. [Raport](docs/workflow/history/P068-05.md)

P068-06: Połączono ActionSelection z oknem: hit-test kursora, wskazane cele R/O, filtrowane Tab/Enter/Escape, reset focusu i hover przy resize/mouse leave. Natychmiastowe akcje i zatwierdzony kandydat używają tego samego resolvera runu. [Raport](docs/workflow/history/P068-06.md)

P068-04: Podłączono wynik podglądu do BoardView i znaczniki trasy z istniejącej komórki pakietu (45,8). Znaczniki zajmują rozłączny lewy obszar; anulowanie, odmowa i focus przywracają live state, Enter publikuje przewidziany stan bez starych znaczników. [Raport](docs/workflow/history/P068-04.md)

P069: Rozbito tekst developerski na wspólny odczyt UTF-16, renderer DirectWrite z lifecycle niezależnym od celu oraz podłączenie istniejącego napisu w osobnym obszarze okna. P070 wymaga P069-03; kontrakt TEXT_PRESENTATION.md określa brak obciętego markera i wspólną geometrię hit-testu. [Raport](docs/workflow/history/P069.md)

P069-01: Wydzielono read_text_utf16: owning odczyt z TextCatalog i ścisła konwersja Win32 bez zastępowania błędnych bajtów. NativeWindow używa wspólnego odczytu tytułu, bez fallbacku identyfikatora. [Raport](docs/workflow/history/P069-01.md)
