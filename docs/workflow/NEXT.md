# Następny mały krok

Task: P075-02

Czytaj AGENTS, next-step, WORKFLOW, STATE i raport P075-01.
Zależności: P075-01; muszą być completed.
Sprawdź git status i walidator.

Dodaj RAII silnika XAudio2 i mastering voice z wstrzykiwaną fabryką,
stanem unavailable po błędzie tworzenia lub krytycznym błędzie silnika
i jawnym ponownym utworzeniem. Bez source voice i buforów.

Kryteria: rzeczywista próba utworzenia daje spójny stan z urządzeniem lub bez;
wstrzyknięte błędy, krytyczny callback, odtworzenie i zwolnienie w poprawnej
kolejności (mastering voice przed silnikiem); bez wyjątków i syntezy.

Czytaj AUDIO_LIFECYCLE.md, audio_output.hpp, CONTENT_POLICY,
DIRECT2D_LIFECYCLE.md i direct2d_target.hpp (wzorzec RAII/wstrzykiwania).
Sprawdź aktualną dokumentację Microsoft XAudio2Create, IXAudio2EngineCallback
i CreateMasteringVoice przed implementacją; linkuj xaudio2.lib z Windows SDK.
Testy nie tworzą próbek audio, także cichych buforów.
Dla C++: cpp-core-step, ARCHITECTURE, GAME_DESIGN i TESTING.
Za szeroki zakres rozbij bez implementacji. Bez nowych mechanik i assetów.
Kod: configure/build/CTest Debug i Release, formatter src/tests/tools.
Uwaga: dysk ma 1 s rozdzielczości czasu; po przywróceniu pliku z mutacji
odczekaj i wykonaj touch przed przebudową.
Zawsze: walidator, diff --check, raport, STATE/NEXT/README i osobny lokalny
commit z kontrolą hasha i zakresu. Bez pobrań i push.
