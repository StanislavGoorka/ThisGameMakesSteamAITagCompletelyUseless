# P088-11 — podział (krok 3/10)

Data: 2026-11-15. Status: P088-11 pozostaje otwarty; wykonano wyłącznie podział.

Cel: dowód integracyjny achievementów 1.0. Zakres P088-11 łączył trzy niezależne dowody o różnych mechanizmach (headless runy z plikiem profilu, rzeczywiste HWND z migracją schema1, proces potomny), co przekracza mikro-zakres jednego kroku (szacunkowo >300 linii testów w dwóch nowych plikach i nowe cele CMake). Walidator dopuszcza tylko jeden poziom ID (P088-NN), więc zgodnie z precedensem P068-05/06 i P085-15/16 dodano rodzeństwo przed istniejącym wpisem domykającym.

Zmiany w tasks.json:
- P088-12 (zależy od P088-10): pełne runy certyfikatami przez RunAchievements i LocalAchievementSink do pliku profilu dla ID1/ID2/ID3 × maski0/7; bity ID7/ID8/ID9..12 tylko dla właściwych przypadków; powtórka nie zmienia bajtów pliku ani .bak.
- P088-13 (zależy od P088-12): plik schema1 w sesji RunWindow; dostawa zapisuje schema2 z zachowaniem bitów v1, .bak równy oryginałowi; uszkodzony v1 nienadpisany.
- P088-11 zawężony do restartu procesu (run w procesie potomnym, odczyt i idempotentna powtórka w nowym procesie, v1 z P088-13 przeżywa restart jako v2); zależy od P088-13. P089 nadal zależy od P088-11.

Pliki: docs/plan/tasks.json, docs/ACHIEVEMENT_EXPANSION_PLAN.md, README.md, docs/workflow/STATE.json, docs/workflow/NEXT.md, ten raport. Bez kodu, tekstów, assetów.

Kontrole: Validate-Project.ps1 PASS; git -c safe.directory=* diff --check PASS. Build/CTest nie dotyczy (brak zmian kodu; ostatnie Debug/Release 178/178 w P088-10).

Następne: P088-12.
