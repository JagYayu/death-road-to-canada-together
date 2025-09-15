/**
 * @file system/ScanCode.hpp
 * @author JagYayu
 * @brief
 * @version 1.0
 * @date 2025
 *
 * @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
 *
 */

#pragma once

#include "util/Micros.hpp"

#include "SDL3/SDL_scancode.h"

#include <cstdint>

namespace tudov
{
	enum class EScanCode : std::int32_t
	{
		// Alphabet Keys

		A = SDL_SCANCODE_A, // A
		B = SDL_SCANCODE_B, // B
		C = SDL_SCANCODE_C, // C
		D = SDL_SCANCODE_D, // D
		E = SDL_SCANCODE_E, // E
		F = SDL_SCANCODE_F, // F
		G = SDL_SCANCODE_G, // G
		H = SDL_SCANCODE_H, // H
		I = SDL_SCANCODE_I, // I
		J = SDL_SCANCODE_J, // J
		K = SDL_SCANCODE_K, // K
		L = SDL_SCANCODE_L, // L
		M = SDL_SCANCODE_M, // M
		N = SDL_SCANCODE_N, // N
		O = SDL_SCANCODE_O, // O
		P = SDL_SCANCODE_P, // P
		Q = SDL_SCANCODE_Q, // Q
		R = SDL_SCANCODE_R, // R
		S = SDL_SCANCODE_S, // S
		T = SDL_SCANCODE_T, // T
		U = SDL_SCANCODE_U, // U
		V = SDL_SCANCODE_V, // V
		W = SDL_SCANCODE_W, // W
		X = SDL_SCANCODE_X, // X
		Y = SDL_SCANCODE_Y, // Y
		Z = SDL_SCANCODE_Z, // Z

		// Numeric Keys (Main Keyboard Area)

		Num1 = SDL_SCANCODE_1, // 1
		Num2 = SDL_SCANCODE_2, // 2
		Num3 = SDL_SCANCODE_3, // 3
		Num4 = SDL_SCANCODE_4, // 4
		Num5 = SDL_SCANCODE_5, // 5
		Num6 = SDL_SCANCODE_6, // 6
		Num7 = SDL_SCANCODE_7, // 7
		Num8 = SDL_SCANCODE_8, // 8
		Num9 = SDL_SCANCODE_9, // 9
		Num0 = SDL_SCANCODE_0, // 0

		// Control Keys

		Return = SDL_SCANCODE_RETURN,       // Enter / Return
		Escape = SDL_SCANCODE_ESCAPE,       // Escape
		Backspace = SDL_SCANCODE_BACKSPACE, // Backspace
		Tab = SDL_SCANCODE_TAB,             // Tab
		Space = SDL_SCANCODE_SPACE,         // Space

		// Symbol Keys

		Minus = SDL_SCANCODE_MINUS,               // -
		Equals = SDL_SCANCODE_EQUALS,             // =
		LeftBracket = SDL_SCANCODE_LEFTBRACKET,   // [
		RightBracket = SDL_SCANCODE_RIGHTBRACKET, // ]
		Backslash = SDL_SCANCODE_BACKSLASH,       // '\'
		Semicolon = SDL_SCANCODE_SEMICOLON,       // ;
		Apostrophe = SDL_SCANCODE_APOSTROPHE,     // '
		Grave = SDL_SCANCODE_GRAVE,               // `
		Comma = SDL_SCANCODE_COMMA,               // ,
		Period = SDL_SCANCODE_PERIOD,             // .
		Slash = SDL_SCANCODE_SLASH,               // /

		// Function Keys

		CapsLock = SDL_SCANCODE_CAPSLOCK, // Caps Lock

		F1 = SDL_SCANCODE_F1,   // F1
		F2 = SDL_SCANCODE_F2,   // F2
		F3 = SDL_SCANCODE_F3,   // F3
		F4 = SDL_SCANCODE_F4,   // F4
		F5 = SDL_SCANCODE_F5,   // F5
		F6 = SDL_SCANCODE_F6,   // F6
		F7 = SDL_SCANCODE_F7,   // F7
		F8 = SDL_SCANCODE_F8,   // F8
		F9 = SDL_SCANCODE_F9,   // F9
		F10 = SDL_SCANCODE_F10, // F10
		F11 = SDL_SCANCODE_F11, // F11
		F12 = SDL_SCANCODE_F12, // F12
		F13 = SDL_SCANCODE_F13, // F13
		F14 = SDL_SCANCODE_F14, // F14
		F15 = SDL_SCANCODE_F15, // F15
		F16 = SDL_SCANCODE_F16, // F16
		F17 = SDL_SCANCODE_F17, // F17
		F18 = SDL_SCANCODE_F18, // F18
		F19 = SDL_SCANCODE_F19, // F19
		F20 = SDL_SCANCODE_F20, // F20
		F21 = SDL_SCANCODE_F21, // F21
		F22 = SDL_SCANCODE_F22, // F22
		F23 = SDL_SCANCODE_F23, // F23
		F24 = SDL_SCANCODE_F24, // F24

		PrintScreen = SDL_SCANCODE_PRINTSCREEN, // Print Screen
		ScrollLock = SDL_SCANCODE_SCROLLLOCK,   // Scroll Lock
		Pause = SDL_SCANCODE_PAUSE,             // Pause / Break

		Insert = SDL_SCANCODE_INSERT,     // Insert
		Home = SDL_SCANCODE_HOME,         // Home
		PageUp = SDL_SCANCODE_PAGEUP,     // Page Up
		Delete = SDL_SCANCODE_DELETE,     // Delete
		End = SDL_SCANCODE_END,           // End
		PageDown = SDL_SCANCODE_PAGEDOWN, // Page Down

		Right = SDL_SCANCODE_RIGHT,
		Left = SDL_SCANCODE_LEFT,
		Down = SDL_SCANCODE_DOWN,
		Up = SDL_SCANCODE_UP,

		// Key Pad Area

		NumLock = SDL_SCANCODE_NUMLOCKCLEAR,       // Num Lock
		KeypadDivide = SDL_SCANCODE_KP_DIVIDE,     // KP /
		KeypadMultiply = SDL_SCANCODE_KP_MULTIPLY, // KP *
		KeypadMinus = SDL_SCANCODE_KP_MINUS,       // KP -
		KeypadPlus = SDL_SCANCODE_KP_PLUS,         // KP +
		KeypadEnter = SDL_SCANCODE_KP_ENTER,       // KP Enter

		Keypad1 = SDL_SCANCODE_KP_1,           // KP 1
		Keypad2 = SDL_SCANCODE_KP_2,           // KP 2
		Keypad3 = SDL_SCANCODE_KP_3,           // KP 3
		Keypad4 = SDL_SCANCODE_KP_4,           // KP 4
		Keypad5 = SDL_SCANCODE_KP_5,           // KP 5
		Keypad6 = SDL_SCANCODE_KP_6,           // KP 6
		Keypad7 = SDL_SCANCODE_KP_7,           // KP 7
		Keypad8 = SDL_SCANCODE_KP_8,           // KP 8
		Keypad9 = SDL_SCANCODE_KP_9,           // KP 9
		Keypad0 = SDL_SCANCODE_KP_0,           // KP 0
		KeypadPeriod = SDL_SCANCODE_KP_PERIOD, // KP .

		KeypadEquals = SDL_SCANCODE_KP_EQUALS,                    // KP =
		KeypadComma = SDL_SCANCODE_KP_COMMA,                      // KP ,
		Keypad00 = SDL_SCANCODE_KP_00,                            // KP 00
		Keypad000 = SDL_SCANCODE_KP_000,                          // KP 000
		KeypadLeftParen = SDL_SCANCODE_KP_LEFTPAREN,              // KP (
		KeypadRightParen = SDL_SCANCODE_KP_RIGHTPAREN,            // KP )
		KeypadLeftBrace = SDL_SCANCODE_KP_LEFTBRACE,              // KP {
		KeypadRightBrace = SDL_SCANCODE_KP_RIGHTBRACE,            // KP }
		KeypadTab = SDL_SCANCODE_KP_TAB,                          // KP Tab
		KeypadBackspace = SDL_SCANCODE_KP_BACKSPACE,              // KP Backspace
		KeypadA = SDL_SCANCODE_KP_A,                              // KP A
		KeypadB = SDL_SCANCODE_KP_B,                              // KP B
		KeypadC = SDL_SCANCODE_KP_C,                              // KP C
		KeypadD = SDL_SCANCODE_KP_D,                              // KP D
		KeypadE = SDL_SCANCODE_KP_E,                              // KP E
		KeypadF = SDL_SCANCODE_KP_F,                              // KP F
		KeypadXor = SDL_SCANCODE_KP_XOR,                          // KP XOR
		KeypadPower = SDL_SCANCODE_KP_POWER,                      // KP Power
		KeypadPercent = SDL_SCANCODE_KP_PERCENT,                  // KP %
		KeypadLess = SDL_SCANCODE_KP_LESS,                        // KP <
		KeypadGreater = SDL_SCANCODE_KP_GREATER,                  // KP >
		KeypadAmpersand = SDL_SCANCODE_KP_AMPERSAND,              // KP &
		KeypadDoubleAmp = SDL_SCANCODE_KP_DBLAMPERSAND,           // KP &&
		KeypadVerticalBar = SDL_SCANCODE_KP_VERTICALBAR,          // KP |
		KeypadDoubleVerticalBar = SDL_SCANCODE_KP_DBLVERTICALBAR, // KP ||
		KeypadColon = SDL_SCANCODE_KP_COLON,                      // KP :
		KeypadHash = SDL_SCANCODE_KP_HASH,                        // KP #
		KeypadSpace = SDL_SCANCODE_KP_SPACE,                      // KP Space
		KeypadAt = SDL_SCANCODE_KP_AT,                            // KP @
		KeypadExclam = SDL_SCANCODE_KP_EXCLAM,                    // KP !
		KeypadMemStore = SDL_SCANCODE_KP_MEMSTORE,                // KP Memory Store
		KeypadMemRecall = SDL_SCANCODE_KP_MEMRECALL,              // KP Memory Recall
		KeypadMemClear = SDL_SCANCODE_KP_MEMCLEAR,                // KP Memory Clear
		KeypadMemAdd = SDL_SCANCODE_KP_MEMADD,                    // KP Memory Add
		KeypadMemSubtract = SDL_SCANCODE_KP_MEMSUBTRACT,          // KP Memory Subtract
		KeypadMemMultiply = SDL_SCANCODE_KP_MEMMULTIPLY,          // KP Memory Multiply
		KeypadMemDivide = SDL_SCANCODE_KP_MEMDIVIDE,              // KP Memory Divide
		KeypadPlusMinus = SDL_SCANCODE_KP_PLUSMINUS,              // KP ±
		KeypadClear = SDL_SCANCODE_KP_CLEAR,                      // KP Clear
		KeypadClearEntry = SDL_SCANCODE_KP_CLEARENTRY,            // KP Clear Entry
		KeypadBinary = SDL_SCANCODE_KP_BINARY,                    // KP Binary
		KeypadOctal = SDL_SCANCODE_KP_OCTAL,                      // KP Octal
		KeypadDecimal = SDL_SCANCODE_KP_DECIMAL,                  // KP Decimal
		KeypadHexadecimal = SDL_SCANCODE_KP_HEXADECIMAL,          // KP Hexadecimal

		// Modifier Keys

		LeftControl = SDL_SCANCODE_LCTRL,  // Left Ctrl
		LeftShift = SDL_SCANCODE_LSHIFT,   // Left Shift
		LeftAlt = SDL_SCANCODE_LALT,       // Left Alt
		LeftGui = SDL_SCANCODE_LGUI,       // Left Windows / Command / Meta
		RightControl = SDL_SCANCODE_RCTRL, // Right Ctrl
		RightShift = SDL_SCANCODE_RSHIFT,  // Right Shift
		RightAlt = SDL_SCANCODE_RALT,      // Right Alt
		RightGui = SDL_SCANCODE_RGUI,      // Right Windows / Command / Meta

		// International Keys

		International1 = SDL_SCANCODE_INTERNATIONAL1,
		International2 = SDL_SCANCODE_INTERNATIONAL2,
		International3 = SDL_SCANCODE_INTERNATIONAL3,
		International4 = SDL_SCANCODE_INTERNATIONAL4,
		International5 = SDL_SCANCODE_INTERNATIONAL5,
		International6 = SDL_SCANCODE_INTERNATIONAL6,
		International7 = SDL_SCANCODE_INTERNATIONAL7,
		International8 = SDL_SCANCODE_INTERNATIONAL8,
		International9 = SDL_SCANCODE_INTERNATIONAL9,
		Lang1 = SDL_SCANCODE_LANG1,
		Lang2 = SDL_SCANCODE_LANG2,
		Lang3 = SDL_SCANCODE_LANG3,
		Lang4 = SDL_SCANCODE_LANG4,
		Lang5 = SDL_SCANCODE_LANG5,
		Lang6 = SDL_SCANCODE_LANG6,
		Lang7 = SDL_SCANCODE_LANG7,
		Lang8 = SDL_SCANCODE_LANG8,
		Lang9 = SDL_SCANCODE_LANG9,

		// Media Control Keys

		MediaNext = SDL_SCANCODE_MEDIA_NEXT_TRACK,
		MediaPrev = SDL_SCANCODE_MEDIA_PREVIOUS_TRACK,
		MediaStop = SDL_SCANCODE_MEDIA_STOP,
		MediaPlay = SDL_SCANCODE_MEDIA_PLAY,
		MediaMute = SDL_SCANCODE_MUTE,
		MediaSelect = SDL_SCANCODE_MEDIA_SELECT,
		AcSearch = SDL_SCANCODE_AC_SEARCH,
		AcHome = SDL_SCANCODE_AC_HOME,
		AcBack = SDL_SCANCODE_AC_BACK,
		AcForward = SDL_SCANCODE_AC_FORWARD,
		AcStop = SDL_SCANCODE_AC_STOP,
		AcRefresh = SDL_SCANCODE_AC_REFRESH,
		AcBookmarks = SDL_SCANCODE_AC_BOOKMARKS,
	}; // namespace tudov

	struct ScanCode
	{
		TE_STATIC_CLASS(ScanCode);
	};
} // namespace tudov