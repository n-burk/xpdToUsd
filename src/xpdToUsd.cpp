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
#include <pxr/base/vt/array.h>
#include <pxr/base/gf/vec3f.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/usd/usdGeom/basisCurves.h>

#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
    std::string xpdPath = argv[1];
    std::string outputUsdPath = argv[2];

    XpdReader* xpd;
    xpd = XpdReader::open(xpdPath.c_str());
    if (!xpd) {
        cerr << "Failed to open xuv file: " << xpdPath << endl;
        return 2;
    }
    pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew(outputUsdPath);
    pxr::UsdGeomBasisCurves curves = pxr::UsdGeomBasisCurves::Define(stage, pxr::SdfPath("/xpdCurves_0"));
    pxr::UsdAttribute vertexCountsAttr = curves.GetCurveVertexCountsAttr();
    pxr::UsdAttribute pointsAttr = curves.GetPointsAttr();
    pxr::UsdAttribute widthsAttr = curves.GetWidthsAttr();
    pxr::VtArray<float> widths;
    pxr::VtArray<pxr::GfVec3f> points;
    pxr::VtArray<int> vertexCounts;

    while (xpd->nextFace()) {
        while (xpd->nextBlock()) {
            safevector<float> data;
            while (xpd->readPrim(data)) {
                for (unsigned int i = 0; i < xpd->numCVs(); i++) {
                    pxr::GfVec3f point;                    
                    for (unsigned int j = 0; j < 3; j++) {
                        point[j] = i == 0 ? data[3 + j] :  data[(i * 3) + j];
                    }
                    points.push_back(point);
                }
                widths.push_back(1.0);
                vertexCounts.push_back(xpd->numCVs());
            }
        }
    }
    vertexCountsAttr.Set(vertexCounts, pxr::UsdTimeCode::Default());
    pointsAttr.Set(points, pxr::UsdTimeCode::Default());

    xpd->close();
    stage->Save();
    cout << endl << "Success." << endl;

}
