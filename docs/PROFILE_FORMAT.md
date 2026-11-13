# Profil lokalny v1

P040-01: model i kontrakt; P040-02/03 wdrażają pamięciowy encoder i decoder.
P040-04/05 wdrażają odczyt oraz zapis Windows z backupem.
Profil przechowuje dane konta lokalnego, niezależnie od runu. Nie zawiera
seedu, HP, długu, modułów, numeru sektora ani stanu planszy. Brak pliku może
oznaczać nowy profil; błąd lub uszkodzenie nie upoważnia do nadpisania danych.
Decyzja o utworzeniu profilu należy do przyszłej sesji aplikacji.

`LocalProfile` ma wartości domyślne: achievement_bits=0, variant_bits=0,
master_volume=100, muted=false. To niezależna wartość, bez usług i platformy.
Głośność jest całkowitym procentem 0..100; mute nie kasuje zapisanej głośności.
Nie dodaje to odtwarzania audio ani UI.

Stałe wewnętrzne AchievementId 1..5: first_delivery, first_sector,
run_without_overdrive, reconfigured_delivery, completed_run, zgodnie z
[architekturą](ARCHITECTURE.md). Bit ID−1 oznacza zdobycie; znana maska 0x001f.
Nie są to etykiety ani predykaty dla gracza. P045 definiuje warunki, P046–P050
wdrażają evaluator zdarzeń, a P042
wdraża idempotentne odblokowanie. P042-01 dodaje czyste `unlock_achievement`
oraz `is_achievement_unlocked`. Znane ID 1..5 i poprawny profil są wymagane;
duplikat jest osobnym wynikiem bez zmiany wartości, invalid nie mutuje profilu.
Zmienia się wyłącznie wskazany bit achievementu, bez wariantów i ustawień.
Zapis plików i predykaty pozostają osobnymi krokami.
Variant_bits 0..3 rezerwuje dwa bity dla
alternatywnych konfiguracji 2/3; bazowa konfiguracja 1 jest zawsze dostępna.
Nie wdrożono konfiguracji alternatywnych ani sposobu ich zdobycia.

Rekord ma dokładnie 24 bajty, bez serializacji pamięci C++. Wszystkie pola
wielobajtowe są unsigned little-endian; brak terminatora i danych po rekordzie.

| Offset | Bajty | Wartość |
| --- | --- | --- |
| 0 | 4 | magic `43 43 50 52` (CCPR) |
| 4 | 4 | schema_version=1 |
| 8 | 2 | achievement_bits, wyłącznie bity 0..4 |
| 10 | 1 | variant_bits, wyłącznie bity 0..1 |
| 11 | 1 | master_volume 0..100 |
| 12 | 1 | muted 0 lub 1 |
| 13 | 3 | reserved=0 |
| 16 | 8 | FNV-1a64 bajtów 0..15 |

Checksum: offset basis 14695981039346656037, prime 1099511628211,
po każdym bajcie XOR i mnożenie modulo 2^64. Wykrywa przypadkowe błędy;
nie uwierzytelnia pliku. Nieznana wersja, długość, bit, zakres, niezerowe
reserved lub checksum oznacza odmowę bez częściowego wyniku. Zmiana zbioru
obsługiwanych bitów wymaga jawnej zmiany wersji i planu migracji.

IO: oddzielny plik profilu i `.bak`, staging `.tmp` i `.bak.tmp`,
prywatny lokalny katalog, jeden zapisujący. Zapis nie dotyka checkpointu.
Usunięcie runu obejmuje tylko jego jawnie wskazane nazwy, nigdy cały katalog.
Wybór katalogu użytkownika i integracja sesji pozostają późniejszym etapem.

`read_profile_file` zwraca optional wartości i DWORD, bez domyślnego profilu
przy błędzie. `save_profile_file` waliduje poprzedni main, zapisuje/flushuje
nowy staging, publikuje zwalidowaną kopię poprzedniego main do backupu, potem
nowy main przez rename w tym samym katalogu. Nie ma okna bez nazwy main.
Dwie podmiany nazw nie są wspólną transakcją. Zły lub niedostępny main
blokuje save; poprawny backup pozostaje zachowany. Retry nadpisuje staging.
Brak automatycznego fallbacku/naprawy profilu i współbieżnych zapisujących.

P040-06 potwierdza trwałość przez zamknięcie writera i nowy proces readera.
Usunięcie czterech nazw runu zachowuje dokładne bajty wszystkich nazw profilu,
również pozostawionych staging. Jest to dowód integracyjny oddzielnych plików;
produkcyjny wybór katalogu, reset runu oraz podłączenie sesji nadal nie istnieją.

P042-02: `unlock_profile_file` czyta bieżący zbiór z dysku, zmienia jeden bit
i publikuje przez istniejący save. Wyniki rozróżniają nowe odblokowanie,
duplikat, obce ID, błąd odczytu i błąd zapisu. Tylko ERROR_FILE_NOT_FOUND
pozwala utworzyć domyślny profil. Duplikat nie zapisuje żadnej z czterech nazw.
Nie ma resetu invalid main ani fallbacku do starszego backupu. Warunek jednego
zapisującego nadal obowiązuje; aplikacja po błędzie może ponowić tę samą operację.
Przerwanie po udanym rename, przed odpowiedzią daje przy retry duplikat.

P042-03 potwierdza tę operację w 24 osobnych procesach: początkowy reader,
po cztery procesy na każde z pięciu ID (nowe odblokowanie, reader, duplikat,
reader), dwa procesy odmowy i końcowy reader. Każdy restart zachowuje pełny
profil, a main/bak mają dokładne kanoniczne bajty; duplikat nie przesuwa backupu.
Uszkodzony i zablokowany main są odrzucane bez resetu.
