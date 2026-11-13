# Następny mały krok

Task: P051-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P051-01, bieżący wpis i jego
zależności w tasks.json. Status i walidator na początku.

Rozszerz istniejący test Windows o osobny tryb procesu dla rzeczywistego MvpAchievementEvaluator. Odczytaj checkpoint sektora 2, odtwórz sektory 2–3, porównaj pełne stany/payloady oraz dokładne main/bak w dwóch nowych procesach. Restore do pustego profilu nie może dać run_without_overdrive. Dotychczasowy worker P044-03 używa fake i nie spełnia tego kryterium. Bez nowych predykatów i zmian formatów.

Dla kodu użyj cpp-core-step; czytaj ARCHITECTURE, GAME_DESIGN, TESTING,
ACHIEVEMENT_PREDICATES, DOMAIN_EVENTS i DOMAIN_DISPATCH. Jeden mały kontrakt.
Configure, build/CTest Debug i Release, clang-format src/tests/tools, walidator
i diff --check; raport, STATE/NEXT/README i lokalny commit z kontrolą zakresu.
Bez assetów, pobrań, tekstów dla gracza, UI, Steam i pushu.
