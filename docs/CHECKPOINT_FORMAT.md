# Checkpoint granicy sektora v1

Specyfikacja P036; kodek pamięciowy wdrożono w P037-01..04, adapter plików Windows w P038-01..04.
Automatyczne odzyskiwanie pozostaje P039. Checkpoint odtwarza
początek bieżącego sektora po starcie lub przejściu, przed pierwszą akcją.
Zamknięcie w trakcie sektora wznawia ten początek. Nie zapisujemy profilu,
achievementów, ustawień ani stanu okna. To dane wewnętrzne, bez tekstów dla gracza.

## Kontrakt wartościowy

`RunCheckpoint` zawiera `seed`, `sector_number`, `player_integrity`, `reserve`,
`overload_debt`, `modules` i `entry_debt_pulse`. Dług jest resztą **po** jednej
próbie spawnu na granicy. Flaga przechowuje istnienie opłaconego impulsu.
Odtworzenie ustawia ten impuls bez ponownego wywołania `spawn_debt_pulse`.

Zakresy: każdy seed uint64, sektor1..3, HP1..3, rezerwa0..3, dług uint32.
Przy braku impulsu dług musi wynosić0..2 (nowy sektor ma wolne wejście).
Przy impulsie reszta mieści się w0..UINT32_MAX−3; doliczenie wcześniejszego
kosztu3 nie może przepełniać. Moduły to dowolny legalny zestaw z katalogu1/2.
Nie dowodzimy historii zdobywania zasobów ani modułów.

`restore_checkpoint` odrzuca niepoprawne zakresy, a następnie buduje niezależny
`RunState`. `capture_checkpoint` przyjmuje wyłącznie pełny stan zgodny
z rekonstrukcją takiego checkpointu. Tura musi wynosić0, faza playing,
reward_claimed=false, a wszystkie lokalne pola muszą dokładnie odpowiadać
początkowi sektora. Nie wolno po cichu utracić zmiany pola. Odmowa nie mutuje
wejścia. Usługa sesji w przyszłości zatrzyma tę wartość na początku sektora;
nie będzie ponownie przechwytywać stanu po ruchu.

## Rekonstrukcja pełnego stanu

| Pole RunState / GameState | Zapis lub rekonstrukcja |
| --- | --- |
| seed, sector_number | Bezpośrednio z checkpointu |
| player_integrity, reserve, overload_debt, modules | Bezpośrednio; bez leczenia i regeneracji |
| ports, rotatable_nodes | `generate_sector_topology` z N-tego `next_u64` lokalnego SplitMix64(root seed) |
| sector_entry, sector_exit, receiver_nodes | Z tej samej topologii |
| player_position | Wejście wygenerowanego sektora |
| blocked_nodes, powered_nodes | Wszystkie false |
| turns_taken | 0 |
| packet | Brak |
| pulses | Puste albo dokładnie jeden impuls EntityId0 na wejściu, według flagi |
| terminal_rotation_used | false |
| reward_claimed | false |
| RunPhase, SectorOutcome | Wyliczane, nigdy serializowane; playing / ongoing |

Tabela obejmuje wszystkie obecne pola GameState i RunState. Bez utrwalonego RNG
i bez serializacji pamięci C++ lub paddingu. Dla każdego zaakceptowanego początku
`restore(capture(run))` musi równać się pełnemu `run`, łącznie z kolejnością impulsów.
Zmiana topologii, reguł lub powyższych domyślnych pól wymaga nowej wersji reguł.

## Format binarny: dokładnie 40 bajtów

Wszystkie liczby wielobajtowe są unsigned little-endian. Brak terminatorów,
opcjonalnych sekcji i kompresji. Minimalny i maksymalny rozmiar to40 bajtów;
czytnik IO ma odrzucać większy plik przed przydzielaniem pamięci jego rozmiaru.
Kodek pamięciowy sprawdza rozmiar przed czytaniem jakiegokolwiek pola.

| Offset | Bajty | Pole | Akceptowane wartości |
| --- | --- | --- | --- |
| 0 | 4 | Magic | 43 43 43 50 (hex, ASCII CCCP) |
| 4 | 4 | schema_version | 1 |
| 8 | 4 | run_rules_version | 1 |
| 12 | 8 | root seed | Pełny uint64 |
| 20 | 1 | sector_number | 1..3 |
| 21 | 1 | player_integrity | 1..3 |
| 22 | 1 | reserve | 0..3 |
| 23 | 4 | overload_debt | Reszta po spawnie, według zakresów powyżej |
| 27 | 2 | modules | 00 00, 01 00, 02 00 albo 01 02; wyłącznie ten porządek |
| 29 | 1 | entry_debt_pulse | 0 lub1 |
| 30 | 2 | reserved | 00 00 |
| 32 | 8 | checksum | FNV-1a64 bajtów0..31 |

FNV-1a64: start14695981039346656037; dla każdego bajtu XOR, potem mnożenie
przez1099511628211 modulo2^64. To kontrola przypadkowego uszkodzenia, bez
uwierzytelniania i bez gwarancji wykrycia każdej zmiany. Nie używa `state_hash`:
tamten hashv4 dotyczy pełnego GameState i służy replay. Checksum obejmuje wersje,
seed, wszystkie pola i bajty zarezerwowane. Wersja reguł runu1 zamraża kontrakty
M3, w tym zasady tury replay8, hashv4 i generator P028/P035. Wersje checkpointu
i replay są odrębne; dodanie kodeka nie zmienia istniejących replayów.

## Odrzucanie i dowody

Nieznana schema lub run_rules zawsze odrzucana, nawet z poprawną checksumą;
brak automatycznej migracji. Złe magic, długość, checksum, zakresy, ID modułów,
duplikaty, luki w slotach, odwrotny porządek, flagi inne niż0/1 i niezerowe
reserved również odrzucane. Błąd daje pusty optional, bez częściowego wyniku.
API nie przyjmuje ścieżek, callbacków, RNG ani istniejącego stanu do nadpisania.

Wymagane testy kodeka: niezależne literalne wektory bajtów, round-trip wszystkich
zasobów i zestawów, pełna rekonstrukcja początku każdego sektora, obie rodziny
topologii, powtórzenia i niezależność kopii. Każde ucięcie0..39, nadmiar danych,
pojedyncze zmiany bajtów, błędne pola z ponownie policzoną checksumą, obie obce
wersje. Impuls/dług po resume ma zachować dokładnie jeden już opłacony spawn.
P038-01..04 wdraża ograniczony reader, staging z flush i publikację ze
zwalidowanym backupem. Testy plikowe i przerwania procesu opisuje
[TESTING](TESTING.md); sam format nie jest dowodem IO. P039 wdraża odczyt
backupu bez zapisu: poprawny main wygrywa; ERROR_FILE_NOT_FOUND,
ERROR_PATH_NOT_FOUND lub ERROR_INVALID_DATA pozwala sprawdzić `main.bak`.
Pozostałe błędy IO main kończą próbę bez fallbacku. Wynik zawiera optional
checkpoint, źródło none/main/backup, surowy main_error oraz optional backup_error
(pusty = nie próbowano). Błąd backupu daje brak checkpointu z obiema diagnozami;
ERROR_INVALID_DATA oznacza uszkodzenie, dwa NOT_FOUND oznaczają brak,
pozostałe kody oznaczają IO. Nie ma naprawy, nadpisania lub promocji temp.
Schema1/run_rules1, hashv4/replay8 bez zmian.
