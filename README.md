# xpdToUsd
Autodesk Xgen Xpd to Usd curve conversion tool. Converts curve grooms baked down to xpd to UsdBasisCurve geometry

## Building

:>cmake .

:>cmake --build .

## Usage

xpdToUsd <pathToXpd> <pathToUsd_output>

## Exporting xpd from xgen

![image](https://user-images.githubusercontent.com/83418742/188806885-3a791561-4cd7-420c-a48e-25ca00f5f447.png)

In Xgen, change renderer to Xpd and point to a directory and hit "Create Xpd File" to generate file

## Dependencies:

Xpd - Shipped with Autodesk Maya

Usd - Pixar Usd

https://github.com/PixarAnimationStudios/USD

