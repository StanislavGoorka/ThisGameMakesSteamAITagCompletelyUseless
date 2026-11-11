# Następny mały krok

Task: P021

## Cel

Oceń zakres resolvera tury przed kodowaniem. P020-02 jest zaimplementowane: istnieje pełna faza impulsów z kontaktami i wspólnym limitem obrażeń. P021 łączy reprezentację i walidację komend, transakcję istniejących kroków oraz obserwowalną kolejność; jeżeli są to odrębne kontrakty, tym krokiem jest wyłącznie podział na małe dzieci. Nie rozpoczynaj pierwszego dziecka w tym samym kroku.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P020-02](history/P020-02.md), wpis P021 i jego zależność w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [ruch gracza](../../src/core/player_movement.hpp), [obrót gracza](../../src/core/player_rotation.hpp), [wait](../../src/core/player_wait.hpp), [naliczanie tury](../../src/core/turn_cost.hpp), [wprowadzenie pakietu](../../src/core/packet_introduction.hpp), [ruch pakietu](../../src/core/packet_movement.hpp), [dostawę](../../src/core/packet_delivery.hpp), [fazę impulsów](../../src/core/pulse_phase.hpp), [testy fazy](../../tests/unit/pulse_phase_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`resolve_pulse_phase(GameState&)` zbiera i sortuje stabilne ID. Najpierw usuwa wszystkie kontakty zastane po akcji gracza, także wysokiego ID, przed jakimkolwiek ruchem. Potem każdy ocalały rusza najwyżej raz według ID, korzystając z bieżącego `next_pulse_step`; kontakt po ruchu usuwa go natychmiast i zwalnia pole następnym jednostkom. Usuwanie trwa także po HP=0. Jedna świeża lokalna blokada ogranicza łączny ubytek do jednego HP na wywołanie; następne wywołanie ma nowy limit. Faza zmienia tylko impulsy i ewentualnie HP, zachowuje kolejność ocalałych i nie nalicza tury. Przyszły resolver ma wywoływać ją dokładnie raz na zaakceptowaną turę.

`advance_pulses` nadal jest osobną funkcją samego ruchu i nie należy wywoływać jej dodatkowo przed pełną fazą. `resolve_pulse_contact` nadal ma własny wąski kontrakt. P020-02 zaliczyło 20/20 CTest w Debug i Release; test wykrył mutację pomijającą kontakty przed ruchem. Kod przywrócono.

Nie ma jeszcze `Command`, `TurnResult` ani resolvera. RNG jest osobnym `SplitMix64`, poza `GameState`; istniejące kroki tury go nie przyjmują i nie losują. `move_player` i `rotate_from_player` wykonują tylko akcję i nie sprawdzają życia gracza. `wait_player` sprawdza życie i sam nalicza turę. `charge_turn` jest jedyną funkcją zmieniającą licznik; integracja nie może podwójnie policzyć `wait`.

Wprowadzenie, ruch i dostawa pakietu to osobne funkcje. `introduce_packet` umieszcza pakiet, gdy go nie ma, istnieje wejście i niezasilony odbiornik; nie rozpoznaje granic tur. Następny pakiet po dostawie wolno wprowadzić dopiero na początku kolejnej zaakceptowanej tury. Wyjście sektora i limit 48 tur pozostają do P022.

## Wykonaj

Ustal jeden obserwowalny kontrakt i test błędu. Jeśli P021 wymaga kilku niezależnych rezultatów, wykonaj wyłącznie podział: zachowaj ID rodzica jako wpis planistyczny, dodaj małe dzieci w porządku topologicznym, przenieś do nich wszystkie pierwotne kryteria, przepnij P022 na ostatnie wymagane dziecko i ustaw pierwsze gotowe dziecko w STATE/NEXT. Uzasadnij podział istniejącymi lukami i testami, nie samym limitem linii.

Kontrakty przyszłej implementacji muszą zachować:

- Odrzucenie nielegalnej komendy przed wprowadzeniem pakietu lub inną mutacją; pełny stan, licznik oraz zewnętrzny RNG pozostają bez zmian. Martwy gracz odrzuca każdą akcję. Wymagane są legalne i nielegalne `move`, `rotate` i `wait`; overdrive nie należy do tego kroku.
- Kolejność po akceptacji: wprowadzenie pakietu na początku tury, akcja gracza, ruch pakietu w nowej topologii, dostawa, jedna pełna faza impulsów, naliczenie kosztu dokładnie raz. Nie wprowadzaj drugiego pakietu zaraz po dostawie i nie wykonuj dodatkowego ruchu impulsów.
- Test kolejności z obserwowalnym wynikiem: np. obrót zmienia trasę pakietu i impulsu, kontakt po wejściu gracza jest rozpoznany przed ruchem impulsów, a następny pakiet czeka do kolejnej tury. Nie dodawaj ogólnego systemu zdarzeń tylko dla testu, jeżeli wystarczy wynik domenowy.
- Wspólną przyszłą ścieżkę preview/commit przez resolver; samo preview pozostaje do P023. Nie zmieniaj kontraktów istniejących helperów bez uzasadnienia, zwłaszcza naliczania `wait`.

## Weryfikacja

Dla podziału wykonaj walidator i kontrolę grafu, także na izolowanej kopii z próbą pominięcia nieukończonych dzieci. Zapisz dokładne wyniki i ograniczenia; zmiana dokumentacyjna nie wymaga CMake/CTest ani formattera. Nie traktuj wcześniejszych 20/20 jako nowych testów tego podziału.

Jeśli ocena wykaże jeden wystarczająco mały kontrakt implementacyjny, wykonaj odpowiednie testy błędu i pełnego stanu, kontrolowaną mutację z potwierdzoną rekompilacją oraz wymagane konfigurację/build/CTest Debug i Release i formatter. Nie oznaczaj całego P021 jako implementacji po dostarczeniu tylko części.

## Nie rozszerzaj i przekaż

Nie implementuj wyjścia, limitu sektora, preview, overdrive, generatora, rezerwy, długu, zapisu i prezentacji. Nie generuj assetów ani treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

Zaktualizuj README/ARCHITECTURE zgodnie z faktem, zapisz `history/P021.md` i spójne STATE/NEXT. Uruchom walidator oraz kontrole diff, wykonaj lokalny commit tylko własnych zmian i sprawdź hash oraz zakres. Zatrzymaj się przed kolejnym ID.
