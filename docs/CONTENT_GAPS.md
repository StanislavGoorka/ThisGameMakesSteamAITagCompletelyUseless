# Audyt brakujących treści — P090

Dokument roboczy, nie jest treścią gry ani propozycją brzmienia. Zawiera
wyłącznie identyfikatory, miejsca użycia, stan i wymagane pochodzenie.
Nie wolno tu dopisywać kandydatów finalnych napisów, opisów, nazw
ani szkiców grafiki ([polityka treści](CONTENT_POLICY.md)).

## Zasady audytu

- Stan pozycji: `placeholder` (wpis `ai_placeholder` w
  [texts.json](../content/texts.json), wartość z prefiksem `[AI PLACEHOLDER]`),
  `approved` (wpis z łańcuchem dowodowym w
  [assets.json](../content/assets.json)), `brak` (gra lub wydanie potrzebuje
  pozycji, a nie istnieje żaden wpis ani plik).
- Wymagane pochodzenie: ludzkie, nie-AI, z licencją na komercyjną dystrybucję
  gry i prawem do publicznego repo albo jawnym wyłączeniem z repo; zgoda na
  konkretny pakiet i wersję przed pobraniem
  ([kontrakt rejestrów](../content/REGISTRY.md)).
- Miejsce użycia wskazuje plik źródłowy i ekran lub stan sesji. Liczby stanu
  gry nie są tekstem autorskim, ale otaczająca je etykieta jest.
- Pozycja `approved` w rejestrze nie dowodzi kompletności ekranu; brak wpisu
  w rejestrze nie znaczy, że ekran nie potrzebuje treści.
- Audyt nie zastępuje przeglądu binariów, wszystkich ekranów i paczki (P101,
  P102) ani obserwacji czytelności (P104).

## Podział (P090)

P090 zamknięto jako podział po kategoriach. Każde dziecko wypełnia tylko
swoją sekcję, bez kodu, tekstów dla gracza, pobrań i zmian rejestrów:

| Dziecko | Sekcja |
| --- | --- |
| P090-01 | Napisy ekranów sesji, HUD i błędów |
| P090-02 | Napisy nagród i modułów |
| P090-03 | Achievementy (komunikat w grze, nazwa i opis Steam, dwie ikony) |
| P090-04 | Grafika, audio, font, ikony aplikacji, tutorial i lokalizacja |
| P090-05 | Noty licencyjne, credits, materiały sklepu i podsumowanie |

P091 zależy od P090-05.

## Napisy ekranów sesji, HUD i błędów

P090-01. Sesja ma trzy ekrany (`SessionScreen`: start, run, result,
`src/application/session_flow.hpp`); okno runu składa napisy w
`RunWindow::show_live` i `status_lines` (`src/platform/windows/run_window.hpp`).
Każda pozycja poniżej ma stan `placeholder`. Wymagane pochodzenie dla
wszystkich: zatwierdzony ludzki tekst UI ze źródła P091, wpis `approved`
z `source_asset_id`; tekst nie może kopiować chronionych napisów innych gier.

| ID texts.json | Miejsce użycia (plik → ekran / stan) | Stan |
| --- | --- | --- |
| developer-window-title | `developer_main.cpp`, `board_view.hpp` → tytuł okna i górny obszar napisu | placeholder |
| session-start | `session_text.hpp` `session_message` → ekran start | placeholder |
| session-won | `session_text.hpp` → ekran result po zwycięstwie runu | placeholder |
| session-death | `session_text.hpp` → ekran result, porażka przez utratę HP | placeholder |
| session-overload | `session_text.hpp` → ekran result, porażka przez limit tur | placeholder |
| start-config-baseline | `session_text.hpp` `session_lines` → start i result, wybrana konfiguracja ID1 | placeholder |
| start-config-range | jw. → konfiguracja ID2 | placeholder |
| start-config-charged | jw. → konfiguracja ID3 | placeholder |
| difficulty-short-turns | `session_text.hpp` `session_lines` → start i result, aktywny bit1 | placeholder |
| difficulty-lean-delivery | jw. → aktywny bit2 | placeholder |
| difficulty-heavy-debt | jw. → aktywny bit4 | placeholder |
| hud-integrity | `hud_text.hpp` → HUD runu, etykieta z liczbą HP | placeholder |
| hud-turns | `hud_text.hpp` → HUD runu, etykieta z liczbą tur | placeholder |
| hud-deliveries | `hud_text.hpp` → HUD runu, etykieta z liczbą dostaw | placeholder |
| checkpoint-corrupt | `checkpoint_text.hpp` `resume_line` → status po starcie, uszkodzony zapis | placeholder |
| checkpoint-unavailable | `checkpoint_text.hpp` `resume_line` → status po starcie, błąd odczytu | placeholder |
| checkpoint-unsaved | `checkpoint_text.hpp`, `run_window.hpp` → status, błąd zapisu granicy sektora | placeholder |
| rotation-binding-help | `run_window.hpp` → wiersz klawisza obrotu (domyślny stan, F2) | placeholder |
| rotation-binding-prompt | `run_window.hpp` → przechwytywanie nowego klawisza | placeholder |
| rotation-binding-conflict | `run_window.hpp` → odrzucony klawisz | placeholder |
| rotation-binding-unavailable | `run_window.hpp` → błąd zapisu klawisza | placeholder |
| rotation-binding-saved | `run_window.hpp` → zapisany klawisz | placeholder |

Ekrany i stany bez żadnego napisu w rejestrze (stan `brak`; czy produkt ich
potrzebuje, rozstrzyga projekt UI przed P092, nie ten audyt):

| Ekran / stan | Obecne zachowanie (kod) | Brakująca pozycja |
| --- | --- | --- |
| Start i result: wybór konfiguracji (←/→) i modyfikatorów (↑/↓) | `session_lines` pokazuje tylko wybór, bez sposobu zmiany | instrukcja klawiszy wyboru |
| Run: HUD | `hud_values` daje HP, tury, dostawy | etykiety rezerwy, długu, numeru i liczby sektorów, limitu tur, wymaganych dostaw |
| Run: sterowanie (ruch, obrót, wait, overdrive, plan, anulowanie) | brak podpowiedzi poza klawiszem obrotu | podpowiedź sterowania (powiązana z tutorialem P090-04) |
| Run: podgląd akcji i trasa pakietu | tylko grafika i HUD preview | brak napisu; prawdopodobnie zbędny |
| Wznowienie udane (`ResumeStatus::resumed`) | `resume_line` nie zwraca wiersza | potwierdzenie wznowienia (opcjonalne) |
| Wyjście, pauza, ustawienia, menu główne | ekranów nie ma; zamknięcie przez okno | etykiety, jeśli ekrany powstaną |
| Nazwa katalogu danych `ClosedCircuit` (`profile_location.hpp`) | widoczna w systemie plików, nie w grze | finalny tytuł produktu (P090-05) |
| Font napisów | systemowy Segoe UI (`text_renderer.hpp`) | ocena w P090-04 |

Komunikaty błędów platformy (kody HRESULT, ścieżki) nie trafiają do gracza:
`checkpoint_text.hpp` przepuszcza wyłącznie ID katalogu. Zgodność sprawdzono
wyszukaniem każdego ID w `src/` (P090-01).

## Napisy nagród i modułów

P090-02. Ekran nagrody to stan `RunPhase::choosing_reward` lub
`ready_for_next` na ekranie run (`RunWindow::show_live`). `reward_lines`
(`src/presentation/reward_text.hpp`) składa instrukcję, etykiety oferowanych
modułów z numerem opcji i wybór; `module_text_id`
(`src/presentation/module_text.hpp`) mapuje każdy moduł katalogu
(`src/core/module_catalog.hpp`, dwanaście ID) na jeden wpis. Stan wszystkich
pozycji: `placeholder`; wymagane pochodzenie jak w P090-01 (ludzki tekst UI
ze źródła P091, wpis `approved` z `source_asset_id`).

| ID texts.json | Miejsce użycia (plik → stan) | Stan |
| --- | --- | --- |
| reward-instruction | `reward_text.hpp` → pierwszy wiersz niepustej oferty | placeholder |
| reward-selected | `reward_text.hpp` → wiersz wybranej opcji z numerem | placeholder |
| reward-error | `run_window.hpp` `show_live` → odrzucone potwierdzenie bez wyboru | placeholder |
| reward-continue | `run_window.hpp` `show_live` → `ready_for_next`, pusta oferta | placeholder |

Pokrycie etykiet modułów (etykieta z numerem opcji jako sufiksem liczbowym):

| ModuleId (wartość) | ID texts.json | Stan |
| --- | --- | --- |
| rotation_range (1) | reward-range | placeholder |
| terminal_rotation (2) | reward-terminal | placeholder |
| delivery_reserve (3) | reward-delivery-reserve | placeholder |
| first_overdrive_debt (4) | reward-first-overdrive | placeholder |
| packet_stride (5) | reward-packet-stride | placeholder |
| pulse_cadence (6) | reward-pulse-cadence | placeholder |
| powered_pulse_barrier (7) | reward-pulse-barrier | placeholder |
| entry_insulation (8) | reward-entry-insulation | placeholder |
| delivery_debt_relief (9) | reward-debt-relief | placeholder |
| overdrive_range (10) | reward-overdrive-range | placeholder |
| reverse_rotation (11) | reward-reverse-rotation | placeholder |
| packet_priority (12) | reward-packet-priority | placeholder |

Każdy moduł katalogu ma wpis; brak modułu bez etykiety. Braki (stan `brak`):

| Ekran / stan | Obecne zachowanie (kod) | Brakująca pozycja |
| --- | --- | --- |
| Run: posiadane moduły | żaden ekran nie wyświetla loadoutu (brak odwołań w `src/presentation`, `src/platform`) | etykiety posiadanych modułów, jeśli projekt UI je pokaże |
| Nagroda: nazwa i opis modułu | jedna etykieta łączy nazwę i efekt | rozdzielenie nazwy i opisu tylko, jeśli wymaga tego projekt UI lub sklep |
| Ikony modułów | moduły nie mają grafiki | ocena w P090-04 |

Zgodność sprawdzono wyszukaniem ID w `src/`: etykiety modułów występują
wyłącznie w `module_text.hpp`, pozostałe cztery w `reward_text.hpp` lub
`run_window.hpp` (P090-02).

## Achievementy

P090-03. Katalog zaimplementowanych ID to `achievement_catalog`
(`src/application/local_profile.hpp`, ID1..12). Powiadomienie w grze tworzy
`AchievementNotice` (`src/application/achievement_notice.hpp`) wyłącznie dla
`first_delivery`; `achievement_line` (`src/presentation/achievement_text.hpp`)
zwraca wiersz tylko dla tego ID. ID2..12 odblokowują się w profilu bez
widocznego komunikatu. Gra nie ma ekranu listy achievementów.

Pozycje Steam na achievement (Steamworks Stats & Achievements): nazwa
wyświetlana, opis, ikona odblokowana i ikona zablokowana. Żadna z nich nie
istnieje w repo ani w rejestrach (stan `brak`; tokeny w kodzie i planie to
wewnętrzne identyfikatory, nie nazwy). Wymagane pochodzenie: nazwa i opis —
ludzki tekst ze źródła P091 (jak P090-01); ikony — ludzka grafika z dossier,
zgodą na pakiet i wersję oraz wpisem `approved` w assets.json, w formacie
wymaganym przez Steamworks (weryfikacja przy P100). Nazwa i ikona muszą być
zgodne z mapowaniem App ID (P100).

| ID / token | Komunikat w grze | Nazwa Steam | Opis Steam | Ikona odblokowana | Ikona zablokowana |
| --- | --- | --- | --- | --- | --- |
| 1 first_delivery | placeholder: achievement-first-delivery | brak | brak | brak | brak |
| 2 first_sector | brak (bez powiadomienia) | brak | brak | brak | brak |
| 3 run_without_overdrive | brak (bez powiadomienia) | brak | brak | brak | brak |
| 4 reconfigured_delivery | brak (bez powiadomienia) | brak | brak | brak | brak |
| 5 completed_run | brak (bez powiadomienia) | brak | brak | brak | brak |
| 6 swift_sector | brak (bez powiadomienia) | brak | brak | brak | brak |
| 7 won_preinstalled_range | brak (bez powiadomienia) | brak | brak | brak | brak |
| 8 won_charged_debt | brak (bez powiadomienia) | brak | brak | brak | brak |
| 9 won_short_turn_limit | brak (bez powiadomienia) | brak | brak | brak | brak |
| 10 won_lean_delivery | brak (bez powiadomienia) | brak | brak | brak | brak |
| 11 won_heavy_overdrive_debt | brak (bez powiadomienia) | brak | brak | brak | brak |
| 12 won_all_modifiers | brak (bez powiadomienia) | brak | brak | brak | brak |

Pozostałe pozycje:

| ID texts.json / pozycja | Miejsce użycia | Stan |
| --- | --- | --- |
| achievement-unavailable | `achievement_text.hpp` `achievement_storage_line` → status runu, niedostępny zapis profilu, Enter ponawia | placeholder |
| Ekran listy achievementów | nie istnieje | brak (tylko jeśli projekt UI go przewidzi; Steam overlay pokazuje listę Steam) |

Razem: 11 komunikatów w grze, 12 nazw, 12 opisów i 24 ikony Steam w stanie
`brak`, 2 napisy `placeholder`. Komunikat w grze dla ID2..12 wymaga także
zmiany kodu powiadomień — osobne zadanie po decyzji projektu UI, nie część
audytu (P090-03).

## Grafika, audio, font, ikony aplikacji, tutorial i lokalizacja

P090-04. Cała grafika sceny pochodzi z jednego zatwierdzonego atlasu
`kenney1bit12-monochrome` (assets.json, zgoda `kenney-1bit-1.2-2026-11-13`,
współrzędne przypięte do hasha w `src/presentation/atlas_tiles.hpp`).
Renderer nie rysuje własnej geometrii ani konturów portów.

| Element sceny | Źródło w kodzie | Kafel atlasu (kolumna, wiersz) | Stan |
| --- | --- | --- | --- |
| Przewód: 5 kształtów × 4 obroty | `tile_for_ports` | (8..12, 0) z obrotem | approved |
| Pole bez portów | `tile_for_ports` | (9, 3) | approved |
| Gracz | `tile_for_symbol(player)`, `unit_projection.hpp` | (24, 0) | approved |
| Pakiet | `SymbolKind::packet` | (45, 8) | approved |
| Impuls | `SymbolKind::pulse` | (25, 7) | approved |
| Odbiornik / zasilony odbiornik | `SymbolKind::receiver`, `powered_receiver` | (39, 14) / (39, 15) | approved |
| Wejście / wyjście sektora | `SymbolKind::entry`, `exit` | (19, 20) / (20, 20) | approved |
| Węzeł stały | `SymbolKind::fixed` | (38, 13) | approved |
| Trasa pakietu w podglądzie | `scene_projection.hpp` | ponownie glif pakietu | approved |

Braki grafiki (stan `brak`):

| Pozycja | Obecne zachowanie (kod) | Uwagi |
| --- | --- | --- |
| Emiter okresowy, opóźniony kondensator, pole zablokowane | pola w `GameState` bez symbolu w `unit_projection.hpp`; obecny generator sektorów ich nie umieszcza | symbol potrzebny dopiero, gdy run je użyje |
| Ikony modułów, konfiguracji i modyfikatorów | tylko napisy | jeśli projekt UI je przewidzi |
| Ikony HUD (HP, tury, rezerwa, dług) | tylko napisy z liczbami | opcjonalne |
| Ikona EXE i okna | brak pliku `.rc` i `LoadIcon`; Windows pokazuje ikonę domyślną | wymagana dla wydania i klienta Steam |
| Splash / ekran ładowania | nie istnieje | opcjonalny |
| Tło planszy | czarne `Clear` celu D2D (`atlas_renderer.hpp`), bez grafiki | opcjonalne |

Audio: port `AudioOutput` zna jeden sygnał `AudioCue::delivery`
(`src/application/audio_output.hpp`), odtwarzany w `RunWindow::play_deliveries`
z zatwierdzonego klipu `comigo4947244-powerup01` (zgoda
`comigo-bleeps-4947244-2026-11-14`). Stan `approved`. Brak sygnałów (stan
`brak`, opcjonalne): obrót, ruch, wait, overdrive, obrażenie, ukończenie
sektora, koniec runu, wybór nagrody, achievement, nawigacja ekranów; brak
muzyki i ustawień głośności.

| Pozycja | Stan | Uwagi |
| --- | --- | --- |
| Font | systemowy Segoe UI, `en-us`, 16 DIP (`text_renderer.hpp`); nie jest w repo ani paczce | zależność od systemu, nie asset; zasady użycia potwierdzić w audycie paczki P102 |
| Tutorial i opis sterowania | brak: gra nie ma tutorialu, ekranu pomocy ani instrukcji poza wierszem klawisza obrotu | dokumentacja repo nie może go zastąpić ([polityka](CONTENT_POLICY.md)); wymaga ludzkiej treści lub świadomej redukcji UI |
| Lokalizacja | brak: jeden zestaw napisów roboczych po angielsku, bez tłumaczeń | deklaracja języków w Steam musi odpowiadać faktycznym finalnym tekstom |

Zgodność sprawdzono odczytem `atlas_tiles.hpp`, `unit_projection.hpp`,
`scene_projection.hpp`, `game_state.hpp`, `audio_output.hpp`,
`run_window.hpp` i wyszukaniem `.rc`/`LoadIcon` w `src/` (P090-04).

## Noty licencyjne, credits i materiały sklepu

P090-05. Noty zatwierdzonych pakietów (assets.json, approvals.json):

| Pakiet / zgoda | Nota w repo | Wymóg atrybucji | Stan |
| --- | --- | --- | --- |
| Kenney 1-Bit Pack 1.2 (`kenney-1bit-1.2-2026-11-13`) | `kenney1bit12-license` → `assets/approved/kenney-1bit-1.2/License.txt` | CC0-1.0; wzmianka o autorze prośbą, „not mandatory” | approved |
| Comigo's Bleeps n Bloops, upload itch.io 4947244 (`comigo-bleeps-4947244-2026-11-14`) | brak pliku noty: archiwum nie zawiera not (assets.json `license_evidence`) | CC0-1.0 ze strony autora; brak wymogu | approved bez noty |

Kod: repo nie zawiera bibliotek stron trzecich ani ich licencji
(`git ls-files`, CMake bez FetchContent/find_package). Gra linkuje biblioteki
systemowe Windows; redystrybucja runtime MSVC należy do allowlisty paczki
(P101), nie do tego audytu.

| Pozycja | Obecne zachowanie | Stan | Uwagi |
| --- | --- | --- | --- |
| Ekran credits / not | nie istnieje | brak | P093 wyświetla wiernie skopiowane zatwierdzone noty; CC0 nie wymaga atrybucji, więc zakres credits zależy od decyzji właściciela |
| Napis otaczający noty (nagłówek ekranu, nawigacja) | nie istnieje | brak | ludzki tekst UI ze źródła P091 |

Materiały sklepu i społeczności Steam. Żaden nie istnieje w repo ani
rejestrach (stan `brak`). Wymagane pochodzenie: ludzki tekst lub grafika
z dossier, zgodą i wpisem `approved`; zakaz AI capsule, trailera i tłumaczeń
(P107). Dokładne wymiary i obowiązkowość każdej pozycji należy sprawdzić
w aktualnej dokumentacji Steamworks przy P107; ten audyt ich nie zakłada.

| Pozycja | Stan | Uwagi |
| --- | --- | --- |
| Finalny tytuł produktu | brak | nazwa robocza istnieje tylko jako placeholder `developer-window-title`; STEAM_RELEASE zabrania jej kopiowania |
| Krótki opis i długi opis („About”) | brak | ludzki tekst; zgodność z faktycznym produktem |
| Capsule sklepu (header, small, main, vertical) | brak | ludzka grafika |
| Zasoby biblioteki (capsule, hero, logo, header) | brak | ludzka grafika |
| Zrzuty ekranu | brak | rzeczywiste zrzuty finalnego builda bez placeholderów; widoczny tekst musi być finalny |
| Trailer | brak | opcjonalny; obraz, audio i napisy o ludzkim pochodzeniu |
| Ikona klienta i ikona społeczności | brak | powiązane z ikoną EXE (P090-04) |
| Lista obsługiwanych języków | brak | wynika z finalnych tekstów; obecnie brak lokalizacji (P090-04) |
| Wymagania systemowe | brak | tylko zmierzone (P105) |
| Content Survey | brak | wypełniany zgodnie z faktami (P106) |

## Podsumowanie

P090-05. Stan po audycie P090-01..05 (liczba pozycji; `brak` obejmuje także
pozycje opcjonalne zależne od projektu UI, opisane w sekcjach):

| Kategoria | approved | placeholder | brak |
| --- | --- | --- | --- |
| Napisy sesji, HUD i błędów | 0 | 22 | 8 |
| Napisy nagród i modułów | 0 | 16 | 3 |
| Achievementy | 0 | 2 | 59 (11 komunikatów, 12 nazw, 12 opisów, 24 ikony) |
| Grafika sceny | 1 atlas (14 kafli, 29 wariantów z obrotem) | 0 | 6 |
| Audio | 1 klip | 0 | 1 grupa sygnałów i muzyka (opcjonalne) |
| Font, tutorial, lokalizacja | 0 (font systemowy poza paczką) | 0 | 2 (tutorial, lokalizacja) |
| Noty i credits | 1 nota (Kenney) | 0 | 2 |
| Sklep i społeczność Steam | 0 | 0 | 10 |

Wszystkie 40 wpisów texts.json mają stan `placeholder`; żaden napis widoczny
w grze nie jest zatwierdzony. Grafika sceny i jedyny dźwięk mają kompletny
łańcuch dowodowy. Wydanie publiczne pozostaje zablokowane co najmniej przez:
40 napisów, finalny tytuł, nazwy i opisy achievementów, ikony achievementów,
ikonę EXE/klienta i materiały sklepu.

Wejście dla P091: korpus tekstów musi pokryć 40 kluczy texts.json, finalny
tytuł, 24 nazwy i opisy achievementów, ewentualne napisy braków UI
(P090-01/02/03), credits oraz opisy sklepu. Część pozycji opisuje specyficzne
reguły tej gry (efekty modułów, warunki achievementów), więc P091 musi
ocenić, czy ogólny korpus etykiet UI w ogóle może je pokryć, czy potrzebny
jest ludzki autor z udokumentowanym pochodzeniem. Ten audyt nie rozstrzyga
źródła i nie zawiera propozycji brzmienia.
