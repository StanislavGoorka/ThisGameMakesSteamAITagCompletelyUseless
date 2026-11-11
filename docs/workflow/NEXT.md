# Następny mały krok

Task: P024

## Cel i lektura

Oceń zakres replay seed + wersja reguł + komendy przed implementacją. P023 dostarcza podgląd przez ten sam resolver, bez efektów ubocznych. Ostatnie lokalne Debug i Release zaliczyły po 24/24 CTest; walidator ma 127 zadań.

Przeczytaj [AGENTS](../../AGENTS.md), [next-step](../../.agents/skills/next-step/SKILL.md), [WORKFLOW](../WORKFLOW.md), [STATE](STATE.json), [raport P023](history/P023.md), wpis P024 i jego zależność P023 w [kolejce](../plan/tasks.json), [ARCHITECTURE](../ARCHITECTURE.md), [GAME_DESIGN](../GAME_DESIGN.md), [TESTING](../TESTING.md), [README](../../README.md), [GameState](../../src/core/game_state.hpp), [identyfikatory](../../src/core/identifiers.hpp), [SplitMix64](../../src/core/splitmix64.hpp), [komendy](../../src/core/command.hpp), [resolver](../../src/core/turn_resolver.hpp), [podgląd](../../src/core/turn_preview.hpp) i [jego testy](../../tests/unit/turn_preview_tests.cpp).

## Ocena przed zmianą

Obecnie seed inicjalizuje tylko RNG. Nie istnieją fabryka stanu z seeda, kanoniczny hash pełnego GameState ani format/narzędzie replay. Resolver i preview nie przyjmują RNG; powtarzanie resolvera dla ręcznie zbudowanej planszy nie spełnia samo kontraktu seed + wersja + komendy. Generator topologii P028 pozostaje późniejszym, osobnym etapem.

Ustal minimalny, jawnie deweloperski zakres replay bez prezentacji i bez pozorowania generatora sektora. Wymagane są zgodne hashe przy identycznym replay oraz jawna odmowa nieobsługiwanej wersji. Określ stabilne kodowanie każdego pola GameState, opcjonalnych wartości i kolekcji, zamiast hashowania surowej pamięci lub std::hash; kolejność impulsów wymaga jawnego kontraktu. Nie dodawaj zapisu gry lub ogólnego formatu serializacji.

Jeżeli brakujące kontrakty nie mieszczą się w jednym małym kroku, zgodnie z WORKFLOW wykonaj **wyłącznie podział P024** na odrębnie ocenialne dzieci. Zachowaj ID rodzica jako planistyczne, topologiczną kolejność i przepnij P025 na ostatnie wymagane dziecko; nie implementuj dzieci w tym samym kroku. Każde dziecko musi mieć dowód sukcesu i właściwy przypadek błędu. Nie zmieniaj mechanik, żeby ominąć brak fabryki stanu.

## Weryfikacja i przekazanie

W kroku dokumentacyjnym zweryfikuj graf, zależności i przekazanie walidatorem oraz izolowanym testem negatywnym zależności nowych dzieci/konsumenta. Nie deklaruj nowych testów gry bez ich wykonania. Gdy zakres faktycznie pozostanie pojedynczą implementacją, zastosuj [cpp-core-step](../../.agents/skills/cpp-core-step/SKILL.md) i wymagane konfigurację, build, CTest Debug/Release, formatter oraz celową mutację z potwierdzoną rekompilacją.

Zaktualizuj README/ARCHITECTURE zgodnie z faktem, zapisz history/P024.md, STATE i konkretne NEXT. Uruchom walidator i kontrole diff; wykonaj lokalny commit wyłącznie własnych zmian, sprawdź hash, zakres i czystość. Bez pushu. Zatrzymaj się po jednym ID.

Nie implementuj P025, overdrive, generatora P028, runu, zapisu ani prezentacji. Nie generuj assetów i treści dla gracza. Znany błąd scripts/Test-ProjectValidator.ps1 pozostaje poza zakresem.
