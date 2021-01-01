#include <iostream>
#include <vector>
#include <functional>
#include <limits>

#include <gp_Pnt.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS.hxx>

#include <BRepLib.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <GC_MakeSegment.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepAlgoAPI_Fuse.hxx> 
#include <BRepOffsetAPI_MakeThickSolid.hxx> 
#include <BRepOffsetAPI_ThruSections.hxx>
#include <TopExp_Explorer.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>

void log(const char* msg) {
    std::cout << msg << std::endl;
}

void exploreEdges(
    const std::function<void(const TopoDS_Edge&)> &edgeConsumer, 
    const TopoDS_Shape &shape) {

    TopExp_Explorer edgeExplorer(shape, TopAbs_EDGE);

    while(edgeExplorer.More()) {
        auto edge = TopoDS::Edge(edgeExplorer.Current());

        edgeConsumer(edge);

        edgeExplorer.Next();
    }
}

void exploreFaces(
    const std::function<void(const TopoDS_Face&)> &faceConsumer,
    const TopoDS_Shape &shape) {

    TopExp_Explorer faceExplorer(shape, TopAbs_FACE);

    while(faceExplorer.More()) {
        auto face = TopoDS::Face(faceExplorer.Current());

        faceConsumer(face);

        faceExplorer.Next();
    }
}

int main() {
    log("OpenCascade hip flask example https://old.opencascade.com/doc/occt-7.3.0/overview/html/occt__tutorial.html");

    double height = 70;
    double width = 50;
    double thickness = 30;

    log("Creating points...");

    gp_Pnt aPnt1(-width / 2.0, 0, 0);
    gp_Pnt aPnt2(-width / 2.0, -thickness / 4.0, 0);
    gp_Pnt aPnt3(0, -thickness / 2.0, 0);
    gp_Pnt aPnt4(width / 2.0, -thickness / 4.0, 0);
    gp_Pnt aPnt5(width / 2.0, 0, 0);

    log("Creating line segments/arcs...");

    Handle(Geom_TrimmedCurve) circleArc = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) seg1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) seg2 = GC_MakeSegment(aPnt4, aPnt5);

    log("Creating wire for first half of profile...");

    TopoDS_Wire wire = BRepBuilderAPI_MakeWire(
        BRepBuilderAPI_MakeEdge(aPnt1, aPnt2),
        BRepBuilderAPI_MakeEdge(circleArc),
        BRepBuilderAPI_MakeEdge(aPnt4, aPnt5));

    log("Mirroring to create second half...");

    // note gp::OX() would also work.
    gp_Ax1 xAxis(
        gp_Pnt(0, 0, 0),
        gp_Dir(1, 0, 0));

    gp_Trsf xMirrorTransform;
    xMirrorTransform.SetMirror(xAxis);

    BRepBuilderAPI_Transform transform(wire, xMirrorTransform);
    TopoDS_Wire mirroredWire = TopoDS::Wire(transform.Shape());

    log("Connecting the two...");

    BRepBuilderAPI_MakeWire wireBuilder;
    wireBuilder.Add(wire);
    wireBuilder.Add(mirroredWire);

    log("Constructing face to act as basis for prism operation...");

    auto closedWire = wireBuilder.Wire();
    TopoDS_Face face = BRepBuilderAPI_MakeFace(closedWire);

    log("Sweeping face...");

    gp_Vec prismVec(0, 0, height);
    TopoDS_Shape body = BRepPrimAPI_MakePrism(face, prismVec);

    log("Adding fillets...");
    BRepFilletAPI_MakeFillet filleter(body);
    std::vector<TopoDS_Edge> edges;
    int edgeCount = 0;
    std::function<void(const TopoDS_Edge&)> edgeConsumer = [thickness, &filleter, &edgeCount] (const TopoDS_Edge &edge) -> void {
        filleter.Add(thickness / 12, edge);
        edgeCount++;
        };

    exploreEdges(edgeConsumer, body);
    std::cout << "Applied fillet to " << edgeCount << " edges..." << std::endl;

    // switch to the filleted body.
    body = filleter.Shape();

    log("Creating neck...");
    Standard_Real neckRad = thickness / 4;
    Standard_Real neckLen = height /10;

    auto neckAxis = gp_Ax2(gp_Pnt(0, 0, height), gp::DZ());
    BRepPrimAPI_MakeCylinder makeCylinder(
        neckAxis,
        neckRad,
        neckLen);

    auto neck = makeCylinder.Shape();

    log("Fusing neck...");
    body = BRepAlgoAPI_Fuse(body, neck);

    log("Hollowing solid...");

    // all fear the face consumer... <_<
    Standard_Real zMax = std::numeric_limits<Standard_Real>::min();
    TopoDS_Face highestFace;
    std::function<void(const TopoDS_Face&)> faceConsumer = [&highestFace, &zMax] (const TopoDS_Face& face) {
        Handle(Geom_Surface) surface = BRep_Tool::Surface(face);

        // trying to find the top of the bottle, so only want flat planes.
        if (surface->DynamicType() != STANDARD_TYPE(Geom_Plane)) {
            return;
        }

        auto plane = Handle(Geom_Plane)::DownCast(surface);
        if (plane->Location().Z() > zMax) {
            zMax = plane->Location().Z();
            highestFace = face;
        }

    };
    exploreFaces(faceConsumer, body);
    std::cout << "Highest face has z coordinate: " << zMax << std::endl;

    TopTools_ListOfShape hollowingFaces;
    hollowingFaces.Append(highestFace);
    BRepOffsetAPI_MakeThickSolid hollower;
    hollower.MakeThickSolidByJoin(body, hollowingFaces, -thickness/50, 1.e-3);
    body = hollower.Shape();

    log("Adding threads...");

    // reuse the neck axis
    Handle(Geom_CylindricalSurface) innerThreadSurface = new Geom_CylindricalSurface(
        neckAxis,
        neckRad * 0.99);

    Handle(Geom_CylindricalSurface) outerThreadSurface = new Geom_CylindricalSurface(
        neckAxis,
        neckRad * 1.05);

    // define a 2d coordinate system for mapping to the cylinders.
    gp_Pnt2d cylinderMappingOrigin(2.0 * M_PI, neckLen / 2);
    gp_Dir2d cylinderMappingXAxis(2.0 * M_PI, neckLen / 4);
    gp_Ax2d cylinderMappingCoords(cylinderMappingOrigin, cylinderMappingXAxis);

    // the way the threads are computed is we make two ellipses
    // mapped to the inner and outer coordinate systems, &
    // create the resulting solid from them.
    Standard_Real aMajor = 2.0 * M_PI;
    Standard_Real aMinor = neckLen / 10;
    Handle(Geom2d_Ellipse) ellipseInner = new Geom2d_Ellipse(
        cylinderMappingCoords,
        aMajor,
        aMinor);

    // Threads get thinner as r increases.
    Handle(Geom2d_Ellipse) ellipseOuter = new Geom2d_Ellipse(
        cylinderMappingCoords,
        aMajor,
        aMinor / 4);


    // cut each ellipse in half, so that the bottom of the
    // threads is a straight line.
    Handle(Geom2d_TrimmedCurve) arcInner = new Geom2d_TrimmedCurve(
        ellipseInner,
        0,
        M_PI);
    Handle(Geom2d_TrimmedCurve) arcOuter = new Geom2d_TrimmedCurve(
        ellipseOuter,
        0,
        M_PI);

    gp_Pnt2d ellipseInnerStart = ellipseInner->Value(0);
    gp_Pnt2d ellipseInnerEnd = ellipseInner->Value(M_PI);
    Handle(Geom2d_TrimmedCurve) segmentInner = GCE2d_MakeSegment(
        ellipseInnerStart,
        ellipseInnerEnd);

    gp_Pnt2d ellipseOuterStart = ellipseOuter->Value(0);
    gp_Pnt2d ellipseOuterEnd = ellipseOuter->Value(M_PI);
    Handle(Geom2d_TrimmedCurve) segmentOuter = GCE2d_MakeSegment(
        ellipseOuterStart,
        ellipseOuterEnd);

    TopoDS_Edge threadInner0 = BRepBuilderAPI_MakeEdge(arcInner, innerThreadSurface);
    TopoDS_Edge threadInner1 = BRepBuilderAPI_MakeEdge(segmentInner, innerThreadSurface);

    TopoDS_Edge threadOuter0 = BRepBuilderAPI_MakeEdge(arcOuter, outerThreadSurface);
    TopoDS_Edge threadOuter1 = BRepBuilderAPI_MakeEdge(segmentOuter, outerThreadSurface);

    TopoDS_Wire threadInner = BRepBuilderAPI_MakeWire(threadInner0, threadInner1);
    TopoDS_Wire threadOuter = BRepBuilderAPI_MakeWire(threadOuter0, threadOuter1);

    BRepLib::BuildCurves3d(threadInner);
    BRepLib::BuildCurves3d(threadOuter);

    // you've had your fun with the sectioning. there'll be no more sectioning today...
    BRepOffsetAPI_ThruSections sectioningTool(Standard_True);
    sectioningTool.AddWire(threadInner);
    sectioningTool.AddWire(threadOuter);
    sectioningTool.CheckCompatibility(Standard_False);
    TopoDS_Shape thread = sectioningTool.Shape();

    TopoDS_Compound compound;
    BRep_Builder builder;
    builder.MakeCompound(compound);
    builder.Add(compound, body);
    builder.Add(compound, thread);

    log("Done!");
}



