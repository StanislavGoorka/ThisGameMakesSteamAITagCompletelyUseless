# Lokalny toolchain Windows

Inwentaryzacja P001 wykonana 2026-11-10 na Windows, w procesie AMD64, z katalogu repozytorium. Są to wyniki lokalnych komend, nie wymagania minimalne dla gracza. Niczego nie instalowano ani nie zmieniano globalnego PATH.

## Wersje i lokalizacje

Skróty ścieżek używane poniżej:

- `VS` = `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools`
- `VC` = `VS\VC\Tools\MSVC\14.44.35207`
- `SDK` = `C:\Program Files (x86)\Windows Kits\10`
- `VS_CMAKE` = `VS\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin`

| Narzędzie | Rzeczywista wersja | Lokalizacja |
| --- | --- | --- |
| CMake z bieżącego PATH | 3.26.4 | `C:\mingw64\bin\cmake.exe` |
| CTest z bieżącego PATH | 3.26.4 | `C:\mingw64\bin\ctest.exe` |
| clang-format z bieżącego PATH | 16.0.4 (built by Brecht Sanders) | `C:\mingw64\bin\clang-format.exe` |
| vswhere | FileVersion 3.1.7.39155; ProductVersion 3.1.7+f39851e70f | `C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe` |
| Visual Studio Build Tools 2022 | 17.14.40; installationVersion 17.14.37628.2 | `VS` |
| MSVC x64 | kompilator 19.44.35228; katalog toolsetu 14.44.35207 | `VC\bin\HostX64\x64\cl.exe` |
| Linker x64 | 14.44.35228.0 | `VC\bin\HostX64\x64\link.exe` |
| MSBuild | 17.14.51.32402 | `VS\MSBuild\Current\Bin\amd64\MSBuild.exe` |
| Windows SDK wybrany przez VS | 10.0.22621.0 | `SDK\Include\10.0.22621.0`, `SDK\Lib\10.0.22621.0` |
| Resource Compiler x64 | FileVersion 10.0.22621.5040 | `SDK\bin\10.0.22621.0\x64\rc.exe` |
| Dodatkowy CMake / CTest z VS | oba 3.31.6-msvc6 | `VS_CMAKE\cmake.exe`, `VS_CMAKE\ctest.exe` |

vswhere potwierdził kompletną, uruchamialną instalację VS 2022, bez wymaganego restartu, oraz komponenty `Microsoft.VisualStudio.Component.VC.Tools.x86.x64` i `Microsoft.VisualStudio.Component.VC.CMake.Project`. Odczyt `Microsoft.VCToolsVersion.default.txt` i lista katalogów MSVC wskazały 14.44.35207. Lista katalogów SDK i rejestr `HKLM:\SOFTWARE\Microsoft\Windows Kits\Installed Roots` potwierdziły powyższy SDK.

## Brak PATH a brak instalacji

W początkowej powłoce `Get-Command` znajduje CMake, CTest i clang-format, ale nie znajduje `cl`, `link`, `rc`, `msbuild` ani `vswhere`. Zmienne `VSCMD_VER`, `VCToolsInstallDir`, `WindowsSdkDir` i `WindowsSDKVersion` są puste. To nie jest dowód braku instalacji.

Po wywołaniu `VS\Common7\Tools\VsDevCmd.bat -no_logo -arch=x64 -host_arch=x64` w procesie potomnym komendy MSVC, linkera, RC i MSBuild są dostępne. Środowisko zgłasza `VSCMD_VER=17.14.40`, host i target `x64`, `VCToolsVersion=14.44.35207` oraz `WindowsSDKVersion=10.0.22621.0\`.

`cl.exe /Bv` uruchomił kompilator i podał wersje jego modułów oraz linkera. Zakończył się kodem **2** i diagnostyką **D8003: brakująca nazwa pliku źródłowego**, ponieważ celowo nie podano źródła. Jest to dowód uruchomienia narzędzia, nie zaliczony build. MSBuild i wszystkie wywołania `--version` zakończyły się kodem 0.

Sprawdzono osiem plików: `VC\include\vector`, `VC\lib\x64\libcpmt.lib`, a pod SDK 10.0.22621.0: `Include\...\um\Windows.h`, `Include\...\um\d2d1.h`, `Include\...\ucrt\stdio.h`, `Lib\...\um\x64\kernel32.lib`, `Lib\...\um\x64\d2d1.lib` i `Lib\...\ucrt\x64\ucrt.lib`. Każde `Test-Path -PathType Leaf` zwróciło `True`.

Oba CMake zgłaszają w `-E capabilities` generator `Visual Studio 17 2022`, obsługę platformy `x64` i wyboru toolsetu. Sama lokalizacja `C:\mingw64\bin` nie oznacza konieczności użycia MinGW do budowania tego projektu.

## Komendy weryfikacji

Zwykła powłoka PowerShell; poniższe komendy wykrywania i wersji wykonano podczas P001:

```powershell
Get-Command cmake, ctest, clang-format, cl, link, rc, msbuild, vswhere -All -ErrorAction SilentlyContinue
cmake --version
ctest --version
clang-format --version
$vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
(Get-Item -LiteralPath $vswhere).VersionInfo | Select-Object FileVersion, ProductVersion
& $vswhere -version '[17.0,18.0)' -products '*' -format json -utf8
$vsRoot = & $vswhere -latest -version '[17.0,18.0)' -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 Microsoft.VisualStudio.Component.VC.CMake.Project -property installationPath
$cmakeBin = Join-Path $vsRoot 'Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin'
& (Join-Path $cmakeBin 'cmake.exe') --version
& (Join-Path $cmakeBin 'ctest.exe') --version
foreach ($cmakePath in @('C:\mingw64\bin\cmake.exe', (Join-Path $cmakeBin 'cmake.exe'))) {
    $capabilities = & $cmakePath -E capabilities | ConvertFrom-Json
    $capabilities.generators | Where-Object name -eq 'Visual Studio 17 2022' | ConvertTo-Json -Depth 5
}
Get-Content -LiteralPath (Join-Path $vsRoot 'VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt')
Get-ChildItem -LiteralPath (Join-Path $vsRoot 'VC/Tools/MSVC') -Directory
Get-ChildItem -LiteralPath 'C:\Program Files (x86)\Windows Kits\10\Include' -Directory
Get-ItemProperty -LiteralPath 'HKLM:\SOFTWARE\Microsoft\Windows Kits\Installed Roots' | Select-Object KitsRoot10
```

Uruchomione komendy w środowisku VS (zmiana środowiska dotyczy wyłącznie procesów potomnych):

```powershell
$devCmd = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat'
& cmd.exe /d /c "call `"$devCmd`" -no_logo -arch=x64 -host_arch=x64 && powershell.exe -NoProfile -OutputFormat Text -Command `"Get-Command cl.exe, link.exe, rc.exe, MSBuild.exe | Select-Object Name, Source | Format-List; Get-Item Env:VSCMD_VER, Env:VSCMD_ARG_HOST_ARCH, Env:VSCMD_ARG_TGT_ARCH, Env:VCToolsInstallDir, Env:WindowsSdkDir, Env:WindowsSDKVersion | Format-List`""
& cmd.exe /d /c "call `"$devCmd`" -no_logo -arch=x64 -host_arch=x64 && cl.exe /Bv"
# Oczekiwany dla tego wywołania: exit 2, D8003 (brak źródła).
& cmd.exe /d /c "call `"$devCmd`" -no_logo -arch=x64 -host_arch=x64 && MSBuild.exe -version -nologo"
```

W dodatkowej sondzie PowerShell uruchomionej w środowisku VS odczytano `(Get-Item -LiteralPath (Get-Command <narzędzie>).Source).VersionInfo` dla `cl.exe`, `link.exe` i `rc.exe` oraz wykonano poniższą kontrolę plików:

```powershell
$sdkVersion = $env:WindowsSDKVersion.TrimEnd('\')
$required = @(
    (Join-Path $env:VCToolsInstallDir 'include/vector'),
    (Join-Path $env:VCToolsInstallDir 'lib/x64/libcpmt.lib'),
    (Join-Path $env:WindowsSdkDir "Include/$sdkVersion/um/Windows.h"),
    (Join-Path $env:WindowsSdkDir "Include/$sdkVersion/um/d2d1.h"),
    (Join-Path $env:WindowsSdkDir "Include/$sdkVersion/ucrt/stdio.h"),
    (Join-Path $env:WindowsSdkDir "Lib/$sdkVersion/um/x64/kernel32.lib"),
    (Join-Path $env:WindowsSdkDir "Lib/$sdkVersion/um/x64/d2d1.lib"),
    (Join-Path $env:WindowsSdkDir "Lib/$sdkVersion/ucrt/x64/ucrt.lib")
)
foreach ($path in $required) { Write-Output "$(Test-Path -LiteralPath $path -PathType Leaf) $path" }
```

## Wniosek dla P002

Brak wykrytej blokady narzędziowej. Do minimalnego builda wybierz `Visual Studio 17 2022`, platformę `x64` i dostępny MSVC. CMake/CTest 3.26.4 z bieżącego PATH spełniają wymaganie 3.26+ i zgłaszają właściwy generator; wersja z VS jest również dostępna przez pełną ścieżkę. Nie zmieniaj globalnego PATH. Bezpośrednie wywołania `cl` wymagają środowiska VS, takiego jak powyżej.

P001 nie utworzył CMakeLists, nie kompilował ani nie linkował źródeł i nie uruchomił testów C++. Konfiguracja, build oraz CTest Debug/Release i kontrolowany błąd testu muszą zostać faktycznie sprawdzone w P002. Presety i CI pozostają późniejszymi zadaniami.
