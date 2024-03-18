#include "AcDbCrossCircleLink.h"


//-----------------------------------------------------------------------------
Adesk::UInt32 AcDbCrossCircleLink::m_version = 1;

ACRX_DXF_DEFINE_MEMBERS(AcDbCrossCircleLink, AcDbEntity, AcDb::kDHL_CURRENT, \
    AcDb::kMReleaseCurrent, 0, ACDBCROSSCIRCLELINK, L"CrossCircleLink")

AcDbCrossCircleLink::AcDbCrossCircleLink() : AcDbEntity() {
    setStart({ 0, 0, 0 });
    setEnd({ 10, 10, 10 });
    m_line.setStartPoint(m_link_start);
    m_line.setEndPoint(m_link_end);
    m_line.setThickness(5);
    acutPrintf(L"\nConstructor\n");
}

AcDbCrossCircleLink::~AcDbCrossCircleLink() {

    acutPrintf(L"\nDestructor\n");
}



// Dwg
// OUT
Acad::ErrorStatus AcDbCrossCircleLink::dwgOutFields(AcDbDwgFiler* pFiler) const
{
    assertReadEnabled();
    Acad::ErrorStatus result;

    //Parent
    result = AcDbEntity::dwgOutFields(pFiler);
    if (result != Acad::eOk)
        return (result);

    //Version
    result = pFiler->writeUInt32(AcDbCrossCircleLink::m_version);
    if (result != Acad::eOk)
        return (result);

    //Members
    pFiler->writePoint3d(m_link_start);
    pFiler->writePoint3d(m_link_end);

    return (pFiler->filerStatus());
}
// IN
Acad::ErrorStatus AcDbCrossCircleLink::dwgInFields(AcDbDwgFiler* pFiler)
{
    assertWriteEnabled();
    Acad::ErrorStatus result;

    //Parent
    result = AcDbEntity::dwgInFields(pFiler);
    if (result != Acad::eOk)
        return (result);


    //Version
    Adesk::UInt32 version = 0;
    result = pFiler->readUInt32(&version);
    if (result != Acad::eOk)
        return (result);

    if (version > AcDbCrossCircleLink::m_version)
        return (Acad::eMakeMeProxy);

    //Members
    pFiler->readPoint3d(&m_link_start);
    pFiler->readPoint3d(&m_link_end);

    return (pFiler->filerStatus());
}

// Dxf
//OUT
Acad::ErrorStatus AcDbCrossCircleLink::dxfOutFields(AcDbDxfFiler* pFiler) const
{
    assertReadEnabled();
    Acad::ErrorStatus result = Acad::eOk;

    //PArent
    result = AcDbEntity::dxfOutFields(pFiler);
    if (result != Acad::eOk)
        return (result);

    //Class voodoo
    result = pFiler->writeItem(AcDb::kDxfSubclass, "AcDbCrossCircleLink");
    if (result != Acad::eOk)
        return (result);

    //VERSION
    result = pFiler->writeUInt32(AcDb::kDxfInt32, AcDbCrossCircleLink::m_version);
    if (result != Acad::eOk)
        return (result);

    //MEMBERS
    result = pFiler->writePoint3d(AcDb::kDxfXCoord, m_link_start);
    if (result != Acad::eOk)
        return result;

    result = pFiler->writePoint3d(AcDb::kDxfXCoord, m_link_end);
    if (result != Acad::eOk)
        return result;

    return (pFiler->filerStatus());
}
//IN
Acad::ErrorStatus AcDbCrossCircleLink::dxfInFields(AcDbDxfFiler* pFiler) {
    assertWriteEnabled();

    Acad::ErrorStatus result = Acad::eOk;
    resbuf buffer;

    if((AcDbEntity::dxfInFields(pFiler) != Acad::eOk || !pFiler->atSubclassData(L"AcDbCrossCircleLink")))
        return pFiler->filerStatus();

    //VERSion
    Adesk::UInt32 version;
    pFiler->readItem(&buffer);
    if (buffer.restype != AcDb::kDxfInt32)
    {
        pFiler->pushBackItem();
        pFiler->setError(
            Acad::eInvalidDxfCode,
            L"\nError: expected group code %d (version)",
            AcDb::kDxfInt32);
        return pFiler->filerStatus();
    }


    version = buffer.resval.rint;
    if (version > m_version) {
        return Acad::eMakeMeProxy;
    }

    AcGePoint3d link_start;
    AcGePoint3d link_end;

    while ((result == Acad::eOk) && ((result = pFiler->readResBuf(&buffer)) == Acad::eOk)) {
        switch (buffer.restype)
        {
        case AcDb::kDxfXCoord:
            m_link_start = asPnt3d(buffer.resval.rpoint);
            break;
        case AcDb::kDxfXCoord + 1:
            m_link_end = asPnt3d(buffer.resval.rpoint);
            break;
        default:
            pFiler->pushBackItem();
            result = Acad::eEndOfFile;
            break;
        }
    }

    if (result != Acad::eEndOfFile)
        return Acad::eInvalidResBuf;
}


//AcDbEntity

//DRAW SUB WORLD
Adesk::Boolean AcDbCrossCircleLink::subWorldDraw(AcGiWorldDraw* worldDraw) {
    assertReadEnabled();
    AcGePoint3d ptArray[3];
    ptArray[0] = m_link_start;
    ptArray[1] = m_link_end;
    ptArray[2] = { 0,0,0 };


    worldDraw->subEntityTraits().setSelectionMarker(1);
    worldDraw->subEntityTraits().setColor(1);

    worldDraw->geometry().circle({ 1,0,0 }, { 1,0,0 }, { 0,0,1 });
    return (Adesk::kTrue);
}

Acad::ErrorStatus
AcDbCrossCircleLink::subGetOsnapPoints(
    AcDb::OsnapMode       osnapMode,
    Adesk::GsMarker       gsSelectionMark,
    const AcGePoint3d& pickPoint,
    const AcGePoint3d& lastPoint,
    const AcGeMatrix3d& viewXform,
    AcGePoint3dArray& snapPoints,
    AcDbIntArray& geomIds) const
{
    assertReadEnabled();
    return (AcDbEntity::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds));
}

Acad::ErrorStatus AcDbCrossCircleLink::subGetGripPoints(
    AcGePoint3dArray& gripPoints,
    AcDbIntArray& osnapModes,
    AcDbIntArray& geomIds) const
{
    assertReadEnabled();


    AcGePoint3dArray aCrCiLink;
    aCrCiLink.setLogicalLength(3);

    aCrCiLink[0] = m_link_start;

    aCrCiLink[1] = {
        (m_link_start.x + m_link_end.x) / 2,
        (m_link_start.y + m_link_end.y) / 2,
        (m_link_start.z + m_link_end.z) / 2 
    };

    aCrCiLink[2] = m_link_end;

    gripPoints.append(aCrCiLink);
    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircleLink::subMoveGripPointsAt(
    const AcDbIntArray& indices,
    const AcGeVector3d& offset)
{

    if (indices.length() == 0 || offset.isZeroLength())
        return Acad::eOk;

    assertWriteEnabled();

    for (int i = 0; i < indices.length(); i++)
    {
        switch (indices[i])
        {
        case 0: // 
            this->setStart(m_link_start + offset);
            break;
        case 1:
            this->setStart(m_link_start + offset);
            this->setEnd(m_link_end + offset);
            break;
        case 2: // центр
            this->setEnd(m_link_end + offset);
            break;
        }
    }

    return Acad::eOk;
}

Acad::ErrorStatus AcDbCrossCircleLink::subGetGripPoints(
    AcDbGripDataPtrArray& grips, const double curViewUnitSize, const int gripSize,
    const AcGeVector3d& curViewDir, const int bitflags) const
{
    assertReadEnabled();

    return (AcDbEntity::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags));
}

Acad::ErrorStatus AcDbCrossCircleLink::subMoveGripPointsAt(
    const AcDbVoidPtrArray& gripAppData, const AcGeVector3d& offset,
    const int bitflags)
{
    assertWriteEnabled();

    return (AcDbEntity::subMoveGripPointsAt(gripAppData, offset, bitflags));
}

Acad::ErrorStatus
AcDbCrossCircleLink::subTransformBy(const AcGeMatrix3d& xform) {
    assertWriteEnabled();

    m_link_start.transformBy(xform);
    m_link_end.transformBy(xform);

    return Acad::eOk;
}

Acad::ErrorStatus
AcDbCrossCircleLink::subExplode(AcDbVoidPtrArray& entitySet) const
{
    assertReadEnabled();

    Acad::ErrorStatus result = Acad::eOk;

    AcGePoint3d center = {
        (m_link_start.x + m_link_end.x) / 2,
        (m_link_start.x + m_link_end.x) / 2,
        (m_link_start.x + m_link_end.x) / 2
    };

    // Первый отрезок
    AcDbLine* line;

    line = new AcDbLine();
    line->setStartPoint(m_link_start);
    line->setEndPoint(center);

    entitySet.append(line);

    // Второй отрезок
    line = new AcDbLine();
    line->setStartPoint(center);
    line->setEndPoint(m_link_end);

    entitySet.append(line);

    return result;
}