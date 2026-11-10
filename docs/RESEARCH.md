# Analiza punktów odniesienia

Sprawdzono 2026-11-10. Analiza materiałów producentów i opisów na Steam, nie deklaracja osobistego ogrania lub pełnego przeglądu rynku. Wnioski projektowe są naszymi hipotezami. Nie pobierano grafik, dźwięków, gier ani plików źródłowych.

| Gra i źródło pierwotne | Obserwowany rdzeń | Wniosek dla projektu | Granica inspiracji |
| --- | --- | --- | --- |
| [Into the Breach](https://store.steampowered.com/app/590380/Into_the_Breach/) | Turowe starcia, sygnalizowane ataki i pozycjonowanie | Mała plansza i wiarygodny podgląd skutków | Jedna jednostka, dostawy i przebudowa sieci zamiast mechów i ochrony budynków |
| [FTL](https://store.steampowered.com/app/212680/FTL_Faster_Than_Light/) | Losowa galaktyka i zarządzanie systemami statku | Współzależne systemy dają decyzje o ryzyku | Bez walk statków, tekstowych wydarzeń, załogi i czasu rzeczywistego |
| [Slay the Spire](https://www.megacrit.com/press-kits/slay-the-spire/) | Karty, relikty i różnorodne konfiguracje runu | Kilka silnie oddziałujących modyfikatorów zamiast kosmetycznych bonusów | Bez talii, kart i kopiowania reliktów |
| [Hoplite](https://www.magmafortress.com/p/hoplite.html) | Taktyczny ruch na małych mapach | Każda akcja przestrzenna powinna mieć znaczenie | Sieć połączeń i niezależny ładunek zamiast heksów i walki hoplity |
| [Invisible, Inc.](https://www.klei.com/games/invisible-inc/) | Turowa infiltracja i adaptacja do zagrożeń | Presja czasu ogranicza bezpieczne stanie w miejscu | Bez szpiegów, fabuły i hakowania korporacji |
| [Cogmind](https://www.gridsagegames.com/cogmind/) | Przebudowa robota z odzyskiwanych części | Konfiguracja powinna zmieniać możliwości | Bez ogromnego katalogu sprzętu i narracyjnej eksploracji |
| [Spelunky Classic](https://www.spelunkyworld.com/original.html) | Zmienne jaskinie, skarby i śmiertelne interakcje | Wspólne reguły tworzą emergentne sytuacje | Bez platformówki, jej postaci, poziomów i pułapek |
| [Enter the Gungeon](https://store.steampowered.com/app/311690/Enter_the_Gungeon/) | Uniki, pociski i bogaty arsenał | Wyrazisty podstawowy czasownik utrzymuje spójność | Bez kosztownej w assetach walki pociskami i naśladowania arsenału |

Dodatkowy kontrast: [Vampire Survivors](https://store.steampowered.com/app/1794680/Vampire_Survivors/) buduje lawinowy wzrost przeciw masom przeciwników. Nasz projekt ogranicza liczbę obiektów i wzrost statystyk, rozwijając zależności topologiczne. To wybór zakresu i czytelności.

## Synteza i wybór

Rozważono arenę z automatycznymi atakami (zbyt duży koszt oprawy i ryzyko generyczności), deckbuilder narzędzi (dużo tekstu i ilustracji) oraz przesuwanie całych pomieszczeń (koszt czytelności i zakresu). Wybrano wspólny przełączany obieg: mały koszt silnika, łatwe testy i wyraźny konflikt dostawa–bezpieczeństwo.

Samo połączenie tur, presji czasu i modułów nie wystarcza. Wyróżnikiem ma być sytuacja, w której **jedna zmiana trasy poprawia transport i jednocześnie pogarsza bezpieczeństwo**. Tę tezę sprawdzamy headless przed rozbudową zawartości. Nie deklarujemy odkrycia całkowicie nowego gatunku ani braku podobnego pomysłu na rynku.

Assety analizowanych gier, ich teksty, screeny i branding nie są źródłem zawartości produktu. Brak opowieści zmniejsza zapotrzebowanie na tekst, lecz finalne etykiety UI i achievementów nadal wymagają legalnego źródła niekorzystającego z generatywnego AI.
