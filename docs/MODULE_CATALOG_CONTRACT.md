# Kontrakt katalogu i reguł modułów — P085-01

Dokument wewnętrzny; tokeny nie są etykietami gry. Rozszerza
[plan](MODULE_EXPANSION_PLAN.md), bez wdrożenia nowych ID.

## Katalog, sloty i zapis

`ModuleId` ma stabilne wartości z tabeli planu: 1..12; 0 oznacza pusty slot
wyłącznie w serializacji. `module_catalog` zawiera tylko zaimplementowane ID,
a `reward_module_catalog` jest osobną, uporządkowaną podpulą. Początkowo oba
zawierają wyłącznie 1/2; dzieci reguł rozszerzają pierwszy, P085-19 drugi.
Żaden decoder ani `ModuleLoadout::add` nie przyjmuje przyszłego ID.

`is_known_module(ModuleId)` sprawdza katalog implementacji. Wspólny
`decode_module_slots(array<uint8_t, 2>) -> optional<ModuleLoadout>` przyjmuje
puste sloty tylko na końcu, rosnące znane ID, bez duplikatów. Nie normalizuje
błędnych bajtów. `add` zachowuje atomową odmowę i kanoniczny porządek przy
dwóch slotach. Pełny loadout zawsze daje pustą ofertę, nawet gdy katalog
zawiera więcej typów. Dotychczasowe cztery zapisy pozostają identyczne.

## Oferty i jedna decyzja runu

`module_offer(modules, root_seed, completed_sector)` jest czystym odczytem.
Pomija posiadane i niedopuszczone ID. Z maksymalnie dwóch pozostałych zwraca
wszystkie; większa pula wybiera dwa różne indeksy przez lokalny SplitMix64
zależny od root seedu i numeru sektora. Wynik sortuje według ID. Losowanie
nie dotyka RNG generatora sektorów ani stanu runu. Algorytm indeksów ma
oddzielne API dla niewyświetlanego korpusu rozmiarów puli 0..12.

`choose_run_module` wylicza ofertę z bieżącego runu, zanim zainstaluje ID;
`offered_run_modules` oraz okno korzystają z tego samego kontekstu. Niski
helper `choose_module` może zachować domyślny kontekst dla dawnych fixture,
ale ścieżka runu zawsze przekazuje seed i ukończony sektor jawnie. Brak
oferty, zła faza, posiadany ID, stary wybór lub pełny loadout dają odmowę
bez zmiany `reward_claimed`. Restart i restore odtwarzają identyczne oferty.

## Reguły i lokalne resety

Każdy wiersz planu jest osobną zmianą. Kolejność nadal: walidacja, jedno
wprowadzenie pakietu, akcja, ruch pakietu, jedna dostawa, impulsy, emitery,
kondensatory, jedna opłata tury i wynik. Preview wykonuje ten sam resolver.

- ID3: dostawa odnawia min(2, brakująca rezerwa); powtórzona nic nie robi.
- ID4: `first_overdrive_used=false` na początku sektora. Pierwszy udany
  overdrive z modułem dodaje 0 długu, także przy UINT32_MAX; nadal zużywa
  rezerwę i obraca dwa razy. Następne dodają 1 i przy MAX odmawiają.
  Każdy udany overdrive oznacza użycie, również bez modułu; zwykły obrót,
  odmowa i preview nie zużywają znacznika w stanie źródłowym. Reset odnawia
  go razem z `terminal_rotation_used`; nie ma resetu w środku sektora.
- ID5: `advance_packet` wykonuje do dwóch krawędzi tej samej trasy BFS,
  zatrzymując się na celu. Nie wprowadza ani nie dostarcza drugiego pakietu.
- ID6: ruch impulsów tylko gdy `(uint64_t(turns_taken)+1) % 2 == 0`.
  Usuwanie zastanych kontaktów działa przed pominięciem ruchu.
- ID7: po zwykłym wyborze kroku BFS impuls czeka, jeżeli cel kroku ma obie
  flagi receiver/powered. Zajęcie, kolejność ID i brak objazdu pozostają.
- ID8: w turze z `turns_taken==0` kontakt impulsu przy graczu na wejściu
  usuwa impuls bez HP i bez zajęcia wspólnego limitu obrażeń. Pozycję
  sprawdza po akcji; opuszczenie wejścia znosi ochronę. Emiter/kondensator
  nadal może zadać 1 HP. Nie potrzeba nowego pola: licznik tury już istnieje.
- ID9: wyłącznie udana dostawa redukuje dług o min(1, dług), po kosztach
  overdrive; rezerwa nadal według ID3 lub bazowej reguły.
- ID10: +1 Manhattan tylko dla overdrive (2 sam, 3 z ID1), bez zmiany rotate.
- ID11: zwykły rotate wykonuje trzy ćwierćobroty zgodne z zegarem;
  overdrive nadal dwa. Zgoda na nieruchomy terminal zużywana dokładnie raz.
- ID12: następny cel to najwyższy niezasilony odbiornik; aktywny pakiet
  zachowuje cel. ID5 nie uruchamia ponownie wyboru celu w tej samej turze.

## Wersje i dowody

Refaktor katalogu/ofert przy puli 1/2 nie zmienia bajtów ani reguł replay.
Każde nowe działające ID zwiększa replay i checkpoint run_rules: checkpoint
akceptuje tylko bieżące reguły, również przy poprawnej checksumie starej
wersji. Schema1/40 bajtów wystarcza dla ID do12 i resetowanych pól lokalnych.
Nie utrwala się środkowej tury. Nowy znacznik ID4 wymaga nowej wersji hasha,
niezależnych literalnych wektorów i dokładnego porównania w capture oraz
baseline witness. Capture odrzuca znacznik true nawet bez modułu; restore
tworzy false. Zmiany wersji aktualizują oba niezależne oracle JS i testy.

Korpus slotów: wszystkie 65536 par bajtów przeciw niezależnej definicji
legalności; odmowy add dla każdego nieznanego uint8 i pełnego zestawu.
Korpus ofert: rozmiary 0..12, sektory 1..6, co najmniej 257 seedów, stabilne
powtórzenia, osiągalność każdego indeksu, brak duplikatów i mutacji. Pula
1/2 zachowuje dokładne dawne oferty; brakujące ID przyszłe są odrzucane.
Każda reguła: pełne porównanie stanu, próba dodatnia i ujemna z tabeli planu,
preview/commit/odmowa, kopie, drugi moduł w slocie, reset/restore, Debug i
Release. Wersje mają literalne wektory niezależne od kodu C++.

Po tych dowodach P085-17 doda katalogowe placeholdery, P085-19 oferty.
Obserwacja pulpitu, ocena czytelności i ludzki playtest pozostają w P104.
