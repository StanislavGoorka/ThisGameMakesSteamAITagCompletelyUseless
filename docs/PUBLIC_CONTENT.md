# Kontrakt trybu public P061

P061 jest wyłącznie podziałem planu. Zakres obejmuje niezależne kontrakty
application, rejestru paczki i systemu budowania; każde dziecko ma własne
testy i commit. Zamknięcie planu nie dowodzi blokady placeholderów w produkcie.

- P061-01: jawny odbiorca katalogu w pamięci. Developer dopuszcza poprawnie
  oznaczone placeholdery; public wymaga całej listy approved. Nieznana wartość
  odbiorcy daje odmowę, a nie przyjęcie polityki domyślnej.
- P061-02: skrypt public gate wymaga istniejącej kontroli assetów oraz
  approved-only content/texts.json. Testy na izolowanych kopiach, bez nowych
  napisów produktu. Syntetyczne źródło testu nie jest dowodem autorstwa.
- P061-03: jawny tryb CMake, niezależny od Debug/Release. Public wymaga
  bramki przy każdym buildzie; developer jest domyślny. Przyszły entrypoint
  otrzyma właściwą politykę katalogu. Test negatywny używa własnego checkoutu
  z oznaczonym wpisem i musi odrzucić public build.

P062 zależy od P061-03. Przyszły roboczy tytuł okna oznacza, że build z nim
może być tylko developerski do czasu pozyskania finalnego tytułu ze źródła.
Żaden tryb konfiguracji nie upoważnia do generowania lub pobierania treści.

P061-01 wdraża wymagany parametr `TextAudience` fabryki TextCatalog.
Nie ma domyślnej polityki; caller wybiera developer albo public_release.
Public odrzuca atomowo również mixed listę z jednym poprawnym placeholderem,
a nieznana wartość enum jest błędem także dla pustej listy. Oba tryby nadal
wymagają pełnej walidacji wpisów i źródeł.

P061-02 wdraża `scripts/Validate-PublicContent.ps1`. Najpierw uruchamia
bramkę approved payloadu, a następnie odrzuca każdy wpis nie-approved
lub zawierający marker. `scripts/Test-PublicContent.ps1` w osobnym checkout
sprawdza różnicę developer/public oraz odmowy metadanych. Rzeczywisty
content/texts.json pozostaje pusty; samo PASS nie oznacza gotowego produktu.
P061-03 dodaje opcję CMake `CLOSED_CIRCUIT_PUBLIC_BUILD` (domyślnie OFF),
presety windows-public i windows-public-debug/release. Zarówno Debug jak
i Release mogą być developerskie; nazwa Release nie oznacza treści public.
Cel validate_content uruchamia się przy każdym buildzie i jest zależnością
game_core, więc obejmuje również jawne budowanie konsumenta biblioteki.
Developer używa Validate-AssetPackage, public — Validate-PublicContent.
`text_build_policy.hpp` wystawia build_text_audience dla przyszłego entrypointu;
nie zastępuje walidacji samych wpisów. Testy niewyświetlane mogą jawnie używać
polityki developer również przy kompilacji public, pozostając poza paczką gry.

`Test-PublicContent.ps1 -BuildModes` przełącza rzeczywisty CMake w izolowanym
checkout z oznaczonym wpisem. Developer Debug/Release buduje wskazanego
konsumenta game_core; public Debug/Release musi odmówić przed jego budową.
CI obejmuje również cztery warianty pełnego builda i CTest.

Ograniczenie: bramki pól i rejestru nie skanują napisów zakodowanych w EXE
ani niewymienionych katalogów depotu. P101 i review wszystkich ekranów mają
objąć faktyczną allowlistę produktu. Pusty katalog napisów nie jest dowodem
ukończenia UI, autorstwa ludzkiego ani gotowości wydania.
