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
| `assets/approved/` | Zatwierdzone zasoby; obecnie katalog nie istnieje |
| `content/` | Rejestry pochodzenia, zgód i plików paczki |
| `scripts/`, `tools/` | Narzędzia deweloperskie, poza paczką gry |

Core nie zależy od prezentacji. Application korzysta z core i portów. Platforma oraz Steam implementują porty, a entrypoint składa moduły. Przy kilkudziesięciu obiektach nie potrzeba ECS.

## Domena i determinizm

- Silne typy `NodeId`, `EntityId`, `RunSeed`, `TurnIndex`; jawne granice zakresów.
  `NodeId` obejmuje 0–48, po jednej wartości na pole planszy 7×7. `EntityId` obejmuje 0–4 294 967 295, pełny zakres `uint32_t`. `from_value` zwraca pusty `std::optional` dla liczby całkowitej spoza zakresu.
- Porty jako maska czterech bitów; sąsiedztwo obliczane, bez mutowania obu końców krawędzi.
- `GameState` + `Command` → `TurnResult`: nowy stan i zdarzenia. Nielegalna komenda nie zużywa RNG.
- Licznik tur zwiększa wyłącznie `charge_turn`. Akcje `move_player` i `rotate_from_player` nie naliczają kosztu; `wait_player` nie ma efektu na planszy, więc składa się z kontroli życia gracza i jednego naliczenia. Resolver nalicza każdą przyjętą komendę dokładnie raz. [Kod naliczania](../src/core/turn_cost.hpp)
- `GameState::packet` przechowuje jedyny aktywny pakiet jako pozycję i cel; puste pole oznacza brak pakietu na planszy. `advance_packet` przesuwa go o jedną krawędź trasy z `shortest_route` i zwraca, czy nastąpił ruch. Trasa zależy wyłącznie od portów. Krok nie nalicza tury, nie zapisuje `blocked_nodes` i nie obsługuje dostawy. [Kod kroku pakietu](../src/core/packet_movement.hpp)
- `GameState::powered_nodes` jest zapisem dostaw: flaga indeksowana `NodeId` oznacza cel, który otrzymał pakiet. Liczba dostaw to liczba flag; osobnego licznika nie ma. `deliver_packet` zdejmuje z planszy pakiet stojący na własnym, niezasilonym jeszcze celu, ustawia flagę celu i zwraca `true`. Bez pakietu, poza celem i na celu już zasilonym zwraca `false` i nie zmienia stanu, więc każdy cel jest zasilany najwyżej raz. Krok nie nalicza tury, nie sprawdza życia gracza, nie czyta `blocked_nodes` i nie wprowadza następnego pakietu. [Kod kroku dostawy](../src/core/packet_delivery.hpp)
- `GameState::receiver_nodes` wskazuje odbiorniki sektora: flaga indeksowana `NodeId` oznacza pole, do którego trafiają pakiety. Osobnego identyfikatora odbiornika nie ma; kolejność „według identyfikatorów” wyznacza `NodeId` pola. `next_packet_target` zwraca najniższy `NodeId`, który jest odbiornikiem i nie ma flagi w `powered_nodes`, albo pusty wynik, gdy stan nie ma odbiorników lub wszystkie są zasilone. Zasilony węzeł niebędący odbiornikiem nie wpływa na wynik. Predykat przyjmuje stan przez stałą referencję, nie czyta pakietu, portów, `blocked_nodes` ani gracza i nie umieszcza pakietu na planszy. `deliver_packet` nie sprawdza, czy cel jest odbiornikiem. [Kod predykatu celu](../src/core/packet_target.hpp)
- Podgląd używa resolvera na kopii, bez zapisu i achievementów.
- Całkowitoliczbowa logika i stabilna iteracja. `SplitMix64::next_in_range(minimum, maximum)` losuje z domkniętego przedziału `uint64_t`. Odwrócone granice dają `std::nullopt`, a równe granice zwracają jedyną wartość. Oba przypadki zachowują stan generatora. Dla pełnego zakresu funkcja zwraca jeden surowy wynik. Dla pozostałych przedziałów odrzuca wyniki poniżej `2⁶⁴ mod liczba_wartości`, dzięki czemu po zastosowaniu modulo każda wartość ma tyle samo surowych odpowiedników.
- `RunSeed` przyjmuje każdą wartość `uint64_t` jako początkowy stan SplitMix64. Każde `next_u64()` najpierw dodaje `0x9e3779b97f4a7c15` do stanu modulo 2⁶⁴, a następnie miesza nową wartość przesunięciami, XOR i mnożeniami modulo 2⁶⁴. [Kod generatora](../src/core/splitmix64.hpp) · [Implementacja referencyjna](https://prng.di.unimi.it/splitmix.c)
- Replay: wersja reguł, seed, komendy. Zmiana reguł może zerwać zgodność; nieobsługiwaną wersję odrzucaj jawnie.

## Zapis

Oddziel `profile` (odblokowania, achievementy, ustawienia) od `run_checkpoint` (seed, granica sektora, reguły, moduły, HP, rezerwa, dług). Pliki w katalogu użytkownika, nie przy exe. Zapis tymczasowy → bezpieczna zamiana → backup ostatniego poprawnego pliku. Schema version, limity rozmiaru i walidacja wartości. Uszkodzony zapis nie nadpisuje poprawnego backupu. Bez serializacji wskaźników.

## Achievementy

Lokalny evaluator reaguje na zdarzenia domenowe. Stałe identyfikatory, np. `ACH_FIRST_DELIVERY`, nie są etykietami dla gracza. Pięć MVP: dostawa, sektor, run bez overdrive, dostawa po rekonfiguracji, ukończenie runu. P045 precyzuje predykaty i resetowanie; implementować każdy oddzielnie.

Port `AchievementSink` ma backend lokalny i później Steam. Najpierw trwały zapis lokalny, potem idempotentna synchronizacja z kolejką i potwierdzeniami. Offline i brak Steam nie blokują gry. [Dokumentacja Steamworks](https://partner.steamgames.com/doc/features/achievements) opisuje odblokowanie i zapis statystyk; aktualne API sprawdzić przy implementacji. Ikony, opisy i nazwy wymagają pochodzenia tak samo jak reszta treści.

## Prezentacja

Renderer używa wyłącznie zatwierdzonego manifestu. Brak zasobu oznacza kontrolowany błąd lub wyłączenie widoku, nie rysowanie nowej ikony. Panele, ścieżki i znaczniki powstają z pozyskanych elementów. Zwykłe pozycjonowanie i skalowanie gotowych bitmap jest operacją prezentacji; nie daje prawa do twórczego przerabiania artworku.

Developer build dopuszcza oznaczone teksty robocze. Release wymaga kompletnych finalnych tekstów z dowodami pochodzenia. Komunikat błędu pokazywany graczowi również jest treścią. Wewnętrznych nazw nie wyświetlaj jako finalnego tytułu. Binarne identyfikatory nie mogą wyciekać na UI jako nieoznaczone etykiety.

## Styl i budżety

4 spacje; `snake_case` dla funkcji i pól, `PascalCase` dla typów; RAII dla COM, uchwytów i pamięci. MSVC `/W4`, `/permissive-`, ostrzeżenia jako błędy dla własnego kodu w CI. Debug i Release. Osobna konfiguracja AddressSanitizer po sprawdzeniu obsługi toolchainu.

Cel pomiarowy: 60 FPS prezentacji, logiczne tury niezależne od FPS, podgląd typowej planszy do 16 ms, RAM poniżej 250 MB. To budżety do sprawdzenia na opisanym sprzęcie, nie ogłoszone wymagania minimalne. Raport benchmarku podaje sprzęt i rozkład czasów.
