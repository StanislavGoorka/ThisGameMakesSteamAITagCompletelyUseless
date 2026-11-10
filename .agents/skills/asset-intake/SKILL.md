---
name: asset-intake
description: Zweryfikuj pochodzenie i licencje nie-AI assetow tej gry, przygotuj zgode przed pobraniem i utrzymaj rejestry. Stosuj tez dla tekstow, ikon achievementow, fontow i marketingu.
---

# Pozyskanie jednej partii treści

Przeczytaj [politykę treści](../../../docs/CONTENT_POLICY.md) i [kontrakt rejestrów](../../../content/REGISTRY.md). Pracuj wyłącznie nad konkretną partią z bieżącego mikro-zadania. Nie używaj narzędzi generatywnych, kodu rysującego nową grafikę ani syntezy audio. Nie twórz finalnego tekstu i nie nazywaj go potem „ręczną redakcją”.

W trybie dossier przeglądaj źródła pierwotne bez pobierania plików, zapisywania preview i kopiowania obrazów. Ustal dokładny pakiet, wersję, autora, źródło praw komercyjnej redystrybucji gry oraz prawo umieszczenia assetu w publicznym Git. Oddzielnie sprawdź non-AI dla tej wersji. Samo CC0, stara data poprzedniej wersji lub ocena stylu nie wystarczają.

Jeśli brakuje dowodów, zapisz brak i szukaj innego źródła; nie wymyślaj oświadczenia autora i nie wysyłaj mu wiadomości bez autoryzacji. Przy wystarczającym dossier przedstaw użytkownikowi konkretny wniosek o pobranie. To wymaganie użytkownika, nie dodatkowa procedura akceptacji estetycznej. Nie pobieraj do czasu wyraźnej odpowiedzi, a odmowę respektuj. Zgoda raz udzielona dla tej wersji i zakresu nie wymaga ponownego pytania.

W trybie importu sprawdź rzeczywistą zgodę w approvals. Pobierz do ignorowanego lokalnego katalogu, zweryfikuj archiwum, hash i noty. Do approved przyjmij minimalne pliki z kompletnymi dowodami i hashami. Nie publikuj plików, na których redystrybucję w repo nie ma prawa. W razie rozbieżności zatrzymaj import, zachowując zadanie jako blocked.

Dla tekstów stosuj `texts.json`: każdy wygenerowany roboczy wpis ma `[AI PLACEHOLDER]` na samym początku; finalny wpis wskazuje zatwierdzone źródło ludzkie. Nie wykorzystuj dokumentacji koncepcji jako etykiet produktu. Wykonaj walidator i negatywny przypadek bieżącej zmiany. Raport nie może twierdzić, że automatyczna kontrola pól dowodzi ludzkiego autorstwa.
