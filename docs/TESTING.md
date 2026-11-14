# Testy i bramki jakości

P084-01: pełny run sześciu sektorów, preview/zdarzenia, kolejka achievementów,
nagrody przy pełnym loadoucie, rzeczywiste HWND i wznowienie w nowym procesie.
Checkpoint corpus obejmuje 26000 pełnych granic i pierwszą turę po restore.
Literalne maski sektorów4..6 dla czterech root seedów wylicza niezależny
tools/corpus/topology_vectors.cjs. Stare reguły1/2 nie uszkadzają backupu3.

P084-03..07: korpus 257 seedów topologii/buildera i certyfikatów finału.
developer_final_witness potwierdza trzy dostawy i wyjście w maksymalnie
31 zaakceptowanych akcjach, chroniąc pełny stan i odmowy certyfikatów.
core_sector_outcome obejmuje oba progi, śmierć, wyjście i granice 48/49.
core_run_transition i application_run_checkpoint sprawdzają normal/final,
zasoby oraz opłacony impuls. windows_checkpoint_recover sprawdza stare
reguły z poprawną checksumą oraz zachowanie dokładnych bajtów backupu.
Pełny run i HWND korzystają z właściwego wariantu; nie jest to playtest.

P080 (zmiana kryteriów zatwierdzona przez użytkownika 2026-11-14): techniczny
odbiór nie wymaga Computer Use. `presentation_atlas_tiles` sprawdza niepuste
i parami różne maski alfa wszystkich ośmiu symboli, całkowicie ignorując RGB.
`windows_scene_renderer` chroni rozłączne pozycje i piksele zatwierdzonego
atlasu. `windows_text_renderer` sprawdza 96/144/192 DPI, pełny blok tekstu
i odmowę rysowania przy braku miejsca. `windows_hud_view` sprawdza resize,
49 pól hit-testu i live/preview/cancel/focus/commit. Testy session/reward/
achievement/resume/rotation_binding_window sprawdzają odpowiadające stanom
teksty i przejścia. Pełny CTest musi przejść w Debug i Release.
To dowody techniczne; różne maski nie dowodzą łatwego rozpoznawania symboli.
Obserwowana macierz DPI/resize i percepcyjna czytelność są niewykonane,
odroczone do P104 przed wydaniem. Nie blokują P081 ani dalszego developmentu.
P081 nadal wymaga dowodu zintegrowanego runu z usługami, nie samego bota core.

P079-06: windows_rotation_binding_restart uruchamia cztery nowe procesy
na jednym prywatnym katalogu. Pierwszy ustawia T przez F2 bez tury, drugi
wczytuje T i wykonuje jeden obrót mimo hold/repeat (R nie działa), trzeci
odtwarza rzeczywistą odmowę rename, ostatni ignoruje pozostały staging Y
i ponownie używa T. Konflikt O i focus nie zmieniają zapisu. Rodzic po
każdym procesie porównuje dokładne 16 bajtów ustawienia oraz osiem nazw
main/bak/tmp/bak.tmp profilu i checkpointu. Timeout kończy tylko własne
procesy potomne. Nie dotyka ustawień użytkownika w LocalAppData.

P079-05: windows_rotation_binding_startup sprawdza missing bez tworzenia,
trzy odczyty T, wyłączną blokadę, korupcję przy poprawnym staging, błędną
lokalizację i brak katalogu. Dokładne bajty ustawienia i sąsiednich plików
są chronione. windows_window_process obejmuje produkcyjny bootstrap.

P079-04: windows_rotation_binding_window przez rzeczywiste HWND sprawdza
anulowanie preview, F2, konflikt O i Space, błąd i wyjątek portu, hold/repeat,
jawny retry, dokładny zapis T, brak starego R, Escape i utratę focusu.
Podczas konfiguracji pełny RunState i widok pozostają live, a dopiero
późniejsze T wykonuje jedną rzeczywistą turę obrotu. Bajty profilu oraz
checkpointu są niezmienne. Brak katalogu odmawia UI z portem ustawienia.

P079-03: windows_rotation_binding_input sprawdza wszystkie 25 przypisań,
brak starego R, zachowanie O, przechwycenie 100 powtórek, zmianę podczas
hold, release/repress, pending przy utracie focusu i bit30 po odzyskaniu.
Istniejący windows_window_keyboard nadal chroni domyślne klawisze i brak
kolejki. Testy korzystają z rzeczywistych HWND, bez zapisu plików.

P079-02: windows_rotation_binding_file używa rzeczywistych plików w Unicode
katalogu. Sprawdza missing, save/read i dokładne bajty, no-op przy zajętym
staging, blokadę odczytu oraz blokadę rename z pozostawionym staging i retry,
corrupt/truncated/oversized main, względną ścieżkę i brak katalogu. Profil
oraz checkpoint zachowują dokładne oryginalne bajty.

P079-01: application_rotation_binding obejmuje 256 bajtów litery, konflikt
O, niezależny literalny wektor R, round-trip każdej poprawnej wartości,
16 ucięć, nadmiar i 128 pojedynczych zmian bitu. Zmiany magic/version/key/
reserved z ponownie policzoną checksumą także są odrzucane. Bez IO.

P078-06: windows_audio_delivery_window sprawdza prawdziwe HWND, preview,
odmowę, hold/repeat i focus, pojedynczą dostawę oraz retry achievementu
w czterech trybach portu (played/muted/unavailable/wyjątek). Każdy tryb
porównuje pełne RunState i planszę oraz dokładne bajty checkpointu,
profilu i backupu. Wyjątek audio nie ustawia błędu sesji.

P078-05: windows_xaudio2_output przez AudioOutput sprawdza unknown,
mute/unmute, dwa submits i jeden Start, brak WAV oraz błędy silnika,
mastering, submit, start i callback. Błędy nie ponawiają operacji;
fake weryfikuje oryginalne bajty oraz lifetime. SilentAudioOutput respektuje
mute. Dotychczasowy windows_audio_window chroni ścieżkę bez pliku;
awarie urządzenia obejmuje osobny test portu z zatwierdzonym WAV.

## Obecnie

`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1` kontroluje spójność dokumentacji, kolejki i pustych rejestrów. `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Test-ProjectValidator.ps1` sprawdza 11 przypadków, w tym uszkodzony hash i brak zgody, na izolowanych kopiach. Syntetyczne rekordy testowe nie są prawdziwymi zgodami ani dowodami autorstwa.

`core_build_smoke` sprawdza kompilację C++20 i wykonanie kodu korzystającego z `std::span` i ranges. `core_identifier_ranges` sprawdza granice i porządek wartości `NodeId` oraz `EntityId`. Kompilacja testu weryfikuje także brak niedozwolonych konwersji. `core_port_rotation` sprawdza obrót o 90° dla każdej maski, powrót po czterech obrotach, zachowanie liczby portów i odrzucanie wartości spoza zakresu. `core_grid_adjacency` sprawdza wzajemność połączeń, granice siatki i brak zawijania między wierszami. Uruchamianie Debug/Release opisuje [README](../README.md). [Raport P002](workflow/history/P002.md) dokumentuje celowy błąd i niezerowe kody runnera oraz CTest w obu konfiguracjach.

## C++ i zakres testów

P062-01: windows_native_window tworzy rzeczywiste ukryte HWND; cztery
powtórzenia dwóch współistniejących okien chronią dokładny tytuł Unicode,
niezależne klasy, IsWindow/IsWindowVisible oraz wyrejestrowanie po destrukcji.
WM_CLOSE przed destruktorem czyści uchwyt. Nieznane ID i invalid UTF-8 dają
brak obiektu oraz właściwy kod; statycznie brak copy/move. To lokalny test
API z timeoutem 20 s, bez widocznego okna, focus, pętli i VM.

P061-03: application_text_catalog sprawdza faktyczny build_text_audience
nadany przez CMake; public nie przyjmuje testowego placeholdera, developer go
przyjmuje. Test-PublicContent.ps1 -BuildModes sprawdza 9 prób rejestru i 4
wyniki rzeczywistego builda konsumenta game_core w izolowanym repo.
Presety windows-public-debug/release uruchamiają tę samą pełną pulę CTest.

P061-02: Test-PublicContent.ps1 sprawdza 9 wyników procesu na izolowanym
checkout: pusty/public, marked/developer, marked/public, approved fixture,
mixed, nieznane źródło/status, marker ze statusem approved i przywrócenie.
Validate-PublicContent.ps1 uruchamia najpierw bramkę assetów i odrzuca
placeholdery bez zmiany rzeczywistych treści. Test approved jest syntetyczny,
bez dowodu autorstwa; to próba walidacji pól.

P061-01 rozszerza application_text_catalog o macierz dwóch TextAudience:
pusty i approved-only katalog, placeholder-only i mixed lista, brak źródła
oraz unknown audience. Public odrzuca całą listę z placeholderem, developer
przyjmuje tylko poprawnie oznaczone; dane wejściowe i lookup pozostają chronione.

P060: application_text_catalog sprawdza wartości, niezależne kopie, pusty
katalog, brak fallbacku ID, dokładny prefiks i brak źródła placeholdera,
źródło approved i zakaz markera, invalid status/ID/NUL, limit 4096/4097,
duplikaty i limit 256/257. Syntetyczny approved fixture pozostaje wyłącznie
w niewyświetlanym teście; nie jest zatwierdzonym tekstem gry.

P059: `scripts/Validate-AssetPackage.ps1` jest bramką assets/approved
w dokładnym checkout Git, używaną również w CI. Wymaga poprawnego rejestru,
zgód, hashy i śledzenia plików. Jawne wyliczanie obejmuje hidden i odrzuca
reparse. `scripts/Test-AssetPackage.ps1` sprawdza 7 wyników na własnym repo:
poprawny payload, orphan, hidden orphan, registered/untracked, korupcja,
odmowa zgody i powrót do poprawnego stanu. P101 obejmie finalną allowlistę
całego depotu; P059 nie sprawdza binarium ani autorstwa.

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

P062-02: windows_window_messages sprawdza rzeczywiste SetFocus/null, cztery zamknięcia po opóźnionym WM_CLOSE, wyzerowanie focus/HWND i kontrolowaną odmowę drugiej pętli. Zniszczony uchwyt wymusza rzeczywiste GetMessage=-1; osobny PostQuitMessage(37) chroni kod wyjścia. Timeout 20 s, lokalne API bez VM.

P062-03: windows_window_process sprawdza trzy osobne uruchomienia developer_window, oznaczony tytuł, wysłane komunikaty focus i zamknięcie z kodem0. Timeout zabija wyłącznie własny proces potomny. Rzeczywisty public gate oraz build public odmawiają obecnego tytułu. CI public usuwa tytuł wyłącznie w checkout runnera po sprawdzeniu odmowy, aby testować infrastrukturę bez developer_window; to nie paczka produktu.

P063-01: application_keyboard_input sprawdza każdy klawisz, 100 prób hold/repeat, release/repress, powtórny focus=true, utratę/odzyskanie focus, niezależność klawiszy i unknown. Bez symulacji i Windows.

P063-02: application_keyboard_commands sprawdza wszystkie 49 pól i cztery kierunki przez niezależne współrzędne row/column, pełne payloady rotate/overdrive/wait, unknown oraz odmowę rzeczywistego resolvera dla geometrycznego sąsiada bez portów.

P063-03: windows_window_keyboard wysyła rzeczywiste WM_KEYDOWN/UP, sprawdza siedem komend, 100 powtórzeń każdego klawisza, bit30 i brak bitu przy hold, keyup/repress, focus loss z pending i odzyskanie focus, unknown, brak kolejki, zamknięcie oraz konsumpcję akcji na krawędzi. Brak dostępu wejścia do GameState.

P064-01: windows_direct2d_target tworzy rzeczywiste fabryki i cele HWND dla dwóch ukrytych okien, czterokrotnie. Kontroluje GetHwnd/GetPixelSize, niezależność celów i zachowanie drugiego po zwolnieniu pierwszego. Null i zniszczony HWND są odrzucane bez obiektu. Brak draw/present, fizycznego resetu GPU i VM.

P064-02: windows_direct2d_resize czterokrotnie sprawdza rzeczywiste rozmiary 1x1, 321x197 i 624x441, trzy warianty zera i powrót, zachowanie fabryki oraz nową tożsamość COM celu po kontrolowanym D2DERR_RECREATE_TARGET. Rzeczywisty SetWindowPos i GetClientRect zasilają Resize; zamknięty HWND odmawia dwukrotnie. S_OK/E_FAIL przechodzą bez zmiany celu. Brak fizycznego resetu GPU, draw/present i VM.

P065: windows_png_read porównuje 832x373/stride3328/1241344 bajtów i SHA-256 pełnego PBGRA z niezależnym GDI+. Sprawdza COM-uninitialized, rzeczywisty manifest, trzy powtórki w Unicode root, niezależność bufora, korupcję przed hashem i kodekiem, brak, retry, non-PNG oraz niezmienność oryginału/rejestru. Nie tworzy artworku ani nowych assetów.

P066-01: presentation_atlas_tiles sprawdza literalne mapowanie wszystkich 16 masek i porty na krawędziach rzeczywistego PNG. Rotuje indeksy krawędzi niezależnie od helpera produkcyjnego. Kontroluje granice komórek i obrotów; nie tworzy grafiki.

P066-02: presentation_board_projection sprawdza wszystkie 49 pól i 16 masek w pięciu viewportach, w tym 1x1. Kontroluje centrowanie, brak zawijania, odmowę zero/negative/NaN/infinity/overflow i pełną niezmienność stanu.

P066-03: windows_atlas_renderer porównuje wszystkie 65 536 kanałów obrazu
offscreen 128x128 z pikselami atlasu, z obrotami i skalą 2x. Chroni transform
callera, odmowę całej błędnej listy bez zmiany klatki, brak pliku, korupcję,
niezgodny manifest i retry. Oryginalne assety pozostają niezmienione.

P066-04: windows_board_view wykonuje trzy resize oraz po dwa odmalowania
przez prawdziwy WM_PAINT. Okno jest widoczne poza pulpitem, bez aktywacji;
ukryte HWND nie otrzymuje UpdateWindow. Symulowany D2DERR_RECREATE_TARGET
powoduje prawdziwe odtworzenie celu i bitmapy. Brak assetu odmawia utworzenia
widoku. Odłączenie callbacka i pełna niezmienność wejściowego stanu są
sprawdzane. windows_window_process nadal sprawdza trzy nowe procesy produktu.

P067-01: presentation_unit_projection sprawdza wspólną pozycję trzech typów
jednostek na wszystkich 49 polach, odrębne narożniki, brak martwego gracza
i nieobecnego pakietu, role pól, zasilenie i stabilny porządek ID impulsów.
Rzeczywista tura z dostawą i kontaktem usuwa symbole pakietu/impulsu i zmienia
symbol odbiornika. Test PNG sprawdza niepuste komórki wszystkich ośmi symboli.

P067-02: windows_scene_renderer sprawdza rozłączne prostokąty kafli i symboli,
pełny stan oraz 802 816 kanałów klatki offscreen 448x448 względem niezależnej
całkowitoliczbowej kompozycji oryginalnych pikseli. BoardView przyjmuje pełny
nowy stan i odmalowuje tę samą projekcję. Pulpit nie został obejrzany:
oba dostępne interfejsy Computer Use zgłosiły niedostępny native pipe.

P067-03: windows_run_window wysyła prawdziwe komunikaty klawiatury dla
wszystkich siedmiu klawiszy i unknown. Każdą próbę porównuje z pełnym
resolve_run_turn i stanem widoku. Obejmuje ruch, obrót, overdrive, wait,
odmowy portów/granicy, dostawę i kontakt, 20 powtórek każdego naciśnięcia,
focus loss/recovery, limit 48 tur i terminalne odmowy. Repaint po akcjach
używa rzeczywistego celu HWND poza pulpitem. Brak nowych usług i reguł.

P072-01..04 oraz P073-01..05: pełna pula 125/125 w Debug i Release. Nowe testy chronią oferty 0/1/2, markery katalogu, overlay i brak hit-testu, obie granice sektorów, pełne stany każdej akcji, trwałe potwierdzenie first_delivery, kolejkę retry/wyjątki, known folder i niezmienione bajty istniejącego profilu. Testy HWND i wspólnego renderera DPI nie zastępują obserwacji pulpitu lub VM.

P074-01: `windows_checkpoint_location` sprawdza zapytanie o rzeczywisty LocalAppData bez tworzenia katalogu użytkownika, `ClosedCircuit/checkpoint.bin` we wspólnym katalogu z profilem (w obu kolejnościach), Unicode, powtórki bez zmiany bajtów obu plików, kolizję z plikiem i odmowę pustej, względnej lub brakującej bazy.

P074-02: `windows_checkpoint_resume` sprawdza klasyfikację wszystkich czterech wektorów jako resumed z main i backupu (pełny RunState równy restore, tura 0), none/corrupt/unavailable dla kombinacji błędów oraz niepoprawną wartość jako corrupt. Rzeczywiste pliki: brak, main, backup po uszkodzonym main, uszkodzenie bez backupu, wyłączna blokada main i brak katalogu; dwa odczyty i niezmienione nazwy oraz bajty katalogu. Timeout 30 s.

P074-03: `presentation_checkpoint_text` sprawdza brak wiersza dla none/resumed i braku błędu zapisu, dokładne wartości lokalnego katalogu, odmowę pustego katalogu, pełny marker bez cyfr i backslasha w rzeczywistym rejestrze developer, nieznane wartości statusu oraz odmowę katalogu public.

P074-04: `application_checkpoint_boundary` dla seedów 0/1/2/MAX sprawdza zapis startu i początków sektorów 2–3 równy capture_checkpoint (z opłaconym impulsem długu), keep po turze, przy wyborze nagrody i ready_for_next, clear dla won/death/overload, keep dla zmienionego pola w turze 0 oraz dwukrotną decyzję bez mutacji wejścia. Mutacja usuwająca clear dla lost kończy test kodem 8.

P074-05: `windows_checkpoint_store` używa portu CheckpointStore na rzeczywistych plikach: clear bez plików, zapis i backup, dwukrotne clear, blokada backupu zatrzymuje clear przed main, blokada main zatrzymuje clear i save, osierocony backup, uszkodzony main bez nadpisania, odmowa niepoprawnej wartości, pozostawiony staging i brak katalogu. Mutacja odwracająca kolejność usuwania kończy test kodem 1. Timeout 30 s.

P074-06: `windows_checkpoint_window` (rzeczywisty HWND) sprawdza brak zapisu przy tworzeniu, dokładne bajty encode(capture) po Enter na starcie, brak zapisu po preview i zwykłych turach, usunięcie main/bak po wyniku, zapis po restarcie, błąd i wyjątek magazynu z oznaczonym wierszem przy resize, bez E_FAIL i blokady gry, ustąpienie wiersza po udanym clear oraz przejście sektora z nagrodą: zapis początku sektora2 albo niezmieniony plik przy błędzie. Odmowa store bez katalogu. Mutacja bez zapisu po przejściu kończy test etapem transition. Timeout 60 s.

P074-07: `windows_resume_window` symuluje kolejne uruchomienia okna z jednym plikiem: brak zapisu -> start, Enter zapisuje, dwie tury nie są zapisywane; trzy ponowne uruchomienia wznawiają pełny początek sektora (stan, plansza, tura) bez zmiany plików; sektor3 z opłaconym impulsem wznawia się dokładnie. Uszkodzony main pokazuje oznaczony wiersz na starcie, Enter zastępuje go wierszem błędu zapisu bez nadpisania pliku; wyłączna blokada daje wiersz unavailable, po zwolnieniu plik znów się wznawia. Każdy wiersz etykiety zaczyna się od markera. Katalog bez wpisu odmawia corrupt, nieznany status daje E_INVALIDARG. Mutacja bez statusu kończy test etapem corrupt launch. Smoke `windows_window_process` uruchamia exe odczytujące rzeczywisty LocalAppData tylko do odczytu; katalog ClosedCircuit pozostał pusty.

P075-01: `application_audio_output` sprawdza wszystkie 256 surowych wartości sygnału trzykrotnie: cicha implementacja zwraca unavailable dla dostawy i unknown dla pozostałych, bezpośrednio i przez port (noexcept). Fake przez port zwraca played/unknown/muted i zachowuje kolejność wywołań. Bez próbek audio i plików. Mutacja zwracająca played z cichej implementacji kończy test kodem 2.

P075-04: windows_xaudio2_device sprawdza kolejność engine/mastering/destroy/release,
błędy obu etapów, S_OK z pustym uchwytem i częściowe zasoby po odmowie.
Trzy rzeczywiste utworzenia XAudio2 i mastering voice dają S_OK i available.
Bez próbek audio, source voice i generowania nawet cichych buforów.

P075-05: windows_xaudio2_recovery wywołuje zarejestrowany callback z osobnego
wątku i sprawdza zachowanie pierwszego błędu, brak operacji API w callbacku,
trzy odtworzenia, błędy engine/register/mastering oraz callback przy rejestracji
i wyrejestrowaniu. Sprawdza unregister przed destroy/release, brak callbacka
po zwolnieniu i brak niejawnego retry. Fizycznego odłączenia nie testowano.

P075-03: windows_audio_window sprawdza port dla wszystkich 256 wartości
oraz prawdziwe HWND i pliki checkpointu w czterech trybach: urządzenie fake
dostępne, błąd silnika, brak urządzenia wyjściowego, krytyczny błąd.
Chroni pełny stan tury/preview, focus, resize, zapis startu i jego niezmienne
bajty po turze, brak prób odtwarzania i zwolnienie callbacka z sesją.

P077: windows_asset_manifest_integration obejmuje również czwarty wpis,
comigo4947244-powerup01; pełne oryginalne bajty, CRC po uszkodzeniu i brak
pliku sprawdzane na kopiach. Oryginalnego WAV nie modyfikuje się w testach.
Test-AssetPackage.ps1 zalicza 7 przypadków (w tym korupcja i odmowa zgody).

P078-01: application_pcm_wave sprawdza prawdziwy zatwierdzony WAV (dokładny
wskaźnik i 71792 bajty danych), wszystkie 71836 ucięć, błędne sygnatury
i pola, stereo/rate 8000/192000, granice rate, fmt18, przestawienie chunków,
duplikaty, odd padding, 256/257 chunków i 1 MiB/+8. Zmienia wyłącznie
metadane kopii oryginału. Nic nie odtwarza i nie generuje danych audio.

P078-02: windows_approved_wave porównuje pełne próbki oryginału po load,
copy, move i przypisaniach. Trzykrotnie sprawdza realne pliki: brak, blokada
wyłączna, błędny nagłówek/hash, zła długość i usunięcie, bez mutacji approved.
Wcześniej wczytany właściciel zachowuje dane. Test rzeczywistego manifestu
porównuje przypięte ID, ścieżkę, rozmiar i digest z rekordem rejestru.

P078-03: windows_xaudio2_playback sprawdza trzy submits i jeden Start,
pełne oryginalne bajty i pola bufora, błąd krytyczny z innego wątku,
odtworzenie z aktywną kolejką, siedem błędów tworzenia/submit/start,
częściowy uchwyt i pusty sukces, brak retry i moved-from właściciela.
Przy DestroyVoice dane są wciąż żywe, a master/engine wymagają zera source.
Rzeczywisty XAudio2 przyjął bufor i opróżnił kolejkę bez Stop/Flush/Destroy;
dodatkowy submit sprawdza bezpieczną destrukcję w trakcie odtwarzania.
SamplesPlayed resetuje się po END_OF_STREAM, więc kryterium to obserwacja
pełnej kolejki przed Start oraz pustej po zakończeniu; peak jest diagnostyką.

P081-01: windows_mvp_window wykonuje trzy sektory od świeżego startu przez HWND z checkpointem i profilem. Pełny stan i plansza po każdej komendzie, preview/odmowa bez zapisu, trzy granice i won z kontrolą plików, profil 31. Bez obserwacji i playtestu.

P081-02: windows_mvp_restart porównuje pliki granicy sektora 2 z niezależnym przebiegiem HWND. Trzy nowe procesy zapisują, wznawiają i odczytują prywatny run. Pełna zgodność kontynuacji z core, odczyt bez zapisu, clear po won i profil 27/backup 11 bez nagrody za nieznaną historię.

P082-02: core_periodic_emitter sprawdza walidację pełnych liczb przed zawężeniem, wszystkie harmonogramy period 2–48, literalną tabelę faz i niezależny wynik uint64 przy UINT32_MAX. Bez zmian GameState i resolvera.

P082-03: core_emitter_footprint sprawdza dokładne zbiory pól, cykle, rozgałęzienia, jednostronne porty, rotację i brak zawijania 6/7. Pełna siatka: 49 źródeł × 48 zasięgów × 49 niezależnych dystansów. Bez obrażeń i resolvera.

P082-04: core_emitter_phase obejmuje 3136 przypadków pozycji/fazy/HP/limitu, pustą listę, obrót, krótki zasięg i sześć kolejności trzech emiterów. Chroni pełny pozostały stan i konfiguracje. Kontakt impulsu przed/po helperze współdzieli limit. Integracja resolvera pozostaje w P082-06.

P082-05: core_emitter_state chroni każde pole, kolejność i kopie kolekcji oraz niezależny wektor hash v5. application_run_checkpoint odrzuca emiter na granicach sektora. Stare wersje replay odrzucane, literalne ślady CLI uaktualnione niezależnym byte oracle tools/replay/hash_vectors.cjs.

P082-06: core_emitter_turn porównuje pełne preview/commit i stan źródłowy po odłączeniu i połączeniu obrotem, przy impulsach, dostawie zakończonej śmiercią, odmowach i trzech stanach terminalnych. Jawna sześcioturowa sekwencja chroni harmonogram, koszt 1 i deterministyczne hashe; istniejący replay/CLI chroni fixture bez emiterów. Sprawdzony zewnętrzny wykorzystany limit helpera impulsów.

P083-02: core_delayed_capacitor sprawdza wszystkie 2304 kombinacje delay/range, po 96 ticków każda, granice pełnych uint64 przed zawężeniem, jeden sygnał i brak underflow, pola ID/pozycji oraz dwie niezależne kopie. constexpr i brak domyślnego konstruktora weryfikowane przy kompilacji.

P083-03: core_capacitor_footprint sprawdza literalne zbiory pól, cykl, rozgałęzienie, jednostronne porty, granicę 6/7 i rotację między tickami oraz 49×48×49 niezależnych dystansów pełnej siatki. Pending/spent mają identyczną geometrię. Istniejące core_emitter_footprint i core_emitter_turn chronią wyodrębniony wspólny BFS.

P083-04: core_capacitor_phase obejmuje 42336 kombinacji pozycji/HP/limitu/delay/progress/range, pustą listę, sześć kolejności trzech kondensatorów i powtórzenia po spent. Rzeczywisty helper impulsów przed/po fazie współdzieli limit. Obrót między tickami zapobiega trafieniu i późniejsze połączenie nie odnawia wybuchu. Pełny GameState i konfiguracja poza remaining chronione.

P083-05: core_capacitor_state izoluje zmianę każdego pola, również delay przy stałym remaining, sprawdza kopie, liczbę, porządek i literalne hashe armed/pending/spent. application_run_checkpoint odrzuca armed/spent na każdej badanej granicy sektora; restore ma pustą kolekcję. Pełne wektory i CLI zaktualizowano niezależnym byte oracle, stare wersje replay odrzucone.

P083-06: core_capacitor_turn chroni pełne preview/commit i niezależne literalne hashe sześciu komend (w tym odmowę i overdrive), odłączenie/połączenie obrotem między tickami i przy wybuchu, ruch przed trafieniem, wszystkie kombinacje impuls/emiter z dwoma wybuchami, dostawę przed śmiercią, trzy stany terminalne bez ticka i expiry w turze 48. Istniejący replay/CLI obejmuje fixture bez kondensatorów.
