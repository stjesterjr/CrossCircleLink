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