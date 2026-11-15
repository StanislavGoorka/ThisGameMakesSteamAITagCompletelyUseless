# Plan achievementów 1.0 — P088

Dokument roboczy, bez nazw, opisów ani ikon do wyświetlania w grze. P088
zamyka wyłącznie podział. Kandydat 1.0 ma dwanaście achievementów
([koncepcja](GAME_DESIGN.md)); MVP dostarczył ID1..5
([predykaty MVP](ACHIEVEMENT_PREDICATES.md)). Ten plan dodaje siedem
predykatów ID6..12. Tekst i ikona każdego achievementu są pozyskiwane osobno
(P090–P093, polityka treści); żadne dziecko P088 nie dodaje tekstów, ikon ani
assetów, a brak ich nie blokuje predykatów.

Stan: P088 — podział. P088-01: profil schema2 (bity 0..11, odczyt v1/v2, zapis v2).
P088-02: stałe ID6..12 i katalog zaimplementowanych ID. P088-03: RunEnded
niesie start_config i difficulty. P088-04: ID6 swift_sector. P088-05/06: ID7
won_preinstalled_range i ID8 won_charged_debt. P088-07/08/09: ID9
won_short_turn_limit, ID10 won_lean_delivery i ID11 won_heavy_overdrive_debt.
P088-10: ID12 won_all_modifiers — katalog obejmuje wszystkie ID1..12.
Dowód integracyjny P088-11 podzielono (2026-11-15) na P088-12 (runy do pliku),
P088-13 (migracja v1 w oknie) i zawężony P088-11 (restart procesu).
P088-12: windows_achievement_profile_run — 46 pełnych runów do pliku profilu.
P088-13: migracja v1→v2 w sesji okna (windows_achievement_window).
Następny jest P088-11 (restart procesu).

## Predykaty

Tokeny to stabilne wewnętrzne identyfikatory, nie teksty gry. ID są
niezmienne: bit ID−1 w profilu i masce kandydatów, później mapowanie Steam
(P097/P100). Każdy predykat czyta wyłącznie zatwierdzony strumień zdarzeń
dispatchera, tak jak ID1..5.

| ID / token | Warunek pozytywny | Przykład negatywny | Pamięć i pochodzenie |
| --- | --- | --- | --- |
| 6 swift_sector | SectorCompleted z key.turn ≤ 16 w dowolnym sektorze | Ukończenie na turze17; tura ≤ 16 bez ukończenia; death/overload | Bez trackera; restore nie wyklucza |
| 7 won_preinstalled_range | RunEnded(won) w sektorze sector_count, run z StartConfigId ID2 | Zwycięstwo ID1/ID3; przegrana ID2; SectorCompleted pośredni | Bez trackera; konfiguracja pochodzi ze stanu runu, więc restore kwalifikuje |
| 8 won_charged_debt | Jak ID7, konfiguracja ID3 | Zwycięstwo ID1/ID2; przegrana ID3 | Jak ID7 |
| 9 won_short_turn_limit | RunEnded(won) z aktywnym bitem1 (dowolna maska z bitem) | Zwycięstwo z maską bez bitu1; przegrana z bitem1 | Maska jest stała przez run i niesiona przez checkpoint; restore kwalifikuje |
| 10 won_lean_delivery | Jak ID9, bit2 | Zwycięstwo bez bitu2 | Jak ID9 |
| 11 won_heavy_overdrive_debt | Jak ID9, bit4 | Zwycięstwo bez bitu4 | Jak ID9 |
| 12 won_all_modifiers | RunEnded(won) z maską7 | Maska3, 5 lub 6; przegrana z maską7 | Jak ID9 |

Jedno zdarzenie może spełnić kilka warunków: zwycięstwo ID3 z maską7 daje
kandydatów 5, 8, 9, 10, 11, 12 (oraz 3, jeśli historia bez overdrive jest
znana). Predykaty konfiguracji i modyfikatorów nie wymagają historii, bo
konfiguracja i maska są niezmienne w obrębie runu i zapisane w checkpoincie;
nie dotyczy to run_without_overdrive, które nadal wymaga fresh_run.

Uzasadnienie progu swift_sector: 1000 certyfikatów sektora (`developer_corpus`
na de72292) ma 13–22 tury, z czego 279 mieści się w 16. Próg jest hipotezą
projektową, nie pomiarem; korekta wymaga osobnego kroku z ADR. Short_turn_limit
(32) nie zmienia progu.

## Format i katalog

- Profil v1 dopuszcza wyłącznie bity 0..4. Zmiana zbioru bitów wymaga jawnej
  wersji ([format profilu](PROFILE_FORMAT.md)): schema2 o tym samym układzie
  24 bajtów dopuszcza achievement_bits 0..11 (maska 0x0fff); bity 12..15 nadal
  odrzucane. Odczyt przyjmuje schema1 (bity 0..4) i schema2; zapis zawsze
  tworzy schema2. Poprawny plik v1 jako poprzedni main jest walidowany
  i trafia do backupu bez zmian. Bez automatycznej naprawy.
- Katalog zaimplementowanych AchievementId zastępuje stałe 1..5 i 0x1f
  w is_achievement_id_valid, is_candidate_mask_valid i pętli dostarczania
  dispatchera. Enum otrzymuje ID6..12 od razu, ale unlock, sink i maska
  kandydatów odrzucają ID spoza katalogu, dopóki predykat nie jest wdrożony.
  Format (0x0fff) i katalog to osobne pojęcia: profil v2 z bitem
  niewdrożonego ID jest poprawnym, nieaktywnym zapisem.
- RunEnded otrzymuje start_config i difficulty runu (wartości domyślne ID1
  i maska0), wypełniane przez resolve_run_turn_events dla każdego powodu
  zakończenia. Payload jest porównywany w całości przy deduplikacji. Zdarzenia
  nie są serializowane; checkpoint, hash i replay bez zmian.
- Predykaty rozszerzają istniejący MvpAchievementEvaluator (nazwa bez zmian,
  maska uint16 mieści 12 bitów). Prezentacja powiadomień nadal obejmuje tylko
  first_delivery; nowe ID nie mają tekstu do czasu P090–P093.

## Kolejność dzieci

Najpierw format i katalog, potem dane, dopiero potem predykaty, aby żaden
kandydat nie trafił do profilu, który go nie przyjmie.

| Task | Jedyna zmiana | Dowód dodatni i ujemny |
| --- | --- | --- |
| P088-01 | Profil schema2: bity 0..11, odczyt v1 i v2, zapis v2 | Wektory v1 dekodują się jak dotąd; v2 round-trip dla 0x0fff; v1 z bitem5 i v2 z bitem12..15 odrzucone; checksum; save z poprzednim v1 publikuje go do .bak bez zmian i zapisuje v2 |
| P088-02 | AchievementId 6..12 i katalog zaimplementowanych ID (1..5) w unlock, masce kandydatów i dispatcherze | Zachowanie ID1..5 bez zmian; ID0, 6..12, 13 odrzucone przez unlock i sink; maska kandydatów z bitem5 nieprawidłowa; profil v2 z bitem6 poprawny, ale is_achievement_unlocked(6) fałsz |
| P088-03 | RunEnded niesie start_config i difficulty | ID1..3 × maski0/1/7: won/death/overload z wartościami runu; inne zdarzenia bez zmian; deduplikacja odróżnia payload; restore niesie wartości z checkpointu |
| P088-04 | ID6 swift_sector w katalogu i evaluatorze | Tura16 kwalifikuje, tura17 nie; brak ukończenia nie; każdy sektor; restored_checkpoint kwalifikuje; dispatch z retry |
| P088-05 | ID7 won_preinstalled_range | Zwycięstwo ID2 tak; ID1/ID3 i przegrana ID2 nie; restore kwalifikuje |
| P088-06 | ID8 won_charged_debt | Zwycięstwo ID3 tak; ID1/ID2 i przegrana ID3 nie |
| P088-07 | ID9 won_short_turn_limit | Zwycięstwo z maskami1/3/5/7 tak; 0/2/4/6 i przegrana z bitem1 nie |
| P088-08 | ID10 won_lean_delivery | Maski2/3/6/7 tak; pozostałe i przegrana nie |
| P088-09 | ID11 won_heavy_overdrive_debt | Maski4/5/6/7 tak; pozostałe i przegrana nie |
| P088-10 | ID12 won_all_modifiers | Maska7 tak; 3/5/6 i przegrana z maską7 nie |
| P088-12 | Dowód: pełne runy do pliku profilu (headless) | ID1/ID2/ID3 × maski0/7 przez RunAchievements i LocalAchievementSink: ID7 tylko ID2, ID8 tylko ID3, ID9..12 tylko maska7; powtórka nowym właścicielem nie zmienia bajtów pliku ani .bak |
| P088-13 | Dowód: migracja v1→v2 w sesji okna | Plik bajtów schema1 w RunWindow z LocalAchievementSink: po dostawie wersja2 z bitami v1 ∪ nowe, .bak = wartości v1 w schema2; ponowienie bez zmian; uszkodzony v1 nienadpisany |
| P088-11 | Dowód domykający: restart procesu | Proces potomny kończy run ID3 z maską7 (i kontrolny ID1 maska0); nowy proces czyta te same bity, powtórka idempotentna; v1 z P088-13 przeżywa restart jako v2 |

Każde dziecko obejmuje wyłącznie swoją zmianę. Jeśli implementacja przekracza
mikro-zakres, osobnym krokiem wolno ją dalej podzielić z zachowaniem
nadrzędnego ID i zależności konsumentów.

## Wspólne bramki

- Predykat nie losuje, nie wywołuje usług i nie zmienia symulacji, checkpointu,
  hasha ani replay. Odrzucone komendy i preview nie trafiają do evaluatora.
- Każdy krok kodowy: Debug i Release, test +/−, clang-format src/tests/tools.
  Bez tekstów, ikon, assetów i pobrań.
- P089 zależy od P088-11, a łańcuch obejmuje wszystkie dzieci (P088-11
  zależy od P088-13, ten od P088-12, ten od P088-10). Sam P088 nie
  dostarcza żadnego achievementu.
