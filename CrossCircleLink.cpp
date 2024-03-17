#include "rxregsvc.h"
#include "acutads.h"
#include "dbapserv.h" //for acdbHostApplicationServices()
#include "geline2d.h" // for drawLine
#include "dbents.h" //for AcDbLine
#include "aced.h"

AcRx::AppRetCode test_function() {
    auto p_app_serv_handler = acdbHostApplicationServices();
    AcDbHostApplicationServices* p_app_serv_ptr = p_app_serv_handler;

    auto p_db_handler = p_app_serv_handler->workingDatabase();
    AcDbDatabase* p_db = p_db_handler;

    return AcRx::AppRetCode::kRetOK;
}

AcDbObjectId drawLine() {
    //Set variables
    AcGePoint3d line_start{ 0.0, 0.0, 0.0 };
    AcGePoint3d line_end{ 10.0,10.0, 10.0 };

    //New lineB
    AcDbLine* p_line = new AcDbLine(line_start, line_end);

    //Block pointer
    AcDbBlockTable* p_BlockTable;

    //These two do the same
    //1
    //acdbHostApplicationServices()->workingDatabase()->getSymbolTable(p_BlockTable, AcDb::kForRead);
    //2
    AcDbHostApplicationServices* p_app_serv_handler = acdbHostApplicationServices();
    AcDbDatabase* p_db_handler = p_app_serv_handler->workingDatabase();
    p_db_handler->getSymbolTable(p_BlockTable, AcDb::OpenMode::kForRead);

    //Block table pointer
    AcDbBlockTableRecord* p_BlockTable_record;

    //
    p_BlockTable->getAt(L"*Model_Space", p_BlockTable_record, AcDb::OpenMode::kForWrite);
    p_BlockTable->close();
    //
    AcDbObjectId lineId;
    p_BlockTable_record->appendAcDbEntity(lineId, p_line);

    //Cleaning
    p_BlockTable_record->close();
    p_line->close();

    return lineId;
}

AcDbLine* drawLineArg(AcGePoint3d line_start, AcGePoint3d line_end) {
    //New lineB
    AcDbLine* p_line = new AcDbLine(line_start, line_end);

    //Block pointer
    AcDbBlockTable* p_BlockTable;

    //These two do the same
    //1
    //acdbHostApplicationServices()->workingDatabase()->getSymbolTable(p_BlockTable, AcDb::kForRead);
    //2
    AcDbHostApplicationServices* p_app_serv_handler = acdbHostApplicationServices();
    AcDbDatabase* p_db_handler = p_app_serv_handler->workingDatabase();
    p_db_handler->getSymbolTable(p_BlockTable, AcDb::OpenMode::kForRead);

    //Block table pointer
    AcDbBlockTableRecord* p_BlockTable_record;

    //
    p_BlockTable->getAt(L"*Model_Space", p_BlockTable_record, AcDb::OpenMode::kForWrite);
    p_BlockTable->close();
    //
    AcDbObjectId lineId;
    p_BlockTable_record->appendAcDbEntity(lineId, p_line);

    //Cleaning
    p_BlockTable_record->close();
    p_line->close();

    return p_line;
}

int printdxf(struct resbuf* eb)
{
    enum type_dummy : short {
        RTNONE = 0,
        RTSTR = 2,
        RT3DPOINT,
        RTREAL,
        RTSHORT
    };
    short rt;

    if (eb == NULL)
        return RTNONE;
    if ((eb->restype >= 0) && (eb->restype <= 9))
        rt = RTSTR;
    else if ((eb->restype >= 10) && (eb->restype <= 19))
        rt = RT3DPOINT;
    else if ((eb->restype >= 38) && (eb->restype <= 59))
        rt = RTREAL;
    else if ((eb->restype >= 60) && (eb->restype <= 79))
        rt = RTSHORT;
    else if ((eb->restype >= 210) && (eb->restype <= 239))
        rt = RT3DPOINT;
    else if (eb->restype < 0)
        rt = eb->restype;
    else
        rt = RTNONE;


    switch (rt) {
    case RTSHORT:
        acutPrintf(L"(%d . %d)\n", eb->restype,
            eb->resval.rint);
        break;
    case RTREAL:
        acutPrintf(L"(%d . %0.3f)\n", eb->restype,
            eb->resval.rreal);
        break;
    case RTSTR:
        acutPrintf(L"(%d . \"%s\")\n", eb->restype,
            eb->resval.rstring);
        break;
    case RT3DPOINT:
        acutPrintf(L"(%d . %0.3f %0.3f %0.3f)\n",
            eb->restype,
            eb->resval.rpoint[X], eb->resval.rpoint[Y],
            eb->resval.rpoint[Z]);
        break;
    case RTNONE:
        acutPrintf(L"(%d . Unknown type)\n", eb->restype);
        break;
    case -1:
    case -2:
        // First block entity
        acutPrintf(L"(%d . <Entity name: %8lx>)\n",
            eb->restype, eb->resval.rlname[0]);
    }
    return eb->restype;
}



Acad::ErrorStatus changeLineColorByID(AcDbObjectId object_id, Adesk::UInt16 new_color) {


    AcDbEntity* entity;
    acdbOpenObject(entity, object_id, AcDb::OpenMode::kForWrite);

    entity->setColorIndex(new_color);
    entity->close();

    return Acad::ErrorStatus::eOk;

}

Acad::ErrorStatus drawCircleLink() {
    ads_name A_name;
    ads_name B_name;
    ads_point A_name_point;
    ads_point B_name_point;
    acedEntSel(L"Select first", A_name, A_name_point);
    acedEntSel(L"Select second", B_name, B_name_point);

    AcDbObjectId A_id;
    AcDbObjectId B_id;
    acdbGetObjectId(A_id, A_name);
    acdbGetObjectId(B_id, B_name);

    AcDbCircle* A_circle;
    AcDbCircle* B_circle;
    acdbOpenObject(A_circle, A_id, AcDb::OpenMode::kForWrite);
    acdbOpenObject(B_circle, B_id, AcDb::OpenMode::kForWrite);

    auto line = drawLineArg(A_circle->center(), B_circle->center());
    A_circle->addPersistentReactor(line->id());
    B_circle->addPersistentReactor(line->id());
    
    A_circle->close();
    B_circle->close();
}

void drawLineWrapper() {

    drawCircleLink();
    drawLine();
}


extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxUnlockApplication(appId);
        acrxRegisterAppMDIAware(appId);
        acedRegCmds->addCommand(L"ASDK",
            L"DRAWLINE", L"¿—ƒ À»Õ»ﬂ", ACRX_CMD_MODAL, drawLineWrapper);
        acutPrintf(L"Application Initilized\n");
        break;

    case AcRx::kUnloadAppMsg:
        acutPrintf(L"Application Unloaded\n");
        break;
    }
    return AcRx::kRetOK;
}