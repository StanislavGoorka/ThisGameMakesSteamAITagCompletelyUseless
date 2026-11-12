# Zdarzenia domenowe v1

P041-01 opisuje i wdraża wyłącznie wartości w application. Nie ma usługi,
callbacku, IO, serializacji ani evaluatorów achievementów. Core pozostaje
wspólnym źródłem reguł. Zdarzenia to dane wewnętrzne, bez treści dla gracza.

| Stałe ID rodzaju | Wartość payload | Znaczenie |
| --- | --- | --- |
| 1 | ActionCommitted z pełną Command | Jedna zaakceptowana akcja, także wait |
| 2 | PacketDelivered z NodeId celu | Faktyczna nowa dostawa pakietu |
| 3 | SectorCompleted | Ukończenie sektora po zagrożeniach i koszcie tury |
| 4 | RunEnded z RunEndReason | Koniec runu: won=1, death=2, overload=3 |

Rodzaj wylicza payload, nie indeks wariantu. Nie przechowujemy drugiej,
potencjalnie sprzecznej kopii rodzaju. Akcja zawiera cel obrotu/overdrive,
więc przyszły evaluator może śledzić rekonfigurację i użycia overdrive.
Predykaty, zakres ich pamięci i reset definiuje P045, nie ten kontrakt.

DomainEventKey zawiera silny RunSeed, SectorNumber, TurnIndex i uint8 ordinal.
Tura to licznik **po** zaakceptowanej akcji, w MVP 1..48. Ordinal to pozycja
w liście tej tury, zaczyna od0 i jest ciągły. Kolejność: ActionCommitted,
opcjonalna PacketDelivered, opcjonalna SectorCompleted, opcjonalna RunEnded.
Maksymalnie4 zdarzenia. Śmierć po dostawie zachowuje dostawę, lecz wyklucza
SectorCompleted. Końcowy sukces daje SectorCompleted, potem RunEnded(won).
Przeciążenie daje RunEnded(overload); odmowa i terminalne wejście dają pustą listę.
Nagroda i przejście do kolejnego sektora nie są turą i nie tworzą tych zdarzeń.

Seed/sektor/tura/ordinal identyfikuje wystąpienie **wewnątrz jednej próby**.
Powtórna symulacja lub restore tej samej granicy daje te same klucze dla
tej samej sekwencji. Seed nie odróżnia dwóch nowych prób o tym samym seedzie
ani dwóch różnych gałęzi po wczytaniu checkpointu. Nie wolno użyć tego klucza
samodzielnie do globalnej trwałej deduplikacji. P044 musi ustalić kontekst
próby oraz regułę ponownego odtwarzania przed dispatch do usług; obecny profil
i checkpoint nie przechowują outboxu, kluczy ani identyfikatora instancji runu.

P041-02 doda wartościowy wrapper resolvera runu; zwróci wynik i listę,
bez mutacji wejścia i bez publikacji do usług. Istniejący preview_run_turn
pozostanie czystym podglądem stanu, bez dostępu do tego kanału publikacji.
P041-03 sprawdzi pełne sekwencje, restore i izolację preview. Wybór aktywnego
stanu i publikacja listy należą do przyszłej aplikacji, nie do symulacji.
