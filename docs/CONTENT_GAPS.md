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

Do uzupełnienia w P090-01.

## Napisy nagród i modułów

Do uzupełnienia w P090-02.

## Achievementy

Do uzupełnienia w P090-03.

## Grafika, audio, font, ikony aplikacji, tutorial i lokalizacja

Do uzupełnienia w P090-04.

## Noty licencyjne, credits i materiały sklepu

Do uzupełnienia w P090-05.

## Podsumowanie

Do uzupełnienia w P090-05.
