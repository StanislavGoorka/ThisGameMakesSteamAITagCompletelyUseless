# Następny mały krok

Task: P020

## Cel

Oceń rozmiar kontaktu i obrażeń przed implementacją P020. P019 i jego trzy dzieci są ukończone; reprezentacja, wybór kroku i ruch impulsów działają. P020 łączy kontakt przed fazą zagrożeń, znikanie jednostek po ruchu oraz wspólny limit jednego obrażenia w turze. Jeśli to więcej niż jeden mały kontrakt, tym krokiem jest wyłącznie podział P020 na dzieci i przepięcie P021 na ostatnie wymagane dziecko. Nie zaczynaj wtedy implementacji pierwszego dziecka.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P019-03](history/P019-03.md), wpis P020 i jego zależność w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [ruch impulsów](../../src/core/pulse_movement.hpp), [wybór kroku](../../src/core/pulse_chase.hpp), [ruch gracza](../../src/core/player_movement.hpp), [testy ruchu impulsów](../../tests/unit/pulse_movement_tests.cpp), [testy gracza](../../tests/unit/player_movement_tests.cpp), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`advance_pulses(GameState&)` działa raz na każdej jednostce w rosnącej kolejności `EntityId`, z wyborem na stanie po wcześniejszych ruchach. Buduje osobną listę ID i zachowuje kolejność `GameState::pulses`. Zajęte pierwsze pole oznacza czekanie bez objazdu; niższe ID może zwolnić pole wyższemu, ale wcześniejszy impuls nie ponawia próby. Funkcja zmienia tylko pozycje. Impuls na graczu pozostaje w kolekcji i blokuje wejście innych impulsów; integralność oraz tura są zachowane.

`move_player` pozwala wejść na impuls po legalnej krawędzi, bez obrażeń i usuwania jednostki. Osobne `blocked_nodes` nadal blokuje ruch gracza. Poprawny stan ma unikalne ID impulsów i najwyżej jedną jednostkę na polu. Gracz i pakiet mogą z nią współdzielić pole. Ostatnie testy to 18/18 CTest Debug i Release; kontrolowana mutacja odwracająca kolejność ID została wykryta i wycofana.

## Kontrakt do zachowania w P020

- Wejście gracza na impuls musi zostać rozpoznane przed ruchem zagrożeń, nawet gdy impuls mógłby później odejść. Kontakt po wejściu impulsu na gracza również usuwa jednostkę.
- Znikanie kontaktującego impulsu powinno od razu zwolnić pole dla późniejszych ID. Samo `advance_pulses`, a potem zbiorczy kontakt, może błędnie blokować drugi impuls. Nie obliczaj całej listy zamiarów ze stanu początkowego. Zachowaj deterministyczny porządek i kolejność pozostałych jednostek w kolekcji.
- Jeden wspólny limit obrażeń ma obejmować kontakty przed ruchem i po ruchu w tej samej turze. Nie przechowuj bez potrzeby drugiej kopii zajęcia ani licznika kontaktów w `GameState`. Reset limitu na granicy tury będzie odpowiedzialnością resolvera P021.
- Wiele kontaktów usuwa wszystkie odpowiednie impulsy, lecz zabiera najwyżej jeden punkt integralności. Integralność nie może spaść poniżej zera; śmierć musi być rozpoznawalna przed przyszłym sprawdzeniem wyjścia. Dostawa nie leczy. Nie naliczaj tury wewnątrz kontaktu.

## Dowód sukcesu i test błędu

Przy podziale zachowaj niezmienne ID, porządek topologiczny i pierwotne kryteria P020 w dzieciach. Walidator ma odrzucać rozpoczęcie dziecka z nieukończoną zależnością i P021 przed ostatnim dzieckiem; testuj na izolowanej kopii, bez zmiany roboczego stanu. Sam podział nie wymaga CMake/CTest.

Jeśli ocena uzasadni jeden wąski krok implementacyjny, testuj legalne wejście gracza na impuls, wejście impulsu na gracza, kontakt przed fazą ruchu oraz wiele kontaktów w obu momentach przy wspólnym limicie. Uwzględnij integralność 1 i 0, brak kontaktu, powtórzenie kroku oraz zwolnienie pola dla późniejszego ID. Porównuj pełny stan, w tym pakiet, dostawy i turę. Celowa mutacja ma ujawnić istotny błąd; wymuś rekompilację celowanego testu i przywróć kod przed końcowymi Debug/Release.

## Nie rozszerzaj

Nie implementuj resolvera P021, wyjścia ani limitu sektora P022, emisji, innych zagrożeń, generatora, rezerwy, długu, zapisu lub prezentacji. Nie zmieniaj BFS i kroków pakietu. Nie generuj assetów ani treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Zapisz `history/P020.md`, zaktualizuj README i ARCHITECTURE zgodnie z rzeczywistym zakresem. Przy podziale ustaw STATE/NEXT na pierwsze dziecko; przy implementacji przejdź do P021 dopiero po kryteriach. Dla kodu wykonaj konfigurację, build, CTest Debug/Release i formatowanie według [README](../../README.md). Zawsze uruchom walidator i kontrole diff, wykonaj lokalny commit wyłącznie własnych zmian oraz sprawdź hash i zakres. Zatrzymaj się przed kolejnym ID.
