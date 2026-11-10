# Kontrakt rejestrów treści

Format JSON UTF-8, `schema_version: 1`. Puste tablice oznaczają brak zatwierdzonych treści, a nie pomyślny audyt gotowej gry. Żaden z tych plików nie jest zgodą na pobranie.

## assets.json

`assets` zawiera obiekty: `id`, `path` (od katalogu repo, wewnątrz `assets/approved/`), `sha256`, `bytes`, `author`, `source_url`, `source_version`, `license`, `license_evidence`, `non_ai_evidence`, `approval_id`, `status`.

Dozwolony status pliku w tym rejestrze: `approved`. Kwestionowane pliki pozostają poza śledzonym katalogiem i są opisane w dossier. `non_ai_evidence` to niepuste wskazanie konkretnego dowodu autorstwa dla tej wersji, nie sam napis „CC0”. `license_evidence` musi opisywać prawo komercyjnej dystrybucji gry oraz publikowania pliku w publicznym repo. Zapis zgody identyfikuje tę samą stronę i wersję. Hash dotyczy faktycznie publikowanego pliku, nie tylko archiwum.

## approvals.json

`approvals` zawiera: `id`, `source_url`, `source_version`, `status` (`approved`, `pending`, `denied`), `approved_at` (ISO 8601 lub null), `evidence` (odnośnik do rzeczywistej odpowiedzi użytkownika lub wierny cytat z datą), `scope`.

Agent nie zatwierdza własnego wniosku i nie fabrykuje cytatu. Zmiana wersji wymaga ponownego uzgodnienia zakresu. Nie umieszczaj danych prywatnych rozmowy poza krótką zgodą.

## texts.json

`texts` zawiera: `id`, `value`, `status` (`ai_placeholder` lub `approved`), `source_asset_id` (null dla placeholdera). Każdy placeholder zaczyna się od `[AI PLACEHOLDER]`. Zatwierdzony tekst wskazuje zatwierdzony plik źródłowy tekstu w `assets/approved/`, z takim samym łańcuchem dowodowym jak obraz i audio. Wydanie wymaga statusu `approved` dla wszystkich używanych wpisów. Dane liczbowe stanu gry nie są autorskim tekstem, ale otaczająca je etykieta już tak.

## Paczkowanie w przyszłości

P101 wprowadzi osobną allowlistę plików binarnych i zasobów. Kod gry i generowane binaria mają pochodzenie kodowe, nie są wpisami `assets`. Przyjęcie assetu nie zastępuje bramki paczki. Testy, dokumentacja, narzędzia i pliki kwarantanny nie trafiają do depotu Steam.
