/**************************************************************************************************
  Filename:       zcl_color_ctrl.c
  Revised:        $Date: 2013-08-08 18:23:54 -0700 (Thu, 08 Aug 2013) $
  Revision:       $Revision: 34927 $

  Description:    This file contains the Zigbee Cluster Library Color Control
                  application callbacks code.


  Copyright 2010-2013 Texas Instruments Incorporated. All rights reserved.

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

#ifdef ZCL_COLOR_CTRL
/*********************************************************************
 * INCLUDES
 */
#include "onboard.h"
#include "zcl_color_ctrl.h"

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

static int32 zclColor_StepSaturation_256 = 0;
static uint16 zclColor_SaturationRemainingTime = 0;
static uint16 zclColor_CurrentSaturation_256 = 0;

static int32 zclColor_StepHue_256 = 0;
static uint16 zclColor_HueRemainingTime = 0;
static uint16 zclColor_CurrentHue_256 = 0;

static int32 zclColor_StepEnhancedHue_256 = 0;
static uint16 zclColor_EnhancedHueRemainingTime = 0;
static uint32 zclColor_EnhancedCurrentHue_256 = 0;

static int32 zclColor_StepColorX_256 = 0;
static int32 zclColor_StepColorY_256 = 0;
static uint32 zclColor_CurrentX_256 = 0;
static uint32 zclColor_CurrentY_256 = 0;

static byte zclLight_TaskID;

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
void zclColor_init( byte taskID )
{
  zclLight_TaskID = taskID;

  //Move to default color
  zclColor_HueRemainingTime = 0;
  hwLight_ApplyUpdate( &zclColor_CurrentHue,
                       &zclColor_CurrentHue_256,
                       &zclColor_StepHue_256,
                       &zclColor_HueRemainingTime,
                       COLOR_HUE_MIN, COLOR_HUE_MAX, TRUE );

  zclColor_SaturationRemainingTime = 0;
  hwLight_ApplyUpdate( &zclColor_CurrentSaturation,
                       &zclColor_CurrentSaturation_256,
                       &zclColor_StepSaturation_256,
                       &zclColor_SaturationRemainingTime,
                       COLOR_SAT_MIN, COLOR_SAT_MAX, FALSE );
}

/*********************************************************************
 * @fn          zclColor_process_color_event
 *
 * @brief       color Event Processor for zclLighting.
 *
 * @param       events
 *
 * @return      none
 */
void zclColor_process( uint16 *events )
{
  if ( *events & COLOR_PROCESS_EVT )
  {
    //update the satruation if mode is hue and saturation
    if( (zclColor_EnhancedColorMode == COLOR_MODE_CURRENT_HUE_SATURATION) ||
        (zclColor_EnhancedColorMode == ENHANCED_COLOR_MODE_ENHANCED_CURRENT_HUE_SATURATION) )
    {
      if( zclColor_SaturationRemainingTime )
      {
        hwLight_ApplyUpdate( &zclColor_CurrentSaturation,
                             &zclColor_CurrentSaturation_256,
                             &zclColor_StepSaturation_256,
                             &zclColor_SaturationRemainingTime,
                             COLOR_SAT_MIN, COLOR_SAT_MAX, FALSE );
      }
      //update the Hue if mode is hue and saturation
      if( zclColor_HueRemainingTime )
      {
        hwLight_ApplyUpdate( &zclColor_CurrentHue,
                             &zclColor_CurrentHue_256,
                             &zclColor_StepHue_256,
                             &zclColor_HueRemainingTime,
                             COLOR_HUE_MIN, COLOR_HUE_MAX, TRUE );
      }

      //update the Enhanced Hue if mode is hue and saturation
      if(zclColor_EnhancedHueRemainingTime)
      {
        hwLight_ApplyUpdate16b( &zclColor_EnhancedCurrentHue,
                                &zclColor_EnhancedCurrentHue_256,
                                &zclColor_StepEnhancedHue_256,
                                &zclColor_EnhancedHueRemainingTime,
                                COLOR_ENH_HUE_MIN, COLOR_ENH_HUE_MAX, TRUE );
      }
    }
    else if(zclColor_EnhancedColorMode == COLOR_MODE_CURRENT_X_Y)
    {
      if(zclColor_ColorRemainingTime)
      {
          hwLight_ApplyUpdate16b( &zclColor_CurrentX,
                                  &zclColor_CurrentX_256,
                                  &zclColor_StepColorX_256,
                                  &zclColor_ColorRemainingTime,
                                  COLOR_XY_MIN, COLOR_XY_MAX, FALSE );
          if(zclColor_ColorRemainingTime != 0xFFFF)
          {
            zclColor_ColorRemainingTime++; //hwLight_ApplyUpdate16b decrements each time.
          }
          hwLight_ApplyUpdate16b( &zclColor_CurrentY,
                                  &zclColor_CurrentY_256,
                                  &zclColor_StepColorY_256,
                                  &zclColor_ColorRemainingTime,
                                  COLOR_XY_MIN, COLOR_XY_MAX, FALSE );
      }
    }

    if ( zclColor_ColorRemainingTime || zclColor_SaturationRemainingTime || zclColor_HueRemainingTime || zclColor_EnhancedHueRemainingTime )
    {
      //set a timer to make the change over 100ms
      osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT, 100 );
    }

    *events = *events ^ COLOR_PROCESS_EVT;
  }

  return;
}

/*********************************************************************
 * @fn          zclColor_process_color_loop_event
 *
 * @brief       color loop Event Processor for zclLighting.
 *              Move the hue according to the required direction and cycle time attributes,
 *              in steps of 100ms.
 *
 * @param       events vector
 *
 * @return      none
 */
void zclColor_processColorLoop( uint16 *events )
{
  if ( *events & COLOR_LOOP_PROCESS_EVT )
  {
    if ( zclColor_ColorLoopActive )
    {
      uint16 indefiniteRemainingTime = 0xFFFF;

      hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
      zclColor_EnhancedCurrentHue_256 = (uint32)zclColor_EnhancedCurrentHue<<8;
      zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
      zclColor_EnhancedColorMode=ENHANCED_COLOR_MODE_ENHANCED_CURRENT_HUE_SATURATION;

      zclColor_StepEnhancedHue_256 = ((int32)COLOR_ENH_HUE_MAX<<8) / ( 10 * zclColor_ColorLoopTime );
      if ( zclColor_ColorLoopDirection == LIGHTING_COLOR_LOOP_DIRECTION_DECREMENT )
      {
        zclColor_StepEnhancedHue_256 = -zclColor_StepEnhancedHue_256;
      }

      hwLight_ApplyUpdate16b( &zclColor_EnhancedCurrentHue,
                              &zclColor_EnhancedCurrentHue_256,
                              &zclColor_StepEnhancedHue_256,
                              &indefiniteRemainingTime,
                              COLOR_ENH_HUE_MIN, COLOR_ENH_HUE_MAX, TRUE);

      //set a timer to make the change over 100ms
      osal_start_timerEx( zclLight_TaskID, COLOR_LOOP_PROCESS_EVT , 100 );
    }
    *events = *events ^ COLOR_LOOP_PROCESS_EVT;
  }

  return;
}

/*********************************************************************
 * @fn      zclColor_MoveToColorCB
 *
 * @brief   This callback is called to process a move to color
 *          Request command.
 *
 * @param   pCmd - command
 *
 * @return  status
 */
ZStatus_t zclColor_MoveToColorCB( zclCCMoveToColor_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_X_Y);
  zclColor_CurrentX_256 = ((int32)zclColor_CurrentX)<<8;
  zclColor_CurrentY_256 = ((int32)zclColor_CurrentY)<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_X_Y;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_X_Y;

  //if transition time = 0 then do immediately
  if ( pCmd->transitionTime == 0 )
  {
      zclColor_ColorRemainingTime = 1;
  }
  else
  {
    zclColor_ColorRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepColorX_256 = ((int32)pCmd->colorX - zclColor_CurrentX)<<8;
  zclColor_StepColorX_256 /= (int32)zclColor_ColorRemainingTime;
  zclColor_StepColorY_256 = ((int32)pCmd->colorY - zclColor_CurrentY)<<8;
  zclColor_StepColorY_256 /= (int32)zclColor_ColorRemainingTime;

  hwLight_ApplyUpdate16b( &zclColor_CurrentX,
                          &zclColor_CurrentX_256,
                          &zclColor_StepColorX_256,
                          &zclColor_ColorRemainingTime,
                          COLOR_XY_MIN, COLOR_XY_MAX, FALSE );
  if ( zclColor_ColorRemainingTime != 0xFFFF )
  {
    zclColor_ColorRemainingTime++;
  }
  hwLight_ApplyUpdate16b( &zclColor_CurrentY,
                          &zclColor_CurrentY_256,
                          &zclColor_StepColorY_256,
                          &zclColor_ColorRemainingTime,
                          COLOR_XY_MIN, COLOR_XY_MAX, FALSE );

  if ( zclColor_ColorRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_MoveColorCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move Color Command for
 *          this application.
 *
 * @param   rateX - the movement in steps per second for x, where step is a
 *                 change in the device's x of one unit
 * @param   rateY - the movement in steps per second for y, where step is a
 *                 change in the device's y of one unit
 *
 * @return  none
 */
void zclColor_MoveColorCB( zclCCMoveColor_t *pCmd )
{
  zclColor_StepColorX_256 = (((int32)pCmd->rateX)<<8)/10;
  zclColor_StepColorY_256 = (((int32)pCmd->rateY)<<8)/10;

  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_X_Y);
  zclColor_CurrentX_256 = ((int32)zclColor_CurrentX)<<8;
  zclColor_CurrentY_256 = ((int32)zclColor_CurrentY)<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_X_Y;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_X_Y;

  //Change for ever - level stop call back will stop this command
  zclColor_ColorRemainingTime = 0xFFFF;

  hwLight_ApplyUpdate16b( &zclColor_CurrentX,
                          &zclColor_CurrentX_256,
                          &zclColor_StepColorX_256,
                          &zclColor_ColorRemainingTime,
                          COLOR_XY_MIN, COLOR_XY_MAX, FALSE );
  hwLight_ApplyUpdate16b( &zclColor_CurrentY,
                          &zclColor_CurrentY_256,
                          &zclColor_StepColorY_256,
                          &zclColor_ColorRemainingTime,
                          COLOR_XY_MIN, COLOR_XY_MAX, FALSE );

  if ( zclColor_ColorRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return;
}

/*********************************************************************
 * @fn      zclColor_StepColorCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control -  Step Color Command for
 *          this application.
 *
 * @param   stepX          - X step size
 * @param   stepY          - Y step size
 * @param   transitionTime - time to perform a single step in 1/10 of second
 *
 * @return  status
 */
ZStatus_t zclColor_StepColorCB( zclCCStepColor_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_X_Y);
  zclColor_CurrentX_256 = ((int32)zclColor_CurrentX)<<8;
  zclColor_CurrentY_256 = ((int32)zclColor_CurrentY)<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_X_Y;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_X_Y;

  if ( pCmd->transitionTime == 0 )
  {
    zclColor_ColorRemainingTime = 1;
  }
  else
  {
    zclColor_ColorRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepColorX_256 = ((((int32)pCmd->stepX)<<8) / zclColor_ColorRemainingTime);
  zclColor_StepColorY_256 = ((((int32)pCmd->stepY)<<8) / zclColor_ColorRemainingTime);

  hwLight_ApplyUpdate16b( &zclColor_CurrentX,
                          &zclColor_CurrentX_256,
                          &zclColor_StepColorX_256,
                          &zclColor_ColorRemainingTime,
                          COLOR_XY_MIN, COLOR_XY_MAX, FALSE );
  if ( zclColor_ColorRemainingTime != 0xFFFF )
  {
    zclColor_ColorRemainingTime++;
  }
  hwLight_ApplyUpdate16b( &zclColor_CurrentY,
                          &zclColor_CurrentY_256,
                          &zclColor_StepColorY_256,
                          &zclColor_ColorRemainingTime,
                          COLOR_XY_MIN, COLOR_XY_MAX, FALSE );

  if ( zclColor_ColorRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_MoveToSaturationCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move to Hue Command for
 *          this application.
 *
 * @param   hue - target hue value
 * @param   direction
 * @param   transitionTime - in seconds
 *
 * @return  status
 */
ZStatus_t zclColor_MoveToSaturationCB( zclCCMoveToSaturation_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_CurrentSaturation_256 = (uint16)zclColor_CurrentSaturation<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;

  //if transition time = 0 then do immediately
  if( pCmd->transitionTime == 0)
  {
    zclColor_SaturationRemainingTime = 1;
  }
  else
  {
    zclColor_SaturationRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepSaturation_256 = ((int32)(pCmd->saturation - zclColor_CurrentSaturation))<<8;
  zclColor_StepSaturation_256 /= (int32)zclColor_SaturationRemainingTime;

  hwLight_ApplyUpdate( &zclColor_CurrentSaturation,
                       &zclColor_CurrentSaturation_256,
                       &zclColor_StepSaturation_256,
                       &zclColor_SaturationRemainingTime,
                       COLOR_SAT_MIN, COLOR_SAT_MAX, FALSE );

  if ( zclColor_SaturationRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_MoveSaturationCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move Hue Command for
 *          this application.
 *
 * @param   moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP, or
 *                     LIGHTING_MOVE_HUE_DOWN
 * @param   rate - the movement in steps per second, where step is a
 *                 change in the device's hue of one unit
 *
 * @return  status
 */
ZStatus_t zclColor_MoveSaturationCB( zclCCMoveSaturation_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_CurrentSaturation_256 = (uint16)zclColor_CurrentSaturation<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;

  switch (pCmd->moveMode)
  {
    case LIGHTING_MOVE_SATURATION_UP:
      zclColor_StepSaturation_256 = (((int32)pCmd->rate)<<8)/10;
      //Change for ever - stop mode stop this command
      zclColor_SaturationRemainingTime = 0xFFFF;
      break;
    case LIGHTING_MOVE_SATURATION_DOWN:
      zclColor_StepSaturation_256 = ((-(int32)pCmd->rate)<<8)/10;
      //Change for ever - stop mode stop this command
      zclColor_SaturationRemainingTime = 0xFFFF;
      break;
    case LIGHTING_MOVE_SATURATION_STOP:
      zclColor_StepSaturation_256 = 0;
      zclColor_SaturationRemainingTime = 0x0;
      break;
   default:
     return ( ZCL_STATUS_INVALID_FIELD );
  }

  hwLight_ApplyUpdate( &zclColor_CurrentSaturation,
                       &zclColor_CurrentSaturation_256,
                       &zclColor_StepSaturation_256,
                       &zclColor_SaturationRemainingTime,
                       COLOR_SAT_MIN, COLOR_SAT_MAX, FALSE );


  if ( zclColor_SaturationRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_StepSaturationCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control -  Step Saturation Command for
 *          this application.
 *
 * @param   stepMode - LIGHTING_STEP_SATURATION_UP,
 *                     LIGHTING_STEP_SATURATION_DOWN
 * @param   transitionTime - time to perform a single step in 1/10 of second
 *
 * @return  status
 */
ZStatus_t zclColor_StepSaturationCB( zclCCStepSaturation_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_CurrentSaturation_256 = (uint16)zclColor_CurrentSaturation<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;

  if ( pCmd->transitionTime == 0 )
  {
    zclColor_SaturationRemainingTime = 1;
  }
  else
  {
    zclColor_SaturationRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepSaturation_256 = ((((int32)pCmd->stepSize)<<8) / zclColor_SaturationRemainingTime);

  if ( pCmd->stepMode == LIGHTING_STEP_SATURATION_DOWN )
  {
    zclColor_StepSaturation_256 = -zclColor_StepSaturation_256;
  }
  else if ( pCmd->stepMode != LIGHTING_STEP_SATURATION_UP )
  {
    return ( ZCL_STATUS_INVALID_FIELD );
  }


  hwLight_ApplyUpdate( &zclColor_CurrentSaturation,
                       &zclColor_CurrentSaturation_256,
                       &zclColor_StepSaturation_256,
                       &zclColor_SaturationRemainingTime,
                       COLOR_SAT_MIN, COLOR_SAT_MAX, FALSE );

  if ( zclColor_SaturationRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );

}

/*********************************************************************
 * @fn      zclTestApp_ColorControlMoveToHueCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move to Hue Command for
 *          this application.
 *
 * @param   hue - target hue value
 * @param   direction
 * @param   transitionTime - in seconds
 *
 * @return  status
 */
ZStatus_t zclColor_MoveToHueCB( zclCCMoveToHue_t *pCmd )
{
  int16 hueDiff;
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_CurrentHue_256 = (uint16)zclColor_CurrentHue<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;

  hueDiff = (int16)pCmd->hue - zclColor_CurrentHue;
  // adjust direction
  switch ( pCmd->direction )
  {
    case LIGHTING_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE:
      if  ( hueDiff > COLOR_HUE_MAX/2 )
      {
        hueDiff -= ( COLOR_HUE_MAX + 1 ); // extra step to wrap
      }
      else if ( hueDiff < -COLOR_HUE_MAX/2 )
      {
        hueDiff += COLOR_HUE_MAX + 1; // extra step to wrap
      }
      break;
    case LIGHTING_MOVE_TO_HUE_DIRECTION_LONGEST_DISTANCE:
      if ( ( hueDiff > 0 ) && ( hueDiff < COLOR_HUE_MAX/2 ) )
      {
        hueDiff -= ( COLOR_HUE_MAX + 1 ); // extra step to wrap
      }
      else if ( ( hueDiff < 0 ) && ( hueDiff > -COLOR_HUE_MAX/2 ) )
      {
        hueDiff += COLOR_HUE_MAX + 1; // extra step to wrap
      }
      break;
    case LIGHTING_MOVE_TO_HUE_DIRECTION_UP:
      if ( hueDiff < 0 )
      {
        hueDiff += COLOR_HUE_MAX;
      }
      break;
    case LIGHTING_MOVE_TO_HUE_DIRECTION_DOWN:
      if ( hueDiff > 0 )
      {
        hueDiff -= COLOR_HUE_MAX;
      }
      break;
    default:
      return ( ZCL_STATUS_INVALID_FIELD );
  }

  //if transition time = 0 then do immediately
  if ( pCmd->transitionTime == 0 )
  {
      zclColor_HueRemainingTime = 1;
  }
  else
  {
    zclColor_HueRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepHue_256 = ((int32)(hueDiff))<<8;
  zclColor_StepHue_256 /= (int32)zclColor_HueRemainingTime;

  hwLight_ApplyUpdate( &zclColor_CurrentHue,
                       &zclColor_CurrentHue_256,
                       &zclColor_StepHue_256,
                       &zclColor_HueRemainingTime,
                       COLOR_HUE_MIN, COLOR_HUE_MAX, TRUE );

  if ( zclColor_HueRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_MoveHueCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move Hue Command for
 *          this application.
 *
 * @param   moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP, or
 *                     LIGHTING_MOVE_HUE_DOWN
 * @param   rate - the movement in steps per second, where step is a
 *                 change in the device's hue of one unit
 *
 * @return  status
 */
ZStatus_t zclColor_MoveHueCB( zclCCMoveHue_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_CurrentHue_256 = (uint16)zclColor_CurrentHue<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedHueRemainingTime = 0x0;

  switch (pCmd->moveMode)
  {
    case LIGHTING_MOVE_HUE_UP:
      zclColor_StepHue_256 = (((int32)pCmd->rate)<<8)/10;
      //Change for ever - stop mode stop this command
      zclColor_HueRemainingTime = 0xFFFF;
      break;
    case LIGHTING_MOVE_HUE_DOWN:
      zclColor_StepHue_256 = ((-(int32)pCmd->rate)<<8)/10;
      //Change for ever - stop mode stop this command
      zclColor_HueRemainingTime = 0xFFFF;
      break;
    case LIGHTING_MOVE_HUE_STOP:
      zclColor_StepHue_256 = 0;
      zclColor_StepEnhancedHue_256 = 0;
      zclColor_HueRemainingTime = 0x0;
      break;
    default:
      return ( ZCL_STATUS_INVALID_FIELD );
  }

  hwLight_ApplyUpdate( &zclColor_CurrentHue,
                       &zclColor_CurrentHue_256,
                       &zclColor_StepHue_256,
                       &zclColor_HueRemainingTime,
                       COLOR_HUE_MIN, COLOR_HUE_MAX, TRUE );

  if ( zclColor_HueRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_StepHueCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control -  Step Saturation Command for
 *          this application.
 *
 * @param   stepMode - LIGHTING_STEP_SATURATION_UP,
 *                     LIGHTING_STEP_SATURATION_DOWN
 * @param   transitionTime - time to perform a single step in 1/10 of second
 *
 * @return  status
 */
ZStatus_t zclColor_StepHueCB( zclCCStepHue_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_CurrentHue_256 = (uint16)zclColor_CurrentHue<<8;
  zclColor_ColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode=COLOR_MODE_CURRENT_HUE_SATURATION;

  if ( pCmd->transitionTime == 0 )
  {
    zclColor_HueRemainingTime = 1;
  }
  else
  {
    zclColor_HueRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepHue_256 = ((((int32)pCmd->stepSize)<<8) / zclColor_HueRemainingTime);

  if ( pCmd->stepMode == LIGHTING_STEP_HUE_DOWN )
  {
    zclColor_StepHue_256 = -zclColor_StepHue_256;
  }
  else if ( pCmd->stepMode != LIGHTING_STEP_HUE_UP )
  {
    return ( ZCL_STATUS_INVALID_FIELD );
  }

  hwLight_ApplyUpdate( &zclColor_CurrentHue,
                       &zclColor_CurrentHue_256,
                       &zclColor_StepHue_256,
                       &zclColor_HueRemainingTime,
                       COLOR_HUE_MIN, COLOR_HUE_MAX, TRUE );

  if ( zclColor_HueRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_StopCB
 *
 * @brief   Callback from the ZCL General Cluster Library when
 *          it received a Color Control - Stop Command for
 *          this application.
 *
 * @param   none
 *
 * @return  status
 */
ZStatus_t zclColor_StopCB( void )
{
    zclColor_SaturationRemainingTime = 0;
    zclColor_EnhancedHueRemainingTime = 0;
    zclColor_HueRemainingTime = 0;
    zclColor_ColorRemainingTime = 0;

    return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_MoveToHueAndSaturationCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move to Hue Command for
 *          this application.
 *
 * @param    uint8 hue;             // a target hue
 * @param    uint8 saturation;      // a target saturation
 * @param    uint16 transitionTime; // time to move, equal of the value of the field in 1/10 seconds
 *
 * @return  status
 */
ZStatus_t zclColor_MoveToHueAndSaturationCB( zclCCMoveToHueAndSaturation_t *pCmd )
{
  zclCCMoveToHue_t hueCmd;
  zclCCMoveToSaturation_t satCmd;

  //Do hue
  hueCmd.hue = pCmd->hue;
  hueCmd.direction = LIGHTING_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE;
  hueCmd.transitionTime = pCmd->transitionTime;
  zclColor_MoveToHueCB( &hueCmd );

  //Do sat
  satCmd.saturation = pCmd->saturation;
  satCmd.transitionTime = pCmd->transitionTime;
  zclColor_MoveToSaturationCB( &satCmd );

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_EnhMoveToHueCB
 *
 * @brief   This callback is called to process a enhance Move To Hue
 *          Request command.
 *
 * @param   pCmd - command
 *                   uint16 enhancedHue;    // target enhanced hue value
 *                   uint8 direction;       // direction field shall be set to 0x01, meaning linear transition through XY space
 *                   uint16 transitionTime; // tame taken to move to the target hue in 1/10 sec increments
 *
 * @return  status
 */
ZStatus_t zclColor_EnhMoveToHueCB( zclCCEnhancedMoveToHue_t *pCmd )
{
  int32 hueDiff;
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_EnhancedCurrentHue_256 = (uint32)zclColor_EnhancedCurrentHue<<8;
  zclColor_ColorMode = COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode = ENHANCED_COLOR_MODE_ENHANCED_CURRENT_HUE_SATURATION;

  hueDiff = (int32)pCmd->enhancedHue - zclColor_EnhancedCurrentHue;
  // adjust direction
  switch ( pCmd->direction )
  {
    case LIGHTING_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE:
      if  ( hueDiff > COLOR_ENH_HUE_MAX/2 )
      {
        hueDiff -= ( COLOR_ENH_HUE_MAX + 1 ); // extra step to wrap
      }
      else if ( hueDiff < -COLOR_ENH_HUE_MAX/2 )
      {
        hueDiff += COLOR_ENH_HUE_MAX + 1; // extra step to wrap
      }
      break;
    case LIGHTING_MOVE_TO_HUE_DIRECTION_LONGEST_DISTANCE:
      if ( ( hueDiff > 0 ) && ( hueDiff < COLOR_ENH_HUE_MAX/2 ) )
      {
        hueDiff -= ( COLOR_ENH_HUE_MAX + 1 ); // extra step to wrap
      }
      else if ( ( hueDiff < 0 ) && ( hueDiff > -COLOR_ENH_HUE_MAX/2 ) )
      {
        hueDiff += COLOR_HUE_MAX + 1; // extra step to wrap
      }
      break;
    case LIGHTING_MOVE_TO_HUE_DIRECTION_UP:
      if ( hueDiff < 0 )
      {
        hueDiff += COLOR_ENH_HUE_MAX;
      }
      break;
    case LIGHTING_MOVE_TO_HUE_DIRECTION_DOWN:
      if ( hueDiff > 0 )
      {
        hueDiff -= COLOR_ENH_HUE_MAX;
      }
      break;
    default:
      return ( ZCL_STATUS_INVALID_FIELD );
  }

  //if transition time = 0 then do immediately
  if ( pCmd->transitionTime == 0 )
  {
      zclColor_EnhancedHueRemainingTime = 1;
  }
  else
  {
    zclColor_EnhancedHueRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepEnhancedHue_256 = ((int32)(hueDiff))<<8;
  zclColor_StepEnhancedHue_256 /= (int32)zclColor_EnhancedHueRemainingTime;

  hwLight_ApplyUpdate16b( &zclColor_EnhancedCurrentHue,
                          &zclColor_EnhancedCurrentHue_256,
                          &zclColor_StepEnhancedHue_256,
                          &zclColor_EnhancedHueRemainingTime,
                          COLOR_ENH_HUE_MIN, COLOR_ENH_HUE_MAX, TRUE );

  if ( zclColor_EnhancedHueRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}


/*********************************************************************
 * @fn      zclColor_MoveEnhHueCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move Hue Command for
 *          this application.
 *
 * @param   moveMode - LIGHTING_MOVE_HUE_STOP, LIGHTING_MOVE_HUE_UP, or
 *                     LIGHTING_MOVE_HUE_DOWN
 * @param   rate - the movement in steps per second, where step is a
 *                 change in the device's hue of one unit
 *
 * @return  status
 */
ZStatus_t zclColor_MoveEnhHueCB( zclCCEnhancedMoveHue_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_EnhancedCurrentHue_256 = (uint32)zclColor_EnhancedCurrentHue<<8;
  zclColor_ColorMode = COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode = ENHANCED_COLOR_MODE_ENHANCED_CURRENT_HUE_SATURATION;

  switch (pCmd->moveMode)
  {
    case LIGHTING_MOVE_HUE_UP:
      zclColor_StepEnhancedHue_256 = (((int32)pCmd->rate)<<8)/10;
      //Change for ever - stop mode stop this command
      zclColor_EnhancedHueRemainingTime = 0xFFFF;
      break;
    case LIGHTING_MOVE_HUE_DOWN:
      zclColor_StepEnhancedHue_256 = ((-(int32)pCmd->rate)<<8)/10;
      //Change for ever - stop mode stop this command
      zclColor_EnhancedHueRemainingTime = 0xFFFF;
      break;
    case LIGHTING_MOVE_HUE_STOP:
      zclColor_StepEnhancedHue_256 = 0;
      zclColor_StepHue_256 = 0;
      zclColor_EnhancedHueRemainingTime = 0x0;
      break;
    default:
      return ( ZCL_STATUS_INVALID_FIELD );
  }

  hwLight_ApplyUpdate16b( &zclColor_EnhancedCurrentHue,
                          &zclColor_EnhancedCurrentHue_256,
                          &zclColor_StepEnhancedHue_256,
                          &zclColor_EnhancedHueRemainingTime,
                          COLOR_ENH_HUE_MIN, COLOR_ENH_HUE_MAX, TRUE );

  if ( zclColor_EnhancedHueRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100);
  }

  return ( ZSuccess );
}


/*********************************************************************
 * @fn      zclColor_StepEnhHueCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control -  Step Saturation Command for
 *          this application.
 *
 * @param   stepMode - LIGHTING_STEP_SATURATION_UP,
 *                     LIGHTING_STEP_SATURATION_DOWN
 * @param   transitionTime - time to perform a single step in 1/10 of second
 *
 * @return  status
 */
ZStatus_t zclColor_StepEnhHueCB( zclCCEnhancedStepHue_t *pCmd )
{
  hwLight_UpdateColorMode(COLOR_MODE_CURRENT_HUE_SATURATION);
  zclColor_EnhancedCurrentHue_256 = (uint32)zclColor_EnhancedCurrentHue<<8;
  zclColor_ColorMode = COLOR_MODE_CURRENT_HUE_SATURATION;
  zclColor_EnhancedColorMode = ENHANCED_COLOR_MODE_ENHANCED_CURRENT_HUE_SATURATION;

  if ( pCmd->transitionTime == 0 )
  {
    zclColor_EnhancedHueRemainingTime = 1;
  }
  else
  {
    zclColor_EnhancedHueRemainingTime = pCmd->transitionTime;
  }

  zclColor_StepEnhancedHue_256 = ((((int32)pCmd->stepSize)<<8) / zclColor_EnhancedHueRemainingTime);

  if ( pCmd->stepMode == LIGHTING_STEP_HUE_DOWN )
  {
    zclColor_StepEnhancedHue_256 = -zclColor_StepEnhancedHue_256;
  }
  else if ( pCmd->stepMode != LIGHTING_STEP_HUE_UP )
  {
    return ( ZCL_STATUS_INVALID_FIELD );
  }

  hwLight_ApplyUpdate16b( &zclColor_EnhancedCurrentHue,
                          &zclColor_EnhancedCurrentHue_256,
                          &zclColor_StepEnhancedHue_256,
                          &zclColor_EnhancedHueRemainingTime,
                          COLOR_ENH_HUE_MIN, COLOR_ENH_HUE_MAX, TRUE );

  if ( zclColor_EnhancedHueRemainingTime )
  {
    //set a timer to make the change over 100ms
    osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 100 );
  }

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_MoveToHueAndSaturationCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Move to Hue Command for
 *          this application.
 *
 * @param    uint8 hue;             // a target hue
 * @param    uint8 saturation;      // a target saturation
 * @param    uint16 transitionTime; // time to move, equal of the value of the field in 1/10 seconds
 *
 * @return  status
 */
ZStatus_t zclColor_MoveToEnhHueAndSaturationCB( zclCCEnhancedMoveToHueAndSaturation_t *pCmd )
{
  zclCCEnhancedMoveToHue_t hueCmd;
  zclCCMoveToSaturation_t satCmd;

  //Do sat - do this first so color mode is correct.
  satCmd.saturation = pCmd->saturation;
  satCmd.transitionTime = pCmd->transitionTime;
  zclColor_MoveToSaturationCB( &satCmd );

  //Do hue
  hueCmd.enhancedHue = pCmd->enhancedHue;
  hueCmd.direction = LIGHTING_MOVE_TO_HUE_DIRECTION_SHORTEST_DISTANCE;
  hueCmd.transitionTime = pCmd->transitionTime;
  zclColor_EnhMoveToHueCB( &hueCmd );

  return ( ZSuccess );
}

/*********************************************************************
 * @fn      zclColor_SetColorLoopCB
 *
 * @brief   Callback from the ZCL Lighting Cluster Library when
 *          it received a Color Control - Color Loop Set Command for
 *          this application.
 *
 * @param   updateFlags - which color loop attributes to update before the color loop is started.
 * @param   action - action to take for the color loop
 * @param   direction - direction for the color loop (decrement or increment)
 * @param   time - number of seconds over which to perform a full color loop
 * @param   startHue - starting hue to use for the color loop
 *
 * @return  status
 */
ZStatus_t zclColor_SetColorLoopCB( zclCCColorLoopSet_t *pCmd )
{
  if ( pCmd->updateFlags.bits.direction )
  {
    zclColor_ColorLoopDirection = pCmd->direction;
  }
  if  ( pCmd->updateFlags.bits.time )
  {
    zclColor_ColorLoopTime = pCmd->time;
  }
  if  ( pCmd->updateFlags.bits.startHue )
  {
    zclColor_ColorLoopStartEnhancedHue = pCmd->startHue;
  }
  if  ( pCmd->updateFlags.bits.action )
  {
    switch ( pCmd->action )
    {
      case LIGHTING_COLOR_LOOP_ACTION_DEACTIVATE:
        if ( zclColor_ColorLoopActive )
        {
          osal_stop_timerEx( zclLight_TaskID, COLOR_LOOP_PROCESS_EVT );
          zclColor_ColorLoopActive = 0;
          // reset to previous
          zclColor_EnhancedCurrentHue = zclColor_ColorLoopStoredEnhancedHue;
          zclColor_HueRemainingTime = 1;
          osal_start_timerEx( zclLight_TaskID, COLOR_PROCESS_EVT , 0 );
        }
        break;

      case LIGHTING_COLOR_LOOP_ACTION_ACTIVATE_FROM_START_HUE:
        zclColor_ColorLoopStoredEnhancedHue = zclColor_EnhancedCurrentHue;
        zclColor_EnhancedCurrentHue = zclColor_ColorLoopStartEnhancedHue;
        zclColor_ColorLoopActive = 1;
        osal_start_timerEx( zclLight_TaskID, COLOR_LOOP_PROCESS_EVT , 100 );
        break;

      case LIGHTING_COLOR_LOOP_ACTION_ACTIVATE_FROM_ENH_CURR_HUE:
        zclColor_ColorLoopStoredEnhancedHue = zclColor_EnhancedCurrentHue;
        zclColor_ColorLoopActive = 1;
        osal_start_timerEx( zclLight_TaskID, COLOR_LOOP_PROCESS_EVT , 100 );
        break;

     default:
       return ( ZCL_STATUS_INVALID_FIELD );
    }
  }
  return ( ZSuccess );
}

#endif //ZCL_COLOR_CTRL
/****************************************************************************
****************************************************************************/


