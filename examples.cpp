//https://help.autodesk.com/view/OARX/2024/ENU/?guid=GUID-EF283E4A-4A67-444C-AD13-F03A2A88A08A
extern "C"
AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        break;
    case AcRx::kUnloadAppMsg:
        break;
        ...
    default:
        break;
    }
    return AcRx::kRetOK;
}

// https://help.autodesk.com/view/OARX/2024/ENU/?guid=GUID-DDBA8E99-7CAF-4619-95A0-DEFE79B42F53
// The initialization function called from the acrxEntryPoint()
// function during the kInitAppMsg case is used to add commands
// to the command stack and to add classes to the ACRX class
// hierarchy.
//
void
initApp()
{
    acedRegCmds->addCommand("ASDK_DICTIONARY_COMMANDS",
        "ASDK_CREATE", "CREATE", ACRX_CMD_MODAL,
        createDictionary);
    acedRegCmds->addCommand("ASDK_DICTIONARY_COMMANDS",
        "ASDK_ITERATE", "ITERATE", ACRX_CMD_MODAL,
        iterateDictionary);
    AsdkMyClass::rxInit();
    acrxBuildClassHierarchy();
}
// The cleanup function called from the acrxEntryPoint() 
// function during the kUnloadAppMsg case removes this application's
// command set from the command stack and removes this application's
// custom classes from the ACRX runtime class hierarchy.
//
void
unloadApp()
{
    acedRegCmds->removeGroup("ASDK_DICTIONARY_COMMANDS");
    // Remove the AsdkMyClass class from the ACRX runtime
    // class hierarchy. If this is done while the database is
    // still active, it should cause all objects of class
    // AsdkMyClass to be turned into proxies.
    //
    deleteAcRxClass(AsdkMyClass::desc());
}


https://help.autodesk.com/view/OARX/2024/ENU/?guid=GUID-3AD35E70-ECDE-4042-B27B-2C9269996015
Acad::ErrorStatus
addCommand(
    const char* cmdGroupName,
    const char* cmdGlobalName,
    const char* cmdLocalName,
    Adesk::Int32 commandFlags,
    AcRxFunctionPtr functionAddr,
    AcEdUIContext* UIContext = NULL,
    int fcode = -1,
    HINSTANCE hResourceHandle = NULL,
    AcEdCommand** cmdPtrRet = NULL);

https://help.autodesk.com/view/OARX/2024/ENU/?guid=GUID-0B228885-708D-440D-A39E-99DB06C04A7D
Acad::ErrorStatus createCircle(AcDbObjectId& circleId)
{
    circleId = AcDbObjectId::kNull;
    AcGePoint3d center(9.0, 3.0, 0.0);
    AcGeVector3d normal(0.0, 0.0, 1.0);
    AcDbCircle* pCirc = new AcDbCircle(center, normal, 2.0);
    if (pCirc == NULL)
        return Acad::eOutOfMemory;
    AcDbBlockTable* pBlockTable;
    Acad::ErrorStatus es =
        acdbHostApplicationServices()->workingDatabase()->
        getSymbolTable(pBlockTable, AcDb::kForRead);
    if (es != Acad::eOk) {
        delete pCirc;
        return es;
    }
    AcDbBlockTableRecord* pBlockTableRecord;
    es = pBlockTable->getAt(ACDB_MODEL_SPACE,
        pBlockTableRecord, AcDb::kForWrite);
    if (es != Acad::eOk) {
        Acad::ErrorStatus es2 = pBlockTable->close();
        if (es2 != Acad::eOk) {
            acrx_abort("\nApp X failed to close Block"
                " Table. Error: %d",
                acadErrorStatusText(es2));
        }
        delete pCirc;
        return es;
    }
    es = pBlockTable->close();
    if (es != Acad::eOk) {
        acrx_abort("\nApp X failed to close Block Table."
            " Error: %d", acadErrorStatusText(es));
    }
    es = pBlockTableRecord->appendAcDbEntity(circleId,
        pCirc);
    if (es != Acad::eOk) {
        Acad::ErrorStatus es2 = pBlockTableRecord->close();
        if (es2 != Acad::eOk) {
            acrx_abort("\nApp X failed to close"
                " Model Space Block Record. Error: %s",
                acadErrorStatusText(es2));
        }
        delete pCirc;
        return es;
    }
    es = pBlockTableRecord->close();
    if (es != Acad::eOk) {
        acrx_abort("\nApp X failed to close"
            " Model Space Block Record. Error: %d",
            acadErrorStatusText(es));
    }
    es = pCirc->close();
    if (es != Acad::eOk) {
        acrx_abort("\nApp X failed to"
            " close circle entity. Error: %d",
            acadErrorStatusText(es));
    }
    return es;
}

Acad::ErrorStatus createNewLayer()
{
    AcDbLayerTableRecord* pLayerTableRecord
        = new AcDbLayerTableRecord;
    if (pLayerTableRecord == NULL)
        return Acad::eOutOfMemory;
    Acad::ErrorStatus es
        = pLayerTableRecord->setName("ASDK_MYLAYER");
    if (es != Acad::eOk) {
        delete pLayerTableRecord;
        return es;
    }
    AcDbLayerTable* pLayerTable;
    es = acdbHostApplicationServices()->workingDatabase()->
        getSymbolTable(pLayerTable, AcDb::kForWrite);
    if (es != Acad::eOk) {
        delete pLayerTableRecord;
        return es;
    }
    // The linetype object ID default is 0, which is
    // not a valid ID.  Therefore, it must be set to a
    // valid ID, the CONTINUOUS linetype.
    // Other data members have valid defaults, so
    // they can be left alone.
    //
    AcDbLinetypeTable* pLinetypeTbl;
    es = acdbHostApplicationServices()->workingDatabase()->
        getSymbolTable(pLinetypeTbl, AcDb::kForRead);
    if (es != Acad::eOk) {
        delete pLayerTableRecord;
        es = pLayerTable->close();
        if (es != Acad::eOk) {
            acrx_abort("\nApp X failed to close Layer"
                " Table. Error: %d",
                acadErrorStatusText(es));
        }
        return es;
    }
    AcDbObjectId ltypeObjId;
    es = pLinetypeTbl->getAt("CONTINUOUS", ltypeObjId);
    if (es != Acad::eOk) {
        delete pLayerTableRecord;
        es = pLayerTable->close();
        if (es != Acad::eOk) {
            acrx_abort("\nApp X failed to close Layer"
                " Table. Error: %d",
                acadErrorStatusText(es));
        }
        return es;
    }
    pLayerTableRecord->setLinetypeObjectId(ltypeObjId);
    es = pLayerTable->add(pLayerTableRecord);
    if (es != Acad::eOk) {
        Acad::ErrorStatus es2 = pLayerTable->close();
        if (es2 != Acad::eOk) {
            acrx_abort("\nApp X failed to close Layer"
                " Table. Error: %d",
                acadErrorStatusText(es2));
        }
        delete pLayerTableRecord;
        return es;
    }
    es = pLayerTable->close();
    if (es != Acad::eOk) {
        acrx_abort("\nApp X failed to close Layer"
            " Table. Error: %d",
            acadErrorStatusText(es));
    }
    es = pLayerTableRecord->close();
    if (es != Acad::eOk) {
        acrx_abort("\nApp X failed to close Layer"
            " Table Record. Error: %d",
            acadErrorStatusText(es));
    }
    return es;
}