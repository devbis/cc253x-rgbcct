/**************************************************************************************************
  Filename:       zcl_advancedlight.h
  Revised:        $Date: 2013-07-03 15:56:37 -0700 (Wed, 03 Jul 2013) $
  Revision:       $Revision: 34689 $

  Description:    This file contains the Zigbee Cluster Library Color Control
                  application callbacks definitions.


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
  PROVIDED �AS IS� WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
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

#ifndef ZCL_COLOR_CNTRL_H
#define ZCL_COLOR_CNTRL_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef ZCL_COLOR_CTRL

/*********************************************************************
 * INCLUDES
 */

#include "zcl_lighting.h"
#include "hw_light_ctrl.h"
//#include "zll_samplelight.h"

/*********************************************************************
 * CONSTANTS
 */

#define COLOR_XY_MIN                 0x0
#define COLOR_XY_MAX                 LIGHTING_COLOR_CURRENT_X_MAX
#define COLOR_SAT_MIN                0x0
#define COLOR_SAT_MAX                LIGHTING_COLOR_SAT_MAX
#define COLOR_HUE_MIN                0x0
#define COLOR_HUE_MAX                LIGHTING_COLOR_HUE_MAX
#define COLOR_ENH_HUE_MIN            0x0
#define COLOR_ENH_HUE_MAX            0xFFFF

//align this with the Application event defined in application header file
#define COLOR_PROCESS_EVT            SAMPLELIGHT_COLOR_PROCESS_EVT
#define COLOR_LOOP_PROCESS_EVT       SAMPLELIGHT_COLOR_LOOP_PROCESS_EVT

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * VARIABLES
 */
// Color control Cluster (server) -----------------------------------------------------

extern uint16 zclColor_CurrentX;
extern uint16 zclColor_CurrentY;
extern uint16 zclColor_EnhancedCurrentHue;
extern uint8  zclColor_CurrentHue;
extern uint8  zclColor_CurrentSaturation;
extern uint8  zclColor_ColorMode;
extern uint8  zclColor_EnhancedColorMode;
extern uint16 zclColor_ColorRemainingTime;
extern uint8 zclColor_ColorLoopActive;
extern uint8 zclColor_ColorLoopDirection;
extern uint16 zclColor_ColorLoopTime;
extern uint16 zclColor_ColorLoopStartEnhancedHue;
extern uint16 zclColor_ColorLoopStoredEnhancedHue;

/*********************************************************************
 * FUNCTIONS
 */
void zclColor_init( byte taskID);
void zclColor_process( uint16 *events );
void zclColor_processColorLoop( uint16 *events );

ZStatus_t zclColor_MoveToColorCB( zclCCMoveToColor_t *pCmd );
void zclColor_MoveColorCB( zclCCMoveColor_t *pCmd );
ZStatus_t zclColor_StepColorCB( zclCCStepColor_t *pCmd );
ZStatus_t zclColor_MoveToSaturationCB( zclCCMoveToSaturation_t *pCmd );
ZStatus_t zclColor_MoveSaturationCB( zclCCMoveSaturation_t *pCmd );
ZStatus_t zclColor_StepSaturationCB( zclCCStepSaturation_t *pCmd );
ZStatus_t zclColor_MoveToHueCB( zclCCMoveToHue_t *pCmd );
ZStatus_t zclColor_MoveHueCB( zclCCMoveHue_t *pCmd );
ZStatus_t zclColor_StepHueCB( zclCCStepHue_t *pCmd );
ZStatus_t zclColor_MoveToHueAndSaturationCB( zclCCMoveToHueAndSaturation_t *pCmd );
ZStatus_t zclColor_StopCB( void );
ZStatus_t zclColor_EnhMoveToHueCB( zclCCEnhancedMoveToHue_t *pCmd );
ZStatus_t zclColor_MoveEnhHueCB( zclCCEnhancedMoveHue_t *pCmd );
ZStatus_t zclColor_StepEnhHueCB( zclCCEnhancedStepHue_t *pCmd );
ZStatus_t zclColor_MoveToEnhHueAndSaturationCB( zclCCEnhancedMoveToHueAndSaturation_t *pCmd );
ZStatus_t zclColor_SetColorLoopCB( zclCCColorLoopSet_t *pCmd );

/*********************************************************************
*********************************************************************/
#endif //ZCL_COLOR_CTRL

#ifdef __cplusplus
}
#endif

#endif /* ZCL_COLOR_CNTRL_H */
