# Testy i bramki jakości

## Obecnie

`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1` kontroluje spójność dokumentacji, kolejki i pustych rejestrów. `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Test-ProjectValidator.ps1` sprawdza 11 przypadków, w tym uszkodzony hash i brak zgody, na izolowanych kopiach. Syntetyczne rekordy testowe nie są prawdziwymi zgodami ani dowodami autorstwa.

`core_build_smoke` sprawdza kompilację C++20 i wykonanie kodu korzystającego z `std::span` i ranges. `core_identifier_ranges` sprawdza granice i porządek wartości `NodeId` oraz `EntityId`. Kompilacja testu weryfikuje także brak niedozwolonych konwersji. `core_port_rotation` sprawdza obrót o 90° dla każdej maski, powrót po czterech obrotach, zachowanie liczby portów i odrzucanie wartości spoza zakresu. `core_grid_adjacency` sprawdza wzajemność połączeń, granice siatki i brak zawijania między wierszami. Uruchamianie Debug/Release opisuje [README](../README.md). [Raport P002](workflow/history/P002.md) dokumentuje celowy błąd i niezerowe kody runnera oraz CTest w obu konfiguracjach.

## C++ i zakres testów

P058-04: windows_asset_manifest_integration czyta rzeczywisty rejestr P057
i porównuje pełne bajty trzech zatwierdzonych plików przez parse/lookup/read.
Kontroluje nieznane ID, traversal, zły hash, korupcję i brak własnych kopii.
Oryginały i rejestr pozostają bez zmian, bez PNG decode i prezentacji.

P058-03: windows_asset_read chroni dokładne bajty abc i pustego pliku,
Unicode katalogu bazowego, powtórki, brak pliku, blokadę, invalid reference,
katalog, złe długości i hash. Rzeczywiste junction na C: (temp) sprawdzają
reparse i ucieczkę poza approved; D: nie obsługuje tych próbek. Testy nie
dotykają zatwierdzonych assetów. Wyłącznie cały bufor albo kontrolowany błąd.

P058-02: `windows_asset_manifest_parse` uruchamia systemowy Windows.Data.Json
w rzeczywistym apartment WinRT. Literalny rekord chroni pełne pola/digest,
puste manifesty, JSON escapes i niezależne kopie. Odmowy obejmują składnię,
root/wersję/typy, brakujące pola/status, hash/ID/ścieżkę, duplikaty,
ujemne/ułamkowe/zbyt duże bytes, 256/257 rekordów, 1 MiB/+1 i głębokość32/33.
Nawiasy, escaped quote i backslash wewnątrz stringów nie zwiększają głębokości.
BOM, NUL i nieprawidłowe UTF-8 są odrzucane; prawidłowe Unicode metadanych
przechodzi. Pierwszy test ujawnił zastępowanie złego UTF-8 w domyślnej konwersji;
regresja przechodzi po ścisłej walidacji Win32 przed konwersją. Brak IO,
renderowania i zmian approved; rzeczywisty rejestr z plikami czeka na P058-04.

P058-01: `application_asset_reference` sprawdza własne wartości ID/ścieżki,
32 literalne bajty SHA-256, wszystkie 256 wartości znaku ID i 16 384 zmian
znaku digestu. Obejmuje długości ID64/65 i ścieżki240/241, zakres bajtów,
Windows device names/ADS/traversal/NUL/Unicode, 256/257 odwołań, duplikaty ID
i ścieżek różniących się case oraz brak nieznanego ID. Kopie i wartości
po move nie mogą udostępnić invalid manifestu. Bez JSON, plików i renderowania.

CTest uruchamia mały natywny runner testów; późniejszą bibliotekę można dodać tylko z uzasadnieniem. Nazwy plików: `*_tests.cpp`; nazwy przypadków opisują warunek i rezultat, np. `rotate_four_times_restores_ports`, `invalid_move_preserves_rng`. Każdy test działa również w Release, bez polegania na `assert`.

Nie narzucamy sztucznego procentu coverage całego produktu. Core musi mieć testy wszystkich reguł i opisanych ścieżek błędów. Liczba zaliczonych testów ani 100% line coverage nie dowodzą grywalności. Przy poprawce błędu najpierw przypadek odtwarzający błąd, potem poprawka i ponowne uruchomienie.

| Warstwa | Dowód wymagany |
| --- | --- |
| Porty i graf | Każda maska, 4 obroty = tożsamość, wzajemność połączeń, krawędzie planszy, remisy BFS, brak trasy |
| Tura | Nielegalna akcja nie mutuje stanu/RNG; kolejność dostawa–zagrożenie–wyjście–limit, kontakt i limit obrażeń |
| Podgląd | Dokładna zgodność przewidywanego stanu z zatwierdzoną akcją; brak efektów ubocznych |
| RNG i replay | Znane wektory, te same seedy i komendy dają te same hashe; różne wersje reguł odrzucane |
| Generator | Co najmniej 1000 ustalonych seedów, granice, brak nakładania spawnów, osiągalność logiczna, znana trasa rozwiązania |
| Run i moduły | Każdy moduł osobno, potem istotne pary; nielegalny wybór nagrody, przenoszenie zasobów, śmierć i reset |
| Zapis | Round-trip, ucięcie, zła wersja, złe zakresy, przerwanie zapisu, poprawny backup, niedostępny katalog |
| Achievementy | Każdy predykat pozytywnie i negatywnie, reset runu, deduplikacja, trwałość, ponowna synchronizacja offline |
| Zasoby | Hash, licencja, zgoda i non-AI oddzielnie; plik poza rejestrem oraz placeholder blokują publiczną paczkę |
| Windows | DPI, resize, utrata urządzenia, focus, klawiatura, zmiana głośności, brak audio i kontrolowane zamknięcie |

Osiągalność w grafie nie oznacza wygrywalności przy zagrożeniach i limicie tur. Generator potrzebuje ograniczonego solvera lub certyfikatu sekwencji legalnych akcji oraz osobnego raportu trudności. Nie odrzucaj wszystkich ciekawych seedów tylko dlatego, że prosty bot przegrywa.

## Częstotliwość

W każdym kroku: walidator projektu, testy zmienianego zachowania, wymagane testy integracyjne, `git diff --check`. Od wdrożenia CI: build i CTest Debug/Release na Windows. Brak kodu C++ w zmianie dokumentacyjnej nie wymaga pozorowanych testów gry.

Na bramce core: pełny zestaw, testy seedów, replayów i memory safety w obsługiwanej konfiguracji. Na MVP: prawdziwe uruchomienie Windows, pełny run i udokumentowana ocena czytelności. Na release: czysta maszyna lub VM bez narzędzi deweloperskich, offline, klient Steam zamknięty/otwarty, katalog użytkownika z polskimi znakami, standardowe konto bez administratora.

## Kodek checkpointu

P037-01..04: `application_run_checkpoint` chroni pełną projekcję początku sektora,
`application_checkpoint_encode/decode` sprawdzają niezależne literalne wektory,
a `application_checkpoint_integration` obejmuje 11 000 granic dla 1000 ustalonych
root seedów (1000 świeżych startów i 10 000 przejść z jawnymi fixture zasobów).
Pełne stany po restore i pierwszej turze są równe oryginałom; impuls nie pobiera
długu ponownie. Nie jest to 1000 pełnych runów ani test zapisu plików.
Korpus odrzuca wszystkie ucięcia 0..39, nadmiar 1..64, 1280 zmian pojedynczego bitu
i 40 800 zmian pojedynczego bajtu bez odnowienia checksumy. Dodatkowo 32 768 zmian
pól z odnowioną checksumą i 65 536 par slotów sprawdza semantykę i kanoniczność.
Raport P037-04 opisuje wymuszony Rebuild z celowo pominiętą walidacją reserved.

## Adapter checkpointu Windows

P038-01: `windows_checkpoint_read` sprawdza rzeczywiste pliki40 bajtów,
literalne wektory i pełne restore, ucięcia, nadmiar do1MiB, uszkodzenie,
brak, katalog i blokadę uchwytu. Odczyt nie zmienia plików.

P038-02: `windows_checkpoint_stage` sprawdza dokładne bajty staging, retry,
resztki poprzedniego staging, odmowę złej wartości, blokadę i brak katalogu.
Istniejące main i bak pozostają niezmienione.

P038-03: `windows_checkpoint_save` sprawdza publikację i kolejne backupy,
niepoprawny/niedostępny main, blokady każdej ścieżki oraz retry. Zły staging,
ta sama ścieżka i publikacja do innego katalogu są odrzucane.

P038-04: `windows_checkpoint_interrupt` uruchamia rzeczywisty save w osobnym
procesie i kończy go przez TerminateProcess po sygnale zdarzenia. Testowe
przechwycenie wywołań Win32 w jednej jednostce kompilacji pozwala zatrzymać
proces po17 zapisanych bajtach, po flush40 bajtów oraz po rename bak i main.
Cztery literalne wektory dają16 przerwań na uruchomienie. Dokładne main/bak/temp,
pełny RunState po restore i retry są sprawdzane w ścieżce z polskimi znakami.
Testuje też brak katalogu i katalog blokujący temp. Mutacja zapisu bezpośrednio
do main jest wykrywana po pierwszym przerwaniu. Szczegóły w raporcie P038-04.
To test zakończenia procesu między wywołaniami API; nie zaniku zasilania,
awarii nośnika, wnętrza syscall ani konkurencyjnych zapisów.

P039: `windows_checkpoint_recover` sprawdza 4 wektory ×16 kombinacji main/bak
(poprawny, brak, invalid, blokada uchwytu), każdą dwukrotnie. Potwierdza
pierwszeństwo main, fallback tylko po braku/invalid, źródło i dokładne kody
błędów obu prób. Porównuje pełny restore, także opłacony impuls i dług,
oraz dokładne bajty main/bak i obu ignorowanych temp. Sprawdza brak rodzica
i nazwę ze spacją oraz polskimi znakami. Nie testuje naprawy ani integracji UI.

## Profil lokalny i zdarzenia

P040-01..03: `application_profile_model/encode/decode` chronią profil v1.
Cztery niezależne literalne wektory, wszystkie 25 856 poprawne wartości,
wszystkie ucięcia 0..23 i nadmiary 1..64, każda zmiana jednego bajtu oraz
wszystkie 256 wartości każdego pola z odnowieniem checksumy. Kontrolują
nieznane bity, wersję, zakresy, reserved i kanoniczne ponowne kodowanie.
Nie są testem zapisu plików ani trwałego odblokowania predykatu.

P040-04/05: `windows_profile_read/save/interrupt` sprawdzają rzeczywiste
pliki, dokładne bajty, Unicode, ograniczenie długości, błędy uchwytów, retry
i resztki staging. Przerwania rzeczywistego save w procesie potomnym obejmują
4 wektory ×4 fazy: po9 bajtach, po flush24 bajtów, po rename backupu i main.
Sprawdzają zachowanie poprawnych main/bak i późniejszy retry.
To zakończenie procesu między wywołaniami API, bez dowodu zaniku zasilania.

P040-06: `windows_profile_independence` uruchamia 12 osobnych procesów:
dla każdego z4 wektorów writer, reader przed i reader po usunięciu runu.
Po usunięciu dokładnie4 nazw checkpointu profil/main/bak i oba pozostawione
staging zachowują wszystkie bajty. Test używa katalogu Unicode i pełnych
wartości profilu; nie dodaje produkcyjnej operacji usuwania ani sesji UI.

P041-01/02: `application_domain_event/run_turn_events` chronią stabilne ID,
wartości payload i kluczy oraz niezależność kopii. Testy każdej komendy,
odmów i terminalnych wejść porównują pełny stan z core i preview przy trzech
powtórzeniach. Wszystkie3 sektory obejmują dostawę i sukces w turze48,
dostawę z przeciążeniem oraz śmierć po dostawie na wyjściu. Kolejność,
payload i ciągłe ordinal są sprawdzane jawnie. Nie zastępuje to korpusu
pełnych sekwencji P041-03 ani testów trwałego dispatchu P044.

P041-03 rozszerza `core_run_integration`: cztery root seedy i obie kolejności
modułów, dwukrotnie pełny run. Każda granica przechodzi capture/encode/decode/
restore, a następnie dwa odtworzenia wszystkich komend z porównaniem każdego
pełnego stanu i listy kluczy/payloadów. Obejmuje opłacony impuls długu,
śmierć i przeciążenie. Każdy podgląd jest powtarzany i zachowuje wejście oraz
oczekującą listę; typ RunResult nie udostępnia zdarzeń. Bez usług i trwałej
deduplikacji pomiędzy odrębnymi próbami lub gałęziami.

P042-01/02: `application_profile_unlock` sprawdza 32 maski ×256 ID z dwoma
zestawami ustawień, duplikaty i invalid profile. `windows_profile_unlock`
sprawdza pięć kolejnych odblokowań, zachowanie ustawień, dokładną niezmienność
czterech nazw przy duplikacie, blokady main/temp/bak/bak.tmp, odmowę publikacji,
retry, uszkodzony main, brak rodzica i nieznane ID. Restart procesu to P042-03.

P042-03: `windows_profile_unlock_restart` uruchamia 24 osobne procesy dla
wszystkich pięciu ID. Sprawdza dokładne main/bak i pełne ustawienia po nowym
odblokowaniu, duplikacie i ponownym odczycie; błąd corrupt/locked main zachowuje
dane. To restart procesu, bez dowodu zaniku zasilania lub współbieżnych writerów.

P043: `windows_achievement_sink` sprawdza cztery odpowiedzi fake portu,
polimorficzne wywołanie lokalnego adaptera, dwa obiekty korzystające z tego
samego profilu, duplikat, nieznane ID, błąd odczytu i retry. Test jest lokalny,
bez zależności od sieci, klienta Steam albo SDK. Preview nie przyjmuje sinka.

P044-01: `application_event_delivery` chroni odrębne typy tokenów próby/gałęzi,
równość pełnego zakresu, niezerowe granice tokenów oraz wszystkie 65536 masek
kandydatów. Fake portu evaluatora zwraca stałą maskę i zachowuje wejściowe
zdarzenie. Bez predykatów MVP i bez dispatchu.

P044-02: `application_event_dispatcher` chroni jednokrotną ocenę, zapamiętaną
maskę, częściowe potwierdzenie, retry odpowiedzi unavailable/invalid oraz
wyjątku po skutku sinka. Sprawdza konflikty payloadu, błędne zakresy, kształt,
ciągłość tur/ordinal/sektorów, terminalne wejście i invalid maskę evaluatora.
Pełny syntetyczny strumień trzech sektorów jest powtarzany trzykrotnie bez
dodatkowych ocen i wywołań sinka. To kontrakt transportu, bez predykatów MVP.

P044-03: `windows_event_dispatcher` przechodzi trzy pełne runy dla seedów
0/1/MAX, używając istniejącego świadka baseline i stałej maski fake evaluatora.
Każdy event jest powtarzany trzykrotnie. Restore sektora2 porównuje pełne stany
i listy kluczy/payloadów; świeża gałąź i nowa próba o tym samym seedzie są
oceniane ponownie, lecz main/bak pozostają identyczne. Sześć osobnych procesów
odczytuje checkpoint i odtwarza sektor2 z lokalnym sinkiem. Preview sprawdza
pełny stan, liczniki usług i dokładne pliki. Bez implementacji predykatów,
trwałego outboxu, utraty zasilania i podłączenia do sesji UI.

## Ocena grywalności

P053: `core_random_commands` sprawdza 16 384 komendy: 64 stałe seedy (0..62
i MAX), dwie rodziny (bazowa oraz dług 6 z opłaconym impulsem i drugim na
wyjściu), cztery zestawy modułów przez seed modulo 4, po 128 prób.
Naprzemiennie losuje z pełnej listy legalnych i nielegalnych komend; po końcu
sektora sprawdza atomowe odmowy. Każdy rodzaj ma sukcesy i odmowy. Pełne stany
preview i dwóch wykonań muszą być równe, wejście pozostaje identyczne.
Niezmienniki obejmują HP (brak leczenia, najwyżej 1 obrażenie), tury, zasoby,
liczbę portów każdego węzła, monotoniczne dostawy, niezmienne dane sektora,
brak nowych/zdublowanych ID i nakładania impulsów. Błąd wypisuje seed,
rodzinę, zestaw modułów i całą sekwencję prób z celami. Indeksy komend w tym
wewnętrznym reproducerze: move=0, rotate=1, wait=2, overdrive=3. Nie jest to
pełny fuzzing niezaufanych bajtów ani dowód balansu.

P051-01 rozszerza windows_event_dispatcher o pełny run root seedu 1 z prawdziwym
MvpAchievementEvaluator. Wszystkie pięć odblokowań daje maskę 31 i backup 15.
Checkpoint sektora 2 jest zapisany/odczytany przez Windows; sektory 2–3
porównują każdy stan i payload ze śladem oryginalnego runu. Pusty profil
odtworzonej gałęzi dostaje 27 i backup 11, bez domniemania braku overdrive.
Liczniki i dokładne bajty chronią preview, cache duplikatu i świeżą gałąź.
P051-02 uruchamia dwa nowe procesy z rzeczywistym evaluatorem. Każdy odczytuje
checkpoint i porównuje wszystkie stany/payloady sektorów 2–3 z niezależnym
pełnym runem. Pierwszy odblokowuje 27/backup 11 w pustym profilu, drugi zachowuje
dokładne bajty; oba zachowują istniejący profil 31/backup 15. Preview i duplikaty
nie wywołują usług. Procesy fake testu transportu pozostają odrębnym dowodem.

P050: completed_run wymaga won w sektorze 3, bez wymogu znanej historii.
Testy odrzucają death/overload i sektory pośrednie. Jedno końcowe zdarzenie
zgłasza dwa ID; częściowy sukces i retry zachowują potwierdzone bity. Test
Windows publikuje cztery rzeczywiste zdarzenia ostatniej tury i sprawdza
trwałe bity 1/2/5 oraz dokładną niezmienność profilu i backupu przy powtórce.

P049: wszystkie 49 celów z rotate i overdrive, inny cel dostawy, obrót bez
dowodu, konsumpcja, reset sektora i nowy evaluator. Retry po błędzie sinka
zachowuje kandydata rekonfiguracji mimo skonsumowanego trackera. Test Windows
łączy faktyczne otwarcie trasy i dostawę w tej samej turze z trwałymi bitami 1/4.

P048: testy MVP sprawdzają czysty run i overdrive w każdym z trzech sektorów,
reset przez nową instancję, death/overload oraz nieznaną historię po restore,
również sektora 1. Jawny fresh_run z błędnym początkiem strumienia nie wystarcza.
Trwały profil zachowuje ustawienia i backup przy powtórkach; świeża gałąź
z nieznaną historią nie dopisuje ani nie usuwa odblokowań.

P047 rozszerza testy MVP o first_sector: trzy sektory, tury 1 i 48, śmierć
na wyjściu, brak drugiej dostawy i pozostanie poza wyjściem. Rzeczywiste
zdarzenia sukcesu zapisują bity dostawy/sektora i zachowują dokładne pliki
przy powtórce w nowej gałęzi.

P046: application_mvp_achievements sprawdza first_delivery dla każdego celu
i sektora oraz retry i duplikat. windows_mvp_achievements publikuje rzeczywiste
zdarzenia dostawy przed śmiercią do lokalnego profilu, sprawdza blokadę odczytu,
retry, ustawienia, dokładny backup i niezmienność plików przy świeżej gałęzi.

P045-01/02: application_domain_event sprawdza równość i konflikt dowodu
rekonfiguracji oraz odmowę dowodu dla wait. application_route_evidence chroni
otwarcie trasy, zmianę wybranej trasy BFS, wprowadzenie nowego pakietu,
overdrive, symetrię, obrót niezwiązanej gałęzi, brak pakietu, pakiet na celu,
zamknięcie, odmowę oraz powtarzalność po checkpoint round-trip. Pełny stan
pozostaje zgodny z core i preview, wejście nie jest mutowane.

Automatyczne boty mierzą dominujące strategie, niewygrywalne stany i liczbę używanych akcji. Nie zastępują playtestu. Agent może wykonać obserwację interfejsu, jeśli ma odpowiednie narzędzia, i opisać jej ograniczenia. Nie zmyślaj opinii graczy ani badań. Zewnętrzni testerzy nie podejmują decyzji za autora wizji; ich dowody mogą uzasadniać samodzielną poprawkę balansu. Kontakt z testerami wymaga osobnej autoryzacji.

Każdy zgłoszony problem otrzymuje małe zadanie odtwarzania i osobno małą poprawkę, jeśli połączenie byłoby zbyt duże. Testy screenshotowe wolno opierać tylko na zatwierdzonych assetach; ich wzorce i raporty nie są dowodem non-AI.
