#ifndef SMOOTHLIFE_H
#define SMOOTHLIFE_H

#include <windows.h>
#include <stdio.h>

#define WIN_HEIGHT				400
#define WIN_WIDTH				800
#define CELL_SIZE				5
#define BOTTOM_STRIP_HEIGHT		30
#define GRID_WIDTH				(WIN_WIDTH / CELL_SIZE)
#define GRID_HEIGHT				(WIN_HEIGHT / CELL_SIZE)
#define MIN_CELL_DISPLAY_VALUE	0.02f
#define R_A						8.0f
#define R_I						5.0f

const int WIN_CENTER						= (WIN_WIDTH / CELL_SIZE) / 2;
const COLORREF BG_COLOR						= RGB(0, 0, 0);			// Black
const COLORREF BOTTOM_STRIP_COLOR			= RGB(240, 240, 240);	// Very Light Grey
const COLORREF TEXT_COLOR					= RGB(0, 0, 0);			// Black
const COLORREF BUTTON_NORMAL_COLOR			= RGB(220, 220, 220);	// Light Grey
const COLORREF BUTTON_PRESSED_COLOR			= RGB(150, 150, 150);	// Grey
const char* TEXT_FONT						= "Cascadia Mono";
const char* BUTTON_TEXT_FONT				= "Monaco";

void initializeState();
void drawAtLocation(HDC hdc, int x, int y);
void displayBottomStrip(HDC hdc);
void displayText(HDC hdc);
void handleDrawing(HWND hwnd);
void handleTimeTick();
void displayCells(HDC hdc);
void calculateStateChange();
void displayButton(HWND hwnd, HDC hdc);

#endif