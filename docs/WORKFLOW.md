# Jedno wznowienie = jeden mikro-krok

## Kontrakt pracy

Polecenie „Zrealizuj następny krok” oznacza wykonanie jednego zadania z kolejki, jego weryfikacji i przekazania pracy. Nie oznacza realizacji całego milestone'u. P000 to jednorazowy pakiet planowania wyraźnie zamówiony przez użytkownika; nie jest wzorcem rozmiaru kolejnych zmian.

Domyślny limit kroku: jedna obserwowalna zmiana zachowania, jeden predykat, jeden adapter o wąskim kontrakcie albo jedna decyzja udokumentowana dowodami. Zwykle do 3 plików produkcyjnych i około 150 nowych linii kodu produkcyjnego, poza testami i przekazaniem. To sygnał do podziału, nie cel do wypełnienia. Zadanie musi dać się oddzielnie ocenić. Nie łącz „dodaj typ przeciwnika, UI i balans”.

## Początek

1. Czytaj root AGENTS, skill [next-step](../.agents/skills/next-step/SKILL.md), `STATE.json`, `NEXT.md` i raport poprzednika. Sprawdź `git status`, nie nadpisuj cudzych zmian.
   Na lokalnym dysku bez informacji o właścicielu użyj `git -c safe.directory=D:/ThisGameMakesSteamAITagCompletelyUseless ...`, zgodnie z README; nie zmieniaj globalnych wyjątków.
2. Wykonaj walidator planu. Znajdź obiekt `current_task` w [tasks.json](plan/tasks.json), sprawdź wszystkie zależności i dokumenty wymagane przez zadanie. Nie wczytuj całego backlogu implementacyjnego, jeśli wystarcza bieżący wpis.
3. Określ jeden dowód sukcesu i test negatywny, jeśli zmienia się zachowanie. Jeżeli rozmiar zadania jest za duży, **tym krokiem jest wyłącznie jego rozbicie** na mniejsze zadania i aktualizacja przekazania; nie rozpoczynaj implementacji.

## Implementacja i test

Najpierw sprawdź kontrakt i istniejące testy. Zmień minimum kodu. Wykonaj testy bezpośrednio dotknięte, a także wymagane bramki etapu. Obejrzyj diff pod kątem assetów, nowych tekstów i wycieków sekretów. Nie dodawaj „przy okazji” nowej architektury. Poprawienie błędu testu jest częścią tego samego małego kroku; nowa funkcja nie jest.

Nie raportuj testu jako zaliczonego, jeśli nie był uruchomiony. Brak kompilatora oznacza niewykonany test i blokadę ukończenia zadania wymagającego kompilacji, nie zielony wynik. Nie używaj reset/clean do usuwania pracy użytkownika. Nie uruchamiaj automatycznych instalacji narzędzi systemowych bez ustalenia potrzeby i odpowiednich uprawnień.

## Zakończenie

- `docs/workflow/history/<ID>.md`: data, cel, pliki, decyzja, dokładne komendy i wyniki, ograniczenia, konkretna instrukcja następcy. Bez fikcyjnych testów i odwołań do nieistniejących logów.
- `STATE.json` jest źródłem stanu: `completed`, `current_task`, `blocked`, `last_report`. Dodaj zadanie do completed dopiero po spełnieniu jego kryteriów. Nie oznaczaj planu jako implementacji.
- `NEXT.md` jest przekazaniem dla człowieka i agenta. Podaje ten sam `Task: <ID>`, mały zakres, zależności, pliki do czytania, testy i zakazy rozszerzania zakresu.
- Zaktualizuj README, jeśli zmienił się stan lub działające komendy. Uruchom walidator i `git diff --check`. Commituj tylko swoje pliki z krótkim opisem zmiany; wstrzymaj commit przy nieznanym zakresie cudzych zmian.
- Bieżące polecenie użytkownika autoryzuje pierwszy publiczny push planu. Dalsze kroki domyślnie kończą się lokalnym commitem. Push wykonuj, gdy użytkownik go poleci lub ustanowi osobną zgodę; nie zakładaj automatycznego publikowania każdego wznowienia.

Zakończ turę krótkim raportem po polsku: co gotowe, co sprawdzono, co jest następne. Nie realizuj następnego zadania w tym samym wznowieniu.

## Kolejka, podział i blokady

Autorytatywny backlog to `tasks.json`; roadmap jest mapą etapów. ID są niezmienne. Przy podziale wpis pozostaje planistycznym zadaniem nadrzędnym, a dzieci otrzymują np. `P085-01`, `P085-02`. W tym kroku zamknij tylko zadanie rozbijające. Zależności konsumentów etapu przenieś na ostatnie wymagane dziecko; samo zamknięcie planu nie oznacza dostarczenia modułów. Utrzymuj porządek topologiczny tablicy zadań.

Przy braku zgody lub narzędzia zachowaj nieukończone zadanie w `blocked` z przyczyną i warunkiem odblokowania. Wybierz jedno inne gotowe, małe zadanie, jeśli można je wykonać bez zależności od blokady. Nie oznaczaj zablokowanego zadania jako done. Gdy żadnego nie ma, pozostaw current_task na blokadzie i zakończ z konkretną potrzebą. Nowe „następny krok” nie jest zgodą na pobranie.

## Autonomia i rzeczywiste wyjątki

Agent sam wybiera szczegóły implementacji, balans i podział zadań w ramach [koncepcji](GAME_DESIGN.md). Użytkownik nie jest kierownikiem projektu ani testerem każdego kroku. Brak decyzji nie oznacza zgody na zmianę wizji.

Wyraźne zgody wymagane przez założenia: pobranie każdego wskazanego pakietu assetów. Zgody mają konkretny zakres i wersję, więc nie trzeba pytać ponownie dla zatwierdzonego zakresu. Wniosek przygotowuje agent; człowiek tylko zezwala lub odmawia. Nie pytamy o pobieranie, dopóki brak ten nie jest potrzebny do konkretnego etapu.

Zewnętrzne warunki wydania: konto Steamworks, prawa do SDK, formalności właściciela, opłaty, finalne treści pochodzenia ludzkiego i aktualne warunki Valve. Agenci przygotowują dowody i integrację; nie mogą samodzielnie stworzyć ludzkiego autorstwa ani podpisać umowy za człowieka. Jeżeli nie znajdą legalnych nie-AI treści bez udziału autora, dana część wydania pozostaje zablokowana. Projekt nie obiecuje całkowicie bezobsługowej premiery ani gwarancji braku oznaczenia AI.
