#include "AcDbCrossCircleLink.h"


//-----------------------------------------------------------------------------
Adesk::UInt32 AcDbCrossCircleLink::m_version = 1;

ACRX_DXF_DEFINE_MEMBERS(
    AcDbCrossCircleLink, AcDbEntity,
    AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
    AcDbProxyEntity::kNoOperation, ACDBCROSSCIRCLE,
    CROSSCIRCLEAPP
    | Product Desc : blah-blah-blah
    | Company : boring,inc.
    | WEB Address : localhost
)

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
Acad::ErrorStatus AcDbCrossCircleLink::dxfOutFields(AcDbDxfFiler* filer) const
{
    assertReadEnabled();
    Acad::ErrorStatus result = Acad::eOk;

    //PArent
    result = AcDbEntity::dxfOutFields(filer);
    if (result != Acad::eOk)
        return (result);

    //Class voodoo
    result = filer->writeItem(AcDb::kDxfSubclass, "AcDbCrossCircleLink");
    if (result != Acad::eOk)
        return (result);

    //VERSION
    result = filer->writeUInt32(AcDb::kDxfInt32, AcDbCrossCircleLink::m_version);
    if (result != Acad::eOk)
        return (result);

    //MEMBERS
    result = filer->writePoint3d(AcDb::kDxfXCoord, m_link_start);
    if (result != Acad::eOk)
        return result;

    result = filer->writePoint3d(AcDb::kDxfXCoord, m_link_end);
    if (result != Acad::eOk)
        return result;

    return (filer->filerStatus());
}

Acad::ErrorStatus AcDbCrossCircleLink::dxfInFields(AcDbDxfFiler* filer) {
    assertWriteEnabled();

    Acad::ErrorStatus result;
    resbuf buffer;

    if((AcDbEntity::dxfInFields(filer) != Acad::eOk || !filer->atSubclassData(L"AcDbCrossCircleLink")))
        return filer->filerStatus();

    //VERSion
    Adesk::UInt32 version;
    filer->readItem(&buffer);
    if (buffer.restype != AcDb::kDxfInt32)
    {
        filer->pushBackItem();
        filer->setError(
            Acad::eInvalidDxfCode,
            L"\nError: expected group code %d (version)",
            AcDb::kDxfInt32);
        return filer->filerStatus();
    }


    version = buffer.resval.rint;
    if (version > m_version) {
        return Acad::eMakeMeProxy;
    }

    AcGePoint3d link_start;
    AcGePoint3d link_end;

    while ((result == Acad::eOk) && ((result = filer->readResBuf(&buffer)) == Acad::eOk)) {
        switch (buffer.restype)
        {
        case AcDb::kDxfXCoord:
            m_link_start = asPnt3d(buffer.resval.rpoint);
            break;
        case AcDb::kDxfXCoord + 1:
            m_link_end = asPnt3d(buffer.resval.rpoint);
            break;
        default:
            filer->pushBackItem();
            result = Acad::eEndOfFile;
            break;
        }
    }

    if (result != Acad::eEndOfFile)
        return Acad::eInvalidResBuf;
}
