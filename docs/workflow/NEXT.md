# Następny mały krok

Task: P024-03

## Cel i lektura

Dodaj jeden wartościowy runner replay: wersja, seed i komendy → ślad oraz niezależny stan końcowy. P024-01 dostarcza kanoniczny hash, P024-02 implementuje seeded_fixture. Debug i Release zaliczyły po26/26 CTest; walidator ma131 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P024-02](history/P024-02.md), [format hasha](history/P024-01.md), P024-03 i jego zależność w [kolejce](../plan/tasks.json), [fixture](../../tools/replay/seeded_fixture.hpp), [hash](../../tools/replay/state_hash.hpp), [resolver](../../src/core/turn_resolver.hpp), [komendy](../../src/core/command.hpp), [GameState](../../src/core/game_state.hpp), [test fixture](../../tests/unit/seeded_fixture_tests.cpp), [test podglądu](../../tests/unit/turn_preview_tests.cpp), [CMake](../../CMakeLists.txt), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md) i komendy [README](../../README.md).

## Wykonaj

Jeden kontrakt w tools/replay/, poza core i paczką gry. Wybierz małe wartościowe typy oraz funkcję przyjmującą wersję uint32, RunSeed i sekwencję Command przez const/span. Obsługiwana wersja1 obejmuje obecne reguły, fixture i kodowanie hasha. Nieobsługiwaną wersję odrzuć jawnie przed tworzeniem wyniku; brak częściowego śladu lub stanu, np. optional wyniku. Bez parsera, plików, CLI i usług.

Dla wersji1 utwórz seeded_fixture(seed), zapisz początkowy state_hash i wykonaj wszystkie próby kolejno przez istniejący resolve_turn. Nie kopiuj reguł ani faz. Każda próba daje wpis accepted/outcome/hash po próbie. Odmowa pozostawia hash, ale nie zatrzymuje kolejnych prób. Dotyczy to także komend po terminalnym stanie. Wynik posiada niezależny końcowy GameState i kolekcję wpisów. Zmiana wejściowych komend lub wyniku nie zmienia innych wykonań; API nie przyjmuje zewnętrznego RNG.

## Dowód sukcesu i błędu

Zarejestruj test z prywatnym include tools, opcjami MSVC i CTest. Obejmij wersje0/2/UINT32_MAX odrzucone bez wyniku, pustą sekwencję, seedy0/MAX oraz znany korpus legalnych i błędnych move/rotate/wait. Ślad i pełny stan porównuj z bezpośrednim resolverem oraz jawnymi oczekiwaniami istotnych rezultatów; samo porównanie dwóch wywołań runnera nie wystarcza. Sprawdź powtórzenia, niezmienność wejściowych komend, niezależność wyników i stanów, w tym impulsów i optional.

Fixture: seedy0/1/2/MAX dają maskę węzła1 odpowiednio6/9/3/12 i początkowy hash7229561696845526205/18006929851103986098/826881297037723540/18070937588695573379. Reszta stanu jest stała. Dobierz sekwencje odrzucenie→legalna próba i kontynuację po końcu sektora; jeśli potrzebujesz dłuższej sekwencji do limitu, nie zmieniaj fixture dla wygody testu. Jawnie chroń długość śladu i niezmieniony hash po odmowie.

Celowo pomiń bramkę wersji albo komendę w produkcyjnym runnerze. Wymuś /t:Rebuild właściwego celu, potwierdź rekompilację i oczekiwany błąd CTest; przywróć kod i przebuduj przed końcowymi testami.

## Weryfikacja i przekazanie

Konfiguracja, build i pełne CTest Debug/Release, formatter src/tests/tools, walidator i kontrole diff. Zaktualizuj README/ARCHITECTURE, zapisz history/P024-03.md i przestaw STATE/NEXT na P024-04 po kryteriach. Następny krok to mały adapter CLI przyjmujący dziesiętne version seed oraz opcjonalne wait/move:NodeId/rotate:NodeId, walidujący całe wejście przed wykonaniem i testowany na rzeczywistym exe; nie implementuj go teraz.

Obowiązkowy lokalny commit wyłącznie własnych zmian, sprawdź hash/zakres/czystość; bez pushu. Nie implementuj drugiego ID, generatora P028, runu, overdrive, zapisu ani prezentacji. Nie generuj assetów lub treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem.
