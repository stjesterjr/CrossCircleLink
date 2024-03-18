#pragma once
#include "dbxHeaders.h"

#ifdef CROSSCIRCLE_MODULE
#define DLLIMPEXP __declspec(dllexport)
#else
#define DLLIMPEXP
#endif

class DLLIMPEXP AcDbCrossCircleLink : public AcDbEntity
{
public:
// ACRX_DECLARE_MEMBERS macro
    ACRX_DECLARE_MEMBERS(AcDbCrossCircleLink);

protected:
    static Adesk::UInt32 m_version;

public:
    AcDbCrossCircleLink();
    virtual ~AcDbCrossCircleLink();

/////////////////////////////////////////////////////////
// Methods specific to AcDbCrossCircleLink
/////////////////////////////////////////////////////////
// Data Members access functions

    AcGePoint3d link_start() const;
    Acad::ErrorStatus setStart(AcGePoint3d pIns);
    AcGePoint3d link_end() const;
    Acad::ErrorStatus setEnd(AcGePoint3d pIns);

//----- AcDbObject protocols

    virtual Acad::ErrorStatus subGetClassID(CLSID* pClsid) const override;

//- Dwg Filing protocol
    virtual Acad::ErrorStatus dwgOutFields(AcDbDwgFiler* pFiler) const override;
    virtual Acad::ErrorStatus dwgInFields(AcDbDwgFiler* pFiler) override;
//- Dxf Filing protocol
    virtual Acad::ErrorStatus dxfOutFields(AcDbDxfFiler* pFiler) const override;
    virtual Acad::ErrorStatus dxfInFields(AcDbDxfFiler* pFiler) override;


//----- AcDbEntity protocols
//- Graphics protocol
protected:
    virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* mode) override;
    virtual Adesk::UInt32 subSetAttributes(AcGiDrawableTraits* traits) override;
    virtual Acad::ErrorStatus subTransformBy(const AcGeMatrix3d& xform) override;
    virtual Acad::ErrorStatus subExplode(AcDbVoidPtrArray& entitySet) const override;
    
//- Osnap points protocol
public:
    virtual Acad::ErrorStatus subGetOsnapPoints(
        AcDb::OsnapMode osnapMode,
        Adesk::GsMarker gsSelectionMark,
        const AcGePoint3d& pickPoint,
        const AcGePoint3d& lastPoint,
        const AcGeMatrix3d& viewXform,
        AcGePoint3dArray& snapPoints,
        AcDbIntArray& geomIds) const override;
    
    virtual Acad::ErrorStatus subGetOsnapPoints(
        AcDb::OsnapMode osnapMode,
        Adesk::GsMarker gsSelectionMark,
        const AcGePoint3d& pickPoint,
        const AcGePoint3d& lastPoint,
        const AcGeMatrix3d& viewXform,
        AcGePoint3dArray& snapPoints,
        AcDbIntArray& geomIds,
        const AcGeMatrix3d& insertionMat) const override;

//- Grip points protocol
    virtual Acad::ErrorStatus subGetGripPoints(
        AcGePoint3dArray& gripPoints,
        AcDbIntArray& osnapModes,
        AcDbIntArray& geomIds) const override;

    virtual Acad::ErrorStatus subMoveGripPointsAt(
        const AcDbIntArray& indices,
        const AcGeVector3d& offset) override;
    
    virtual Acad::ErrorStatus subGetGripPoints(
        AcDbGripDataPtrArray& grips,
        const double curViewUnitSize,
        const int gripSize,
        const AcGeVector3d& curViewDir,
        const int bitflags) const override;

    virtual Acad::ErrorStatus subMoveGripPointsAt(
        const AcDbVoidPtrArray& gripAppData,
        const AcGeVector3d& offset,
        const int bitflags) override;

private:
// Data Members
    AcGePoint3d m_link_start;
    AcGePoint3d m_link_end;
};

#ifdef CROSSCIRCLE_MODULE
ACDB_REGISTER_OBJECT_ENTRY_AUTO(AcDbCrossCircleLink);
#endif