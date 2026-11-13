# Dossier P055: Kenney 1-Bit Pack 1.2

Sprawdzono 2026-11-13. Status: wystarczające do konkretnego wniosku P056,
bez zgody i bez przyjętych plików. Czytano wyłącznie strony tekstowe;
nie pobrano archiwum, grafik ani preview. Dossier jest dokumentacją wewnętrzną.

| Pole | Ustalenie i źródło pierwotne |
| --- | --- |
| Autor/dystrybutor | Kenney (Assets), profil [dystrybutora](https://kenney-assets.itch.io/) wskazuje kenney.nl i główny bundle Kenney |
| Konkretny pakiet | [1-Bit Pack](https://kenney-assets.itch.io/1-bit-pack), obecnie plik `1bitpack_kenney_1.2.zip`, deklarowany rozmiar 642 kB |
| Wersja | 1.2; [devlog autora](https://kenney-assets.itch.io/1-bit-pack/devlog/313041/content-update) z 2021-11-09 opisuje dodanie 22 płytek i metadanych importu, pokazuje `kenney_1bitpack_1.2.zip` i 680 kB |
| Zakres zawartości | Ponad 1000 sprite'ów, arkusze i pojedyncze sprite'y, wariant monochromatyczny oraz przezroczysty według strony pakietu |
| Potencjalny cel | Minimalny zestaw istniejących płytek i symboli do przyszłej prezentacji; dopasowanie portów, ostrzeżeń i stanów wymaga przeglądu po zgodzie |

## Licencja gry i publicznego Git

Strona konkretnego pakietu przypisuje CC0 1.0 i dopuszcza projekty komercyjne.
[FAQ Kenney](https://kenney.nl/support) potwierdza użycie komercyjne i brak
obowiązkowej atrybucji; logo Kenney jest zastrzeżone dla projektów autora.
Nie wybieramy logo, brandingu ani grafik reklamowych do importu.

[CC0 Legal Code](https://creativecommons.org/publicdomain/zero/1.0/legalcode.en),
sekcje 2–3: zrzeczenie praw i fallback obejmują kopiowanie oraz rozpowszechnianie
w każdym medium, także komercyjnie. Wniosek projektowy: obejmuje to zarówno
dystrybucję grafiki w grze, jak i niezmienionych, wybranych plików w publicznym
Git. Nie ma wymogu zamknięcia źródeł. CC0 nie obejmuje znaków towarowych i nie
gwarantuje praw osób trzecich. W archiwum trzeba potwierdzić License.txt
i brak wyjątków dotyczących wybranych plików.

## Osobny dowód non-AI

Strona konkretnego pakietu i jego obecnego pliku 1.2 ma w polu Content
deklarację dystrybutora: **No generative AI was used**. To źródłowa deklaracja
dotycząca tego produktu, niezależna od CC0 i daty devloga.
Kenney dodatkowo [oświadcza w dyskusji swojego bundle](https://itch.io/post/17815623),
że jego zawartość powstała bez AI na jakimkolwiek etapie. Ten drugi wpis dotyczy
bundle All-in-1, więc jest wyłącznie wsparciem, nie identyfikacją bajtów
darmowego archiwum 1.2. Nie opieramy ustalenia na stylu obrazków.

Nie jest to niezależny audyt warsztatu autora ani kryptograficzna gwarancja
pochodzenia. Jawne braki do zamknięcia po zgodzie:

- Różne nazwy i rozmiary na stronie pobrania oraz devlogu. Nie zakładać
  identyczności archiwów. Przyjmować tylko bieżący plik 1.2 ze strony pakietu;
  sprawdzić wersję wewnątrz i zapisać rzeczywiste bajty oraz SHA-256.
- kenney.nl pokazuje wpis 1.0/2021, więc nie utożsamiać jego archiwum z 1.2.
  Zgoda na 1.2 z itch.io nie uprawni do niezweryfikowanego wariantu z innego URL.
- Nie sprawdzono License.txt, pełnej listy plików ani mapowania do wszystkich
  stanów gry. Niezgodna wersja/licencja lub brak potrzebnej płytki zatrzymują
  import lub prezentację; nie wolno dorobić grafiki kodem.
- Font, audio, finalne teksty, ikony achievementów i sklep są poza dossier.

## Zakres proponowanego wniosku P056

Jedno darmowe pobranie `1bitpack_kenney_1.2.zip` (1.2) ze wskazanej strony
itch.io do ignorowanego `.local/asset-intake/kenney-1bit-1.2/`; bez zakupu
All-in-1 i bez darowizny. Sprawdzić archiwum, licencję i hashe; minimalne
wybrane, niezmienione pliki PNG oraz noty można dopiero potem przyjąć do
`assets/approved/kenney-1bit-1.2/` i rejestru. Nie wykonywać dostarczonych
skryptów, nie importować preview, przykładów scen, logo ani całego pakietu
do publicznego repo. Zgoda musi być rzeczywistą odpowiedzią użytkownika.
