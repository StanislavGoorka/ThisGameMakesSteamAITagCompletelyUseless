# Następny mały krok

Task: P040

Przeczytaj AGENTS, next-step, WORKFLOW, STATE, raport P039 i bieżący wpis
z zależnościami w tasks.json. Status i walidator na początku; P039 completed.

P040 dotyczy trwałego profilu odblokowań i ustawień oddzielnego od runu.
Najpierw oceń rozmiar: brak kontraktu profilu może wymagać wyłącznie podziału
na małe dzieci (model/specyfikacja, kodek, IO, dowód niezależności usunięcia
runu). Przepnij konsumentów na ostatnie wymagane dziecko; podczas podziału
nie implementuj dzieci. Bez achievementów, UI, treści i napraw checkpointu.

Czytaj ARCHITECTURE, GAME_DESIGN, TESTING, CHECKPOINT_FORMAT, README i adaptery
checkpoint_*.hpp. P039 jest wyłącznie odczytem main/bak: IO main zatrzymuje
wybór, brak/invalid pozwala fallback. Nie naprawia main; save nadal odrzuca
uszkodzony main. Katalog użytkownika i integracja sesji nie są wdrożone.
Dla kodu cpp-core-step; jeden krok ma jeden kontrakt.

Dla podziału: walidator, diff --check, raport, STATE/NEXT/README i lokalny
commit z kontrolą hash/zakresu/statusu. Dla kodu dodatkowo configure,
build/CTest Debug i Release i clang-format src/tests/tools. Bez assetów,
pobrań i pushu. Zatrzymaj się po jednym zadaniu.
