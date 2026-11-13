# Kontrakt zasobów Direct2D P064

P064 dzieli backend na P064-01 i P064-02. Własność obejmuje single-threaded
fabrykę i cel HWND, zwalniane przez RAII w kolejności cel, fabryka. HWND jest
pożyczony: okno musi żyć dłużej niż adapter. Tworzenie, użycie i niszczenie na
tym samym wątku co okno. Brak kopiowania i przenoszenia adaptera.

P064-01 tworzy fabrykę oraz cel z rozmiarem obszaru klienta istniejącego HWND.
Null, zniszczony HWND i pusty obszar klienta dają kontrolowany HRESULT bez
opublikowania adaptera. Dwa obiekty posiadają niezależne cele. Domyślny typ
celu pozwala systemowi wybrać urządzenie. Brak własnych tekstur lub shaderów.

P064-02 aktualizuje rozmiar do jawnej liczby pikseli. Zero któregokolwiek
wymiaru zawiesza cel i zwalnia zasoby zależne od niego; powrót do dodatniego
rozmiaru tworzy cel ponownie. Kod D2DERR_RECREATE_TARGET zwalnia cel,
zachowuje fabrykę i pozwala odtworzyć zasoby przy kolejnym resize. Inne błędy
nie są maskowane jako utrata urządzenia. Odmowa po zniszczeniu okna nie może
pozostawić dostępnego celu. Każdy nieudany resize usuwa nieaktualny cel.

Testy sprawdzają prawdziwą fabrykę, HWND target, GetPixelSize i Resize.
Przekazanie statusu D2DERR_RECREATE_TARGET jest kontrolowanym testem ścieżki,
nie fizycznym resetem GPU. Odtworzony cel powstaje przez rzeczywiste API.
Brak BeginDraw/EndDraw, Clear, pędzli, geometrii i prezentacji. Nie ma
proceduralnych assetów ani renderera podłączonego do entrypointu.

Kontrakty systemowe:
[ID2D1HwndRenderTarget](https://learn.microsoft.com/en-us/windows/win32/api/d2d1/nn-d2d1-id2d1hwndrendertarget),
[zasoby i ich domeny](https://learn.microsoft.com/en-us/windows/win32/direct2d/resources-and-resource-domains).
P065 wymaga ukończenia ostatniego dziecka, a nie samego podziału.

P064-01 wdraża Direct2DTarget::create(HWND). Wynik zawiera unique_ptr albo HRESULT; cel i fabryka mają własność ComPtr. Wskaźniki target()/factory() są pożyczone, nie przenoszą własności. Brak publicznego konstruktora, copy i move. Adapter nie niszczy HWND, nie rysuje ani nie prezentuje bufora.

P064-02 wdraża resize(width,height) i handle_draw_result(HRESULT). Zero zwraca S_FALSE i usuwa cel; dodatni rozmiar tworzy lub zmienia cel. Nieudany resize usuwa cel. Status D2DERR_RECREATE_TARGET usuwa wyłącznie zasoby celu, pozostałe statusy są przekazywane bez zmiany celu. Fabryka pozostaje ta sama. Podłączenie WM_SIZE/EndDraw do przyszłego produktu pozostaje poza adapterem.
