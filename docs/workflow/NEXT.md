# Następny mały krok

Task: P005

## Cel

Dodaj jeden workflow GitHub Actions, który buduje i testuje projekt na Windows x64 w Debug i Release. Zależność P004 jest ukończona. Workflow ma działać bez assetów.

## Przeczytaj

[AGENTS.md](../../AGENTS.md), [skill next-step](../../.agents/skills/next-step/SKILL.md), [STATE.json](STATE.json), [raport P004](history/P004.md), wpis P005 w [tasks.json](../plan/tasks.json), [CMakePresets.json](../../CMakePresets.json) i [README](../../README.md).

## Wykonaj

Umieść workflow w `.github/workflows/`. Wybierz runner Windows zgodny z generatorem `Visual Studio 17 2022` i użyj presetów z README. Ogranicz zadanie do konfiguracji, kompilacji i testów. Kod C++, pakowanie i integracja Steam są poza zakresem.

## Weryfikacja i przekazanie

Sprawdź YAML i lokalnie uruchom komendy configure, build i CTest obu konfiguracji z workflow. Wynik zdalny zapisz dopiero po faktycznym uruchomieniu w GitHub Actions. Uruchom walidator projektu na początku i końcu, zapisz wyniki w `history/P005.md`, zaktualizuj STATE i NEXT oraz wykonaj `git diff --check`. Zakończ lokalnym commitem i sprawdź jego hash oraz zakres. Push wymaga osobnej zgody.
