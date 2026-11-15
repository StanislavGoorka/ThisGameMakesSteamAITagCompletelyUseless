# Następny mały krok

Task: P085-10

Czytaj AGENTS, next-step, WORKFLOW, STATE, TESTING, README i raport P085-09. P085-10: tylko ID9 delivery_debt_relief: rzeczywista dostawa odejmuje min(1, dług) po kosztach overdrive tej samej tury; rezerwa nadal według ID3 lub bazowej reguły. Dług0/1/MAX, brak i duplikat dostawy bez ulgi, overdrive+dostawa w jednej turze, pary z delivery_reserve/first_overdrive_debt, restore, preview/odmowy, wersje i oba buildy. Bez ofert/UI/assetów.

Dla kodu: skill cpp-core-step, pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Bez nowych assetów i finalnych napisów. Obserwacja pulpitu i percepcyjna czytelność w P104.
