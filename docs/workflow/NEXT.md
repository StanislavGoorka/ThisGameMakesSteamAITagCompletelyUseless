# Następny mały krok

Task: P020-02

## Cel

Dodaj jedną fazę impulsów z kontaktem przed ruchem i natychmiast po ruchu każdej jednostki. P020-01 jest zaimplementowane, a P020 pozostaje podziałem planu. Nie zaczynaj resolvera P021.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [skill cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE.json](STATE.json), [raport P020-01](history/P020-01.md), [raport podziału P020](history/P020.md), wpis P020-02 i jego zależność w [tasks.json](../plan/tasks.json), [GameState](../../src/core/game_state.hpp), [kontakt](../../src/core/pulse_contact.hpp), [wybór kroku](../../src/core/pulse_chase.hpp), [sam ruch](../../src/core/pulse_movement.hpp), [ruch gracza](../../src/core/player_movement.hpp), [testy kontaktu](../../tests/unit/pulse_contact_tests.cpp), [testy ruchu impulsów](../../tests/unit/pulse_movement_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), sekcję „Kolejność tury” w [GAME_DESIGN](../GAME_DESIGN.md) i [TESTING](../TESTING.md).

## Stan zastany

`resolve_pulse_contact(GameState&, EntityId, bool& damage_applied)` wyszukuje jednostkę po ID. Brak ID lub pozycja poza graczem zwraca `false` bez zmiany stanu i blokady. Kontakt usuwa tylko wskazany impuls i zachowuje kolejność ocalałych; zwraca `true` także przy HP=0 lub wykorzystanym limicie. Przy świeżej blokadzie i HP>0 odbiera jeden HP i ustawia blokadę. Przy HP=0 nie zmienia blokady ani nie odejmuje. Nie przesuwa, nie nalicza tury i nie zmienia pakietu. Blokadę tworzy wywołujący; nie jest polem stanu.

`next_pulse_step` wybiera pojedynczą krawędź BFS na bieżącym stanie. Zajęty pierwszy krok oznacza czekanie bez objazdu. `advance_pulses` pozostaje osobną funkcją samego ruchu: kolejność ID, jeden krok, bez usuwania i obrażeń. Nie zmieniaj tego kontraktu. Poprawny stan ma unikalne ID i pozycje impulsów; gracz i pakiet mogą współdzielić ich pole. P020-01 zaliczyło 19/19 CTest Debug i Release, z wykrytą i wycofaną mutacją pomijającą limit obrażeń.

## Wykonaj

Jedna funkcja fazy przyjmująca `GameState&`; nazwę wybiera wykonawca. Utwórz jedną świeżą lokalną blokadę `bool damage_applied = false` na wywołanie. Używaj tego samego kontekstu we wszystkich kontaktach tej fazy.

1. Zbierz stabilne ID i rozwiąż wszystkie kontakty zastane po akcji gracza przed jakimkolwiek ruchem, także gdy kontaktujący impuls ma najwyższe ID. Usunięcie zwalnia pole od razu.
2. Dla ocalałych impulsów w rosnącej kolejności `EntityId` oblicz `next_pulse_step` na bieżącym stanie. Każdy rusza najwyżej raz; czekający nie ponawia próby.
3. Natychmiast po kroku rozwiąż kontakt wskazanej jednostki przez helper P020-01. Po usunięciu późniejsze ID widzą wolne pole i mogą wejść na gracza oraz zniknąć. Nie wykonuj całego `advance_pulses` przed zbiorczym kontaktem.

Zachowaj kolejność ocalałych w kolekcji. Nie trzymaj iteratorów ani referencji przez usunięcie elementu vectora; ID i ponowne wyszukanie wystarczą. Faza ma dokończyć kontakty także po utracie ostatniego HP. Nowe wywołanie ma świeży limit, ale samo nie wyznacza granic tur; P021 wywoła fazę raz po zatwierdzonej akcji. Zmieniaj tylko pozycje/usunięcie impulsów oraz ewentualnie integralność. Bez naliczenia tury, nowych pól stanu i ogólnego systemu zdarzeń.

## Dowód sukcesu i test błędu

Testuj pustą kolekcję, brak ruchu i kontaktu, jeden krok, kontakt po legalnym wejściu gracza na impuls przed fazą oraz wejście impulsu na gracza. Szczególnie sprawdź kontakt zastany o wysokim ID, który musi zniknąć zanim niższe ID spróbuje wejść na jego pole. Dwa lub więcej impulsów wchodzących kolejno na gracza ma zniknąć natychmiast, przy łącznym ubytku jednego HP.

Kontakty przed i po ruchu współdzielą limit; nowa faza pozwala na kolejne obrażenie. Sprawdź HP=1/0, usuwanie kontaktów po śmierci i brak underflow. Testuj wszystkie permutacje kolekcji z porównaniem pełnego stanu i kolejnością ocalałych. Zachowaj reguły ruchu: wcześniejsze zwolnienie pola, brak retry oraz blokada bez objazdu. Pakiet, porty, flagi, wejście i niezerowa tura mają być chronione pełnym porównaniem.

Nowy test zarejestruj w trzech miejscach CMake. Celowa mutacja produkcyjna ma potwierdzić istotny przypadek negatywny, np. opóźnienie usunięcia po kontakcie lub pominięcie kontaktów przed ruchem. Wymuś `/t:Rebuild` właściwego celu i potwierdź rekompilację w logu. Przywróć kod przed końcowymi Debug/Release.

## Nie rozszerzaj

Nie implementuj resolvera, wyjścia, limitu sektora, emisji, generatora, rezerwy, długu, zapisu ani prezentacji. Nie zmieniaj BFS, kroków pakietu ani kontraktów `move_player`, `advance_pulses` i pojedynczego kontaktu. Nie generuj assetów i treści dla gracza. Nie naprawiaj przy okazji znanego błędu `scripts/Test-ProjectValidator.ps1` opisanego w raportach P018 i P018-03.

## Weryfikacja i przekazanie

Wykonaj konfigurację, build i CTest Debug/Release oraz formatowanie według [README](../../README.md). Zaktualizuj README i ARCHITECTURE zgodnie z wynikiem. Zapisz `history/P020-02.md`; STATE i NEXT przestaw na P021 dopiero po kryteriach. P021 najpierw wymaga oceny zakresu, bo łączy walidację komend, transakcję kroków i obserwowalną kolejność. Uruchom walidator i kontrole diff, wykonaj lokalny commit własnych zmian, sprawdź hash oraz zakres i zatrzymaj się przed kolejnym ID.
