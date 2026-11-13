# Predykaty MVP

P045: wewnętrzna specyfikacja, bez etykiet i opisów dla gracza. Evaluator
otrzymuje wyłącznie zatwierdzony, ciągły strumień dispatchera. Zwraca maskę
kandydatów, a trwałe odblokowanie i retry należą do sinka i dispatchera.
Wiele warunków może spełnić jedno zdarzenie. Trwały zbiór profilu nigdy nie
jest resetowany przez nowy run ani restore.

| ID | Warunek i przykład pozytywny | Przykład negatywny | Pamięć i reset |
| --- | --- | --- | --- |
| 1 first_delivery | PacketDelivered, także przed śmiercią w tej samej turze | Sama akcja, pakiet obok celu lub już zasilony cel | Bez trackera; każda faktyczna dostawa może ponowić kandydata |
| 2 first_sector | SectorCompleted po zagrożeniach i koszcie; także tura 48 | Gracz na wyjściu z HP=0 albo bez wymaganych dostaw | Bez trackera; sektor 1, 2 i 3 kwalifikują się |
| 3 run_without_overdrive | RunEnded(won) po obserwacji całego runu od początku sektora 1 bez ActionCommitted(OverdriveCommand) | Jeden zaakceptowany overdrive, death, overload lub nieznana historia | Flaga użycia żyje przez trzy sektory. Nowy evaluator z jawnym pochodzeniem fresh_run zaczyna czysty; restored_checkpoint zaczyna z nieznaną historią, również dla sektora 1. Nowa próba tworzy nowy evaluator |
| 4 reconfigured_delivery | Dostawa do celu pakietu, którego wybrana trasa BFS została wcześniej zmieniona zaakceptowanym rotate/overdrive w tym sektorze; otwarcie trasy z braku również kwalifikuje | Obrót innej gałęzi, symetrycznej maski, zamknięcie bez nowej trasy, sam ruch pakietu lub dostawa do innego celu | Zbiór celów ma 49 bitów; dowód trwa do dostawy do danego celu. Dostawa usuwa bit, zmiana sektora i nowy evaluator zerują zbiór. Restore nie dziedziczy dowodów sprzed granicy |
| 5 completed_run | RunEnded(won) po trzecim sektorze | SectorCompleted sektora pośredniego, death lub overload | Bez trackera; restore sektora 2/3 nie wyklucza późniejszego zwycięstwa |

## Dowód rekonfiguracji

Obecne ActionCommitted(command) nie zawiera dowodu zmiany trasy. P045-01
rozszerzy je o opcjonalny `reconfigured_packet_target`, domyślnie pusty.
P045-02 wypełni pole w wartościowym wrapperze resolvera, przed konsumentami.
Przed ruchem pakietu ustalamy ten sam pakiet po fazie wprowadzenia i porównujemy
pełne sekwencje NodeId zwracane przez wspólny BFS dla portów przed i po akcji.
Dowód wymaga rotate/overdrive, niepustej nowej trasy i różnicy sekwencji.
Remisy nadal rozstrzyga N/E/S/W. Późniejsze zamknięcie trasy nie usuwa
wcześniejszego dowodu; warunek oznacza dostawę po faktycznej rekonfiguracji,
nie dowód konieczności obrotu ani przejścia każdej zmienionej krawędzi.

Pole dotyczy celu aktywnego pakietu, także wprowadzonego w tej turze.
Brak aktywnego pakietu daje brak dowodu. Pakiet już na celu ma trasę długości 1,
więc obrót nie zmienia jej. Porównanie nie powtarza resolvera ani faz i nie
zmienia symulacji, checkpointu, hasha lub replay. Zdarzenia nie są serializowane.
Rozszerzony payload wymaga porównania całej wartości przy deduplikacji.

## Granice zaufania i odtworzenie

Pochodzenie historii jest jawnym argumentem konstruktora evaluatora, domyślnie
restored_checkpoint. fresh_run wolno podać tylko dla nowej próby od start_run,
z pierwszym sektorem 1. Seed, zerowy dług i brak zdarzeń overdrive po restore
nie dowodzą braku wcześniejszego użycia. Nie zmieniamy formatu checkpointu.
Odrzucone komendy i preview nie trafiają do evaluatora. Retry nie ocenia
zdarzenia ponownie. Nowa gałąź otrzymuje świeży evaluator; ponowne kandydatury
po restore są bezpieczne dzięki idempotentnemu trwałemu zbiorowi ID.

Testy każdego predykatu obejmą pozytywne i negatywne zdarzenia, właściwy reset,
dispatch z retry oraz zapis lokalny. P051 połączy rzeczywiste predykaty w pełnym
runie i osobno sprawdzi ponowienie po restarcie procesu.
