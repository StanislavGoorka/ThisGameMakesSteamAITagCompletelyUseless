# Kontrakt wejścia P063

P063 rozbito na trzy zadania. Filtr przechowuje osobny stan każdego z siedmiu
klawiszy logicznych; na początku nie ma focus. Tylko pierwszy key-down przy
focus jest akcją. Key-up odblokowuje następne naciśnięcie. Utrata focus usuwa
stan i oczekującą akcję; wiadomość z bitem powtórzenia po odzyskaniu focus
nie może naliczyć nowej akcji. Nieznany klawisz jest ignorowany.

Mapowanie: strzałki oznaczają ruch N/E/S/W; R obraca pole gracza, O wykonuje
overdrive tego pola, Space czeka. Ruch przez granicę planszy nie ma kandydata.
Pozostała legalność należy do istniejącego resolvera, który przy odmowie nie
zmienia stanu. Mapowanie nie czyta plików, nie zapisuje i nie nalicza tur.
Sterowanie celami sąsiednimi i wskazywanie myszą są poza tym kontraktem.

Adapter Win32 przechowuje najwyżej jedną akcję do pobrania. Kolejne naciśnięcia
przed pobraniem nie tworzą kolejki przyszłych tur. Pobranie konsumuje akcję
nawet przy braku kandydata na krawędzi planszy. Focus loss oraz zniszczenie
okna usuwają oczekujący wpis. WM_SYSKEYDOWN pozostaje w obsłudze systemu,
w szczególności Alt+F4. P063 nie podłącza jeszcze sesji ani widoku planszy.

Testy są niewyświetlane. P063-01 sprawdza filtr, P063-02 wszystkie 49 pól,
a P063-03 rzeczywiste wiadomości Windows. P064 zależy od ostatniego dziecka.

P063-01 wdraża KeyboardInput bez Win32 i Command: siedem niezależnych stanów held, domyślny brak focus, brak akcji dla repeat/unknown i reset przy focus loss. Powtórne powiadomienie focus=true nie resetuje trzymanego klawisza.
