# Jedno przypisanie obrotu

P079 to podział na sześć dzieci. Zmieniamy tylko rotate;
pozostałe klawisze zachowują KEYBOARD_INPUT. Brak zmian zasad core.

RotationBinding jest poprawną wartością jednej wielkiej litery ASCII A-Z,
domyślnie R. O jest konfliktem z overdrive. Inne bajty są niedozwolone;
nie normalizujemy małych liter ani nie przejmujemy klawiszy systemowych.
To ograniczony kontrakt pierwszej zmiany przypisania, bez ogólnego edytora.

Osobny rotation-key.bin obok profile.bin: dokładnie 16 bajtów.
0..3 magic 43 43 4b 42 (CCKB), 4 wersja 1, 5 litera, 6..7 zera,
8..15 FNV-1a64 bajtów 0..7 little-endian (basis 14695981039346656037,
prime 1099511628211). Obce wersje, długości, reserved, checksum i konflikt
są odrzucane. Profil v1 i checkpoint pozostają bez zmian, bez migracji.

Jeden writer w prywatnym katalogu. Read jest bounded i tylko do odczytu.
Missing daje domyślne R bez tworzenia pliku. Corrupt/unavailable też
pozostawia grywalne R, ale nie pozwala nadpisać pliku przy save.
Save waliduje main; identyczna wartość niczego nie zapisuje. Inna wartość
trafia do .tmp, flush i rename z zastąpieniem main. Nie przenosimy main
na bok. Odmowa zachowuje main, może zostawić staging; jawny retry usuwa
ten problem nadpisaniem staging. Bez backupu tego pojedynczego ustawienia.
Port store zwraca błąd bez zmiany aktywnego przypisania. Jedna para nazw
jest oddzielna od profilu i checkpointu; nie ma wspólnej transakcji.

NativeWindow ma wartość aktywnego przypisania oraz opcjonalny callback
pierwszego fizycznego key-down, przed mapowaniem na InputKey. Callback
może skonsumować naciśnięcie bez komendy. Hold, bit30, utrata focusu i
zmiana mapowania podczas hold nie mogą wydać nowej akcji. WM_SYSKEYDOWN
pozostaje obsługą systemu. Nie drukujemy surowego kodu klawisza.

RunWindow z portem ustawień: F2 zaczyna wybór. Następna litera A-Z poza O
zapisuje ustawienie; dopiero sukces przełącza mapowanie. Konflikt/błąd
zachowuje capture do jawnej ponownej próby. Escape lub focus loss anuluje
capture, czyści preview i pozostawia stan oraz zapis gry bez zmian.
Nowy klawisz nie wykonuje obrotu w chwili przypisania. UI korzysta tylko
z osobnych oznaczonych tekstów [AI PLACEHOLDER] w developer katalogu.
R pozwala przywrócić domyślne przypisanie tą samą ścieżką.

Bootstrap używa istniejącego prywatnego katalogu. Nie ma zapisu przy
wczytywaniu, fallbacku do staging ani automatycznej naprawy korupcji.
Testy restartu uruchamiają własne procesy na własnym katalogu, bez
mutowania rzeczywistego LocalAppData użytkownika.

P079-01 wdraża RotationBinding i kodek pamięciowy. Wektor domyślnego R:
43 43 4b 42 01 52 00 00 e5 33 a5 cf 5e 39 d6 7f. Prywatne pole wartości
uniemożliwia encode niepoprawnej litery. IO i interfejs czekają na dzieci.

P079-02: RotationBindingStore ma operację save, a LocalRotationBindingStore
realizuje ją przez zwalidowane .tmp i rename. Read wymaga absolutnej ścieżki,
zwykłego pliku i dokładnie 16 bajtów, odrzuca reparse oraz błędny kodek.
Identyczny zapis nie otwiera staging. Nie tworzymy katalogu w adapterze.

P079-03: NativeWindow::set_rotation_binding zmienia wyłącznie mapowanie
i usuwa oczekujący klawisz logiczny. Physical held pozostaje do key-up;
focus loss i destrukcja czyszczą go. Opcjonalny binding_key w handlerze
otrzymuje tylko pierwszy key-down przed mapowaniem. Zwrócenie true
konsumuje go bez komendy. Adapter nie zapisuje ustawień i nie ma UI.
