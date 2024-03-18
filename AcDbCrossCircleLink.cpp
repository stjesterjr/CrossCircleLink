#include "AcDbCrossCircleLink.h"


//-----------------------------------------------------------------------------
Adesk::UInt32 AcDbCrossCircleLink::m_version = 1;

ACRX_DXF_DEFINE_MEMBERS(AcDbCrossCircleLink, AcDbEntity, AcDb::kDHL_CURRENT, \
    AcDb::kMReleaseCurrent, 0, ACDBCROSSCIRCLELINK, L"CrossCircleLink")

AcDbCrossCircleLink::AcDbCrossCircleLink() : AcDbEntity() {
    setStart({ 0, 0, 0 });
    setEnd({ 10, 10, 10 });
}

AcDbCrossCircleLink::~AcDbCrossCircleLink() {

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

//- Dxf OUT
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
