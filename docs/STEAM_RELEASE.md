# Przygotowanie wydania Steam

Stan źródeł: 2026-11-10. To plan działań, nie informacja o utworzonym koncie Steamworks, App ID, opłaconej aplikacji ani uzyskanym review.

## Zgodne ujawnienie AI

[Content Survey](https://partner.steamgames.com/doc/gettingstarted/contentsurvey) opisuje generatywne treści trafiające do gry i odbierane przez graczy oraz generowanie podczas działania gry. Narzędzia poprawiające efektywność pracy nie są głównym przedmiotem tej sekcji. Nie obiecujemy decyzji Valve dla konkretnej implementacji.

Cel projektu to spełnienie faktycznych warunków pozwalających nie deklarować generowanych treści, a nie obchodzenie ankiety. Brak oznaczenia nie jest techniczną funkcją gry ani warunkiem, który można zagwarantować przed review. Sprawdź ponownie aktualną ankietę i także treści sklepu/community. Jeśli obowiązek ujawnienia istnieje, nie wolno zaznaczać odpowiedzi przeczącej. Oznacz blokadę i przyczynę.

Każda publiczna paczka, demo i beta musi być wolna od placeholderów AI. Grafiki, dźwięki, tekst, ikony achievementów, capsule art, opisy, napisy trailera i lokalizacje mają zatwierdzone nie-AI pochodzenie. Dokumentacji tego repo nie publikujemy jako instrukcji gry. Szczegółowa bramka: [CONTENT_POLICY.md](CONTENT_POLICY.md).

## Achievementy i SDK

Pierwszy produktowy backend jest lokalny; integracja Steam jest opcjonalnym adapterem. Używaj stabilnych ID niezależnych od tekstów i zapewnij dokładnie jedno lokalne odblokowanie. Offline zachowuje progres. Retry synchronizuje po potwierdzonej gotowości usług i utrwala kolejkę.

[Steamworks Stats and Achievements](https://partner.steamgames.com/doc/features/achievements) dokumentuje odblokowanie i utrwalanie statystyk. Aktualny kontrakt, callbacki i wymagania inicjalizacji sprawdź przy wybranej wersji SDK. Test z fake adapterem nie zastępuje testu prawdziwego App ID. Przykładowe aplikacje testowe Valve nie dowodzą zgodności konfiguracji naszej gry.

SDK i narzędzia partnera tylko z legalnego dostępu; nie wrzucaj prywatnego SDK, kluczy, tokenów, danych podatkowych ani plików sesji do publicznego repo. Sposób redystrybucji potrzebnych bibliotek sprawdź według umowy. Domyślny build bez SDK musi nadal działać.

## Warunki właściciela i harmonogram

[Steam Direct](https://partner.steamgames.com/steamdirect) wymaga formalności partnera, opłaty za produkt i weryfikacji danych, następnie konfiguracji sklepu i builda. Publiczna strona w chwili sprawdzenia podaje opłatę 100 USD, oczekiwanie 21 dni od opłaty dla pierwszych tytułów i co najmniej dwa tygodnie publicznego Coming Soon. **Te wartości trzeba sprawdzić ponownie przed planowaniem daty**, wraz z faktycznym panelem partnera; nie traktować zapisu w repo jako niezmiennej reguły. Review i terminy to zależności zewnętrzne, nie test lokalny.

Agent przygotowuje checklistę, build i dowody. Właściciel wykonuje czynności wymagające jego tożsamości, umów i uprawnień; nie podejmuje przy tym decyzji projektowych za agentów. Ta zależność uniemożliwia uczciwą obietnicę całkowicie samodzielnej formalnej premiery.

## Dossier przed publikacją

1. Hash konkretnego artefaktu, commit, wersja reguł/save, czysta ścieżka builda i lista plików depotu.
2. Testy Debug/Release, deterministyczność, zapisy, offline, Steam achievementy, rzeczywiste QA na czystym Windows.
3. Pokrycie wszystkich plików, napisów i materiałów sklepu rejestrami; zero placeholderów i nierozstrzygniętych praw/non-AI.
4. Finalny tytuł i opisy ze zweryfikowanego źródła ludzkiego, ikony i noty licencyjne. Nie kopiować nazwy roboczej z tego dokumentu.
5. Aktualny Content Survey, zgodność opisów z produktem, prawa redystrybucji i wymagane formalności.
6. Wersja rollbacku, kopie poprawnych zapisów, instrukcja aktualizacji i jedna sprawdzona procedura odzyskania.

Przygotowanie dossier nie publikuje produktu. Upload, zmiana widoczności i premiera wymagają uprawnień oraz autoryzacji danej operacji. Nie pytaj o nie, zanim nie istnieje konkretny, zweryfikowany artefakt. Nie wysyłaj wiadomości do Valve, autorów assetów ani testerów bez autoryzacji kontaktu.
