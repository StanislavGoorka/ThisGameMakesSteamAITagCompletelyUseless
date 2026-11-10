---
name: release-audit
description: Sprawdz jeden maly element paczki gry, integracji Steam lub dossier wydania pod wzgledem testow i pochodzenia tresci. Nie publikuje automatycznie ani nie gwarantuje braku disclosure AI.
---

# Mały audyt wydania

Czytaj [plan Steam](../../../docs/STEAM_RELEASE.md), [politykę](../../../docs/CONTENT_POLICY.md), [testy](../../../docs/TESTING.md) i bieżący wpis kolejki. Nie rozszerzaj pojedynczego zadania na kompletną premierę.

Odnoś się do konkretnego commitu i hasha builda. Testy, opinia z poprzedniej wersji i same rejestry nie wystarczają do uznania nowego artefaktu za poprawny. Pochodzenie, licencja i zgoda są oddzielnymi kontrolami. Każdy asset, napis, ikona achievementu i materiał sklepu ma źródło; publiczna paczka nie zawiera żadnego AI placeholdera, danych testowych i dokumentacji roboczej.

Przy tasku dotyczącym polityki Steam przeczytaj aktualne źródło pierwotne i zapisz datę. Rozróżnij fakt z dokumentacji, interpretację i nierozstrzygnięte pytanie. Nie gwarantuj braku oznaczenia AI i nie ukrywaj użycia generatywnych treści. Nierozstrzygnięty obowiązek ujawnienia blokuje wydanie bez ujawnienia.

Testuj offline, trwałość i idempotencję achievementów. Fake backend nie dowodzi działania prawdziwego konta/App ID. Nie publikuj prywatnego SDK ani danych sesji. Brak dostępu to stan blocked, nie test zaliczony.

Przygotuj konkretny wynik do przeglądu, zanim potrzebna będzie autoryzacja uploadu lub premiery. Samo wywołanie tego skillu nie upoważnia do kontaktowania się z innymi, opłacania Steam Direct, podpisywania umów lub publikacji. Odnotuj rzeczywistą autoryzację i jej zakres; jeżeli już istnieje, nie pytaj ponownie. Oddaj raport next-step z dowodami i zatrzymaj się po jednym zadaniu.
