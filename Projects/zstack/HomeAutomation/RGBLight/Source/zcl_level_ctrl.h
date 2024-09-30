/**************************************************************************************************
  Filename:       zcl_advancedlight.h
  Revised:        $Date: 2013-04-03 14:59:11 -0700 (Wed, 03 Apr 2013) $
  Revision:       $Revision: 33727 $

  Description:    This file contains the Zigbee Cluster Library Level Control
                  application callbacks definitions.

  Copyright 2010-2012 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

#ifndef ZCL_LEVEL_CTRL_H
#define ZCL_LEVEL_CTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ZCL_LEVEL_CTRL
/*********************************************************************
 * INCLUDES
 */
#include "hw_light_ctrl.h"
#include "zll_samplelight.h"

/*********************************************************************
 * CONSTANTS
 */

#define LEVEL_MIN                 0x01
#define LEVEL_MAX                 0xFE

//align this with the Application event defined in application header file
#define LEVEL_PROCESS_EVT         SAMPLELIGHT_LEVEL_PROCESS_EVT

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * VARIABLES
 */

// Level control Cluster (server) -----------------------------------------------------
extern uint8 zclLevel_CurrentLevel;
extern uint16 zclLevel_LevelRemainingTime;

/*********************************************************************
 * FUNCTIONS
 */

void zclLevel_init( byte taskID, zclGCB_OnOff_t zcl_OnOffCB );
void zclLevel_process( uint16 *events );

void zclLevel_MoveToLevelCB( zclLCMoveToLevel_t *pCmd );
void zclLevel_MoveCB( zclLCMove_t *pCmd );
void zclLevel_StepCB(zclLCStep_t *pCmd );
void zclLevel_StopCB( void );

/*********************************************************************
*********************************************************************/
#endif //ZCL_LEVEL_CTRL

#ifdef __cplusplus
}
#endif

#endif /* ZCL_LEVEL_CTRL_H */
