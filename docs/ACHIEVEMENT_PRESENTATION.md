# Jedno lokalne powiadomienie

P073 jest podziałem na P073-01 -> P073-02 -> P073-03 -> P073-04 -> P073-05.
P074 zależy od P073-05. Zamknięcie podziału nie oznacza implementacji dzieci.

Prezentujemy wyłącznie istniejące AchievementId::first_delivery, bez nowego
predykatu. Dekorator sinka tworzy pending dopiero po wyniku unlocked.
already_unlocked, odmowa i wyjątek nie są nowym odblokowaniem. Powiadomienie
ma jedną pozycję i jawne dismiss; powtórka nie odtwarza go po dismiss.

Tekst jest osobnym wpisem TextCatalog z pełnym prefiksem [AI PLACEHOLDER].
Nie dodajemy ikon ani nie kopiujemy opisów koncepcji jako finalnych tekstów.
Brak katalogowego ID daje odmowę, bez wyświetlania identyfikatora.

Istniejący evaluator i EventDispatcher dostają wyłącznie zdarzenia wykonanej
tury. Preview i odrzucone komendy nie wywołują usług. Nową kolejkę należy
przygotować przed publikacją widoku; po publikacji przenieść bez alokacji.
Błąd lub wyjątek sinka zachowuje zdarzenia do retry bez ponownej oceny.
Kolejne tury mogą dopisać zdarzenia, a restart czeka na opróżnienie starego
scope. Fresh run otrzymuje świeży evaluator; restore zachowuje ostrożny origin.

Profil używa istniejącego formatu i LocalAchievementSink w prywatnym katalogu
LocalAppData. Adapter lokalizacji nie wprowadza innego formatu ani checkpointu.
Nie zerować istniejącego profilu przy błędzie. Wiersz błędu pozostaje oznaczony.
Właściciele sinka, dekoratora i sesji mają jawne czasy życia.

Powiadomienie może być dodatkowym wierszem HUD lub ekranu sesji, bez ukrywania
planszy i zmiany hit-testu przez samą obecność wiersza. Zachować pełne markery,
resize/DPI oraz cały dotychczasowy stan preview. Escape może schować wiersz
bez mutacji runu. Obserwacja pulpitu i testy API są osobnymi dowodami.

Zapis/wznowienie checkpointu pozostaje P074. Bez nowych assetów i pobrań.

P073-01..05 są zaimplementowane. Okno developer podłącza LocalAchievementSink, dekorator i jeden RunAchievements na scope. Wyłącznie zaakceptowana tura publikuje kolejkę; preview, odmowy, resize i focus nie uruchamiają sinka. Błąd pozwala kontynuować tury, a Enter ponawia bez wykonania akcji. Restart czeka na opróżnienie starego scope. Status nie ukrywa planszy. Debug i Release po 125/125 CTest. Kolejka retry jest ulotna; zamknięcie procesu przy niedostępnym profilu nie utrwala oczekujących zdarzeń. Profil zachowuje kontrakt jednego procesu zapisującego. Testy używają własnych plików, nie profilu użytkownika. Obserwacja pulpitu i VM niewykonane.
