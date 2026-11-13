# Podgląd akcji: kontrakt P068

P068 jest wyłącznie podziałem planu. Nie implementuje dzieci ani trybu UI.
Zależności: P067-03 -> P068 -> P068-01..04 -> P069.

1. P068-01 zwróci pełny RunResult przez istniejący preview_run_turn.
   Wynik musi być identyczny z resolve_run_turn dla tej samej komendy.
   Dane aktywnego pakietu należy odczytać po istniejącym introduce_packet
   na kopii; trasę wyliczyć istniejącym shortest_route po portach wyniku
   (tylko akcja zmienia porty). Nie kopiować algorytmu faz. Odmowa i terminalny
   run nie mają aktywnej trasy preview. Dostawa nadal zachowuje trasę tej tury,
   nawet gdy w wyniku nie ma już pakietu. Trasa opisuje połączenia po tej akcji,
   nie obiecuje przyszłych tur po kolejnych zmianach planszy.
2. P068-02 oddzieli wskazanie od wykonania. Tab przełącza tryb planowania;
   w nim strzałki/R/O/Space tylko ustawiają kandydata, Enter oddaje go raz,
   Escape anuluje. Utrata focus czyści tryb, kandydata i wskazanie.
   R/O mogą wskazywać wybrane pole; bez wskazania pozostaje własne pole.
   Model nie nalicza tur, nie ocenia achievementów i nie wywołuje usług.
3. P068-03 połączy ten model z komunikatami Win32 i kursorem. Hit-test używa
   tego samego fit_board co renderer: margines nie jest polem, resize zmienia
   geometrię, granice nie zawijają. W trybie bez planowania pozostaje obecna
   natychmiastowa akcja, z opcjonalnym wskazanym celem R/O. W planowaniu
   dopiero Enter wywołuje dotychczasowy resolver sesji. Zasięg, stałe węzły
   i zasoby pozostają regułami core, bez zmiany legalności w warstwie UI.
4. P068-04 użyje tylko gotowych symboli przypiętego atlasu 1.2. Każdy nowy
   wybór wymaga kontroli konkretnej komórki. Dodatkowy znacznik trasy może
   zajmować wolny obszar z lewej strony środka komórki (25% boku), rozłączny
   z centralnym kaflem i narożnikami. Nie rysować własnych linii ani strzałek.
   Anulowanie pokazuje live state, zatwierdzenie dokładnie wcześniej
   przewidziany wynik. Nie publikować starego preview po zmianie stanu.

Testy: pełna równość stanu, wyników i odmów; powtórki bez mutacji; obrót,
nowy pakiet, dostawa i brak trasy; wybór/anulowanie/focus oraz geometryczne
granice; realne komunikaty HWND i porównanie pikseli z zatwierdzonym atlasem.
Debug/Release i formatter, walidator projektu i diff --check dla każdego
dziecka. Brak assetu blokuje prezentację, nie uzasadnia nowej grafiki.

Nie wdrażać HUD, menu startu, modułów, zapisu ani usług w tych dzieciach.
Kontrola wzrokowa wymaga działającego Computer Use; bieżąca sesja nie ma
połączenia z native pipe. Testy pikseli nie zastępują oceny grywalności.
