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

Do uzupełnienia w P090-03.

## Grafika, audio, font, ikony aplikacji, tutorial i lokalizacja

Do uzupełnienia w P090-04.

## Noty licencyjne, credits i materiały sklepu

Do uzupełnienia w P090-05.

## Podsumowanie

Do uzupełnienia w P090-05.
