#include "rxregsvc.h"
#include "acutads.h"
#include "dbapserv.h" //for acdbHostApplicationServices()
#include "geline2d.h" // for drawLine
#include "dbents.h" //for AcDbLine

// Simple acrxEntryPoint code. Normally intialization and cleanup
// (such as registering and removing commands) should be done here.
//
extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
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
        acutPrintf(L"\nExample Application Loaded");
        break;
    case AcRx::kUnloadAppMsg:
        acutPrintf(L"\nExample Application Unloaded");
        break;
    }
    return AcRx::kRetOK;
}

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
    AcDbDatabase*   p_db_handler = p_app_serv_handler->workingDatabase();
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