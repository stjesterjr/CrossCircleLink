#include "rxregsvc.h"
#include "acutads.h"
#include "dbapserv.h" //for acdbHostApplicationServices()
#include "geline2d.h" // for drawLine
#include "dbents.h" //for AcDbLine
#include "aced.h"


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

AcRx::AppRetCode drawCircleLink() {
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

    AcDbLine* line = drawLineArg(A_circle->center(), B_circle->center());
    A_circle->addPersistentReactor(line->id());
    B_circle->addPersistentReactor(line->id());
    
    A_circle->close();
    B_circle->close();

    return AcRx::kRetOK;
}

void drawLineCommand() {

    drawCircleLink();
    drawLineArg({ 0.0,0.0,0.0 }, {1.0,3.0,0.0});
}


extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxUnlockApplication(appId);
        acrxRegisterAppMDIAware(appId);
        acedRegCmds->addCommand(L"ASDK",
            L"DRAWLINE", L"¿—ƒ À»Õ»ﬂ", ACRX_CMD_MODAL, drawLineCommand);
        acutPrintf(L"Application Initilized\n");
        break;

    case AcRx::kUnloadAppMsg:
        acutPrintf(L"Application Unloaded\n");
        break;
    }
    return AcRx::kRetOK;
}