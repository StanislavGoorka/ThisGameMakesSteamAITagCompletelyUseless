# Decyzje i rejestr ryzyk

## Przyjęte decyzje — 2026-11-10

| Id | Decyzja | Powód / warunek rewizji |
| --- | --- | --- |
| ADR-001 | C++20, Windows x64, Win32 + Direct2D, bez silnika | Mały natywny stos i ścisła zgodność z wizją; P001 sprawdza toolchain |
| ADR-002 | Turowy wspólny obieg na małej siatce | Testowalność i niski koszt oprawy; rewizja skali po headless proof |
| ADR-003 | Core przed jakąkolwiek prezentacją | Brak prawa do generowania placeholderowej grafiki i brak assetów |
| ADR-004 | Jeden mały krok na wznowienie | Przeglądalne zmiany, silne testy i jednoznaczne przekazanie |
| ADR-005 | Oddzielna zgoda na pobranie i dowód non-AI | Dokładne wymaganie użytkownika; sama licencja nie wystarcza |
| ADR-006 | Brak permanentnego wzrostu statystyk | Powtarzalna ocena reguł i ograniczenie grindu |
| ADR-007 | Najpierw lokalne achievementy, potem adapter Steam | Gra i testy działają bez konta partnera, sieci i SDK |
| ADR-008 | Tytuł oraz wszystkie opisy tutaj są wewnętrzne | Brak finalnych tekstów AI w produkcie; wymiana wymaga pochodzenia |
| ADR-009 | Brak licencji własnego kodu na etapie planu | Publiczna dostępność nie powinna automatycznie udzielać szerokich praw; licencje zależności są osobne |

Nowe decyzje dopisuj z dowodem, konsekwencjami i identyfikatorem zadania. Nie eskaluj wyboru nazw klas, bibliotek pomocniczych ani balansu do człowieka.

## Ryzyka i reakcje

| Ryzyko | Sygnał | Reakcja |
| --- | --- | --- |
| Niejasna granica treści AI | Nowa interpretacja ankiety lub treść wykonana przez kod | Audyt konkretnego rezultatu, sprawdzenie aktualnego źródła, blokada zamiast zatajenia |
| Brak zweryfikowanych assetów | Dossier nie potwierdza wersji lub non-AI | Kontynuować headless; szukać innych źródeł bez pobierania; nie generować |
| Brak finalnych napisów / store art | MVP działa wyłącznie na placeholderach | Ograniczyć zakres UI, poszukać legalnego korpusu; wydanie pozostaje zablokowane |
| Obieg jest zbyt skomplikowany | Podgląd nieczytelny, długie rozważanie każdej tury | Mniejsza plansza, jeden rodzaj zagrożenia, mniej modułów; pomiar przed dodawaniem treści |
| Czekanie lub overdrive dominuje | Bot lub obserwacja znajduje jedną bezpieczną strategię | Jeden eksperyment balansu i regresja; nie dokładać waluty |
| Losowe sektory niewygrywalne | Certyfikat nie mieści się w limicie | Ograniczony generator z odrzuceniem i wersjonowaną pulą seedów |
| Rozrost silnika / scope | Krok wymaga wielu systemów naraz | Podzielić zadanie, utrzymać porty i małe typy, odrzucić framework bez potrzeby |
| Brak narzędzi / uprawnień | Build niewykonany | Udokumentować dokładny brak i warunek odblokowania; nie udawać testów |
| Formalności Steam | Brak konta, App ID lub zgody na umowę/opłatę | Przygotować lokalny build i dossier; właściciel wykonuje wymagane formalności |
| Fałszywe poczucie jakości | Boty przechodzą, lecz interfejs niezweryfikowany | Oddzielne QA Windows i obserwowane przejście runu; nie zastępować ich testami unit |

Nie podajemy daty premiery: liczba mikro-kroków zależy od dowodów, zgód i wyników grywalności. Plan jest kolejką zależności, nie obietnicą czasu realizacji.
