---
name: cpp-core-step
description: Implementuj jedna mala zmiane logiki, testow lub natywnego adaptera C++ tej gry Windows bez silnika. Stosuj podczas implementacji wpisu kolejki, nie do projektowania oprawy.
---

# Mała zmiana C++

Czytaj [architekturę](../../../docs/ARCHITECTURE.md), [reguły](../../../docs/GAME_DESIGN.md) oraz [testy](../../../docs/TESTING.md). Wykonuj wyłącznie bieżący wpis przekazany przez next-step. Jeżeli nie istnieją wymagane narzędzia lub zależności, raportuj brak zamiast zastępować dowód kompilacji analizą statyczną.

Core jest deterministyczny i nie zależy od Windows, plików, renderera ani Steam. Dane liczbowe testów nie są wyświetlaną planszą prototypu. Nie dodawaj artworku, proceduralnych ikon, shaderów ani dźwięku „dla debugowania”. Gra może później składać zatwierdzone elementy; brak zasobu nie daje prawa do jego generowania.

Domyślnie jeden nowy predykat, komenda, funkcja lub wąski kontrakt na krok. Zachowaj kolejność tury, niezmienność przy błędnej komendzie i wspólną implementację preview/commit. Używaj RAII, silnych ID, stabilnego porządku i integerów w symulacji. Nie twórz silnika ogólnego zastosowania ani ECS bez potrzeby.

Testy są wykonywalne w Debug i Release, bez samych `assert`. Sprawdź rezultat pozytywny, właściwy przypadek błędu i istotny niezmiennik. Przy błędzie zapisu lub callbacku uwzględnij powtórzenie i przerwanie operacji, jeśli dotyka ich wpis. Nie pisz testów, które tylko powtarzają nazwy implementacji.

Wykonaj dostępne komendy CMake/CTest z README po ich wdrożeniu, formatter dla zmienionych plików, walidator projektu i kontrolę diff. Nowe komunikaty dla gracza przechodzą katalog tekstów i prefiks placeholdera. Przekaż dokładne wyniki do raportu next-step; nie rozpoczynaj dodatkowej funkcji po zaliczeniu testów.
