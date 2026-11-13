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
wymagają pełnej walidacji wpisów i źródeł. P061-02/03 pozostają do wdrożenia.

Ograniczenie: bramki pól i rejestru nie skanują napisów zakodowanych w EXE
ani niewymienionych katalogów depotu. P101 i review wszystkich ekranów mają
objąć faktyczną allowlistę produktu. Pusty katalog napisów nie jest dowodem
ukończenia UI, autorstwa ludzkiego ani gotowości wydania.
