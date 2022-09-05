/* Copyright (c) 2022 Szabadi László Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/VisualDiscordBotCreator/blob/master/LICENSE
 */
#pragma once
#include "pch.h"

#define		WN_KEY_MOUSE_LEFT		1
#define		WN_KEY_MOUSE_RIGHT		2
#define		WN_KEY_MOUSE_MIDDLE		4

#define		WN_KEY_MOUSE_X1			5
#define		WN_KEY_MOUSE_X2			6

#define		WN_KEY_BACKSPACE		8
#define		WN_KEY_TAB				9
#define		WN_KEY_CLEAR			12
#define		WN_KEY_RETURN			13
#define		WN_KEY_ENTER			13

#define		WN_KEY_SHIFT			16
#define		WN_KEY_CONTROL			17
#define		WN_KEY_MENU				18
#define		WN_KEY_ALT				18
#define		WN_KEY_PAUSE			19
#define		WN_KEY_CAPITAL			20
#define		WN_KEY_CAPSLOCK			20

#define		WN_KEY_ESCAPE			27
#define		WN_KEY_SPACE			32

#define		WN_KEY_PAGE_UP			33
#define		WN_KEY_PAGE_DOWN		34

#define		WN_KEY_END				35
#define		WN_KEY_HOME				36

#define		WN_KEY_ARROW_LEFT		37
#define		WN_KEY_ARROW_UP			38
#define		WN_KEY_ARROW_RIGHT		39
#define		WN_KEY_ARROW_DOWN		40

#define		WN_KEY_SELECT			41
#define		WN_KEY_PRINT			42
#define		WN_KEY_EXECUTE			43
#define		WN_KEY_PRINTSCREEN		44
#define		WN_KEY_SNAPSHOT			44

#define		WN_KEY_INSERT			45
#define		WN_KEY_DELETE			46
#define		WN_KEY_HELP				47

#define		WN_KEY_0				48
#define		WN_KEY_1				49
#define		WN_KEY_2				50
#define		WN_KEY_3				51
#define		WN_KEY_4				52
#define		WN_KEY_5				53
#define		WN_KEY_6				54
#define		WN_KEY_7				55
#define		WN_KEY_8				56
#define		WN_KEY_9				57

#define 	WN_KEY_A				65
#define 	WN_KEY_B				66
#define 	WN_KEY_C				67
#define 	WN_KEY_D				68
#define 	WN_KEY_E				69
#define 	WN_KEY_F				70
#define 	WN_KEY_G				71
#define 	WN_KEY_H				72
#define 	WN_KEY_I				73
#define 	WN_KEY_J				74
#define 	WN_KEY_K				75
#define 	WN_KEY_L				76
#define 	WN_KEY_M				77
#define 	WN_KEY_N				78
#define 	WN_KEY_O				79
#define 	WN_KEY_P				80
#define 	WN_KEY_Q				81
#define 	WN_KEY_R				82
#define 	WN_KEY_S				83
#define 	WN_KEY_T				84
#define 	WN_KEY_U				85
#define 	WN_KEY_V				86
#define 	WN_KEY_W				87
#define 	WN_KEY_X				88
#define 	WN_KEY_Y				89
#define 	WN_KEY_Z				90

#define		WN_KEY_WINDOWS_LEFT		91
#define		WN_KEY_WINDOWS_RIGHT	92
			 
#define		WN_KEY_SUPER_LEFT		91
#define		WN_KEY_SUPER_RIGHT		92
			 
#define		WN_KEY_APPS				93
			 
#define		WN_KEY_SLEEP			95

#define		WN_KEY_NUMPAD_0			96
#define		WN_KEY_NUMPAD_1			97
#define		WN_KEY_NUMPAD_2			98
#define		WN_KEY_NUMPAD_3			99
#define		WN_KEY_NUMPAD_4			100
#define		WN_KEY_NUMPAD_5			101
#define		WN_KEY_NUMPAD_6			102
#define		WN_KEY_NUMPAD_7			103
#define		WN_KEY_NUMPAD_8			104
#define		WN_KEY_NUMPAD_9			105

#define		WN_KEY_MULTIPLY			106
#define		WN_KEY_ADD				107
#define		WN_KEY_SEPARATOR		108
#define		WN_KEY_SBUTRACT			109
#define		WN_KEY_DECIMAL			110
#define		WN_KEY_DIVIDE			111

#define		WN_KEY_F1				112
#define		WN_KEY_F2				113
#define		WN_KEY_F3				114
#define		WN_KEY_F4				115
#define		WN_KEY_F5				116
#define		WN_KEY_F6				117
#define		WN_KEY_F7				118
#define		WN_KEY_F8				119
#define		WN_KEY_F9				120
#define		WN_KEY_F10				121
#define		WN_KEY_F11				122
#define		WN_KEY_F12				123
#define		WN_KEY_F13				124
#define		WN_KEY_F14				125
#define		WN_KEY_F15				126
#define		WN_KEY_F16				127
#define		WN_KEY_F17				128
#define		WN_KEY_F18				129
#define		WN_KEY_F19				130
#define		WN_KEY_F20				131
#define		WN_KEY_F21				132
#define		WN_KEY_F22				133
#define		WN_KEY_F23				134
#define		WN_KEY_F24				135

#define		WN_KEY_NUMLOCK			144
#define		WN_KEY_SCROLLLOCK		145

#define		WN_KEY_SHIFT_LEFT		160
#define		WN_KEY_SHIFT_RIGHT		161

#define		WN_KEY_CONTROL_LEFT		162
#define		WN_KEY_CONTROL_RIGHT	163

#define		WN_KEY_ALT_LEFT			164
#define		WN_KEY_ALT_RIGHT		165

#define		WN_BROWSER_BACK			0xA6
#define		WN_BROWSER_FORWARD		0xA7
#define		WN_BROWSER_REFRESH		0xA8
#define		WN_BROWSER_STOP			0xA9
#define		WN_BROWSER_SEARCH		0xAA
#define		WN_BROWSER_FAVORITES	0xAB
#define		WN_BROWSER_HOME			0xAC

#define		WN_VOLUME_MUTE			0xAD
#define		WN_VOLUME_DOWN			0xAE
#define		WN_VOLUME_UP			0xAF
#define		WN_MEDIA_NEXT_TRACK		0xB0
#define		WN_MEDIA_PREV_TRACK		0xB1
#define		WN_MEDIA_STOP			0xB2
#define		WN_MEDIA_PLAY_PAUSE		0xB3
#define		WN_LAUNCH_MAIL			0xB4
#define		WN_LAUNCH_MEDIA_SELECT	0xB5
#define		WN_LAUNCH_APP1			0xB6
#define		WN_LAUNCH_APP2			0xB7

#define		WN_OEM_1				0xBA   // ';:' for US
#define		WN_OEM_PLUS				0xBB   // '+' any country
#define		WN_OEM_COMMA			0xBC   // ',' any country
#define		WN_OEM_MINUS			0xBD   // '-' any country
#define		WN_OEM_PERIOD			0xBE   // '.' any country
#define		WN_OEM_2				0xBF   // '/?' for US
#define		WN_OEM_3				0xC0   // '`~' for US

#define		WN_OEM_4				0xDB  //  '[{' for US
#define		WN_OEM_5				0xDC  //  '\|' for US
#define		WN_OEM_6				0xDD  //  ']}' for US
#define		WN_OEM_7				0xDE  //  ''"' for US
#define		WN_OEM_8				0xDF
			
#define		WN_OEM_AX				0xE1  //  'AX' key on Japanese AX kbd
#define		WN_OEM_102				0xE2  //  "<>" or "\|" on RT 102-key kbd.
#define		WN_ICO_HELP				0xE3  //  Help key on ICO
#define		WN_ICO_00				0xE4  //  00 key on ICO

#define		WN_ATTN					0xF6
#define		WN_CRSEL				0xF7
#define		WN_EXSEL				0xF8
#define		WN_EREOF				0xF9
#define		WN_PLAY					0xFA
#define		WN_ZOOM					0xFB
#define		WN_NONAME				0xFC
#define		WN_PA1					0xFD
#define		WN_OEM_CLEAR			0xFE