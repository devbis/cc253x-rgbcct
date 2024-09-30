/**************************************************************************************************
  Filename:       zcl_level_ctrl.c
  Revised:        $Date: 2013-04-03 14:59:11 -0700 (Wed, 03 Apr 2013) $
  Revision:       $Revision: 33727 $


  Description:    This file contains the Zigbee Cluster Library Level Control
                  application callbacks code.


  Copyright 2010-2011 Texas Instruments Incorporated. All rights reserved.

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

/*********************************************************************
  This device will be like a Light device.  This application is not
  intended to be a Light device, but will use the device description
  to implement this sample code.
*********************************************************************/

#ifdef ZCL_LEVEL_CTRL
/*********************************************************************
 * INCLUDES
 */

#include "zcl_level_ctrl.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
//static afAddrType_t zclLevel_DstAddr;

static int32 zclLevel_StepLevel_256 = 0;
static uint16 zclLevel_CurrentLevel_256 = ((uint16)LEVEL_MAX)<<8;

static uint16 zclLevel_LevelWithOnOff = 0;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static byte zclLight_TaskID;
static zclGCB_OnOff_t zclLevel_OnOffCB;
/*********************************************************************
 * LOCAL FUNCTIONS
 */


/*********************************************************************
 * @fn          zclLevel_init
 *
 * @brief
 *
 * @param       none
 *
 * @return      none
 */
void zclLevel_init( byte taskID, zclGCB_OnOff_t OnOffCB )
{
  zclLight_TaskID = taskID;
  zclLevel_OnOffCB = OnOffCB;

  //Move lamp to default level
  zclLevel_LevelRemainingTime = 0;
   hwLight_ApplyUpdate( &zclLevel_CurrentLevel,
                        &zclLevel_CurrentLevel_256,
                        &zclLevel_StepLevel_256,
                        &zclLevel_LevelRemainingTime,
                        LEVEL_MIN, LEVEL_MAX, FALSE );
}

/*********************************************************************
 * @fn          zclLevel_process_level_event
 *
 * @brief       Level Event Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
void zclLevel_process( uint16 *events )
{
  if ( *events & LEVEL_PROCESS_EVT )
  {
    //update the level
    if(zclLevel_LevelRemainingTime)
    {
      hwLight_ApplyUpdate( &zclLevel_CurrentLevel,
                           &zclLevel_CurrentLevel_256,
                           &zclLevel_StepLevel_256,
                           &zclLevel_LevelRemainingTime,
                           LEVEL_MIN, LEVEL_MAX, FALSE );

      if( (zclLevel_CurrentLevel == LEVEL_MIN) && (zclLevel_LevelWithOnOff) )
      {
        zclLevel_OnOffCB(COMMAND_OFF);
      }
    }

    if(zclLevel_LevelWithOnOff)
    {
      if(zclLevel_CurrentLevel == LEVEL_MIN)
      {
        zclLevel_OnOffCB(COMMAND_OFF);
      }
      else
      {
        zclLevel_OnOffCB(COMMAND_ON);
      }
    }

    if (zclLevel_LevelRemainingTime)
    {
      //set a timer to make the change over zclLevel_StepTime 100ms
      osal_start_timerEx( zclLight_TaskID, LEVEL_PROCESS_EVT , 100 );
    }

    *events = *events ^ LEVEL_PROCESS_EVT;
  }

  return;
}



/*********************************************************************
 * @fn      zclLevel_LevelControlMoveToLevelCB
 *
 * @brief   This callback is called to process a move to color
 *          Request command.
 *
 * @param   pCmd - command
 *
 * @return  ZStatus_t
 */
void zclLevel_MoveToLevelCB( zclLCMoveToLevel_t *pCmd )
{
  zclLevel_LevelWithOnOff = pCmd->withOnOff;

  //if transition time = 0 then do immediately
  if ( pCmd->transitionTime == 0 )
  {
      zclLevel_LevelRemainingTime = 1;
  }
  else
  {
    zclLevel_LevelRemainingTime = pCmd->transitionTime;
  }
    zclLevel_StepLevel_256 = ((int32)(pCmd->level - zclLevel_CurrentLevel))<<8;
    zclLevel_StepLevel_256 /= (int32)zclLevel_LevelRemainingTime;

  hwLight_ApplyUpdate( &zclLevel_CurrentLevel,
                       &zclLevel_CurrentLevel_256,
                       &zclLevel_StepLevel_256,
                       &zclLevel_LevelRemainingTime,
                       LEVEL_MIN, LEVEL_MAX, FALSE );

  if ( zclLevel_LevelWithOnOff )
  {
    if ( zclLevel_StepLevel_256 > 0 )
    {
      zclLevel_OnOffCB(COMMAND_ON);
    }
    else if( (zclLevel_CurrentLevel == LEVEL_MIN) && (zclLevel_LevelWithOnOff) )
    {
      zclLevel_OnOffCB(COMMAND_OFF);
    }
  }

  if (zclLevel_LevelRemainingTime)
  {
    //set a timer to make the change over zclLevel_StepTime 100ms
    osal_start_timerEx( zclLight_TaskID, LEVEL_PROCESS_EVT , 100 );
  }

  return;
}

/*********************************************************************
 * @fn      zclLevel_MoveCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received a Level Control - Move Command for
 *          this application.
 *
 * @param   moveMode -
 * @param   rate -
 * @param   withOnOff - with On/off command
 *
 * @return  none
 */
void zclLevel_MoveCB( zclLCMove_t *pCmd )
{
  zclLevel_LevelWithOnOff = pCmd->withOnOff;
  zclLevel_StepLevel_256 = (((int32)pCmd->rate)<<8)/10;

  switch (pCmd->moveMode)
  {
    case LEVEL_MOVE_UP:
      if(zclLevel_LevelWithOnOff)
      {
        zclLevel_OnOffCB(COMMAND_ON);
      }
      break;
    case LEVEL_MOVE_DOWN:
      zclLevel_StepLevel_256 = -zclLevel_StepLevel_256;
      break;
  }

  //Change for ever - level stop call back will stop this command
  zclLevel_LevelRemainingTime = 0xFFFF;

  hwLight_ApplyUpdate( &zclLevel_CurrentLevel,
                       &zclLevel_CurrentLevel_256,
                       &zclLevel_StepLevel_256,
                       &zclLevel_LevelRemainingTime,
                       LEVEL_MIN, LEVEL_MAX, FALSE);

  if( (zclLevel_CurrentLevel == LEVEL_MIN) && (zclLevel_LevelWithOnOff) )
  {
    zclLevel_OnOffCB(COMMAND_OFF);
  }

  if (zclLevel_LevelRemainingTime)
  {
    //set a timer to make the change over zclLevel_StepTime 100ms
    osal_start_timerEx( zclLight_TaskID, LEVEL_PROCESS_EVT , 100);
  }

  return;
}

/*********************************************************************
 * @fn      zclLevel_StepCB
 *
 * @brief   This callback is called to process a Level step
 *          Request command.
 *
 * @param   pCmd - command
 *
 * @return  ZStatus_t
 */
void zclLevel_StepCB(zclLCStep_t *pCmd )
{
  zclLevel_LevelWithOnOff = pCmd->withOnOff;

  if ( (pCmd->transitionTime == 0) || (pCmd->transitionTime == 0xFFFF) )
  {
    zclLevel_LevelRemainingTime = 1;
  }
  else
  {
    zclLevel_LevelRemainingTime = pCmd->transitionTime;
  }

  zclLevel_StepLevel_256 = ((((int32)pCmd->amount)<<8) / zclLevel_LevelRemainingTime);
  switch (pCmd->stepMode)
  {
    case LEVEL_STEP_UP:
      if(zclLevel_LevelWithOnOff)
      {
        zclLevel_OnOffCB(COMMAND_ON);
      }
      break;
    case LEVEL_STEP_DOWN:
      zclLevel_StepLevel_256 = -zclLevel_StepLevel_256;
      break;
  }

  hwLight_ApplyUpdate( &zclLevel_CurrentLevel,
                       &zclLevel_CurrentLevel_256,
                       &zclLevel_StepLevel_256,
                       &zclLevel_LevelRemainingTime,
                       LEVEL_MIN, LEVEL_MAX, FALSE );

  if( (zclLevel_CurrentLevel == LEVEL_MIN) && (zclLevel_LevelWithOnOff) )
  {
    zclLevel_OnOffCB(COMMAND_OFF);
  }

  if (zclLevel_LevelRemainingTime)
  {
    //set a timer to make the change over zclLevel_StepTime 100ms
    osal_start_timerEx( zclLight_TaskID, LEVEL_PROCESS_EVT , 100 );
  }

  return;

}

/*********************************************************************
 * @fn      zclLevel_StopCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received a Level Control - Stop Command for
 *          this application.
 *
 * @param   stepMode -
 * @param   amount - number of levels to step
 * @param   transitionTime - time to take a single step
 *
 * @return  none
 */
void zclLevel_StopCB( void )
{
    osal_stop_timerEx( zclLight_TaskID, LEVEL_PROCESS_EVT);
    zclLevel_LevelRemainingTime = 0;
    // align variables
    zclLevel_CurrentLevel_256 = ((int32)zclLevel_CurrentLevel)<<8;
}

#endif //ZCL_LEVEL_CTRL
/****************************************************************************
****************************************************************************/


