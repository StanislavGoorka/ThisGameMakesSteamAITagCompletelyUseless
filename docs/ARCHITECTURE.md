# Architektura docelowa

Plan, nie opis zaimplementowanych modułów. Budujemy małą grę testowalną bez wyświetlania treści, nie silnik ogólnego zastosowania.

## Stos

Windows x64, desktop Win32, C++20, MSVC z Visual Studio 2022 i Windows SDK, CMake 3.26+. Windows 11 jest celem wsparcia 1.0; Windows 10 można sprawdzać dodatkowo bez obiecywania zgodności. Narzędzia musi zweryfikować P001; ich obecność nie jest założona.

[Direct2D](https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-quickstart) renderuje gotowe bitmapy; [WIC](https://learn.microsoft.com/en-us/windows/win32/wic/-wic-about-windows-imaging-codec) dekoduje zatwierdzone PNG. DirectWrite składa zatwierdzone napisy lub oznaczone placeholdery, bez projektowania fontów. XAudio2 odtwarza zatwierdzone WAV, bez syntezy. To API systemu, nie zewnętrzny silnik.

Nie dodawaj frameworka, ECS, fizyki ani edytora bez konkretnego uzasadnienia. Początkowo mały runner testów C++ z niezerowym kodem błędu, rejestrowany w CTest; nie polegaj na `assert`, które znika w Release. Zależności wymagają wersji i licencji. Pobranie biblioteki zawierającej assety nie omija zasad zgody.

## Granice modułów

| Katalog docelowy | Odpowiedzialność |
| --- | --- |
| `src/core/` | Stan, graf, komendy, tury, RNG, zdarzenia; bez Windows, plików i Steam |
| `src/application/` | Sesja, checkpointy, porty usług |
| `src/platform/windows/` | Win32, zegar, wejście, pliki, urządzenia |
| `src/presentation/` | Przekład stanu na zatwierdzone bitmapy i teksty |
| `src/steam/` | Opcjonalny adapter Steamworks |
| `tests/unit/`, `tests/integration/` | Logika, trwałość, kontrakty |
| `tests/fixtures/` | Niewyświetlane dane syntetyczne, nigdy w paczce gry |
| `assets/approved/` | P057: jeden niezmieniony atlas Kenney 1.2 i dwie noty; rejestr z hashami |
| `content/` | Rejestry pochodzenia, zgód i plików paczki |
| `scripts/`, `tools/` | Narzędzia deweloperskie, poza paczką gry |

Core nie zależy od prezentacji. Application korzysta z core i portów. Platforma oraz Steam implementują porty, a entrypoint składa moduły. Przy kilkudziesięciu obiektach nie potrzeba ECS.

## Domena i determinizm

- P031-02: `GameState::modules` jest wartościowym `ModuleLoadout` z dwoma kanonicznymi slotami i stabilnymi ID1/2, domyślnie pustym. Hash v3: tag uint32 little-endian 3, dotychczasowe pola w tym samym porządku i na końcu dwa bajty slotów (0=pusty,1/2=ID). P032 aktywuje `rotation_range` przez czysty port `rotation_rules`: zasięg Manhattan2 zamiast1 dla rotate i overdrive. Nieruchome nadal odrzucane; `terminal_rotation` czeka na P033. Bieżący replay6 odrzuca wcześniejsze wersje; hashv3 i pusty fixture bez zmian. Opisy v2/replay4/5 dokumentują wcześniejsze kroki. [Raport P031-02](workflow/history/P031-02.md) · [Raport P032](workflow/history/P032.md)

- Silne typy `NodeId`, `EntityId`, `RunSeed`, `TurnIndex`; jawne granice zakresów.
  `NodeId` obejmuje 0–48, po jednej wartości na pole planszy 7×7. `EntityId` obejmuje 0–4 294 967 295, pełny zakres `uint32_t`. `from_value` zwraca pusty `std::optional` dla liczby całkowitej spoza zakresu.
- Porty jako maska czterech bitów; sąsiedztwo obliczane, bez mutowania obu końców krawędzi.
- `Command` jest wartościowym `std::variant<MoveCommand, RotateCommand, WaitCommand, OverdriveCommand>`; ruch i obrót przechowują silny `NodeId`, a wait nie ma celu. `is_command_legal(const GameState&, const Command&)` wymaga SectorOutcome::ongoing dla każdego wariantu. Warunki ruchu i obrotu sprawdza przez istniejące helpery na niezależnej kopii, bez zmiany ich kontraktów; legalność wait zależy tylko od trwania sektora. Predykat zachowuje wejściowy stan, turę i komendę, nie wprowadza pakietu i nie korzysta z RNG. [Kod komend](../src/core/command.hpp) · [Raport P021-01](workflow/history/P021-01.md)
- `resolve_turn(const GameState&, const Command&)` zwraca wartościowy `TurnResult` z niezależnym `GameState state`, `bool accepted` i `SectorOutcome outcome`. Najpierw waliduje komendę; odmowa zwraca niezmieniony pełny stan i istniejącą klasyfikację bez faz i kosztu, także dla każdego terminalnego wejścia. Po akceptacji na kopii wykonuje kolejno jedno wprowadzenie pakietu, akcję gracza, ruch pakietu, dostawę, jedną pełną fazę impulsów i końcowy `charge_turn`, także po śmierci gracza. Move/rotate używają istniejących helperów. Wait nie ma efektu na planszy, więc resolver nie wywołuje dodatkowo kosztującego `wait_player`. Następny pakiet po dostawie czeka do kolejnej zaakceptowanej tury. Wejściowy stan i komenda pozostają nietknięte, a kolekcja impulsów w wyniku jest niezależna także po odmowie. Zewnętrzny `SplitMix64` jest poza API; resolver nie losuje i nie implementuje rollbacku RNG. P022-02 wylicza outcome przez sector_outcome dopiero po zagrożeniach i końcowym koszcie. Kolejna komenda po completed/death/overload jest odrzucana przed fazami, bez tury 49. P023 udostępnia podgląd przez ten sam resolver. [Kod resolvera](../src/core/turn_resolver.hpp) · [Raport P021-02](workflow/history/P021-02.md)
- Licznik tur zwiększa wyłącznie `charge_turn`. Akcje `move_player` i `rotate_from_player` nie naliczają kosztu; `wait_player` nie ma efektu na planszy, więc składa się z kontroli życia gracza i jednego naliczenia. Resolver nalicza każdą przyjętą komendę dokładnie raz. [Kod naliczania](../src/core/turn_cost.hpp)
- P022 jest ukończonym podziałem planu. P022-01 implementuje `GameState::sector_exit` jako opcjonalny `NodeId`; inicjalizacja pozostawia go pustym, a kolejne kopie zachowują wartość niezależnie. `sector_outcome(const GameState&)` zwraca `SectorOutcome::ongoing/completed/death/overload` bez mutacji i bez wykonywania zaległych faz. HP=0 zawsze daje death. Żywy gracz na wyjściu z co najmniej dwoma indeksami mającymi jednocześnie flagi odbiornika i zasilenia wygrywa najpóźniej na turze 48; na 48 sukces wyprzedza przeciążenie. Bez sukcesu co najmniej 48 tur daje overload; po limicie nie ma spóźnionej wygranej. Wynik nie jest trwałym polem GameState i nie ma osobnego licznika dostaw. P022-02 podaje go w TurnResult po zagrożeniach i koszcie oraz odrzuca terminalne komendy przed fazami. Resolver egzekwuje koniec sektora; niskopoziomowe helpery zachowują wcześniejsze kontrakty. [Kod odczytu](../src/core/sector_outcome.hpp) · [Raport P022-01](workflow/history/P022-01.md) · [Raport P022-02](workflow/history/P022-02.md)
- `GameState::packet` przechowuje jedyny aktywny pakiet jako pozycję i cel; puste pole oznacza brak pakietu na planszy. `advance_packet` przesuwa go o jedną krawędź trasy z `shortest_route` i zwraca, czy nastąpił ruch. Trasa zależy wyłącznie od portów. Krok nie nalicza tury, nie zapisuje `blocked_nodes` i nie obsługuje dostawy. [Kod kroku pakietu](../src/core/packet_movement.hpp)
- `GameState::powered_nodes` jest zapisem dostaw: flaga indeksowana `NodeId` oznacza cel, który otrzymał pakiet. Liczba dostaw to liczba flag; osobnego licznika nie ma. `deliver_packet` zdejmuje z planszy pakiet stojący na własnym, niezasilonym jeszcze celu, ustawia flagę celu i zwraca `true`. Bez pakietu, poza celem i na celu już zasilonym zwraca `false` i nie zmienia stanu, więc każdy cel jest zasilany najwyżej raz. Krok nie nalicza tury, nie sprawdza życia gracza, nie czyta `blocked_nodes` i nie wprowadza następnego pakietu. [Kod kroku dostawy](../src/core/packet_delivery.hpp)
- `GameState::receiver_nodes` wskazuje odbiorniki sektora: flaga indeksowana `NodeId` oznacza pole, do którego trafiają pakiety. Osobnego identyfikatora odbiornika nie ma; kolejność „według identyfikatorów” wyznacza `NodeId` pola. `next_packet_target` zwraca najniższy `NodeId`, który jest odbiornikiem i nie ma flagi w `powered_nodes`, albo pusty wynik, gdy stan nie ma odbiorników lub wszystkie są zasilone. Zasilony węzeł niebędący odbiornikiem nie wpływa na wynik. Predykat przyjmuje stan przez stałą referencję, nie czyta pakietu, portów, `blocked_nodes` ani gracza i nie umieszcza pakietu na planszy. `deliver_packet` nie sprawdza, czy cel jest odbiornikiem. [Kod predykatu celu](../src/core/packet_target.hpp)
- `GameState::sector_entry` to opcjonalny `NodeId` wejścia sektora; `GameState::initial` zachowuje sygnaturę i pozostawia go pustym. Dane dostarczy generator. `introduce_packet(GameState&)` jest osobnym krokiem początku tury: gdy nie ma aktywnego pakietu, jest wejście i `next_packet_target` zwraca cel, tworzy pakiet na wejściu z tym celem i zwraca `true`. Zmienia wyłącznie pakiet; bez wejścia, bez niezasilonego odbiornika lub przy aktywnym pakiecie zwraca `false` i zachowuje cały stan. Nie czyta gracza, portów ani `blocked_nodes`, nie przesuwa pakietu i nie nalicza tury. Nie ma znacznika oczekującego pakietu: ten sam warunek wprowadza pierwszy pakiet na początku pierwszej tury i kolejne po dostawach. Gdy wejście jest celem, pakiet powstaje na celu, ruch go pozostawia, a dostawę wykonuje dopiero `deliver_packet`. Wywołujący uruchamia wprowadzenie raz na początku tury, przed ruchem i dostawą; po dostawie czeka z nim do kolejnej tury. Krok sam nie rozpoznaje granic tur; kolejność zapewnia resolve_turn. [Kod wprowadzenia](../src/core/packet_introduction.hpp)
- `preview_turn(const GameState&, const Command&)` deleguje bezpośrednio do `resolve_turn` i zwraca jego wartościowy TurnResult. Resolver zapewnia jedną niezależną kopię, więc podgląd nie kopiuje ponownie stanu ani reguł. Powtarzanie zachowuje pełny live state i komendę; mutacja wyniku nie zmienia innych podglądów lub wykonań. API nie przyjmuje usług zapisu, achievementów ani RNG. Testy porównują pełne state/accepted/outcome z wykonaniem oraz jawnymi oczekiwaniami. [Kod podglądu](../src/core/turn_preview.hpp) · [Raport P023](workflow/history/P023.md)
- `GameState::pulses` jest wartościową kolekcją `Pulse` z silnym `EntityId` i pozycją `NodeId`, początkowo pustą. Każda kopia stanu posiada własną kolekcję, ID i pozycje. Poprawny stan ma unikalne ID i najwyżej jeden impuls na polu; gracz i pakiet mogą współdzielić pole z impulsem. `pulse_occupies(const GameState&, NodeId)` odczytuje zajęcie wyłącznie z pozycji impulsów, niezależnie od kolejności przechowywania, bez zmiany stanu i tury. Nie powiela flag w `blocked_nodes`: impulsy pozwalają graczowi wejść na ich pole, a osobna flaga `blocked_nodes` nadal blokuje gracza. [Kod predykatu](../src/core/pulse_occupancy.hpp) · [Raport P019-01](workflow/history/P019-01.md)
- `next_pulse_step(const GameState&, EntityId)` wyszukuje impuls po ID i zwraca pierwszy krok istniejącego `shortest_route` do bieżącej pozycji gracza, z remisami N, E, S, W. Brak wskazanej jednostki, brak trasy, impuls już na graczu lub pierwszy krok zajęty przez inny impuls dają pusty wynik. Zajęcie sprawdzane jest po wyznaczeniu trasy: impuls czeka, nawet jeśli istnieje dłuższa wolna droga. Wybór czyta bieżącą topologię; nie zapisuje zamiaru, nie mutuje stanu ani tury i nie korzysta z `blocked_nodes` lub integralności. Kontakt działa osobno w P020-01, a faza z ruchem w P020-02. [Kod wyboru kroku](../src/core/pulse_chase.hpp) · [Raport P019-02](workflow/history/P019-02.md)
- `advance_pulses(GameState&)` buduje osobną listę ID, sortuje ją rosnąco i wywołuje `next_pulse_step` dopiero w kolejce każdej jednostki. Wcześniejszy ruch może zwolnić lub zająć pole dla późniejszego impulsu; jednostka, która czekała, nie ponawia próby. Funkcja zmienia wyłącznie pozycje, zachowuje ID i kolejność kolekcji oraz nie nalicza tury. Nie usuwa impulsów i nie zmienia integralności po kontakcie; osobny helper P020-01 obsługuje kontakt, a P020-02 łączy go z ruchem. Samo wywołanie całego ruchu i dopiero potem usunięcie jednostek może zablokować późniejsze impulsy polem, które powinno być już zwolnione. [Kod ruchu](../src/core/pulse_movement.hpp) · [Raport P019-03](workflow/history/P019-03.md)
- P020 jest ukończonym podziałem planu. P020-01 implementuje `resolve_pulse_contact(GameState&, EntityId, bool& damage_applied)`: brak ID lub pozycja poza graczem zwracają `false` i zachowują cały stan oraz blokadę. Kontakt usuwa tylko wskazany impuls, zachowuje kolejność ocalałych i zwraca `true`. Przy świeżej blokadzie oraz dodatnim HP odejmuje jeden HP i ustawia blokadę; po wykorzystaniu limitu nadal usuwa kontaktujące jednostki. Przy HP=0 nie odejmuje ani nie zmienia blokady. Funkcja nie czyta portów i `blocked_nodes`, nie zmienia pakietu ani tury. Blokada należy do wywołującego, poza `GameState`. [Kod kontaktu](../src/core/pulse_contact.hpp) · [Raport P020-01](workflow/history/P020-01.md)
- `resolve_pulse_phase(GameState&)` tworzy jedną świeżą lokalną blokadę obrażeń i posortowaną listę stabilnych ID. Najpierw rozwiązuje wszystkie kontakty zastane po akcji gracza, także wysokiego ID, przed jakimkolwiek ruchem. Następnie dla ocalałych wybiera po jednym kroku przez `next_pulse_step` na bieżącym stanie i od razu rozwiązuje kontakt. Usunięcie natychmiast zwalnia pole późniejszym impulsom, także po spadku HP do zera. Jedna blokada ogranicza łączny ubytek do jednego HP; kolejne wywołanie ma świeży limit. Faza zachowuje kolejność ocalałych, nie trzyma iteratorów przez usunięcie i zmienia wyłącznie impulsy oraz ewentualnie integralność. Nie nalicza tury; resolve_turn wywołuje ją raz na zaakceptowaną turę. Dotychczasowy `advance_pulses` pozostaje krokiem samego ruchu. [Kod fazy](../src/core/pulse_phase.hpp) · [Raport P020-02](workflow/history/P020-02.md)
- Całkowitoliczbowa logika i stabilna iteracja. `SplitMix64::next_in_range(minimum, maximum)` losuje z domkniętego przedziału `uint64_t`. Odwrócone granice dają `std::nullopt`, a równe granice zwracają jedyną wartość. Oba przypadki zachowują stan generatora. Dla pełnego zakresu funkcja zwraca jeden surowy wynik. Dla pozostałych przedziałów odrzuca wyniki poniżej `2⁶⁴ mod liczba_wartości`, dzięki czemu po zastosowaniu modulo każda wartość ma tyle samo surowych odpowiedników.
- `RunSeed` przyjmuje każdą wartość `uint64_t` jako początkowy stan SplitMix64. Każde `next_u64()` najpierw dodaje `0x9e3779b97f4a7c15` do stanu modulo 2⁶⁴, a następnie miesza nową wartość przesunięciami, XOR i mnożeniami modulo 2⁶⁴. [Kod generatora](../src/core/splitmix64.hpp) · [Implementacja referencyjna](https://prng.di.unimi.it/splitmix.c)
- P024 zamknięto jako plan czterech dzieci. P024-01 implementuje `developer::state_hash(const GameState&)` w tools/replay/state_hash.hpp: dokładny pełny GameState koduje do jawnego strumienia v2 (FNV-1a 64-bit, stałe szerokości little-endian, tagi optional i kolejność kolekcji impulsów), bez mutacji lub surowej pamięci. Pełny format i niezależne wektory opisuje [raport P024-01](workflow/history/P024-01.md). Core nie zależy od tools; CMake udostępnia narzędzia wyłącznie właściwym celom deweloperskim. P024-02 implementuje `developer::seeded_fixture(RunSeed)` w tools/replay/seeded_fixture.hpp: niewyświetlany kwadrat 0/1/7/8, gracz0/HP3/tura0, wejście0, odbiorniki1/8, wyjście8 i impuls ID1 na7. Tylko węzeł1 jest obrotowy; lokalny SplitMix64 przez next_in_range(0,3) wybiera jego początkową orientację. Pozostałe pola są stałe; różne seedy mogą dać ten sam stan. Runner P024-03 oraz adapter CLI P024-04 są zaimplementowane. [Raport P024-02](workflow/history/P024-02.md). To dane narzędzia deweloperskiego, poza core i paczką gry; generator sektorów pozostaje w P028. P024-03 implementuje `developer::run_replay(uint32_t, RunSeed, span<const Command>)` w tools/replay/replay_runner.hpp. Wersja 4 łączy obecne reguły, fixture i hash; inne wersje zwracają nullopt przed utworzeniem wyniku. Wartościowy ReplayResult posiada initial_hash, vector<ReplayStep> z accepted/outcome/hash po każdej próbie oraz końcowy GameState. Odmowa nie przerywa pętli; terminalne próby zachowują stan i hash zgodnie z resolverem. Wynik i wejściowe komendy są niezależne; brak usług, plików i RNG w API. [Raport P024-03](workflow/history/P024-03.md) P024-04 implementuje main.cpp jako deweloperskie CLI budowane przy BUILD_TESTING. Parsuje całe wejście version/seed/wait/move:NodeId/rotate:NodeId przed runnerem i wyjściem; jawne liczby outcome 0/1/2/3 oznaczają ongoing/completed/death/overload. Odrzucona komenda gry jest wpisem, natomiast błędny argument lub obca wersja daje kod 1 i pusty stdout. CTest uruchamia rzeczywiste exe przez execute_process; narzędzie nie wchodzi do paczki gry i nie ma prezentacji planszy. [Raport P024-04](workflow/history/P024-04.md) Zmiana reguł, fixture lub kodowania hasha wymaga decyzji o nowej wersji replay; hash nie jest kryptograficzny i nie gwarantuje braku kolizji. [Raport P024](workflow/history/P024.md)

- P025 dodaje wyłącznie wykonywalny eksperyment w tests/unit/core_experiment_tests.cpp. Trzy sekwencje po trzy tury startują z tego samego stanu numerycznego; obrót węzła 7 otwiera wspólną krawędź 7–8 dla pakietu i impulsu, a kolejny odcina impuls od gracza. Test używa istniejącego resolvera i preview oraz pełnych jawnych oczekiwań bez nowego kodu produkcyjnego. [Raport P025](workflow/history/P025.md) dokumentuje dane, wyniki i ograniczenia; nie stanowi playtestu ani dowodu balansu.

P026 zamknięto wyłącznie jako podział planu. Łańcuch P026-01…05 oddziela dane zasobów i pełny hash, regenerację dostawy, helper dwóch obrotów, integrację komendy z transakcją oraz parser CLI. P027 zależy od ostatniego dziecka. Docelowe pola GameState to reserve uint8 (start 2, limit gry 3) i overload_debt uint32 (start 0); helper odrzuci przepełnienie długu przed mutacją. P026-01 dodaje te pola i pełny hash v2: format v1 z prefiksem 2, reserve uint8 i overload_debt uint32 LE na końcu. P026-02 dodaje regenerację przy rzeczywistej dostawie (+1 do limitu 3), bez leczenia i zmiany długu. Replay 4 odrzuca wcześniejsze wersje. P026-03 implementuje overdrive_from_player w player_overdrive.hpp: zasoby sprawdzane przed pierwszą mutacją, cel jak rotate, dwa obroty i -1 reserve/+1 debt; bez faz i tury. Dług MAX i brak rezerwy odrzucane atomowo. P026-04 dodaje OverdriveCommand i integrację helpera w legalności oraz fazie akcji resolvera przed dostawą; preview pozostaje wspólne. Koszt zasobów poprzedza regenerację dostawy, jeden charge_turn zostaje na końcu. P026-05 dodaje token overdrive:NodeId do pełnego parsowania CLI; nie zmienia reguł ani wersji. [Raport P026-01](workflow/history/P026-01.md)

P027 podzielono na wybór najmniejszego wolnego EntityId i spawn na wejściu sektora. Jedno wejście mieści jeden impuls: debt6 pozostawi3 po jednym spawnie; przy zajęciu nic nie znika. Gracz/pakiet nie są blokerami, blocked_nodes i impulsy są. Wywołanie raz na granicy sektora będzie obowiązkiem przyszłego runu; resolver nie spawnuje długu. P027-01 implementuje available_pulse_id(const GameState&) w pulse_identity.hpp: najmniejszy wolny ID, odczyt bez alokacji i mutacji, szeroki kandydat chroni uint32 przed zawinięciem. P027-02 implementuje spawn_debt_pulse(GameState&): debt>=3 i wolne wejście, append jednego impulsu z wolnym ID, potem -3 długu. Błąd alokacji zachowuje pełny stan. Bez pola granicy sektora i bez wywołania przez resolver. [Raport P027](workflow/history/P027.md)

P028 implementuje SectorTopology i generate_sector_topology(RunSeed) w core/sector_topology.hpp. Lokalny SplitMix64 wybiera jedną z dwóch tras13 pól, odbiorniki na indeksach4/8, entry0 i exit48; przyłącza nieruchome,9 pozostałych węzłów obrotowych. Reszta siatki ma maski0. Zwracane dane są niezależne, bez tworzenia GameState, zagrożeń i runu. To odrębne API od replay fixture, które nie zmienia wersji replay4/hashv2. P029 dodaje validate_solution(GameState, span<const Command>) w solution_certificate.hpp: początkowy ongoing, wszystkie akcje accepted, końcowy completed przez wspólny resolver. Nie zmienia wejść, nie zastępuje wygranej osiągalnością grafu, nie jest solverem. Testowe świadki czterech seedów nie są produkcyjnym generatorem rozwiązań. [Raport P028](workflow/history/P028.md)

## Zapis

Oddziel `profile` (odblokowania, achievementy, ustawienia) od `run_checkpoint` (seed, granica sektora, reguły, moduły, HP, rezerwa, dług). Pliki w katalogu użytkownika, nie przy exe. Zapis tymczasowy → bezpieczna zamiana → backup ostatniego poprawnego pliku. Schema version, limity rozmiaru i walidacja wartości. Uszkodzony zapis nie nadpisuje poprawnego backupu. Bez serializacji wskaźników.

P040-01..03: `LocalProfile` jest niezależną wartością w application, bez pól
runu. [Profil v1](PROFILE_FORMAT.md) ma 24 bajty, schema1, stałe ID 1..5,
maskę wariantów oraz ustawienia volume/mute. `encode_profile` i `decode_profile`
walidują długość, wersję, reserved, checksum i zakresy; nie korzystają z IO.
P040-04/05: adapter Windows odczytuje jeden rekord oraz zapisuje przez flush
i publikację backupu przed main. Odmawia zapisu przy invalid/niedostępnym main;
zachowuje poprawny backup. Nie wdraża automatycznej naprawy lub fallbacku profilu.

P038-01..03: adapter `platform/windows/checkpoint_*.hpp` czyta dokładnie40
bajtów i zapisuje `main.tmp` z kontrolą WriteFile/FlushFileBuffers/CloseHandle.
Przy istniejącym poprawnym main przygotowuje `main.bak.tmp` z jego wartości,
publikuje go jako `main.bak`, a potem `main.tmp` jako main. Publikacja używa
[MoveFileExW](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-movefileexw)
z REPLACE_EXISTING i WRITE_THROUGH w tym samym katalogu, bez copy/delete.
Uszkodzony/niedostępny main blokuje zapis i nie narusza backupu.
Odmowa może pozostawić staging; retry go zastępuje. Przy odmowie ostatniej
zamiany backup może już zawierać nadal aktualny main. Kontrakt wymaga jednego
zapisującego w prywatnym katalogu lokalnym; nie jest transakcją dwóch nazw
ani gwarancją wobec zaniku zasilania. Wybór katalogu użytkownika i wywołania
z sesji są nadal planem. P039 dodaje read-only `recover_checkpoint_file`:
poprawny main ma pierwszeństwo, brak/uszkodzenie pozwala czytać `.bak`,
inny błąd IO zatrzymuje wybór. Wynik zachowuje źródło i diagnostykę obu prób.
Odczyt nie naprawia plików ani nie promuje staging; zapis nadal odrzuca
uszkodzony main. Polityka naprawy i podłączenie do sesji pozostają poza API.

## Achievementy

Lokalny evaluator reaguje na zdarzenia domenowe. Stałe AchievementId nie są
etykietami dla gracza. Pięć MVP: dostawa, sektor, run bez overdrive, dostawa po
rekonfiguracji, ukończenie runu. [P045](ACHIEVEMENT_PREDICATES.md) precyzuje
predykaty i resetowanie; P046–P050 implementują je w MvpAchievementEvaluator.
Evaluator nie alokuje i nie wykonuje IO. Powstaje od nowa dla każdej próby
i gałęzi, a historia po restore jest domyślnie nieznana. ActionCommitted
przenosi dowód zmiany trasy BFS wyliczony przez wrapper w P045-02.

P041-01/02: [kontrakt zdarzeń v1](DOMAIN_EVENTS.md) definiuje wartościowe
payloady i klucz wewnątrz próby. `resolve_run_turn_events` wywołuje istniejący
resolver raz i zwraca listę: akcja, dostawa, sukces sektora, koniec runu.
Odmowa daje pustą listę; preview nadal zwraca tylko wynik core, bez usług.
Korpus pełnych sekwencji i restore sprawdzono w P041-03. P051-01 sprawdza pełny
run z rzeczywistymi predykatami i restore w procesie; P051-02 sprawdza dwa
nowe procesy, pełne stany/payloady i dokładne main/bak profili 31 oraz 27.
Brak trwałego outboxu i globalnej tożsamości
prób; seed nie jest unikalnym ID runu. Dispatch opisano poniżej.

Port `AchievementSink` ma backend lokalny i później Steam. Najpierw trwały zapis lokalny, potem idempotentna synchronizacja z kolejką i potwierdzeniami. Offline i brak Steam nie blokują gry. [Dokumentacja Steamworks](https://partner.steamgames.com/doc/features/achievements) opisuje odblokowanie i zapis statystyk; aktualne API sprawdzić przy implementacji. Ikony, opisy i nazwy wymagają pochodzenia tak samo jak reszta treści.

P043: `AchievementSink` jest małym portem application z wynikami unlocked,
already_unlocked, invalid i unavailable. Oba pierwsze potwierdzają trwały zbiór;
błąd pozwala na retry tego samego ID. `windows::LocalAchievementSink` posiada
wyłącznie ścieżkę profilu i deleguje do unlock_profile_file, bez cache, sieci
i Steam SDK. Instancje odczytują ten sam trwały zbiór. Predykaty pozostają
przyszłymi krokami; symulacja i preview nie otrzymują tego portu.

P044-01: [dispatch v1](DOMAIN_DISPATCH.md) definiuje DeliveryScope z tokenami
próby/gałęzi i początkową granicą sektora. AchievementEvaluator zwraca fixed
maskę kandydatów bez IO i wyjątków. Cache dispatchu jest pamięciowy, a trwała
idempotencja pozostaje zbiorem ID profilu. P044-02 wdraża pamięciowy dispatcher
z zachowaniem wyniku oceny przed sinkiem i retry wyłącznie pending ID.
P044-03 potwierdza pełne sekwencje resolvera, restore i nowe procesy z lokalnym
sinkiem; duplikaty nie przesuwają backupu. Checkpoint nie zachowuje historii
evaluatora; brak historii nie oznacza spełnienia predykatu runu bez overdrive.

## Prezentacja

P062-01: [NativeWindow](../src/platform/windows/native_window.hpp) posiada
ukryty HWND i własną klasę Win32; tytuł pochodzi z TextCatalog i ścisłej
konwersji UTF-8. Życie obiektu jest przypisane do wątku, bez kopiowania
i przenoszenia. WM_NCDESTROY unieważnia uchwyt; destruktor niszczy okno
i wyrejestrowuje klasę. Bez renderera, pętli komunikatów i focus w tym kroku.
Pełny zakres kolejnych dzieci: [kontrakt okna](WINDOW_LIFECYCLE.md).

P058 dzieli [odczyt zatwierdzonych odwołań](ASSET_MANIFEST.md) na cztery dzieci:
model application, pamięciowy parser Windows.Data.Json, chroniony odczyt pliku
z SHA-256 oraz dowód integracyjny. Plan nie jest jeszcze gotowym czytnikiem;
P059 czeka na P058-04. P058-01 wdraża model, a P058-02 parser pamięciowy;
P058-03 wdraża chroniony odczyt z SHA-256 BCrypt i kanoniczną lokalizacją.
P058-04 pozostaje dowodem integracji rejestru. Systemowe API nie rozszerza
uprawnień do treści.

Renderer używa wyłącznie zatwierdzonego manifestu. Brak zasobu oznacza kontrolowany błąd lub wyłączenie widoku, nie rysowanie nowej ikony. Panele, ścieżki i znaczniki powstają z pozyskanych elementów. Zwykłe pozycjonowanie i skalowanie gotowych bitmap jest operacją prezentacji; nie daje prawa do twórczego przerabiania artworku.

Developer build dopuszcza oznaczone teksty robocze. Release wymaga kompletnych finalnych tekstów z dowodami pochodzenia. Komunikat błędu pokazywany graczowi również jest treścią. Wewnętrznych nazw nie wyświetlaj jako finalnego tytułu. Binarne identyfikatory nie mogą wyciekać na UI jako nieoznaczone etykiety.

P060: `TextCatalog` w application posiada niezależne wartości `TextRecord`
i publikuje całą listę dopiero po walidacji. ID jak dla odwołań assetów,
do 256 wpisów, każdy 1..4096 bajtów bez NUL. ai_placeholder wymaga dokładnego
początku `[AI PLACEHOLDER]` i braku źródła; approved odrzuca ten marker
i wymaga source_asset_id istniejącego w poprawnym zatwierdzonym manifeście.
Lookup nieznanego ID daje nullptr. To kontrakt pamięciowy bez JSON/IO/renderera;
zgodność treści z ludzkim źródłem nadal wymaga rejestru i review. P061
dostarczy rozdzielenie developer/public i bramkę placeholderów.

## Styl i budżety

4 spacje; `snake_case` dla funkcji i pól, `PascalCase` dla typów; RAII dla COM, uchwytów i pamięci. MSVC `/W4`, `/permissive-`, ostrzeżenia jako błędy dla własnego kodu w CI. Debug i Release. Osobna konfiguracja AddressSanitizer po sprawdzeniu obsługi toolchainu.

Cel pomiarowy: 60 FPS prezentacji, logiczne tury niezależne od FPS, podgląd typowej planszy do 16 ms, RAM poniżej 250 MB. To budżety do sprawdzenia na opisanym sprzęcie, nie ogłoszone wymagania minimalne. Raport benchmarku podaje sprzęt i rozkład czasów.

P033-01: `GameState::terminal_rotation_used` ma domyślnie false i jest częścią pełnego stanu. Hashv4: tag4, pola v3, na końcu bajt0/1 zużycia. Replay7 wiąże tę wersję. Reguła obrotu i reset czekają na P033-02/03.

P033-02: wspólny rotate_from_player stosuje jednorazowe uprawnienie do nieruchomej maski o jednym porcie; overdrive dziedziczy walidację i wykonuje drugi obrót bez ponownego zużycia. Replay8, hashv4. Reset i granica runu pozostają osobne.

P035 to podział planu na sześć dzieci, nie gotowy run. Docelowy RunState posiada root seed, SectorNumber1..3, aktywny GameState i reward_claimed. RunPhase jest wyliczany z sector_outcome, numeru i nagrody. Zasoby pozostają wyłącznie w aktywnym sektorze. SektorN używa N-tego wyniku lokalnego SplitMix64(root seed); brak odziedziczonego RNG. Hashv4/replay8 obejmują dotychczasowy GameState i fixture, nie metadane runu. Checkpoint i wersjonowanie runu dopiero P036.

P035-01 implementuje wyłącznie RunState, ograniczony SectorNumber i niemutujący run_phase w core/run_state.hpp. Gotowy odczyt faz nie oznacza działania startu, tury, nagrody ani przejścia runu.

P035-02 implementuje start_run(RunSeed) w core/run_start.hpp: pierwszy seed sektora pochodzi z pierwszego losowania lokalnego SplitMix64. Złożony GameState zaczyna na wejściu z domyślnymi zasobami. API nie przechowuje RNG, nie wywołuje usług i nie wykonuje tury ani przejścia. Pełny run czeka na P035-03..06.

P035-03..06 implementują resolver i preview runu, bramkowaną nagrodę i wartościowe przejście. Test integracyjny przez trzy generowane sektory używa prawdziwych API, tworzy dług przez overdrive, przenosi moduły i HP oraz blokuje czwarty sektor i akcje po porażce. Jest to dowód headless, bez prezentacji, zapisu ani oceny grywalności.

P036 opisuje [checkpoint granicy sektora v1](CHECKPOINT_FORMAT.md): 40 bajtów, niezależne wersje schematu/reguł, FNV-1a64 i pełna rekonstrukcja początku sektora. Dług po spawnie i flaga opłaconego impulsu zapobiegają podwójnemu kosztowi przy wznowieniu. Kodek i IO nie są jeszcze zaimplementowane.

P037-01..04 implementują pamięciowy kodek w `src/application/`: wartościowy
RunCheckpoint, ścisłą projekcję/rekonstrukcję pełnej granicy oraz encoder/decoder40
bajtów ze sprawdzeniem wersji, checksumy, zakresów i kanoniczności. Korpus11000
granic sprawdza pełny stan i pierwszą turę po resume; błędne dane nie zwracają
częściowego wyniku. Adaptery IO, publikacji i odczytu backupu opisano wyżej.

P063 rozbito na filtr naciśnięć, mapowanie kandydatów Command i adapter wiadomości Win32. Kontrakt w KEYBOARD_INPUT.md; P064 wymaga P063-03. Brak implementacji dzieci w kroku planistycznym.

P064 rozbito na RAII fabryki/celu HWND oraz resize/recreate. DIRECT2D_LIFECYCLE.md określa pożyczony HWND, pojedynczy wątek i brak rysowania. P065 wymaga P064-02; sam podział nie implementuje backendu.

P065: read_png_asset łączy kontrolę pochodzenia bajtów P058 z systemowym decoderem PNG WIC i owning PBGRA. COM jest własnością caller; brak częściowego obrazu i fallbacku. PNG_DECODING.md określa limity i niezależny numeryczny oracle GDI+. Bez renderowania.

P066-01..04: presentation mapuje maski na gotowe komórki atlasu i współrzędne
7x7; AtlasRenderer posiada bitmapę i jej dokładny cel Direct2D. BoardView
jest właścicielem zasobów planszy i pożyczonego callbacka WM_PAINT, odłączanego
przed destrukcją. NativeWindow i apartment COM żyją dłużej. Resize i utrata
celu odtwarzają zasoby. Developer entrypoint używa istniejącego start_run,
przypiętego hasha atlasu i ścieżki checkoutu nadanej przez CMake. Nie ma
zależności core od rendererów. Kontrakt: [BOARD_PRESENTATION](BOARD_PRESENTATION.md).

P067-01..03: scene_draws składa kafle i gotowe symbole w rozłącznych obszarach.
RunWindow posiada wartościowy RunState i BoardView; otrzymuje filtrowane
naciśnięcie przez pożyczony WindowCommandHandler. Resolver core jest jedynym
źródłem legalności i faz. Najpierw powstaje pełny wynik i kopia do widoku,
potem niezgłaszające wyjątków move publikuje run. Callbacki są odłączane przed
destrukcją. Brak zależności od checkpointów, evaluatora achievementów i usług.
