# Dossier P091: źródło finalnych tekstów

Sprawdzono 2026-11-15. Status: **blocked** — brak źródła spełniającego zakres
bez decyzji i udziału człowieka. Nie pobrano, nie skopiowano i nie zapisano
żadnych cudzych tekstów; nie kontaktowano się z autorami. Dokument wewnętrzny,
bez propozycji brzmienia.

## Wymagany zakres

Z [audytu P090](../CONTENT_GAPS.md) (podsumowanie): 40 kluczy texts.json
(sesja, HUD, błędy, nagrody, etykiety 12 modułów, 2 napisy achievementów),
finalny tytuł produktu, 12 nazw i 12 opisów achievementów Steam, napisy
ewentualnych nowych elementów UI, napisy ekranu credits, krótki i długi opis
sklepu. Każda pozycja potrzebuje ludzkiego autorstwa, licencji na komercyjną
dystrybucję gry i publiczne repo (albo jawnego wyłączenia z repo), dowodu
non-AI dla konkretnej wersji i zapisu zgody
([kontrakt rejestrów](../../content/REGISTRY.md)).

## Ocenione kategorie źródeł

| Kategoria | Pokrycie zakresu | Wynik |
| --- | --- | --- |
| A. Ogólny licencjonowany korpus etykiet UI (np. tłumaczenia interfejsów projektów open source, terminologia platform, dane lokalizacyjne) | najwyżej ogólne słowa poleceń; nie zawiera tytułu, efektów 12 modułów tej gry, warunków achievementów ani opisów sklepu | odrzucone na etapie zakresu; nie wskazano pakietu, więc nie badano licencji ani non-AI konkretnej wersji |
| B. Ludzki autor: właściciel projektu albo autor zlecony na piśmie | pełne | jedyna kategoria pokrywająca zakres; wymaga działania człowieka, którego agent nie może wykonać ani zasymulować |
| C. Redukcja UI (zastąpienie części napisów zatwierdzonymi kaflami) | częściowe; nie usuwa tytułu, nazw i opisów achievementów ani opisów sklepu | uzupełnienie B, nie samodzielne źródło; decyzja projektu UI |

Kategoria A dodatkowo: kopiowanie chronionych tekstów innych gier jest
zakazane ([polityka](../CONTENT_POLICY.md)), a nowsze wersje otwartych
korpusów mogą zawierać tłumaczenie maszynowe, więc każdy konkretny pakiet
wymagałby osobnego dowodu non-AI dla wersji. Skoro zakres i tak nie jest
pokryty, dalsze badanie A nie odblokowuje P092.

## Dowody wymagane dla kategorii B

1. Autor: tożsamość znana właścicielowi; w Git wystarczy identyfikator roli
   lub pseudonim, bez danych prywatnych.
2. Oświadczenie autora z datą: teksty napisane przez człowieka, bez
   generatywnego AI i bez tłumaczenia maszynowego.
3. Prawa: licencja lub przeniesienie praw obejmujące komercyjną dystrybucję
   gry, materiały sklepu i publikację pliku w publicznym repo.
4. Zakres: lista kluczy z CONTENT_GAPS, którą dostarczony plik pokrywa.
5. Dostawa: plik tekstowy (np. UTF-8) przyjęty do `assets/approved/` z SHA-256,
   wpisem assets.json i zgodą w approvals.json; wpisy texts.json zmieniają
   status na `approved` ze `source_asset_id` dopiero w zadaniach P092.

Agent nie pisze tych tekstów, nie redaguje cudzego tekstu do nowej wersji
i nie fabrykuje oświadczenia ani zgody.

## Decyzja wymagana od właściciela

P091 pozostaje w `blocked` do jednej z odpowiedzi:

- właściciel dostarczy (sam lub przez zleconego autora) ludzkie teksty
  z dowodami 1–5, albo zgodzi się na przyjęcie ich w tym zakresie, gdy będą
  gotowe; lub
- właściciel wskaże konkretny korpus (nazwa, wersja, URL) do oceny w trybie
  dossier, ze świadomością, że pokryje on tylko część zakresu.

Brak odpowiedzi nie jest zgodą. Do czasu decyzji build developerski działa
z oznaczonymi placeholderami, a public build pozostaje zablokowany bramką
Validate-PublicContent.
