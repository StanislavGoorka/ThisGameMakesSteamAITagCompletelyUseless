# Logiczny opóźniony kondensator

Kontrakt P083-01. Dokument roboczy, bez tekstów dla gracza, nowych assetów,
spawnu, zmiany generatora i prezentacji. P083 oznacza wyłącznie podział planu.

## Wartość i odliczanie

`DelayedCapacitor` posiada `EntityId id`, `NodeId position`, `delay` 1–48,
`range` 1–48 oraz `remaining` 0–delay. Fabryka przyjmuje pełne liczby przed
zawężeniem; błędne opóźnienie lub zasięg daje brak wartości. Początkowe
`remaining = delay`. Nie ma publicznego konstruktora niepoprawnego stanu ani
settera remaining. Kopia posiada niezależną wartość. ID są unikalne w jednej
kolekcji kondensatorów, z osobną przestrzenią wobec impulsów i emiterów.

Kondensator jest uzbrojony przy utworzeniu. Nie wymaga kontaktu ani obecności
gracza. Nie porusza się, nie blokuje ruchu, nie zajmuje miejsca impulsu.
`advance_fuse()` oznacza jeden tick zaakceptowanej akcji: remaining>0 maleje
o 1, a wynik jest true wyłącznie przy przejściu 1→0. Dla remaining=0 zwraca
false i niczego nie zmienia. Zero oznacza spent na resztę życia wartości:
bez ponownego uzbrajania, odejmowania poniżej zera i usuwania z kolekcji.
Nie potrzeba zegara systemowego ani arytmetyki turns_taken+delay.

| delay | przed akcją 1 | po akcji 1 | po akcji 2 | po akcji 3 |
| --- | --- | --- | --- | --- |
| 1 | 1 | 0, wybuch | 0, bez wybuchu | 0, bez wybuchu |
| 2 | 2 | 1, bez wybuchu | 0, wybuch | 0, bez wybuchu |
| 48 | 48 | 47, bez wybuchu | 46, bez wybuchu | 45, bez wybuchu |

Dla delay=48 sygnał następuje dopiero przy ticku 48. Przy śmierci lub sukcesie
przed tym terminem dalsze komendy są odrzucane i licznik przestaje tykać.
Nielegalne komendy i zapytania o podgląd nie zmieniają licznika żywego stanu.
Podgląd posiada osobną kopię, z wynikiem tej samej symulacji co commit.

## Zasięg w chwili detonacji

Footprint jest czystym odczytem: źródło oraz pola osiągalne w najwyżej range
wzajemnych krawędziach N/E/S/W. Wynik to 49 flag według NodeId. Semantyka
odpowiada [emiterowi](PERIODIC_EMITTER.md): bez zawijania, duplikatów w cyklu,
blokowania przez jednostki lub blocked_nodes i bez przejścia przez jednostronny
port. Odczyt jest dostępny także dla niedojrzałej i spent wartości; nie detonuje
i nie zmienia konfiguracji ani licznika.

Nie zapamiętuj grafu z chwili uzbrojenia. Przy źródle 0 i połączeniach 0–1–2
range=1 obejmuje dokładnie 0/1, a range=2 obejmuje 0/1/2. Kondensator delay=2
nie trafia w pierwszej turze. Obrót odcinający 1–2 w drugiej turze usuwa pole
2 z wybuchu; ponowne połączenie przed detonacją dodaje je do footprint.
Późniejszy obrót nie wywoła kolejnego wybuchu spent kondensatora.

## Faza i resolver

Helper `resolve_capacitor_phase(GameState&, span<DelayedCapacitor>, bool&)`
wykonuje jeden tick każdego elementu. Wyłącznie nowy sygnał 1→0 może zadać
obrażenie. Gdy gracz jest w footprint, ma HP>0 i limit nie jest wykorzystany,
traci 1 HP, a damage_applied staje się true. W innych przypadkach HP i limit
pozostają bez zmian. Wszystkie liczniki nadal tykają, również po trafieniu
innego źródła, przy HP0, poza zasięgiem i przy wykorzystanym limicie.
Wybuch nie leczy, nie uszkadza pakietu, grafu ani innych zagrożeń, nie
wywołuje reakcji łańcuchowej i nie odnawia rezerwy. Porządek listy nie zmienia
wyniku; liczniki zmieniają wyłącznie własną wartość.

Docelowa kolejność zaakceptowanej tury: akcja → pakiet/dostawa → impulsy →
emitery → kondensatory → jeden koszt tury → wynik sektora. Wspólny bool
damage_applied należy do resolvera. Dotychczasowe helpery zachowują kontrakty.
Śmierć od wybuchu ma pierwszeństwo przed sukcesem po dostawach na wyjściu.
Terminalna/nielegalna komenda nie uruchamia żadnej fazy. Helper sam nie
sprawdza legalności komendy ani nie zwiększa turns_taken; caller odpowiada
za jedno wywołanie na zaakceptowaną akcję.

## Stan, zapis i dowody

P083-02 dostarcza wartość i licznik; P083-03 współdzielony grafowy BFS
i footprint; P083-04 samodzielną fazę na jawnej liście. Dopiero P083-05
dodaje kolekcję do GameState, pełne porównania i wersjonowany hash/replay.
Hash obejmuje id, position, delay, range i remaining, również spent, liczbę
i kolejność elementów. Niezależne wektory nie pochodzą z implementacji C++.
Checkpoint obecnego generatora odtwarza pustą kolekcję; capture musi odrzucać
każdą niepustą, również spent, zamiast cicho ją usuwać. Format zapisu nadal
nie serializuje dowolnego stanu śródsektorowego. P083-06 podłącza resolver.

Testy: walidacja granic i pełnych liczb; delay 1/2/48 i powtarzane ticki po
spent; kopie; dokładne zbiory footprint z cyklem/rozgałęzieniem/obrotem;
kilka jednoczesnych wybuchów w różnych kolejnościach; HP0 i zużyty limit;
pełna niezmienność innych pól; preview/commit po obrocie między fazami i
w turze detonacji; odmowy bez ticka; śmierć przed sukcesem i koszt 1.
Bez rozmieszczenia zagrożenia w generatorze ani symbolu zastępczego w UI.
