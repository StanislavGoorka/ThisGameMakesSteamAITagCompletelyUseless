# Następny mały krok

Task: P019-01

## Cel

Dodaj reprezentację impulsów i jeden predykat zajęcia pola przez impuls. P019 jest ukończonym wpisem planistycznym, nie implementacją zagrożeń. Zależność P018-03 była zaimplementowana przed podziałem; pierwsze dziecko P019-01 jest gotowe do realizacji. Nie zaczynaj wyboru kroku P019-02 ani ruchu P019-03.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P019](history/P019.md), [raport P018-03](history/P018-03.md), wpisy P019 i P019-01 w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [ruch gracza](../../src/core/player_movement.hpp), [testy stanu](../../tests/unit/game_state_tests.cpp), [testy ruchu gracza](../../tests/unit/player_movement_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`GameState` nie ma reprezentacji zagrożeń ani kolekcji jednostek. Ma porty, flagi obrotu i blokowania, gracza z integralnością, licznik tur, opcjonalny pakiet, zapis zasilonych węzłów, odbiorniki i opcjonalne wejście sektora. `GameState::initial` pozostawia pakiet i wejście puste. `EntityId` istnieje, lecz stan go jeszcze nie używa.

`blocked_nodes` to tablica flag zajęcia przez blokujące jednostki, czytana przez `move_player`. Nie ma obiektów będących źródłem tych flag; testy ustawiają je bezpośrednio. Pakiet nie blokuje ruchu i może współdzielić pole z graczem. BFS czyta wyłącznie porty. W kodzie nie ma impulsów, obrażeń ani resolvera tury. Ostatni wynik 15/15 CTest w Debug i Release pochodzi z P018-03; podział P019 nie uruchamiał ponownie testów gry.

## Wykonaj

Jeden kontrakt: stan przechowuje impulsy wartościowo, a predykat odczytuje, czy wskazane pole jest zajęte przez impuls.

- Każdy impuls ma silny `EntityId` i pozycję `NodeId`. Zalecana prosta kolekcja wartości, bez ECS, wskaźników i zależności platformowych. Nie utrzymuj osobnej kopii flag zajęcia impulsów. Nazwy typu, kolekcji i predykatu wybiera wykonawca.
- Stan początkowy ma pustą kolekcję; zachowaj dotychczasową sygnaturę `GameState::initial`. Kopia stanu ma niezależnie posiadać wszystkie jednostki, ich ID i pozycje. Prawidłowy stan ma unikalne ID i najwyżej jeden impuls na polu. Udokumentuj te warunki; nie dodawaj jeszcze fabryki spawnu ani walidatora całego sektora. Gracz i pakiet mogą dzielić pole z impulsem.
- Predykat przyjmuje stan przez stałą referencję i `NodeId`, zwraca zajęcie przez dowolny impuls, niezależnie od kolejności zapisu jednostek. Nie czyta portów, integralności ani `blocked_nodes`; nie mutuje stanu i nie nalicza tury.
- **Nie wpisuj impulsów do `blocked_nodes`.** Reguły P020 wymagają wejścia gracza na impuls, więc takie pole jest legalnym celem ruchu po wzajemnej krawędzi. Osobna flaga `blocked_nodes` nadal odrzuca ruch. Predykat zajęcia impulsów będzie w P019-02 blokował wybrany krok innego impulsu; nie zmieniaj teraz `move_player` bez potrzeby.

Każde nowe pole `GameState` dopisz do testów inicjalizacji/kopiowania i siedmiu helperów `same_state`: `player_movement`, `player_rotation`, `player_wait`, `packet_movement`, `packet_delivery`, `packet_target`, `packet_introduction`. W odpowiednich fabrykach testowych umieść niepustą kolekcję, aby istniejące kroki nie mogły jej po cichu wyzerować. Nowe testy rejestruj w trzech miejscach CMake wzorem ostatnich testów.

## Dowód sukcesu i test błędu

Testy mają wykazać pustą kolekcję po inicjalizacji, niezależność dwóch kolejnych kopii z niepustą kolekcją oraz odczyt zajętego i wolnego pola przy pustej i niepustej kolekcji, również dla NodeId 0 i 48 i po odwróceniu kolejności jednostek. Predykat musi zachowywać pełny stan, w tym licznik tur.

Test integrujący z istniejącym ruchem gracza: połączenie na pole impulsu pozwala na ruch i zmienia wyłącznie pozycję gracza; ten sam cel z osobną flagą `blocked_nodes` odrzuca ruch i zachowuje cały stan. Kontakt nie zmienia integralności ani nie usuwa impulsu, bo te zachowania należą do P020.

Celowy błąd produkcyjny ma potwierdzić istotny test negatywny, np. predykat błędnie zwracający zajęcie wolnego pola. Przy próbie sprawdź w wyjściu builda rekompilację testu; `/t:Rebuild` chroni przed starą binarką po szybkiej zmianie nagłówka. Przywróć kod przed końcowymi testami Debug/Release.

## Nie rozszerzaj

Nie implementuj wyboru następnego pola, BFS impulsu, ruchu, sortowania fazy zagrożeń, kontaktu, obrażeń, znikania jednostek ani emisji. Nie dodawaj resolvera, innych typów zagrożeń, generatora sektora, rezerwy, długu, wyjścia, zdarzeń, zapisu, achievementów ani prezentacji. Nie zmieniaj BFS i kroków pakietu. Nie generuj treści dla gracza ani assetów. Nie naprawiaj przy okazji `scripts/Test-ProjectValidator.ps1`; znany błąd opisano w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz sprawdzenie formatowania według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z faktycznym wynikiem. Zapisz `history/P019-01.md`, zaktualizuj STATE i NEXT na P019-02 dopiero po spełnieniu kryteriów. Uruchom walidator projektu i `git diff --check`, wykonaj lokalny commit wyłącznie własnych zmian, sprawdź hash i zakres. Zatrzymaj się przed P019-02.
