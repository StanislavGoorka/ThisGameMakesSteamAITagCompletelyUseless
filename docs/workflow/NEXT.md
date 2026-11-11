# Następny mały krok

Task: P019

## Cel

Dodaj jeden typ zagrożenia podążający po grafie: impuls pościgowy. Zależność P018-03 jest ukończona i zaimplementowana; wszystkie trzy dzieci P018 są gotowe. P020 zależy od P019 i zajmie się kontaktem oraz obrażeniami.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P018-03](history/P018-03.md), wpis P019 i zależności w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [BFS](../../src/core/grid_pathfinding.hpp), [ruch pakietu](../../src/core/packet_movement.hpp), [ruch gracza](../../src/core/player_movement.hpp), [testy BFS](../../tests/unit/grid_pathfinding_tests.cpp), [testy ruchu pakietu](../../tests/unit/packet_movement_tests.cpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`GameState` ma porty, flagi obrotu i blokowania, gracza z integralnością, licznik tur, opcjonalny pakiet, zapis zasilonych węzłów, odbiorniki i opcjonalne wejście sektora `sector_entry`. `GameState::initial` zachowuje dotychczasową sygnaturę i pozostawia wejście puste; dane wejścia dostarczy generator. Testy ustawiają pola bezpośrednio. Nie ma reprezentacji zagrożeń ani ich kolekcji. `blocked_nodes` to wyłącznie tablica flag zajęcia przez blokujące jednostki, czytana przez ruch gracza; nie ma jeszcze źródła tych flag w postaci obiektów jednostek. `EntityId` już istnieje, ale stan go nie używa.

`shortest_route` oblicza trasę po wzajemnych portach, z remisami N, E, S, W; nie czyta zajęcia pól. `advance_packet` wykonuje jeden ruch, nie nalicza tury. `introduce_packet` wprowadza pakiet tylko przy braku aktywnego pakietu, obecności wejścia i niezasilonego odbiornika. Nie ma znacznika oczekującego pakietu: wywołujący ma używać kroku raz na początku tury, przed ruchem i dostawą. Wywołanie go po dostawie w tej samej turze może wprowadzić kolejny pakiet; dopiero resolver P021 wymusi poprawną kolejność. Test P018-03 składa tę kolejność ręcznie.

Pakiety nie blokują ruchu. Dostawa nie sprawdza, czy cel jest odbiornikiem; nie zmieniaj jej przy okazji. Obecnie jest piętnaście testów CTest, wszystkie przeszły w Debug i Release.

## Wykonaj

Najpierw oceń rozmiar P019 według WORKFLOW: jedno obserwowalne zachowanie ruchu impulsu, do około 150 linii kodu produkcyjnego i trzech plików, poza testami. Jeśli reprezentacja, ruch i utrzymywanie zajęcia wymagają osobno ocenialnych kontraktów wykraczających poza mikro-krok, wykonaj wyłącznie rozbicie P019 na dzieci, przepnij zależność P020 i pozostaw pierwsze dziecko jako następne. Nie zaczynaj wtedy implementacji.

Przy implementacji zachowaj reguły: impuls idzie o jedną krawędź BFS do bieżącej pozycji gracza, czeka bez trasy, remisy N, E, S, W. Zagrożenia są rozwiązywane w stabilnej kolejności identyfikatorów; próba wejścia na pole innego zagrożenia oznacza czekanie. Nie szukaj objazdu wokół zajętego pola w miejsce tej odmowy. Zdefiniuj reprezentację i kontrakt zajęcia względem `blocked_nodes`, żeby ruch gracza i impulsy odczytywały spójny stan. Zachowaj wartościowe kopiowanie `GameState`, deterministyczny porządek i silne ID.

Testy powinny wykazać jeden ruch po grafie, remisy, brak trasy i stabilne blokowanie innym impulsem, niezależnie od kolejności zapisania jednostek. Sprawdzaj pełny stan oraz brak naliczenia tury. Celowy błąd produkcyjny ma potwierdzić istotny test negatywny. Przy każdej próbie sprawdź w wyjściu builda rekompilację testu; `/t:Rebuild` pozwala uniknąć starej binarki przy szybkich zmianach nagłówka.

Każde nowe pole `GameState` dopisz do testów inicjalizacji/kopiowania i siedmiu helperów `same_state`: `player_movement`, `player_rotation`, `player_wait`, `packet_movement`, `packet_delivery`, `packet_target`, `packet_introduction`. Nowe testy rejestruj w trzech miejscach CMake wzorem ostatnich testów.

## Nie rozszerzaj

Nie implementuj kontaktu, obrażeń, znikania impulsu po kontakcie, limitu obrażeń ani porażki przy zerowej integralności: to P020. Nie dodawaj resolvera tury P021, innych typów zagrożeń, emitowania impulsów, generatora sektora, rezerwy, długu, wyjścia, zdarzeń, achievementów, zapisu ani prezentacji. Nie zmieniaj reguł BFS ani kroków pakietu. Nie generuj treści dla gracza ani assetów. Nie naprawiaj przy okazji `scripts/Test-ProjectValidator.ps1`; znany błąd opisano w raportach P018 i P018-03. Nie zaczynaj P020.

## Weryfikacja i przekazanie

Przy zmianie C++ wykonaj konfigurację, build i CTest Debug/Release oraz sprawdzenie formatowania według [README](../../README.md). Zaktualizuj README i ARCHITECTURE odpowiednio do rezultatu. Zapisz raport `history/P019.md`, STATE i NEXT z następnym gotowym ID. Uruchom walidator projektu i `git diff --check`, następnie wykonaj lokalny commit wyłącznie własnych zmian i sprawdź jego hash oraz zakres. Przy samym rozbiciu nie uruchamiaj pozorowanych testów gry; sprawdź zależności walidatorem, także negatywnie na izolowanej kopii.
