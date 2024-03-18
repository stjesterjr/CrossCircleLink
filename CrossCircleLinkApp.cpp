#include "rxregsvc.h"
#include "acutads.h"
#include "dbapserv.h" //for acdbHostApplicationServices()
#include "geline2d.h" // for drawLine
#include "dbents.h" //for AcDbLine
#include "aced.h"
#include "AcDbCrossCircle.h"
#include "AcDbCrossCircleLink.h"

AcDbLine* drawLineArg(AcGePoint3d line_start, AcGePoint3d line_end) {
    //New lineB
    AcDbLine* p_line = new AcDbLine(line_start, line_end);

    //Block pointer
    AcDbBlockTable* p_BlockTable;

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

Acad::ErrorStatus addToDb(AcDbEntity* p_entity) {

    AcDbHostApplicationServices* p_app_serv_handler = acdbHostApplicationServices();
    AcDbDatabase* p_db_handler = p_app_serv_handler->workingDatabase();

    AcDbBlockTable* p_blockTable;
    p_db_handler->getSymbolTable(p_blockTable, AcDb::OpenMode::kForRead);


    AcDbBlockTableRecord* p_block_table_record;
    p_blockTable->getAt(L"*Model_Space", p_block_table_record, AcDb::OpenMode::kForWrite);
    p_blockTable->close();


    AcDbObjectId ID;
    p_block_table_record->appendAcDbEntity(ID, p_entity);
    p_block_table_record->close();

    return Acad::eOk;
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

    AcDbCrossCircle* A_circle;
    AcDbCrossCircle* B_circle;
    acdbOpenObject(A_circle, A_id, AcDb::OpenMode::kForRead);
    acdbOpenObject(B_circle, B_id, AcDb::OpenMode::kForRead);

    AcDbCrossCircleLink* p_link = new AcDbCrossCircleLink;
    p_link->setStart(A_circle->center());
    p_link->setEnd(B_circle->center());

    
    A_circle->close();
    B_circle->close();

    if (addToDb(p_link) == Acad::eOk)
        return AcRx::kRetOK;
    else
        return AcRx::kRetError;
}

void Command() {
    drawCircleLink();
}


extern "C" AcRx::AppRetCode
acrxEntryPoint(AcRx::AppMsgCode msg, void* appId)
{
    switch (msg) {
    case AcRx::kInitAppMsg:
        acrxUnlockApplication(appId);
        acrxRegisterAppMDIAware(appId);
        acedRegCmds->addCommand(L"CrCiLink",
            L"CrCiLink", L"ÊðÊðÑâÿçü", ACRX_CMD_MODAL, Command);
        acutPrintf(L"Application Initilized\n");
        break;

    case AcRx::kUnloadAppMsg:
        acutPrintf(L"Application Unloaded\n");
        break;
    }
    return AcRx::kRetOK;
}