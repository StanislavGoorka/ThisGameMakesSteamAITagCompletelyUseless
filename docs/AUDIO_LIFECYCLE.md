# Backend audio bez dźwięku zastępczego

P075 jest podziałem na P075-01 -> P075-02 -> P075-03. P076 zależy od
P075-03. Zamknięcie podziału nie oznacza implementacji dzieci.

Zgodnie z [ARCHITECTURE](ARCHITECTURE.md) XAudio2 odtwarza wyłącznie
zatwierdzone WAV. [CONTENT_POLICY](CONTENT_POLICY.md) zakazuje syntezy,
tonów sinusoidalnych, „tymczasowych” próbek i nagrań wbudowanych w kod.
Do czasu P076/P077 żaden sygnał nie ma pliku, więc port nigdy nie odtwarza.

P075-01: identyfikator sygnału jest wartością logiczną (np. dostawa), nie
treścią. Port zwraca played, muted, unavailable albo unknown. Cicha
implementacja zwraca unavailable. Mapowanie zdarzeń tury na sygnały należy
do P078.

P075-02: RAII silnika i mastering voice z wstrzykiwaną fabryką, aby testy
mogły odtworzyć brak urządzenia. Błąd tworzenia lub krytyczny callback
silnika daje unavailable; ponowne utworzenie jest jawne. Zwolnienie:
mastering voice przed silnikiem. Bez source voice, buforów i formatu WAV.

P075-03: developer_window posiada urządzenie przez port. Brak urządzenia
nie blokuje tur, preview, zapisu ani focusu i nie dodaje tekstu. Test
procesu i HWND z niedostępnym urządzeniem.

Testy nie generują próbek audio, także cichych buforów. Odtwarzanie bufora
zatwierdzonego pliku zostanie sprawdzone po imporcie w P078.

P075-01 jest zaimplementowane: `application/audio_output.hpp` (AudioCue::delivery, AudioPlayResult, port AudioOutput, SilentAudioOutput).

P075-02 jest podziałem na P075-04 (RAII pary zasobów) i P075-05
(callback oraz jawne odtworzenie). P075-03 wymaga drugiego dziecka.

P075-04 wdraża XAudio2Device i pożyczony XAudio2Api. Obiekt bez copy/move
tworzy silnik i mastering voice w apartamencie COM wątku sesji. Błąd lub
pusty wynik fabryki usuwa częściowe zasoby; error() zachowuje HRESULT.
Destruktor niszczy voice przed Release silnika. Fabryka żyje dłużej od
adaptera. Testowe uchwyty są nieodczytywanymi tokenami. Bez source voice.

Microsoft Learn sprawdzono 2026-11-14:
[XAudio2Create](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-xaudio2create),
[CreateMasteringVoice](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2-createmasteringvoice).
Linkowanie xaudio2.lib z Windows SDK; bez pobierania SDK i treści.

P075-05: callback wykonuje wyłącznie lock-free zapis pierwszego błędu
do atomic HRESULT. available() natychmiast uwzględnia błąd; callback nie
zwalnia zasobów i nie tworzy urządzenia. recreate() na wątku sesji wykonuje
unregister, DestroyVoice, Release, czyści stary błąd i tworzy nowe zasoby.
API fabryki musi zakończyć callbacki przed powrotem z release_engine.
Rejestracja nieudana nie wymaga unregister. Bez automatycznych prób.

Źródła sprawdzone 2026-11-14:
[Callbacks](https://learn.microsoft.com/en-us/windows/win32/xaudio2/xaudio2-callbacks),
[OnCriticalError](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2enginecallback-oncriticalerror),
[UnregisterForCallbacks](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2-unregisterforcallbacks).
