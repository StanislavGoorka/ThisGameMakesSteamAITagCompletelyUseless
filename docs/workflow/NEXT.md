# Następny mały krok

Task: P061-02

Czytaj AGENTS, next-step, WORKFLOW, STATE, raport P061-01, bieżący wpis
i jego zależności w tasks.json. Sprawdź status i uruchom walidator na początku.

Publiczna bramka rejestru napisów. Dodaj skrypt public content gate na bazie bramki approved: content/texts.json musi zawierać wyłącznie approved i żadnego markera. Bez skanera binariów i bez zmiany treści.

Kryteria: Izolowane rejestry: pusty i approved fixture przechodzą, oznaczony placeholder dozwolony przez developer walidator jest odrzucony publicznie, mixed i zły status/źródło odrzucone. Właściwe kody procesu.

Jeżeli zakres przekracza mikro-krok, wykonaj wyłącznie podział i przepnij
zależności konsumentów. Dla C++ stosuj cpp-core-step, ARCHITECTURE,
GAME_DESIGN i TESTING. Dla treści asset-intake i CONTENT_POLICY.
Configure, build i CTest Debug/Release, formatter src/tests/tools;
dla dokumentacji walidator i diff --check. Raport, STATE/NEXT/README
i osobny lokalny commit z kontrolą hasha oraz zakresu. Bez nowych assetów,
pobrań i pushu.
