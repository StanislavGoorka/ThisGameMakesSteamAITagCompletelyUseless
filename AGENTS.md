# Repository Guidelines

## Cel i ograniczenia

Gra singleplayer na Windows x64, C++20, bez zewnętrznego silnika. Samodzielne biblioteki są dozwolone; stos bazowy to Win32 i Direct2D. Projekt i plan opisuje [README.md](README.md). Dokumentacja robocza nie jest zawartością gry.

**Nie generuj żadnych assetów widocznych lub słyszalnych dla gracza.** Dotyczy to także SVG, ikon, shaderów artystycznych, proceduralnych grafik, syntezowanych dźwięków, muzyki, tłumaczeń i materiałów sklepu. Kod nie może służyć obejściu zakazu. Jedyny wyjątek: roboczy tekst zaczynający się dokładnie od `[AI PLACEHOLDER]`. Placeholdery nie mogą wejść do wydania. Wczytuj i wyświetlaj tylko zatwierdzone treści o sprawdzonym pochodzeniu.

Przed pobraniem assetu uzyskaj wyraźną zgodę użytkownika na konkretny pakiet i wersję. Brak odpowiedzi nie oznacza zgody. Sama licencja CC0 nie dowodzi braku AI. Zasady: [docs/CONTENT_POLICY.md](docs/CONTENT_POLICY.md).

## „Zrealizuj następny krok”

1. Przeczytaj [.agents/skills/next-step/SKILL.md](.agents/skills/next-step/SKILL.md), [stan](docs/workflow/STATE.json) i [przekazanie](docs/workflow/NEXT.md).
2. Wykonaj **jedno bardzo małe zadanie** z [kolejki](docs/plan/tasks.json), którego zależności są ukończone. Wykonaj jego testy. Nie realizuj całego etapu.
3. Zaktualizuj stan, zapisz raport w `docs/workflow/history/` i pozostaw konkretny następny krok w `NEXT.md`.

Decyzje podejmuj samodzielnie zgodnie z planem. Nie pytaj o wybór mechanik. Wyjątki wymagające człowieka opisuje [docs/WORKFLOW.md](docs/WORKFLOW.md). Nie uruchamiaj dodatkowych agentów równolegle do implementacji tego samego kroku.

## Struktura, komendy i styl

Obecnie: dokumentacja i walidator planu. Docelowo: `src/core/`, `src/platform/windows/`, `src/presentation/`, `tests/`, `assets/approved/`. Weryfikacja teraz: `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1`. Przyszłe komendy CMake opisuje README.

C++: 4 spacje, `snake_case` dla funkcji/zmiennych, `PascalCase` dla typów; RAII, silne typy identyfikatorów, brak zależności platformowych w core. Konfiguracja clang-format w P003. Testy przez CTest; bramki: [docs/TESTING.md](docs/TESTING.md).

## Commity i publikacja

Małe commity w trybie rozkazującym, np. `Add deterministic port rotation tests`. Raportuj wykonane i niewykonane testy. Nie publikuj sekretów, prywatnego Steamworks SDK ani nieudokumentowanych assetów. Nie deklaruj gwarancji braku oznaczenia AI; ankieta Steam musi być zgodna z faktami.
