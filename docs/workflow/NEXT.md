# Następny mały krok

Task: P020-01

## Cel

Dodaj jeden kontrakt kontaktu wskazanego impulsu z graczem oraz współdzielonego limitu obrażeń. P020 jest ukończonym wpisem planistycznym, nie implementacją kontaktów. P019-03 jest zaimplementowane. Nie zaczynaj pełnej fazy P020-02 ani resolvera P021.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P020](history/P020.md), [raport P019-03](history/P019-03.md), wpisy P020 i P020-01 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [ruch gracza](../../src/core/player_movement.hpp), [ruch impulsów](../../src/core/pulse_movement.hpp), [testy ruchu gracza](../../tests/unit/player_movement_tests.cpp), [testy ruchu impulsów](../../tests/unit/pulse_movement_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`GameState::pulses` to wartościowa kolekcja `Pulse {EntityId id, NodeId position}`. Poprawny stan ma unikalne ID i najwyżej jeden impuls na polu. Gracz i pakiet mogą współdzielić pole z impulsem. `move_player` pozwala na wejście na impuls po legalnej krawędzi; osobne `blocked_nodes` nadal blokuje gracza. Nie ma obsługi kontaktu ani obrażeń. `advance_pulses` przesuwa jednostki w kolejności ID, zmienia wyłącznie ich pozycje i zachowuje kolejność kolekcji. Nie wolno zmieniać jego dotychczasowego kontraktu w tym kroku.

`player_integrity` jest `uint8_t`; zero oznacza martwego gracza. Żaden istniejący krok nie odejmuje HP. Ostatnie 18/18 CTest w Debug i Release pochodzi z P019-03; podział P020 nie uruchamiał testów gry.

## Wykonaj

Jedna funkcja przyjmuje `GameState&`, `EntityId` oraz przekazywaną przez referencję lokalną blokadę obrażeń. Nazwy i prostą reprezentację blokady wybiera wykonawca; wystarczy informacja, czy obrażenie już zadano. Blokada nie jest polem `GameState` i nie ma automatycznego resetu w funkcji kontaktu.

- Znajdź wskazany impuls po ID. Gdy go nie ma albo stoi poza graczem, zachowaj cały stan oraz blokadę i zwróć brak kontaktu.
- Gdy stoi na graczu, usuń wyłącznie tę jednostkę, zachowując kolejność pozostałych. Zwróć informację, że kontakt wystąpił, również gdy gracz już nie żyje lub limit był zużyty.
- Jeśli blokada nie jest zużyta i HP > 0, odejmij dokładnie jeden HP i oznacz obrażenie jako zadane. Przy HP = 0 nie odejmuj niczego i pozostaw blokadę bez zmiany. Zużyta blokada nigdy nie pozwala na kolejne obrażenie; nadal usuwaj kontaktujące impulsy.
- Nie przesuwaj jednostek i nie naliczaj tury. Zachowaj wszystkie inne pola, w tym pakiet i flagi dostaw. Kontakt nie czyta portów ani `blocked_nodes`.

P020-02 utworzy jedną świeżą blokadę na wywołanie fazy i użyje jej zarówno przed ruchem, jak i po każdym ruchu. Najpierw usunie wszystkie zastane kontakty, niezależnie od ich ID. Potem każdy ocalały impuls ruszy najwyżej raz w kolejności ID i zniknie od razu po kontakcie. To przyszły zakres, nie implementacja tego kroku. Nie dodawaj nowych pól stanu ani ogólnego systemu zdarzeń.

## Dowód sukcesu i test błędu

Testuj brak wskazanego ID, brak kontaktu, kontakt z HP 3, HP 1 i HP 0, już zużytą blokadę i ponowienie po usunięciu jednostki. Uwzględnij graniczne ID, różne kolejności kolekcji oraz zachowanie kolejności ocalałych. Każde wywołanie porównuje pełny stan i blokadę z oczekiwaniem, łącznie z niepustym pakietem oraz niezerową turą.

Test wielu kontaktów może wykonać kolejne wywołania z tą samą blokadą, ustawiając przed każdym kolejną jednostkę na graczu w poprawnym stanie: wszystkie wskazane impulsy znikają, HP maleje tylko raz. Nowa, świeża blokada umożliwia obrażenie w następnej turze; w tym kroku testuje to wywołujący, nie resolver. Test integrujący `move_player` z kontaktem ma wykazać legalne wejście gracza na impuls, usunięcie jednostki i ubytek jednego HP, bez zmiany tury i pakietu. Nie łącz jeszcze kontaktu z ruchem impulsów.

Nowy test zarejestruj w trzech miejscach CMake według ostatnich testów. Celowa mutacja produkcyjna ma sprawdzić istotny negatywny przypadek, np. pominięcie blokady obrażeń przy drugim kontakcie. Wymuś `/t:Rebuild` właściwego celu, potwierdź rekompilację w logu i przywróć kod przed końcowymi Debug/Release.

## Nie rozszerzaj

Nie implementuj P020-02, sortowania kontaktów całej fazy, resolvera, wyjścia, limitu sektora, emisji, generatora, rezerwy, długu, zapisu ani prezentacji. Nie zmieniaj BFS, kroków pakietu, `move_player` ani kontraktu `advance_pulses`. Nie generuj assetów i treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatowanie według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z rzeczywistym wynikiem. Zapisz `history/P020-01.md`; STATE i NEXT przestaw na P020-02 dopiero po kryteriach. Uruchom walidator i kontrole diff, wykonaj lokalny commit wyłącznie własnych zmian i sprawdź hash oraz zakres. Zatrzymaj się przed kolejnym ID.
