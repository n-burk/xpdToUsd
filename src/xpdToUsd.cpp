// xpdToUsd.cpp : This tool converts Autodesk Xgen Xpd to Pixar UsdBasisCurve Geometry
// Args:
//      string: path to xpd bake down
//      string: path to save out usd file
//
#define NOMINMAX
#define TBB_USE_ASSERT 0
#define TBB_USE_THREADING_TOOLS 0
#define TBB_USE_PERFORMANCE_WARNINGS 0


#include <xpd/src/core/Xpd.h>

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>
#include "pxr/usd/sdf/types.h"
#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usdGeom/basisCurves.h>
#include <pxr/usd/usdGeom/tokens.h>

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>

using namespace std;

const char* FACEIDATTRNAME = "xpd:faceIds"; 
const char* UVLOCATIONS = "xpd:uvLocations";

void xpd2Usd(std::string& xpdPath, std::string& outputUsdPath) {
    XpdReader* xpd;
    xpd = XpdReader::open(xpdPath.c_str());
    if (!xpd) {
        cerr << "Failed to open xuv file: " << xpdPath << endl;
        return;
    }
    pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(outputUsdPath);
    pxr::UsdGeomBasisCurves curves = pxr::UsdGeomBasisCurves::Define(stage, pxr::SdfPath("/xpdConvert/xpdCurves_0"));
    pxr::UsdAttribute vertexCountsAttr = curves.GetCurveVertexCountsAttr();
    pxr::UsdAttribute pointsAttr = curves.GetPointsAttr();
    pxr::UsdAttribute widthsAttr = curves.GetWidthsAttr();
    pxr::UsdAttribute typeAttr = curves.GetTypeAttr();
    // preserving faceIdAttribute
    pxr::UsdAttribute faceIdAttr = curves.GetPrim().CreateAttribute(pxr::TfToken(FACEIDATTRNAME), pxr::SdfValueTypeNames->IntArray);
    // preservering uvLocationAttr
    pxr::UsdAttribute uvLocAttr = curves.GetPrim().CreateAttribute(pxr::TfToken(UVLOCATIONS), pxr::SdfValueTypeNames->Vector3fArray);
    pxr::VtArray<float> widths;
    pxr::VtArray<pxr::GfVec3f> points;
    pxr::VtArray<int> vertexCounts;
    pxr::VtArray<int> faceIds;
    pxr::VtArray<pxr::GfVec3f> uvLocArray;
    while (xpd->nextFace()) {
        int face = xpd->faceid();
        while (xpd->nextBlock()) {
            safevector<float> data;
            while (xpd->readPrim(data)) {
                for (unsigned int i = 0; i < xpd->numCVs(); i++) {
                    pxr::GfVec3f point;
                    for (unsigned int j = 0; j < 3; j++) {
                        point[j] = data[(i * 3) + j + 3];
                    }
                    points.push_back(point);
                }
                pxr::GfVec3f uvLoc;
                uvLoc[0] = data[1];
                uvLoc[1] = data[2];
                widths.push_back(1.0);
                vertexCounts.push_back(xpd->numCVs());
                faceIds.push_back(face);
                uvLocArray.push_back(uvLoc);
            }
        }
    }
    vertexCountsAttr.Set(vertexCounts, pxr::UsdTimeCode::Default());
    pointsAttr.Set(points, pxr::UsdTimeCode::Default());
    typeAttr.Set(pxr::UsdGeomTokensType().linear, pxr::UsdTimeCode::Default());
    faceIdAttr.Set(faceIds, pxr::UsdTimeCode::Default());
    uvLocAttr.Set(uvLocArray, pxr::UsdTimeCode::Default());
    xpd->close();
    stage->Save();
    return;
}

void usd2Xpd(std::string& usdPath, std::string& outputXpdPath, std::string& primPath) {
    // xpd is a face centric geometry format. Usd is not
    pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(usdPath);
    pxr::UsdPrim prim = stage->GetPrimAtPath(pxr::SdfPath(primPath));
    if (!prim.IsValid()) {
        printf("%s is an invalid prim path. Exiting...", primPath.c_str());
        return;
    }
    pxr::UsdGeomBasisCurves curves = pxr::UsdGeomBasisCurves(prim);
    pxr::VtArray<int> curveVertCount;
    pxr::VtArray<int> faceIds;
    pxr::VtArray<int> widths;
    pxr::VtArray<pxr::GfVec3f> points;
    pxr::VtArray<pxr::GfVec3f> uvLocations;
    curves.GetCurveVertexCountsAttr().Get(&curveVertCount, pxr::UsdTimeCode::Default());
    curves.GetPointsAttr().Get(&points, pxr::UsdTimeCode::Default());
    curves.GetWidthsAttr().Get(&widths, pxr::UsdTimeCode::Default());
    curves.GetPrim().GetAttribute(pxr::TfToken(FACEIDATTRNAME)).Get(&faceIds, pxr::UsdTimeCode::Default());
    curves.GetPrim().GetAttribute(pxr::TfToken(UVLOCATIONS)).Get(&uvLocations, pxr::UsdTimeCode::Default());;
    
    safevector<std::string> keys;
    safevector<std::string> blocks;
    blocks.push_back("BakedGroom");
#define PRIM_ATTR_VERSION 3
    XpdWriter* xFile = XpdWriter::open(outputXpdPath, curveVertCount.size(),
        Xpd::Spline, PRIM_ATTR_VERSION,
        Xpd::Object, blocks,
        (float)0, curveVertCount[0],
        &keys);
    if (!xFile) {
        printf("%s : Failed to create XPD file. Exiting...", outputXpdPath.c_str());
        return;
    }

    // cvCount is uniform so we actually don't care about curveVertCount's real number
    int cvCount = curveVertCount[0];
    // organize data by faceIds
    std::map<int, std::vector<int>> faceMap;
    for (auto& index : faceIds) {
        auto faceFind = faceMap.find(index);
        if (faceFind == faceMap.end()) {
            std::pair<int, std::vector<int>> faceIdPair = { index, {} };
            faceMap.emplace(faceIdPair);
        }
    }
    // loop through faceIds and organize
    for (int i = 0; i < faceIds.size(); i++) {
        faceMap[faceIds[i]].push_back(i);
    }
    unsigned int id = 0;
    for (const auto& [faceId, curveIndices] : faceMap) {
        for (int i = 0; i < curveIndices.size(); i++) {
            if (!xFile->startFace(i)) {
                printf("failed to start face in XPD file: %s", outputXpdPath.c_str());
                return;
            }
            else if (!xFile->startBlock()) {
                printf("failed to start black in XPD file: %s", outputXpdPath.c_str());
                return;
            }
            
            
            for (int j : curveIndices) {
                if (curveVertCount[j] != cvCount) {
                    printf("index %i of curve vert count attr at index does not agree with index 0 curveVertCount of %i. Exiting...", i, cvCount);
                    return;
                }
                safevector<float> primData;
                // face id
                primData.push_back((float)id++);
                // uv location data
                primData.push_back(uvLocations[j][0]);
                primData.push_back(uvLocations[j][1]);
                for (int k = 0; k < cvCount; k++) {
                    primData.push_back((float)points[(j * cvCount) + k][0]);
                    primData.push_back((float)points[(j * cvCount) + k][1]);
                    primData.push_back((float)points[(j * cvCount) + k][2]);
                }
                //cv attr
                primData.push_back(1.0); // length
                primData.push_back(widths[j]); // width
                primData.push_back(0.0); // taper
                primData.push_back(0.0); // taper end
                primData.push_back(1.0f); // width vector x
                primData.push_back(0.0f); // width vector y
                primData.push_back(0.0f); // width vector z

                xFile->writePrim(primData);
            }
        }
    }

    xFile->close();
    return;
}

int main(int argc, char* argv[])
{
    std::string inFilePath = argv[1];
    std::string outputPath = argv[2];

    filesystem::path path = inFilePath;
    if (path.extension().string() == ".xpd") {
        xpd2Usd(inFilePath, outputPath);
    }
    if (path.extension() == ".usd" || path.extension() == ".usda" || path.extension() == ".usdc") {
        std::string primPath = argv[3];
        usd2Xpd(inFilePath, outputPath, primPath);
    }
 
    cout << endl << "Success." << endl;

}
