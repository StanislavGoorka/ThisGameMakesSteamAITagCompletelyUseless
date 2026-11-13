# Odczyt PNG P065

read_png_asset przyjmuje zaufany root i AssetReference z zatwierdzonego
manifestu. read_asset_file najpierw sprawdza lokalizację, długość i SHA-256;
WIC dostaje wyłącznie cały zweryfikowany bufor. COM inicjalizuje caller na
swoim wątku. Brak inicjalizacji daje CO_E_NOTINITIALIZED bez obrazu.

Systemowy decoder PNG czyta jeden obraz do owning bufora top-down PBGRA
(32 bity, premultiplied alpha), ze stride width*4 i bez paddingu. Limity:
4096 pikseli na wymiar i najwyżej 64 MiB dekodowanych danych. Sprawdzenie
wymiarów poprzedza alokację pikseli. Bufor wejściowy żyje dłużej niż stream,
decoder, frame i converter. Wszystkie błędy odczytu/kodeka zwracają HRESULT
i brak częściowego obrazu; błędy alokacji C++ mogą propagować wyjątek.
Nie ma generowanego fallbacku, zmiany pliku, renderowania ani eksportu PNG.

Test używa wyłącznie zatwierdzonego atlasu P057. Niezależny od WIC odczyt
GDI+ (System.Drawing.Bitmap, Clone Format32bppPArgb, LockBits i kopiowanie
każdego wiersza bez paddingu) dał 832x373, stride3328 i 1241344 bajtów.
SHA-256 pełnego bufora: 50FE0EA51E1FB5C947B3D40D7CAFF4D08800A897323E5B6937563300ADEB850E.
Porównanie jest numerycznym testem dekodera; żaden wynik nie jest zapisywany
ani prezentowany jako nowy asset. Sprawdzono również Unicode root, powtórki,
niezależność bufora, uszkodzoną kopię (z oryginalnym i z odnowionym hashem),
brak pliku, retry oraz zatwierdzoną notę tekstową odrzuconą jako non-PNG.
Syntetyczne odwołanie z odnowionym hashem jest wyłącznie negatywnym fixture,
nie nową zgodą ani wpisem rejestru. Oryginał i manifest pozostają identyczne.

Kontrakty systemowe:
[IWICStream](https://learn.microsoft.com/en-us/windows/win32/api/wincodec/nn-wincodec-iwicstream),
[konwerter formatu](https://learn.microsoft.com/en-us/windows/win32/api/wincodec/nf-wincodec-iwicformatconverter-initialize).
Brak QA wizualnego, fizycznego resetu GPU, VM i połączenia z Direct2D.

Odtworzenie niezależnego odczytu w PowerShell z katalogu repo (wyłącznie
pamięć procesu, bez zapisu obrazu):

```powershell
Add-Type -ReferencedAssemblies System.Drawing -TypeDefinition @'
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
public static class PngOracle {
  public static string Read(string path) {
    using (var source = new Bitmap(path))
    using (var image = source.Clone(new Rectangle(0,0,source.Width,source.Height), PixelFormat.Format32bppPArgb)) {
      var bits = image.LockBits(new Rectangle(0,0,image.Width,image.Height), ImageLockMode.ReadOnly, PixelFormat.Format32bppPArgb);
      var pixels = new byte[image.Width*image.Height*4];
      for (int row=0;row<image.Height;row++) Marshal.Copy(IntPtr.Add(bits.Scan0,row*bits.Stride),pixels,row*image.Width*4,image.Width*4);
      image.UnlockBits(bits);
      using(var hash = SHA256.Create()) return image.Width+"x"+image.Height+" "+BitConverter.ToString(hash.ComputeHash(pixels)).Replace("-","");
    }
  }
}
'@
[PngOracle]::Read((Join-Path $PWD 'assets/approved/kenney-1bit-1.2/Tilesheet/monochrome-transparent.png'))
```
