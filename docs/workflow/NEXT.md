# Następny mały krok

Task: P087-05

Czytaj AGENTS, next-step, cpp-core-step, WORKFLOW, STATE, TESTING, GAME_DESIGN, DIFFICULTY_MODIFIER_PLAN, CHECKPOINT_FORMAT i raport P087-04 (wzór modyfikatora). P087-05: wyłącznie bit2 lean_delivery w difficulty_modifier_catalog i w deliver_packet (src/core/packet_delivery.hpp): dostawa przywraca o 1 rezerwę mniej (0 bez modułu, 1 z delivery_reserve), z limitem3. Zasilenie odbiornika i delivery_debt_relief bez zmian. Bez innych modyfikatorów, wyboru, UI, tekstów i assetów.

Testy +/−: rezerwa po dostawie z bitem +0 bez modułu, +1 z delivery_reserve, przy rezerwie3 bez zmian; bez bitu +1/+2 jak dotąd; zasilenie i debt_relief identyczne z bitem i bez; pełna tura przez resolver i podgląd; maski 2 i 3 (z bitem1) działają niezależnie; checkpoint: nowy wektor z maską2 (vectors.cjs), korpus integracyjny dopuszcza bajt31 ≤ 3, dekoder odrzuca 4/8/255; fabryka przyjmuje 0..3; restore/restart zachowują bit; testy podrabiające bit2 (run_checkpoint pole17, encode pole9) przenieś na bit4. Mutacje: dostawa ignorująca bit, bit odejmujący także z modułem do 0.

Kod: pełny build/CTest Debug i Release, clang-format src/tests/tools. Po mutacji odczekaj ponad 1 s i touch przed buildem. Walidator, diff --check, raport, STATE/NEXT/README, lokalny commit i kontrola hasha/zakresu. Git z lokalnym -c safe.directory; .claude/ poza zmianami; bez push. Obserwacja pulpitu i percepcyjna czytelność w P104.
