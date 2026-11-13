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
