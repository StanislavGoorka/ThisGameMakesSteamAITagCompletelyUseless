# Bramka headless P054

Data: 2026-11-13. Decyzja: przejść do M5, weryfikacji pochodzenia treści.
Nie zmieniono reguł ani zakresu gry. M2–M4 dostarczają wykonywalny dowód
logiki i trwałości; nie stanowi to akceptacji grywalnego MVP lub wydania.

| Obszar | Dowód sprawdzony w bramce | Granica wniosku |
| --- | --- | --- |
| Reguły i preview | Pełne stany, kolejność faz, odmowy, moduły; 16 384 deterministyczne próby P053 | Ograniczony korpus; bez dowodu balansu |
| Generator | 1000 certyfikatów i 7000 scenariuszy spawnów, powtarzalny raport CLI | Bazowa rodzina dwóch tras; solver nie obejmuje dowolnych plansz i zagrożeń |
| Run | Trzy sektory, nagrody, transfer zasobów i długu, śmierć i overload | Brak sesji i UI; brak oceny różnorodności |
| Replay | Dokładne literalne hashe, seedy 0/MAX, odmowy parsera i wersji; hashv4/replay8 | Fixture replay różni się od generatora runu |
| Checkpoint | 11 000 granic, uszkodzone dane, IO, backup, retry i przerwanie procesu | Schema1/run_rules1, początek sektora; brak zaniku zasilania i wielu writerów |
| Profil | Kodek24, maski i ustawienia, niezależność runu, trwałe odblokowania i restart | Schema1; brak automatycznej naprawy uszkodzonego main |
| Achievementy | Pięć rzeczywistych predykatów; pełny run31 i restore27, dwa nowe procesy i dokładne main/bak | Brak trwałego outboxu; crash przed zapisem profilu może utracić zdarzenie |
| Wydajność | Dwa pomiary Release P052, p50/p95 i sprzęt w osobnym raporcie | Tylko symulacja, bez renderera i usług |

Ponownie wykonano configure, build i wszystkie CTest Debug/Release oraz
formatter całego src/tests/tools. Wyniki: 83/83 w obu konfiguracjach.
Dokładne polecenia i dodatkowe wyniki kontroli pamięci są w
[raporcie P054](workflow/history/P054.md). Walidator projektu i jego testy
regresyjne kontrolują dokumentację/rejestry, nie autorstwo zawartości.

Ograniczenia, które pozostają jawne przy przejściu:

- Brak assetów i finalnych tekstów; niczego nie wyświetlano ani nie pobrano.
- Brak QA klawiatury, DPI, focusu, urządzenia, audio i kontrolowanego zamknięcia
  gry; brak pełnego runu obserwowanego w Windows. To kryteria przyszłego M6.
- Brak czystej maszyny/VM, konta standardowego i integracji Steam.
- Procesowe testy przerwań nie są dowodem zachowania podczas zaniku zasilania.
- Headless potwierdza poprawność ustalonych reguł. Przed rozszerzaniem treści
  potrzebna jest obserwacja czytelności i powtarzalności decyzji. P081 pozostaje
  rzeczywistą bramką MVP; brak interesujących decyzji wymaga korekty zakresu.

Następny krok P055: dossier jednego konkretnego pakietu, bez pobierania
i bez zapisywania preview. Brak dowodu non-AI lub zgody zatrzyma import;
poprawna logika nie zastępuje tych warunków.
