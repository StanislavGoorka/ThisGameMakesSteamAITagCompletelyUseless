# Dispatch zdarzeń v1

P044-01 wdraża typy i ten kontrakt. P044-02 wdraża EventDispatcher, a P044-03
sprawdza restore/restart. Predykaty pięciu achievementów definiuje
[P045](ACHIEVEMENT_PREDICATES.md). P046 dodaje MvpAchievementEvaluator:
first_delivery dla PacketDelivered, a P047 dodaje first_sector wyłącznie dla
SectorCompleted. Oba predykaty są bez trackera i IO; koniec sektora pochodzi
z resolvera po obrażeniach i koszcie, więc sama pozycja na wyjściu nie wystarcza.

P048: RunHistoryOrigin domyślnie oznacza restored_checkpoint. Wyłącznie jawny
fresh_run z pierwszą akcją tury 1 sektora 1 pozwala śledzić run bez overdrive.
Każda zaakceptowana akcja overdrive wyklucza ten warunek do końca instancji,
także przez granice sektorów. Kandydat powstaje dopiero na RunEnded(won)
w sektorze 3. Nowa próba/gałąź wymaga nowego evaluatora; nie ma resetu w miejscu.

P049 dodaje stały zbiór 49 celów o dowiedzionej rekonfiguracji. Cel ustawia
wyłącznie dowód z ActionCommitted; PacketDelivered konsumuje bit i zgłasza
równocześnie first_delivery oraz reconfigured_delivery. Inny cel nie konsumuje
dowodu. Zmiana sektora i nowy evaluator zerują zbiór. Nie ma alokacji ani IO.
Retry po konsumpcji dowodu używa maski zapamiętanej przez dispatcher.

P050 zamyka pięć predykatów MVP: RunEnded(won) w sektorze 3 zgłasza completed_run
niezależnie od pochodzenia historii. Z czystą historią zgłasza równocześnie
run_without_overdrive. SectorCompleted sam nie zgłasza ukończenia runu.

P051 jest wyłącznie podziałem planu: P051-01 łączy rzeczywiste predykaty,
trzy generowane sektory, profil i restore w procesie. P051-02 osobno sprawdzi
ponowienie w nowych procesach. Dotychczasowy dowód P044-03 używał stałej maski,
więc nie zastępuje tych kryteriów. P052 czeka na oba dzieci.

Dowód P051-01 dla root seedu 1 przechodzi pełny run przez start, nagrody
i przejścia. Rzeczywisty evaluator zgłasza wszystkie pięć ID (31); profil
zachowuje ustawienia, a backup przed końcowym ID ma maskę 15. Zapis i odczyt
checkpointu sektora 2 daje pełną granicę równą oryginałowi. Odtworzenie
sektorów 2–3 porównuje każdy pełny stan i payload. Z domyślnie nieznaną historią
oraz pustym profilem odblokowuje wyłącznie maskę 27, z backupem 11. Dokładne
duplikaty nie oceniają ponownie; świeża gałąź ocenia ponownie bez zapisu
duplikatów. Preview i obie powtórki zachowują dokładne bajty profili/backupów.
Ten nowy scenariusz nie uruchamia procesu potomnego; taki dowód dla rzeczywistych
predykatów pozostaje w P051-02.

DeliveryScope zawiera seed, silny RunAttemptToken, silny RunBranchToken
oraz first_sector (granicę, od której zaczyna się strumień). Tokeny są
niezerowymi wartościami uint64, przydzielanymi przez przyszłą sesję aplikacji.
Nowy run otrzymuje nowy token próby, nawet przy tym samym seedzie. Każdy restore
otrzymuje nowy token gałęzi i świeży dispatcher oraz evaluator; pierwszy sektor
zakresu jest sektorem odtworzonej granicy. Tokenów nie wolno ponownie używać
w obrębie żyjącej sesji. Nie są trwałym globalnym ID ani częścią checkpointu.
Dispatcher porównuje pełny zakres przy każdym wywołaniu, a seed zdarzenia
musi pasować do zakresu. Nie deduplikuje globalnie samym seedem.

Wywołujący publikuje pełny strumień wyłącznie zaakceptowanych, wybranych tur
z resolve_run_turn_events, po przyjęciu nowego stanu. Preview nie dostaje
dispatchera ani sinka. Kolejność zdarzeń pochodzi z DOMAIN_EVENTS; pierwsze
zdarzenie to akcja tury1 pierwszego sektora zakresu. Kolejne ordinal są ciągłe,
rodzaje rosną w obrębie tury, tury rosną o1. Następny sektor zaczyna turą1
po SectorCompleted; po RunEnded nie przyjmuje się nowych zdarzeń. To kontrola
ciągłości protokołu, nie ponowna walidacja reguł symulacji.

AchievementEvaluator jest portem bez usług i IO. Jedno evaluate otrzymuje
zdarzenie i zwraca uint16 maski kandydatów: bit ID−1, znane wyłącznie 0x1f.
Może aktualizować prywatny tracker, ale nie rzuca, nie alokuje i nie odblokowuje
ID samodzielnie. Świeży evaluator po restore nie zna wcześniejszej historii
akcji; nie wolno domniemywać np. braku wcześniejszego overdrive. P045 musi
ustalić konserwatywną obsługę nieznanej historii przed implementacją predykatów.

Dispatcher zachowuje kopię klucza, payloadu i wyniku evaluate w pamięci.
Przygotowuje miejsce na rekord przed evaluate, aby błąd alokacji nie wykonał
trackera bez zapamiętania wyniku. Dokładny duplikat nie wywołuje evaluate drugi
raz. Ten sam klucz z innym payloadem jest konfliktem bez skutku. Błędny zakres,
kształt lub kolejność zdarzenia są odrzucane przed evaluatorem i sinkiem.
Niepoprawna maska evaluatora jest zapamiętanym błędem kontraktu: brak publikacji,
brak ponownej oceny, zablokowane dalsze zdarzenia tego zakresu.

Kandydaci trafiają do AchievementSink w rosnącej kolejności ID. unlocked oraz
already_unlocked potwierdzają bit; pozostałe odpowiedzi zachowują go do retry.
Wyjątek sinka propaguje się z zachowaniem pending. Retry nie ocenia ponownie
zdarzenia ani nie ponawia potwierdzonych ID. Nowe zdarzenie czeka na zakończenie
pending poprzednika. Całość ma jednego właściciela, działa szeregowo i bez
reentrancji; evaluator i sink muszą żyć dłużej od dispatchera.

Gwarancja jednokrotnej oceny dotyczy tylko żyjącego zakresu. Nie ma trwałego
outboxu, zapisu trackera ani gwarancji dostarczenia zdarzenia utraconego przy
crashu przed zapisem profilu. Po restore/restart zdarzenia mogą być ocenione
ponownie w świeżym zakresie, a lokalny trwały zbiór ID zapobiega powtórnemu
odblokowaniu i zapisowi duplikatu. Nie jest to dokładnie jednokrotne wykonanie
dowolnych efektów zewnętrznych. Profil/checkpoint i ich wersje pozostają bez zmian.

Dowód P044-03 używa stałej maski fake evaluatora, bez reguł achievementów MVP.
Dla seedów0/1/MAX przechodzi trzy generowane sektory przez resolver i lokalny
sink. Duplikat jest oceniany raz na zakres. Restore sektora2 porównuje każdą
listę kluczy/payloadów oraz pełny stan, a nowa gałąź, nowa próba o tym samym
seedzie i dwa nowe procesy ponownie oceniają zdarzenia bez zmiany main/bak.
Preview zachowuje liczniki usług i dokładne pliki. Tokeny pozostają zadaniem
przyszłej sesji, nie zostały dopisane do checkpointu ani profilu.
