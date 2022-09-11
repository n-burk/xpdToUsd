# xpdToUsd
Autodesk Xgen Xpd to Usd curve conversion tool. Converts curve grooms baked down to xpd to UsdBasisCurve geometry

## Building

:>cmake .

:>cmake --build .

## Usage

xpd2Usd
xpdToUsd pathToXpd pathToUsd_output

usd2Xpd
xpdToUsd pathToUsd pathToXpd_output primPath

## Note about Usd->Xpd conversion
Xpd is a very "growth geometry centric" format as the end client is xgen, an instancing software. So some special attributes need to exist on the basisCurve before conversion. These attributes will exist inside the Usd geometry upon xpd->Usd conversion

xpd:faceIds - VtArray<int>
  
xpd:uvLocations - VtArray<GfVec3f> (we toss the third element as this is just barycentric coordinates of a face)



## Exporting xpd from xgen

![image](https://user-images.githubusercontent.com/83418742/188806885-3a791561-4cd7-420c-a48e-25ca00f5f447.png)

In Xgen, change renderer to Xpd and point to a directory and hit "Create Xpd File" to generate file

## Dependencies:

Xpd - Shipped with Autodesk Maya

Usd - Pixar Usd

https://github.com/PixarAnimationStudios/USD

