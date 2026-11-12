# Testy i bramki jakości

## Obecnie

`powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Validate-Project.ps1` kontroluje spójność dokumentacji, kolejki i pustych rejestrów. `powershell -NoProfile -ExecutionPolicy Bypass -File scripts/Test-ProjectValidator.ps1` sprawdza 11 przypadków, w tym uszkodzony hash i brak zgody, na izolowanych kopiach. Syntetyczne rekordy testowe nie są prawdziwymi zgodami ani dowodami autorstwa.

`core_build_smoke` sprawdza kompilację C++20 i wykonanie kodu korzystającego z `std::span` i ranges. `core_identifier_ranges` sprawdza granice i porządek wartości `NodeId` oraz `EntityId`. Kompilacja testu weryfikuje także brak niedozwolonych konwersji. `core_port_rotation` sprawdza obrót o 90° dla każdej maski, powrót po czterech obrotach, zachowanie liczby portów i odrzucanie wartości spoza zakresu. `core_grid_adjacency` sprawdza wzajemność połączeń, granice siatki i brak zawijania między wierszami. Uruchamianie Debug/Release opisuje [README](../README.md). [Raport P002](workflow/history/P002.md) dokumentuje celowy błąd i niezerowe kody runnera oraz CTest w obu konfiguracjach.

## C++ i zakres testów

CTest uruchamia mały natywny runner testów; późniejszą bibliotekę można dodać tylko z uzasadnieniem. Nazwy plików: `*_tests.cpp`; nazwy przypadków opisują warunek i rezultat, np. `rotate_four_times_restores_ports`, `invalid_move_preserves_rng`. Każdy test działa również w Release, bez polegania na `assert`.

Nie narzucamy sztucznego procentu coverage całego produktu. Core musi mieć testy wszystkich reguł i opisanych ścieżek błędów. Liczba zaliczonych testów ani 100% line coverage nie dowodzą grywalności. Przy poprawce błędu najpierw przypadek odtwarzający błąd, potem poprawka i ponowne uruchomienie.

| Warstwa | Dowód wymagany |
| --- | --- |
| Porty i graf | Każda maska, 4 obroty = tożsamość, wzajemność połączeń, krawędzie planszy, remisy BFS, brak trasy |
| Tura | Nielegalna akcja nie mutuje stanu/RNG; kolejność dostawa–zagrożenie–wyjście–limit, kontakt i limit obrażeń |
| Podgląd | Dokładna zgodność przewidywanego stanu z zatwierdzoną akcją; brak efektów ubocznych |
| RNG i replay | Znane wektory, te same seedy i komendy dają te same hashe; różne wersje reguł odrzucane |
| Generator | Co najmniej 1000 ustalonych seedów, granice, brak nakładania spawnów, osiągalność logiczna, znana trasa rozwiązania |
| Run i moduły | Każdy moduł osobno, potem istotne pary; nielegalny wybór nagrody, przenoszenie zasobów, śmierć i reset |
| Zapis | Round-trip, ucięcie, zła wersja, złe zakresy, przerwanie zapisu, poprawny backup, niedostępny katalog |
| Achievementy | Każdy predykat pozytywnie i negatywnie, reset runu, deduplikacja, trwałość, ponowna synchronizacja offline |
| Zasoby | Hash, licencja, zgoda i non-AI oddzielnie; plik poza rejestrem oraz placeholder blokują publiczną paczkę |
| Windows | DPI, resize, utrata urządzenia, focus, klawiatura, zmiana głośności, brak audio i kontrolowane zamknięcie |

Osiągalność w grafie nie oznacza wygrywalności przy zagrożeniach i limicie tur. Generator potrzebuje ograniczonego solvera lub certyfikatu sekwencji legalnych akcji oraz osobnego raportu trudności. Nie odrzucaj wszystkich ciekawych seedów tylko dlatego, że prosty bot przegrywa.

## Częstotliwość

W każdym kroku: walidator projektu, testy zmienianego zachowania, wymagane testy integracyjne, `git diff --check`. Od wdrożenia CI: build i CTest Debug/Release na Windows. Brak kodu C++ w zmianie dokumentacyjnej nie wymaga pozorowanych testów gry.

Na bramce core: pełny zestaw, testy seedów, replayów i memory safety w obsługiwanej konfiguracji. Na MVP: prawdziwe uruchomienie Windows, pełny run i udokumentowana ocena czytelności. Na release: czysta maszyna lub VM bez narzędzi deweloperskich, offline, klient Steam zamknięty/otwarty, katalog użytkownika z polskimi znakami, standardowe konto bez administratora.

## Kodek checkpointu

P037-01..04: `application_run_checkpoint` chroni pełną projekcję początku sektora,
`application_checkpoint_encode/decode` sprawdzają niezależne literalne wektory,
a `application_checkpoint_integration` obejmuje 11 000 granic dla 1000 ustalonych
root seedów (1000 świeżych startów i 10 000 przejść z jawnymi fixture zasobów).
Pełne stany po restore i pierwszej turze są równe oryginałom; impuls nie pobiera
długu ponownie. Nie jest to 1000 pełnych runów ani test zapisu plików.
Korpus odrzuca wszystkie ucięcia 0..39, nadmiar 1..64, 1280 zmian pojedynczego bitu
i 40 800 zmian pojedynczego bajtu bez odnowienia checksumy. Dodatkowo 32 768 zmian
pól z odnowioną checksumą i 65 536 par slotów sprawdza semantykę i kanoniczność.
Raport P037-04 opisuje wymuszony Rebuild z celowo pominiętą walidacją reserved.

## Ocena grywalności

P038-01: `windows_checkpoint_read` sprawdza rzeczywiste pliki40 bajtów,
literalne wektory i pełne restore, ucięcia, nadmiar do1MiB, uszkodzenie,
brak, katalog i blokadę uchwytu. Odczyt nie zmienia plików.

P038-02: `windows_checkpoint_stage` sprawdza dokładne bajty staging, retry,
resztki poprzedniego staging, odmowę złej wartości, blokadę i brak katalogu.
Istniejące main i bak pozostają niezmienione.

Automatyczne boty mierzą dominujące strategie, niewygrywalne stany i liczbę używanych akcji. Nie zastępują playtestu. Agent może wykonać obserwację interfejsu, jeśli ma odpowiednie narzędzia, i opisać jej ograniczenia. Nie zmyślaj opinii graczy ani badań. Zewnętrzni testerzy nie podejmują decyzji za autora wizji; ich dowody mogą uzasadniać samodzielną poprawkę balansu. Kontakt z testerami wymaga osobnej autoryzacji.

Każdy zgłoszony problem otrzymuje małe zadanie odtwarzania i osobno małą poprawkę, jeśli połączenie byłoby zbyt duże. Testy screenshotowe wolno opierać tylko na zatwierdzonych assetach; ich wzorce i raporty nie są dowodem non-AI.
