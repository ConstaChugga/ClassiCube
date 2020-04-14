#ifndef CC_WINDOW_H
#define CC_WINDOW_H
#include "String.h"
#include "Bitmap.h"
/* Abstracts creating and managing the native window.
   Copyright 2014-2019 ClassiCube | Licensed under BSD-3 | Based on OpenTK code
*/

/*
   The Open Toolkit Library License
  
   Copyright (c) 2006 - 2009 the Open Toolkit library.
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights to
   use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
   the Software, and to permit persons to whom the Software is furnished to do
   so, subject to the following conditions:
  
   The above copyright notice and this permission notice shall be included in all
   copies or substantial portions of the Software.
  
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
   OTHER DEALINGS IN THE SOFTWARE.
*/

/* The states the window can be in. */
enum WindowState { WINDOW_STATE_NORMAL, WINDOW_STATE_MINIMISED, WINDOW_STATE_FULLSCREEN };

/* Number of bits per pixel. (red bits + green bits + blue bits + alpha bits) */
/* NOTE: Only 24 or 32 bits per pixel are officially supported. */
/* Support for other values of bits per pixel is platform dependent. */
extern int Display_BitsPerPixel;
/* Scale based on number of physical dots per inch of the display. (horizontally and vertically) */
/* NOTE: Usually 1 for compatibility, even if the display's DPI is really higher. */
/* GUI elements must be scaled by this to look correct. */
extern float Display_DpiX, Display_DpiY;
/* Position and size of this display. */
/* NOTE: Position may be non-zero in a multi-monitor setup. Platform dependent. */
extern Rect2D Display_Bounds;

/* Scales the given X coordinate from 96 dpi to current display dpi. */
int Display_ScaleX(int x);
/* Scales the given Y coordinate from 96 dpi to current display dpi. */
int Display_ScaleY(int y);

/* Size of the content area of the window. (i.e. area that can draw to) */
/* This area does NOT include borders and titlebar surrounding the window. */
extern int Window_Width, Window_Height;
/* Whether the window is actually valid (i.e. not destroyed). */
extern cc_bool Window_Exists;
/* Whether the user is interacting with the window. */
extern cc_bool Window_Focused;
/* Readonly platform-specific handle to the window. */
extern const void* Window_Handle;
/* Whether the platform only supports on-screen keyboard. */
extern cc_bool Window_SoftKeyboard;

/* Initalises state for window. Also sets Display_ members. */
void Window_Init(void);
/* Creates the window as the given size at centre of the screen. */
void Window_Create(int width, int height);
/* Sets the text of the titlebar above the window. */
CC_API void Window_SetTitle(const String* title);
/* TODO: IMPLEMENT void Window_SetIcon(Bitmap* bmp); */

typedef void (*RequestClipboardCallback)(String* value, void* obj);
/* Gets the text currently on the clipboard. */
CC_API void Clipboard_GetText(String* value);
/* Sets the text currently on the clipboard. */
CC_API void Clipboard_SetText(const String* value);
/* Calls a callback function when text is retrieved from the clipboard. */
/* NOTE: On most platforms this just calls Clipboard_GetText. */
/* With emscripten however, the callback is instead called when a 'paste' event arrives. */
void Clipboard_RequestText(RequestClipboardCallback callback, void* obj);

/* Makes the window visible and focussed on screen. */
void Window_Show(void);
/* Gets the current state of the window, see WindowState enum. */
int Window_GetWindowState(void);
/* Attempts to switch the window to occupy the entire screen. */
cc_result Window_EnterFullscreen(void);
/* Attempts to restore the window to before it entered full screen. */
cc_result Window_ExitFullscreen(void);

/* Sets the size of the internal bounds of the window. */
/* NOTE: This size excludes the bounds of borders + title */
void Window_SetSize(int width, int height);
/* Closes then destroys the window. */
/* Raises the WindowClosing and WindowClosed events. */
void Window_Close(void);
/* Processes all pending window messages/events. */
void Window_ProcessEvents(void);

/* Sets the position of the cursor. */
/* NOTE: This should be avoided because it is unsupported on some platforms. */
void Cursor_SetPosition(int x, int y);
/* Sets whether the cursor is visible when over this window. */
/* NOTE: You MUST BE VERY CAREFUL with this! OS typically uses a counter for visibility,
so setting invisible multiple times means you must then set visible multiple times. */
void Cursor_SetVisible(cc_bool visible);

/* Shows a dialog box window. */
CC_API void Window_ShowDialog(const char* title, const char* msg);
/* Allocates a framebuffer that can be drawn/transferred to the window. */
/* NOTE: Do NOT free bmp->Scan0, use Window_FreeFramebuffer. */
/* NOTE: This MUST be called whenever the window is resized. */
void Window_AllocFramebuffer(Bitmap* bmp);
/* Transfers pixels from the allocated framebuffer to the on-screen window. */
/* r can be used to only update a small region of pixels (may be ignored) */
void Window_DrawFramebuffer(Rect2D r);
/* Frees the previously allocated framebuffer. */
void Window_FreeFramebuffer(Bitmap* bmp);

/* Displays on-screen keyboard for platforms that lack physical keyboard input. */
/* NOTE: On desktop platforms, this won't do anything. */
void Window_OpenKeyboard(void);
/* Sets the text used for keyboard input. */
/* NOTE: This is only used for mobile on-screen keyboard input with the web client, */
/* because it is backed by a HTML input, rather than true keyboard input events. */
/* As such, this is necessary to ensure the HTML input is consistent with */
/* whatever text input widget is actually being displayed on screen. */
void Window_SetKeyboardText(const String* text);
/* Hides/Removes the previously displayed on-screen keyboard. */
void Window_CloseKeyboard(void);

/* Begins listening for raw input and starts raising PointerEvents.RawMoved. */
/* NOTE: Some backends only raise it when Window_UpdateRawMouse is called. */
/* Cursor will also be hidden and moved to window centre. */
void Window_EnableRawMouse(void);
/* Updates mouse state. (such as centreing cursor) */
void Window_UpdateRawMouse(void);
/* Disables listening for raw input and stops raising PointerEvents.RawMoved */
/* Cursor will also be unhidden and moved back to window centre. */
void Window_DisableRawMouse(void);

#ifdef CC_BUILD_GL
/* Initialises an OpenGL context. */
/* NOTE: You MUST have created the window beforehand, as the GL context is attached to the window. */
void GLContext_Init(void);
/* Updates the OpenGL context after the window is resized. */
void GLContext_Update(void);
/* Attempts to restore a lost OpenGL context. */
cc_bool GLContext_TryRestore(void);
/* Destroys the OpenGL context. */
/* NOTE: This also unattaches the OpenGL context from the window. */
void GLContext_Free(void);

#define GLCONTEXT_DEFAULT_DEPTH 24
#define GLContext_IsInvalidAddress(ptr) (ptr == (void*)0 || ptr == (void*)1 || ptr == (void*)-1 || ptr == (void*)2)
/* Returns the address of a function pointer for the given OpenGL function. */
/* NOTE: The implementation may still return an address for unsupported functions. */
/* You MUST check the OpenGL version and/or GL_EXTENSIONS string for actual support! */
void* GLContext_GetAddress(const char* function);
/* Swaps the front and back buffer, displaying the back buffer on screen. */
cc_bool GLContext_SwapBuffers(void);
/* Sets whether synchronisation with the monitor is enabled. */
/* NOTE: The implementation may choose to still ignore this. */
void GLContext_SetFpsLimit(cc_bool vsync, float minFrameMs);
#endif
#endif
