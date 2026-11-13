# Zapis i wznowienie sesji

P074 jest podziałem na P074-01 -> P074-02 -> P074-03 -> P074-04 -> P074-05
-> P074-06 -> P074-07. P075 zależy od P074-07. Zamknięcie podziału nie
oznacza implementacji dzieci.

Używamy wyłącznie istniejącego [checkpointu v1](CHECKPOINT_FORMAT.md),
kodeka, `save_checkpoint_file` i `recover_checkpoint_file`. Bez nowego
formatu, migracji, naprawy plików i promocji staging.

Lokalizacja: `LocalAppData/ClosedCircuit/checkpoint.bin`, w tym samym
prywatnym katalogu co profil, z tym samym kontraktem jednego procesu
zapisującego. Testy używają własnych katalogów, nie profilu użytkownika.

Odczyt mapuje wynik odzyskiwania na status: `none` (oba pliki nie istnieją),
`resumed` (main albo backup, z pełnym RunState z `restore_checkpoint`),
`corrupt` (ERROR_INVALID_DATA bez poprawnego pliku) i `unavailable`
(pozostałe błędy IO). Odczyt nigdy nie modyfikuje plików.

Teksty: osobne wpisy TextCatalog z pełnym prefiksem `[AI PLACEHOLDER]` dla
uszkodzonego, niedostępnego i niezapisanego checkpointu. Kody Windows,
ścieżki i nazwy wyjątków nie trafiają do wierszy. Brak ID daje odmowę.

Granice zapisu: początek runu, restart i początek kolejnego sektora po
`advance_run` zapisują `capture_checkpoint`. Tura w sektorze, preview,
nagroda przed przejściem i odmowy nie zapisują. Wynik won/lost usuwa
checkpoint, bo zgon kończy run ([GAME_DESIGN](GAME_DESIGN.md)). Usuwanie
zaczyna od backupu, potem main, aby nie odsłonić starszego backupu.

Okno: błąd zapisu lub usunięcia pokazuje oznaczony wiersz i nie blokuje gry.
Wznowienie startuje bezpośrednio w sektorze z originem `restored_checkpoint`;
błąd odczytu zaczyna od ekranu startu z oznaczonym wierszem. Ograniczenie
istniejącego kontraktu: uszkodzony main blokuje kolejne zapisy, nawet gdy
backup pozwolił wznowić; agent nie naprawia go automatycznie.

Bez nowych mechanik, assetów i pobrań. Obserwacja pulpitu jest osobnym
dowodem od testów API i HWND.

P074-01 jest zaimplementowane: `checkpoint_location_in` i `local_checkpoint_location` dzielą z profilem `private_directory_in`; adapter tylko rozwiązuje ścieżkę.
