# Plan etapów i kryteria przejścia

Autorytatywne wpisy, zależności, zakres i test każdego zadania: [tasks.json](plan/tasks.json). Początkowo **115 wpisów P000–P113**, w tym P000 planowania. Niektóre późne wpisy jedynie rozbijają ekspansję na dzieci; ich zakończenie nie oznacza ukończenia tej ekspansji. Nie realizuj całego wiersza tabeli w jednym uruchomieniu.

| Etap | Zadania | Wynik i bramka |
| --- | --- | --- |
| M0 Plan | P000 | Koncepcja, reguły treści, kolejka, skills, walidator i publiczne repo |
| M1 Fundament | P001–P005 | Rzeczywisty toolchain, minimalny C++20 + CTest, format, presety i CI Windows |
| M2 Obieg | P006–P025 | Porty, BFS, komendy, pakiet, impuls, transakcja tury, replay i niewizualny dowód wyróżnika |
| M3 Run | P026–P035 | Overdrive, dług, generator z certyfikatem, dwa moduły i trzy sektory |
| M4 Trwałość | P036–P054 | Checkpointy, profil, pięć lokalnych achievementów; pełna bramka headless |
| M5 Pochodzenie | P055–P061 | Dossier, konkretna zgoda, jeden import, manifesty i blokada placeholderów |
| M6 MVP Windows | P062–P081 | Prezentacja zatwierdzonymi elementami, sterowanie, zapis, audio i obserwowany run |
| M7 1.0 | P082–P089 + dzieci | Trzy zagrożenia, sześć sektorów, moduły i warianty; każda cecha osobno |
| M8 Finalne treści | P090–P093 + dzieci | Zewnętrzne nie-AI napisy, brak placeholderów, wymagane noty i credits |
| M9 Steam | P094–P100 | Legalny lokalny SDK, opcjonalny backend, retry, prawdziwe mapowanie achievementów |
| M10 Kandydat | P101–P105 + dzieci | Allowlista paczki, audyt treści, czysty Windows i zmierzone budżety |
| M11 Premiera | P106–P112 + dzieci | Aktualna ankieta, ludzkie materiały sklepu, formalności, beta i dossier wydania |
| M12 Utrzymanie | P113 i kolejne małe zadania | Smoke wydania i regresje; bez niezamówionego rozwoju zakresu |

## Jak czytać wpis zadania

`scope` ogranicza zmianę; `acceptance` definiuje dowód; `depends_on` wymaga rzeczywistego ukończenia wszystkich poprzedników; `gate` przypomina o uprawnieniach i brakach. Wpisy nie są automatycznym zezwoleniem na pobranie assetów, użycie konta partnera, opłatę ani publikację.

Przy zbyt szerokim wpisie zaplanuj dzieci w oddzielnym kroku. Przykład: P085 zamyka wyłącznie plan brakujących modułów. Dzieci P085-01 itd. implementują po jednym module z testami, a P086 otrzymuje zależność od ostatniego wymaganego dziecka. Następny agent nie może przeskoczyć do P086 tylko dlatego, że P085 ma status completed.

Podobnie generator, adapter lub ekran wymagający kilku niezależnych zachowań należy rozbić przed kodowaniem. Użytkownik nie musi zatwierdzać tego podziału. Dodanie nowego rodzaju assetu wymaga jednak osobnego łańcucha dowodów i zgody, nawet jeśli zadanie mechaniki jest gotowe.

## Warunki zatrzymania lub redukcji

- Headless nie potwierdza sensowności obiegu: najpierw mały eksperyment upraszczający, nie produkcja większej zawartości.
- Brak dowodów non-AI lub zgody: brak pobierania, brak renderowanego zamiennika; możliwa dalsza niezależna praca logiczna.
- Finalne napisy, ikony lub materiały sklepu niedostępne: publiczne wydanie zablokowane, nawet jeśli mechanika działa.
- Steamworks lub formalności niedostępne: lokalna gra i testy pozostają użyteczne; nie oznaczamy premiery jako osiągniętej.

Plan nie obejmuje daty premiery ani gwarancji akceptacji przez Valve. Ma dostarczać małe, zweryfikowane przyrosty prowadzące do produktu spełniającego rzeczywiste warunki.
