/* Copyright (c) 2022 Szabadi L�szl� Zsolt
 * You should have received a copy of the GNU AGPL v3.0 license with
 * this file. If not, please write to: pliexe, or visit : https://github.com/Pliexe/HeliosEngine/blob/master/HeliosEngine/LICENSE.txt
 */
#pragma once
#include "pch.h"

#ifdef HELIOS_PLATFORM_WINDOWS // Windows

#define		HL_KEY_MOUSE_LEFT		1
#define		HL_KEY_MOUSE_RIGHT		2
#define		HL_KEY_MOUSE_MIDDLE		4

#define		HL_KEY_MOUSE_X1			5
#define		HL_KEY_MOUSE_X2			6

#define		HL_KEY_BACKSPACE		8
#define		HL_KEY_TAB				9
#define		HL_KEY_CLEAR			12
#define		HL_KEY_RETURN			13
#define		HL_KEY_ENTER			13

#define		HL_KEY_SHIFT			16
#define		HL_KEY_CONTROL			17
#define		HL_KEY_MENU				18
#define		HL_KEY_ALT				18
#define		HL_KEY_PAUSE			19
#define		HL_KEY_CAPITAL			20
#define		HL_KEY_CAPSLOCK			20

#define		HL_KEY_ESCAPE			27
#define		HL_KEY_SPACE			32

#define		HL_KEY_PAGE_UP			33
#define		HL_KEY_PAGE_DOWN		34

#define		HL_KEY_END				35
#define		HL_KEY_HOME				36

#define		HL_KEY_ARROW_LEFT		37
#define		HL_KEY_ARROW_UP			38
#define		HL_KEY_ARROW_RIGHT		39
#define		HL_KEY_ARROW_DOWN		40

#define		HL_KEY_SELECT			41
#define		HL_KEY_PRINT			42
#define		HL_KEY_EXECUTE			43
#define		HL_KEY_PRINTSCREEN		44
#define		HL_KEY_SNAPSHOT			44

#define		HL_KEY_INSERT			45
#define		HL_KEY_DELETE			46
#define		HL_KEY_HELP				47

#define		HL_KEY_0				48
#define		HL_KEY_1				49
#define		HL_KEY_2				50
#define		HL_KEY_3				51
#define		HL_KEY_4				52
#define		HL_KEY_5				53
#define		HL_KEY_6				54
#define		HL_KEY_7				55
#define		HL_KEY_8				56
#define		HL_KEY_9				57

#define 	HL_KEY_A				65
#define 	HL_KEY_B				66
#define 	HL_KEY_C				67
#define 	HL_KEY_D				68
#define 	HL_KEY_E				69
#define 	HL_KEY_F				70
#define 	HL_KEY_G				71
#define 	HL_KEY_H				72
#define 	HL_KEY_I				73
#define 	HL_KEY_J				74
#define 	HL_KEY_K				75
#define 	HL_KEY_L				76
#define 	HL_KEY_M				77
#define 	HL_KEY_N				78
#define 	HL_KEY_O				79
#define 	HL_KEY_P				80
#define 	HL_KEY_Q				81
#define 	HL_KEY_R				82
#define 	HL_KEY_S				83
#define 	HL_KEY_T				84
#define 	HL_KEY_U				85
#define 	HL_KEY_V				86
#define 	HL_KEY_W				87
#define 	HL_KEY_X				88
#define 	HL_KEY_Y				89
#define 	HL_KEY_Z				90

#define		HL_KEY_WINDOWS_LEFT		91
#define		HL_KEY_WINDOWS_RIGHT	92
			 
#define		HL_KEY_SUPER_LEFT		91
#define		HL_KEY_SUPER_RIGHT		92
			 
#define		HL_KEY_APPS				93
			 
#define		HL_KEY_SLEEP			95

#define		HL_KEY_NUMPAD_0			96
#define		HL_KEY_NUMPAD_1			97
#define		HL_KEY_NUMPAD_2			98
#define		HL_KEY_NUMPAD_3			99
#define		HL_KEY_NUMPAD_4			100
#define		HL_KEY_NUMPAD_5			101
#define		HL_KEY_NUMPAD_6			102
#define		HL_KEY_NUMPAD_7			103
#define		HL_KEY_NUMPAD_8			104
#define		HL_KEY_NUMPAD_9			105

#define		HL_KEY_MULTIPLY			106
#define		HL_KEY_ADD				107
#define		HL_KEY_SEPARATOR		108
#define		HL_KEY_SBUTRACT			109
#define		HL_KEY_DECIMAL			110
#define		HL_KEY_DIVIDE			111

#define		HL_KEY_F1				112
#define		HL_KEY_F2				113
#define		HL_KEY_F3				114
#define		HL_KEY_F4				115
#define		HL_KEY_F5				116
#define		HL_KEY_F6				117
#define		HL_KEY_F7				118
#define		HL_KEY_F8				119
#define		HL_KEY_F9				120
#define		HL_KEY_F10				121
#define		HL_KEY_F11				122
#define		HL_KEY_F12				123
#define		HL_KEY_F13				124
#define		HL_KEY_F14				125
#define		HL_KEY_F15				126
#define		HL_KEY_F16				127
#define		HL_KEY_F17				128
#define		HL_KEY_F18				129
#define		HL_KEY_F19				130
#define		HL_KEY_F20				131
#define		HL_KEY_F21				132
#define		HL_KEY_F22				133
#define		HL_KEY_F23				134
#define		HL_KEY_F24				135

#define		HL_KEY_NUMLOCK			144
#define		HL_KEY_SCROLLLOCK		145

#define		HL_KEY_SHIFT_LEFT		160
#define		HL_KEY_SHIFT_RIGHT		161

#define		HL_KEY_CONTROL_LEFT		162
#define		HL_KEY_CONTROL_RIGHT	163

#define		HL_KEY_ALT_LEFT			164
#define		HL_KEY_ALT_RIGHT		165

#define		HL_BROWSER_BACK			0xA6
#define		HL_BROWSER_FORWARD		0xA7
#define		HL_BROWSER_REFRESH		0xA8
#define		HL_BROWSER_STOP			0xA9
#define		HL_BROWSER_SEARCH		0xAA
#define		HL_BROWSER_FAVORITES	0xAB
#define		HL_BROWSER_HOME			0xAC

#define		HL_VOLUME_MUTE			0xAD
#define		HL_VOLUME_DOWN			0xAE
#define		HL_VOLUME_UP			0xAF
#define		HL_MEDIA_NEXT_TRACK		0xB0
#define		HL_MEDIA_PREV_TRACK		0xB1
#define		HL_MEDIA_STOP			0xB2
#define		HL_MEDIA_PLAY_PAUSE		0xB3
#define		HL_LAUNCH_MAIL			0xB4
#define		HL_LAUNCH_MEDIA_SELECT	0xB5
#define		HL_LAUNCH_APP1			0xB6
#define		HL_LAUNCH_APP2			0xB7

#define		HL_OEM_1				0xBA   // ';:' for US
#define		HL_OEM_PLUS				0xBB   // '+' any country
#define		HL_OEM_COMMA			0xBC   // ',' any country
#define		HL_OEM_MINUS			0xBD   // '-' any country
#define		HL_OEM_PERIOD			0xBE   // '.' any country
#define		HL_OEM_2				0xBF   // '/?' for US
#define		HL_OEM_3				0xC0   // '`~' for US

#define		HL_OEM_4				0xDB  //  '[{' for US
#define		HL_OEM_5				0xDC  //  '\|' for US
#define		HL_OEM_6				0xDD  //  ']}' for US
#define		HL_OEM_7				0xDE  //  ''"' for US
#define		HL_OEM_8				0xDF
			
#define		HL_OEM_AX				0xE1  //  'AX' key on Japanese AX kbd
#define		HL_OEM_102				0xE2  //  "<>" or "\|" on RT 102-key kbd.
#define		HL_ICO_HELP				0xE3  //  Help key on ICO
#define		HL_ICO_00				0xE4  //  00 key on ICO

#define		HL_ATTN					0xF6
#define		HL_CRSEL				0xF7
#define		HL_EXSEL				0xF8
#define		HL_EREOF				0xF9
#define		HL_PLAY					0xFA
#define		HL_ZOOM					0xFB
#define		HL_NONAME				0xFC
#define		HL_PA1					0xFD
#define		HL_OEM_CLEAR			0xFE

#elif defined(__linux__) // GLFW

#include <GLFW/glfw3.h>

#define		HL_KEY_MOUSE_LEFT		GLFW_MOUSE_BUTTON_LEFT
#define		HL_KEY_MOUSE_RIGHT		GLFW_MOUSE_BUTTON_RIGHT
#define		HL_KEY_MOUSE_MIDDLE		GLFW_MOUSE_BUTTON_MIDDLE

#define		HL_KEY_MOUSE_X1			GLFW_MOUSE_BUTTON_4
#define		HL_KEY_MOUSE_X2			GLFW_MOUSE_BUTTON_5

#define		HL_KEY_BACKSPACE		GLFW_KEY_BACKSPACE
#define		HL_KEY_TAB				GLFW_KEY_TAB
#define		HL_KEY_CLEAR			GLFW_KEY_CLEAR
#define		HL_KEY_RETURN			GLFW_KEY_ENTER
#define		HL_KEY_ENTER			GLFW_KEY_ENTER

#define		HL_KEY_SHIFT			GLFW_KEY_LEFT_SHIFT  // or GLFW_KEY_RIGHT_SHIFT if you want generic
#define		HL_KEY_CONTROL			GLFW_KEY_LEFT_CONTROL
#define		HL_KEY_MENU				GLFW_KEY_LEFT_ALT
#define		HL_KEY_ALT				GLFW_KEY_LEFT_ALT
#define		HL_KEY_PAUSE			0  // GLFW does not define PAUSE key, you can define your own or leave 0
#define		HL_KEY_CAPITAL			0  // CapsLock is GLFW_KEY_CAPS_LOCK but it's unsigned int, use as below
#define		HL_KEY_CAPSLOCK			GLFW_KEY_CAPS_LOCK

#define		HL_KEY_ESCAPE			GLFW_KEY_ESCAPE
#define		HL_KEY_SPACE			GLFW_KEY_SPACE

#define		HL_KEY_PAGE_UP			GLFW_KEY_PAGE_UP
#define		HL_KEY_PAGE_DOWN		GLFW_KEY_PAGE_DOWN

#define		HL_KEY_END				GLFW_KEY_END
#define		HL_KEY_HOME				GLFW_KEY_HOME

#define		HL_KEY_ARROW_LEFT		GLFW_KEY_LEFT
#define		HL_KEY_ARROW_UP			GLFW_KEY_UP
#define		HL_KEY_ARROW_RIGHT		GLFW_KEY_RIGHT
#define		HL_KEY_ARROW_DOWN		GLFW_KEY_DOWN

#define		HL_KEY_SELECT			0 // no direct GLFW key, define as 0 or your own
#define		HL_KEY_PRINT			0
#define		HL_KEY_EXECUTE			0
#define		HL_KEY_PRINTSCREEN		GLFW_KEY_PRINT_SCREEN
#define		HL_KEY_SNAPSHOT			GLFW_KEY_PRINT_SCREEN

#define		HL_KEY_INSERT			GLFW_KEY_INSERT
#define		HL_KEY_DELETE			GLFW_KEY_DELETE
#define		HL_KEY_HELP				0 // no direct GLFW key

#define		HL_KEY_0				GLFW_KEY_0
#define		HL_KEY_1				GLFW_KEY_1
#define		HL_KEY_2				GLFW_KEY_2
#define		HL_KEY_3				GLFW_KEY_3
#define		HL_KEY_4				GLFW_KEY_4
#define		HL_KEY_5				GLFW_KEY_5
#define		HL_KEY_6				GLFW_KEY_6
#define		HL_KEY_7				GLFW_KEY_7
#define		HL_KEY_8				GLFW_KEY_8
#define		HL_KEY_9				GLFW_KEY_9

#define 	HL_KEY_A				GLFW_KEY_A
#define 	HL_KEY_B				GLFW_KEY_B
#define 	HL_KEY_C				GLFW_KEY_C
#define 	HL_KEY_D				GLFW_KEY_D
#define 	HL_KEY_E				GLFW_KEY_E
#define 	HL_KEY_F				GLFW_KEY_F
#define 	HL_KEY_G				GLFW_KEY_G
#define 	HL_KEY_H				GLFW_KEY_H
#define 	HL_KEY_I				GLFW_KEY_I
#define 	HL_KEY_J				GLFW_KEY_J
#define 	HL_KEY_K				GLFW_KEY_K
#define 	HL_KEY_L				GLFW_KEY_L
#define 	HL_KEY_M				GLFW_KEY_M
#define 	HL_KEY_N				GLFW_KEY_N
#define 	HL_KEY_O				GLFW_KEY_O
#define 	HL_KEY_P				GLFW_KEY_P
#define 	HL_KEY_Q				GLFW_KEY_Q
#define 	HL_KEY_R				GLFW_KEY_R
#define 	HL_KEY_S				GLFW_KEY_S
#define 	HL_KEY_T				GLFW_KEY_T
#define 	HL_KEY_U				GLFW_KEY_U
#define 	HL_KEY_V				GLFW_KEY_V
#define 	HL_KEY_W				GLFW_KEY_W
#define 	HL_KEY_X				GLFW_KEY_X
#define 	HL_KEY_Y				GLFW_KEY_Y
#define 	HL_KEY_Z				GLFW_KEY_Z

#define		HL_KEY_WINDOWS_LEFT		GLFW_KEY_LEFT_SUPER
#define		HL_KEY_WINDOWS_RIGHT	GLFW_KEY_RIGHT_SUPER
			 
#define		HL_KEY_SUPER_LEFT		GLFW_KEY_LEFT_SUPER
#define		HL_KEY_SUPER_RIGHT		GLFW_KEY_RIGHT_SUPER
			 
#define		HL_KEY_APPS				0 // no direct GLFW key

#define		HL_KEY_SLEEP			0 // no direct GLFW key

#define		HL_KEY_NUMPAD_0			GLFW_KEY_KP_0
#define		HL_KEY_NUMPAD_1			GLFW_KEY_KP_1
#define		HL_KEY_NUMPAD_2			GLFW_KEY_KP_2
#define		HL_KEY_NUMPAD_3			GLFW_KEY_KP_3
#define		HL_KEY_NUMPAD_4			GLFW_KEY_KP_4
#define		HL_KEY_NUMPAD_5			GLFW_KEY_KP_5
#define		HL_KEY_NUMPAD_6			GLFW_KEY_KP_6
#define		HL_KEY_NUMPAD_7			GLFW_KEY_KP_7
#define		HL_KEY_NUMPAD_8			GLFW_KEY_KP_8
#define		HL_KEY_NUMPAD_9			GLFW_KEY_KP_9

#define		HL_KEY_MULTIPLY			GLFW_KEY_KP_MULTIPLY
#define		HL_KEY_ADD				GLFW_KEY_KP_ADD
#define		HL_KEY_SEPARATOR		0
#define		HL_KEY_SBUTRACT			GLFW_KEY_KP_SUBTRACT
#define		HL_KEY_DECIMAL			GLFW_KEY_KP_DECIMAL
#define		HL_KEY_DIVIDE			GLFW_KEY_KP_DIVIDE

#define		HL_KEY_F1				GLFW_KEY_F1
#define		HL_KEY_F2				GLFW_KEY_F2
#define		HL_KEY_F3				GLFW_KEY_F3
#define		HL_KEY_F4				GLFW_KEY_F4
#define		HL_KEY_F5				GLFW_KEY_F5
#define		HL_KEY_F6				GLFW_KEY_F6
#define		HL_KEY_F7				GLFW_KEY_F7
#define		HL_KEY_F8				GLFW_KEY_F8
#define		HL_KEY_F9				GLFW_KEY_F9
#define		HL_KEY_F10				GLFW_KEY_F10
#define		HL_KEY_F11				GLFW_KEY_F11
#define		HL_KEY_F12				GLFW_KEY_F12
#define		HL_KEY_F13				0
#define		HL_KEY_F14				0
#define		HL_KEY_F15				0
#define		HL_KEY_F16				0
#define		HL_KEY_F17				0
#define		HL_KEY_F18				0
#define		HL_KEY_F19				0
#define		HL_KEY_F20				0
#define		HL_KEY_F21				0
#define		HL_KEY_F22				0
#define		HL_KEY_F23				0
#define		HL_KEY_F24				0

#define		HL_KEY_NUMLOCK			GLFW_KEY_NUM_LOCK
#define		HL_KEY_SCROLLLOCK		GLFW_KEY_SCROLL_LOCK

#define		HL_KEY_SHIFT_LEFT		GLFW_KEY_LEFT_SHIFT
#define		HL_KEY_SHIFT_RIGHT		GLFW_KEY_RIGHT_SHIFT

#define		HL_KEY_CONTROL_LEFT		GLFW_KEY_LEFT_CONTROL
#define		HL_KEY_CONTROL_RIGHT	GLFW_KEY_RIGHT_CONTROL

#define		HL_KEY_ALT_LEFT			GLFW_KEY_LEFT_ALT
#define		HL_KEY_ALT_RIGHT		GLFW_KEY_RIGHT_ALT

// Browser/media keys and OEM keys have no GLFW equivalents, define 0 or your own mapping
#define		HL_BROWSER_BACK			0
#define		HL_BROWSER_FORWARD		0
#define		HL_BROWSER_REFRESH		0
#define		HL_BROWSER_STOP			0
#define		HL_BROWSER_SEARCH		0
#define		HL_BROWSER_FAVORITES	0
#define		HL_BROWSER_HOME			0

#define		HL_VOLUME_MUTE			0
#define		HL_VOLUME_DOWN			0
#define		HL_VOLUME_UP			0
#define		HL_MEDIA_NEXT_TRACK		0
#define		HL_MEDIA_PREV_TRACK		0
#define		HL_MEDIA_STOP			0
#define		HL_MEDIA_PLAY_PAUSE		0
#define		HL_LAUNCH_MAIL			0
#define		HL_LAUNCH_MEDIA_SELECT	0
#define		HL_LAUNCH_APP1			0
#define		HL_LAUNCH_APP2			0

#define		HL_OEM_1				0
#define		HL_OEM_PLUS				0
#define		HL_OEM_COMMA			0
#define		HL_OEM_MINUS			0
#define		HL_OEM_PERIOD			0
#define		HL_OEM_2				0
#define		HL_OEM_3				0

#define		HL_OEM_4				0
#define		HL_OEM_5				0
#define		HL_OEM_6				0
#define		HL_OEM_7				0
#define		HL_OEM_8				0
			
#define		HL_OEM_AX				0
#define		HL_OEM_102				0
#define		HL_ICO_HELP				0
#define		HL_ICO_00				0

#define		HL_ATTN					0
#define		HL_CRSEL				0
#define		HL_EXSEL				0
#define		HL_EREOF				0
#define		HL_PLAY					0
#define		HL_ZOOM					0
#define		HL_NONAME				0
#define		HL_PA1					0
#define		HL_OEM_CLEAR			0

#else

#error "Unsupported platform for HL key codes"

#endif
