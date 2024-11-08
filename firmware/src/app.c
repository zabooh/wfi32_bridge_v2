/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
bool debug_CMDInit(void);

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;
extern EXCEPT_MSG last_expt_msg;
bool print_delay=false;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

bool console_wait = false;

/* TODO:  Add any necessary local functions.
*/

/*******************************************************************************
  Function:
    void* APP_Calloc(size_t nElems,size_t elemSize)

  Remarks:
    See prototype in app.h.
 */
void* APP_Calloc(size_t nElems,size_t elemSize)
{
    size_t nBytes = nElems * elemSize;
    
    /* Call the OSAL_Malloc() and initialized the assigned memory with zero*/
    void* ptr = OSAL_Malloc(nBytes);
    if(ptr)
    {
        memset(ptr, 0, nBytes);
    }
    return ptr;

}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;

    debug_CMDInit();

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
uint32_t DeviceID;

    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {
                console_wait = true;
                appData.state = APP_STATE_WAIT_FOR_TCP_STACK_READY;
            }
            break;
        }

        case APP_STATE_WAIT_FOR_TCP_STACK_READY:
        {
            if (TCPIP_STACK_Status(sysObj.tcpip) == SYS_STATUS_READY) {
                print_delay=true;
                DeviceID = DEVID;
                SYS_CONSOLE_PRINT(
                        "======================================================\n\r");
                SYS_CONSOLE_PRINT("L2 Bridge Build Time  " __DATE__ " " __TIME__ "\n\r");
                SYS_CONSOLE_PRINT("https://github.com/zabooh/wfi32_bridge_v2.git\n\r");
                SYS_CONSOLE_PRINT("Device ID: %08x\n\r", DeviceID);
                if (last_expt_msg.magic == MAGIC_CODE) {
                    SYS_CONSOLE_PRINT(VT100_TEXT_DEFAULT "\n\r!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
                    SYS_CONSOLE_PRINT(VT100_TEXT_DEFAULT "Last Runtime has ended with the following Message:\n\r");
                    {
                        char ch;
                        int ix = 0;
                        for (ix = 0; ix < 4096; ix++) {
                            ch = last_expt_msg.msg[ix];
                            if (ch == 0)break;
                            SYS_CONSOLE_PRINT("%c", ch);
                        }
                    }
                    SYS_CONSOLE_PRINT("%c", last_expt_msg.msg[0]);
                    SYS_CONSOLE_PRINT(VT100_TEXT_DEFAULT "\n\r!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");                
                }            
                last_expt_msg.magic = 0;
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }


        case APP_STATE_SERVICE_TASKS:
        {

            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}



#include "config/pic32mz_w1_eth_wifi_freertos/system/command/sys_command.h"


void     CommandDump(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // Dump Memory
void     CommandHeap(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv);              // Display Heap Statistics

const SYS_CMD_DESCRIPTOR debug_cmd_tbl[] = {
    {"heap",    CommandHeap,    ": heap statistics"},    
    {"dump",    CommandDump,    ": dump memory"},        
};

bool debug_CMDInit(void) {
    bool ret = false;

    if (!SYS_CMD_ADDGRP(debug_cmd_tbl, sizeof (debug_cmd_tbl) / sizeof (*debug_cmd_tbl), "debug", ": Debug Commands")) {
        ret = true;
    }
    return ret;
}


void CommandHeap(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    HeapStats_t xHeapStats;
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    (*pCmdIO->pCmdApi->msg)(cmdIoParam, "\n\rHeap Statistics\r\n");

    vPortGetHeapStats(&xHeapStats);
    
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "configTOTAL_HEAP_SIZE           : %d\r\n", configTOTAL_HEAP_SIZE);        
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xAvailableHeapSpaceInBytes      : %d\r\n", xHeapStats.xAvailableHeapSpaceInBytes);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xSizeOfLargestFreeBlockInBytes  : %d\r\n", xHeapStats.xSizeOfLargestFreeBlockInBytes);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xSizeOfSmallestFreeBlockInBytes : %d\r\n", xHeapStats.xSizeOfSmallestFreeBlockInBytes);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xNumberOfFreeBlocks             : %d\r\n", xHeapStats.xNumberOfFreeBlocks);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xMinimumEverFreeBytesRemaining  : %d\r\n", xHeapStats.xMinimumEverFreeBytesRemaining);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xNumberOfSuccessfulAllocations  : %d\r\n", xHeapStats.xNumberOfSuccessfulAllocations);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "xNumberOfSuccessfulFrees        : %d\r\n", xHeapStats.xNumberOfSuccessfulFrees);
 //   (*pCmdIO->pCmdApi->print)(cmdIoParam, "xNumberOfFaileddAllocations     : %d\r\n", xHeapStats.xNumberOfFaileddAllocations);

}


void CommandDump(SYS_CMD_DEVICE_NODE* pCmdIO, int argc, char** argv) {
    const void* cmdIoParam = pCmdIO->cmdIoParam;
    uint32_t addr;
    uint32_t count;
    uint32_t ix, jx;
    uint8_t *puc;
    char str[64];
    int flag = 0;

    addr = strtoul(argv[1], NULL, 16);
    count = strtoul(argv[2], NULL, 16);
    puc = (uint8_t *) addr;
    puc = (uint8_t *) addr;
    
    if(argc<3){
        (*pCmdIO->pCmdApi->print)(cmdIoParam, "Usage: dump address count\n\r");
        return;
    }

    jx = 0;
    for (ix = 0; ix < count; ix++) {
        if ((ix % 16) == 0) {
            if(flag == 1){
                str[16] = 0;
                (*pCmdIO->pCmdApi->print)(cmdIoParam, "   %s", str);
            }
            (*pCmdIO->pCmdApi->print)(cmdIoParam, "\n\r%08x: ", puc);
            flag = 1;
            jx = 0;
        }
        (*pCmdIO->pCmdApi->print)(cmdIoParam, " %02x", *puc);
        if ( (*puc > 31) && (*puc < 127) )
            str[jx++] = *puc;
        else
            str[jx++] = '.';
        puc++;
    }
    str[jx] = 0;
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "   %s", str);
    (*pCmdIO->pCmdApi->print)(cmdIoParam, "\n\rReady\n\r");
}



/*******************************************************************************
 End of File
 */
