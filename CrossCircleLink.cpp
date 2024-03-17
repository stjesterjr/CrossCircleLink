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

void drawLineWrapper() {
    drawLine();
}

Acad::ErrorStatus changeLineColorByID(AcDbObjectId object_id, Adesk::UInt16 new_color) {


    AcDbEntity* entity;
    acdbOpenObject(entity, object_id, AcDb::OpenMode::kForWrite);

    entity->setColorIndex(new_color);
    entity->close();

    return Acad::ErrorStatus::eOk;

}

// Simple acrxEntryPoint code. Normally intialization and cleanup
// (such as registering and removing commands) should be done here.
extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{

    acutPrintf(L"EntryPoint call\n");
    switch (msg) {
    case AcRx::kInitAppMsg:
        // Allow application to be unloaded
        // Without this statement, AutoCAD will
        // not allow the application to be unloaded
        // except on AutoCAD exit.
        //
        acrxUnlockApplication(appId);
        // Register application as MDI aware. 
        // Without this statement, AutoCAD will
        // switch to SDI mode when loading the
        // application.
        //
        acrxRegisterAppMDIAware(appId);
        acedRegCmds->addCommand(L"ASDK",
            L"DRAWLINE", L"—ƒ À»Õ»ﬂ", ACRX_CMD_MODAL, drawLineWrapper);

        acutPrintf(L"Application Initilized\n");
        drawLine();
        acutPrintf(L"Test line draw\n");
        break;
    case AcRx::kUnloadAppMsg:
        acutPrintf(L"Application Unloaded\n");
        break;
    }
    return AcRx::kRetOK;
}