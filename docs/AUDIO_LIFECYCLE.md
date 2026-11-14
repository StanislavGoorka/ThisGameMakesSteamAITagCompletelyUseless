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

P075-03: XAudio2Output implementuje AudioOutput i posiada XAudio2Device.
RunWindow posiada opcjonalny port przez unique_ptr; entrypoint przekazuje
XAudio2Output przez create_resumed_window. Port jest zwalniany przed COM.
Bez mapowania zdarzeń (P078) i bez plików wszystkie znane sygnały zwracają
unavailable, niezależnie od dostępności urządzenia. Unknown daje unknown.

P078 jest podziałem na P078-01 (dekoder RIFF/WAVE PCM16), P078-02
(właściciel pliku po weryfikacji hashy), P078-03 (RAII source voice)
i P078-04 (port, zdarzenie dostawy i mute). Konsumenci wymagają P078-04.
P077 przyjął jeden niezmieniony PowerUp_01.wav; zakaz tworzenia próbek
w testach pozostaje. Testy dekodera modyfikują tylko metadane kopii pliku.

P078-01: decode_pcm16_wave (application/pcm_wave.hpp) zwraca pożyczony
widok PCM16 bez IO i alokacji. Wymaga RIFF/WAVE, dokładnej długości RIFF,
jednego fmt (16 albo 18 z cbSize=0) i niepustego data z pełnymi ramkami.
Mono/stereo, 8000–192000 Hz, block align i byte rate muszą być spójne.
Limit wejścia 1 MiB, 256 chunków. Nieznane metadane są pomijane; padding
WORD musi istnieć. Dane i fmt mogą wystąpić w dowolnej kolejności.
Widok żyje tylko tak długo jak niezmieniony bufor wejściowy.

Źródła sprawdzone 2026-11-14:
[RIFF](https://learn.microsoft.com/en-us/windows/win32/xaudio2/resource-interchange-file-format--riff-),
[WAVEFORMATEX](https://learn.microsoft.com/en-us/windows/win32/api/mmreg/ns-mmreg-waveformatex).
Obsługiwany podzbiór jest celowo węższy niż całe API XAudio2.

P078-02: ApprovedDeliveryWave::load wymaga absolutnego trusted root,
czyta tylko delivery_audio_reference (ID, ścieżka, 71836 bajtów, SHA-256
z rejestru) przez read_asset_file i sprawdza dekoderem format. Brak lub
błąd daje nullopt. Konstruktor z bajtów jest prywatny; nie ma fallbacku.
Obiekt posiada vector bajtów, a view() oblicza pożyczony widok we własnym
buforze. Kopie i move nie przenoszą zapisanych wskaźników między obiektami;
przeniesiony pusty obiekt daje pusty widok. Bez tworzenia urządzeń/audio.

P078-03: XAudio2Playback posiada ApprovedDeliveryWave, XAudio2Device
i source voice przez pożyczony XAudio2VoiceApi. Wszystkie operacje sesji
wykonuje wątek właściciela w apartamencie COM. Bez copy/move i source
callbacków. Domyślne API używa CreateSourceVoice bez pitch i efektów.
Play przesyła dokładnie pełne, oryginalne data przez SubmitSourceBuffer,
bez loop i bez zmiany częstotliwości. Start wykonuje tylko raz na voice.
Kolejne wywołania dodają ten sam, stale posiadany bufor do kolejki XAudio2.
Played oznacza przyjęcie do kolejki, bez gwarancji słyszalności.

Błąd create/submit/start daje unavailable; voice i jego kolejka są niszczone,
bez niejawnego retry. Błąd submit usuwa także wcześniejszą kolejkę. Krytyczny
callback tylko oznacza urządzenie jako unavailable; kolejny play usuwa voice.
Recreate/destrukcja zawsze niszczy source przed mastering voice, silnikiem
i bajtami WAV. Fabryki muszą żyć dłużej; destroy_source musi zakończyć
wszystkie odczyty danych. Borrowed engine() służy tylko do tworzenia source
na tym samym wątku; wszystkie source muszą zniknąć przed lifecycle silnika.
Tworzenie voice pobiera wskaźnik silnika raz, zanim sprawdzi go na null,
aby callback zmieniający stan nie dał innego wyniku w drugim zapytaniu.

Dokumenty Microsoft sprawdzone 2026-11-14:
[CreateSourceVoice](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2-createsourcevoice),
[SubmitSourceBuffer](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2sourcevoice-submitsourcebuffer),
[Start](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2sourcevoice-start),
[DestroyVoice](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/nf-xaudio2-ixaudio2voice-destroyvoice),
[VOICE_STATE](https://learn.microsoft.com/en-us/windows/win32/api/xaudio2/ns-xaudio2-xaudio2_voice_state).

W produkcie XAudio2Output pozostaje cichy do P078-04; standalone playback
sprawdzono w integracji. Zatwierdzony WAV jest jedynym źródłem próbek.
