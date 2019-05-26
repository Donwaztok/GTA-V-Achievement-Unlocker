/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com			
			(C) Alexander Blade 2015
*/

/*
	Delete				activate
	NUM2/8/4/6			navigate thru the menus and lists (numlock must be on)
	NUM5 				select
	NUM0/BACKSPACE/F4 	back
	NUM9/3 				use vehicle boost when active
	NUM+ 				use vehicle rockets when active
*/

#include "script.h"
#include "keyboard.h"

#include <string>
#include <ctime>

#pragma warning(disable : 4244 4305) // double <-> float conversions

void draw_rect(float A_0, float A_1, float A_2, float A_3, int A_4, int A_5, int A_6, int A_7)
{
	GRAPHICS::DRAW_RECT((A_0 + (A_2 * 0.5f)), (A_1 + (A_3 * 0.5f)), A_2, A_3, A_4, A_5, A_6, A_7);
}

void draw_menu_line(std::string caption, float lineWidth, float lineHeight, float lineTop, float lineLeft, float textLeft, bool active, bool title, bool rescaleText = true)
{
	// default values
	int text_col[4] = {255, 255, 255, 255},
		rect_col[4] = {70, 95, 95, 255};
	float text_scale = 0.35;
	int font = 0;

	// correcting values for active line
	if (active)
	{
		text_col[0] = 0;
		text_col[1] = 0;
		text_col[2] = 0;

		rect_col[0] = 218;
		rect_col[1] = 242;
		rect_col[2] = 216;

		if (rescaleText) text_scale = 0.40;
	}

	if (title)
	{
		rect_col[0] = 0;
		rect_col[1] = 0;
		rect_col[2] = 0;

		if (rescaleText) text_scale = 0.50;
		font = 1;
	}

	int screen_w, screen_h;
	GRAPHICS::GET_SCREEN_RESOLUTION(&screen_w, &screen_h);

	textLeft += lineLeft;

	float lineWidthScaled = lineWidth / (float)screen_w; // line width
	float lineTopScaled = lineTop / (float)screen_h; // line top offset
	float textLeftScaled = textLeft / (float)screen_w; // text left offset
	float lineHeightScaled = lineHeight / (float)screen_h; // line height

	float lineLeftScaled = lineLeft / (float)screen_w;

	// this is how it's done in original scripts

	// text upper part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_SET_TEXT_ENTRY("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	UI::_DRAW_TEXT(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

	// text lower part
	UI::SET_TEXT_FONT(font);
	UI::SET_TEXT_SCALE(0.0, text_scale);
	UI::SET_TEXT_COLOUR(text_col[0], text_col[1], text_col[2], text_col[3]);
	UI::SET_TEXT_CENTRE(0);
	UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
	UI::SET_TEXT_EDGE(0, 0, 0, 0, 0);
	UI::_0x521FB041D93DD0E4("STRING");
	UI::_ADD_TEXT_COMPONENT_STRING((LPSTR)caption.c_str());
	int num25 = UI::_0x9040DFB09BE75706(textLeftScaled, (((lineTopScaled + 0.00278f) + lineHeightScaled) - 0.005f));

	// rect
	draw_rect(lineLeftScaled, lineTopScaled + (0.00278f), 
		lineWidthScaled, ((((float)(num25)* UI::_0xDB88A37483346780(text_scale, 0)) + (lineHeightScaled * 2.0f)) + 0.005f),
		rect_col[0], rect_col[1], rect_col[2], rect_col[3]);	
}

bool trainer_switch_pressed()
{
	return IsKeyJustUp(VK_DELETE);
}

void get_button_state(bool *a, bool *b, bool *up, bool *down, bool *l, bool *r)
{
	if (a) *a = IsKeyDown(VK_NUMPAD5);
	if (b) *b = IsKeyDown(VK_NUMPAD0) || trainer_switch_pressed() || IsKeyDown(VK_BACK);
	if (up) *up = IsKeyDown(VK_NUMPAD8);
	if (down) *down = IsKeyDown(VK_NUMPAD2);
	if (r) *r = IsKeyDown(VK_NUMPAD6);
	if (l) *l = IsKeyDown(VK_NUMPAD4);
}

void menu_beep()
{
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
}

std::string statusText;
DWORD statusTextDrawTicksMax;
bool statusTextGxtEntry;

void update_status_text()
{
	if (GetTickCount() < statusTextDrawTicksMax)
	{
		UI::SET_TEXT_FONT(0);
		UI::SET_TEXT_SCALE(0.55, 0.55);
		UI::SET_TEXT_COLOUR(255, 255, 255, 255);
		UI::SET_TEXT_WRAP(0.0, 1.0);
		UI::SET_TEXT_CENTRE(1);
		UI::SET_TEXT_DROPSHADOW(0, 0, 0, 0, 0);
		UI::SET_TEXT_EDGE(1, 0, 0, 0, 205);
		if (statusTextGxtEntry)
		{
			UI::_SET_TEXT_ENTRY((char *)statusText.c_str());
		} else
		{
			UI::_SET_TEXT_ENTRY("STRING");
			UI::_ADD_TEXT_COMPONENT_STRING((char *)statusText.c_str());
		}
		UI::_DRAW_TEXT(0.5, 0.5);
	}
}

void set_status_text(std::string str, DWORD time = 2500, bool isGxtEntry = false)
{
	statusText = str;
	statusTextDrawTicksMax = GetTickCount() + time;
	statusTextGxtEntry = isGxtEntry;
}

std::string line_as_str(std::string text, bool *pState)
{
	while (text.size() < 18) text += " ";
	return text + (pState ? (*pState ? " [ON]" : " [OFF]") : "");
}
int activeLineIndexMain = 0;


void process_main_menu()
{
	const float lineWidth = 250.0;
	const int lineCount = 1;	

	std::string caption = "Achievement Unlocker";

	static LPCSTR lineCaption[lineCount] = {
		"Unlock All Achievements"
	};

	DWORD waitTime = 150;
	while (true)
	{
		// timed menu draw, used for pause after active line switch
		DWORD maxTickCount = GetTickCount() + waitTime;
		do 
		{
			// draw menu
			draw_menu_line(caption, lineWidth, 15.0, 18.0, 0.0, 5.0, false, true);
			for (int i = 0; i < lineCount; i++)
				if (i != activeLineIndexMain)
					draw_menu_line(lineCaption[i], lineWidth, 9.0, 60.0 + i * 36.0, 0.0, 9.0, false, false);
			draw_menu_line(lineCaption[activeLineIndexMain], lineWidth + 1.0, 11.0, 56.0 + activeLineIndexMain * 36.0, 0.0, 7.0, true, false);
			
			WAIT(0);
		} while (GetTickCount() < maxTickCount);
		waitTime = 0;

		// process buttons
		bool bSelect, bBack, bUp, bDown;
		get_button_state(&bSelect, &bBack, &bUp, &bDown, NULL, NULL);
		if (bSelect)
		{
			menu_beep();
			switch (activeLineIndexMain)
			{
				case 0:
					//Unlock all Achievements
					for (int i = 0; i < 0x44; i++)
					{
						if (PLAYER::HAS_ACHIEVEMENT_BEEN_PASSED(i) == 0)
						{
							PLAYER::GIVE_ACHIEVEMENT_TO_PLAYER(i);
							set_status_text("All Achievements Unlocked");
							Sleep(250);
						}						
					}
					break;
			}
			waitTime = 200;
		} else
		if (bBack || trainer_switch_pressed())
		{
			menu_beep();
			break;
		} else
		if (bUp)
		{
			menu_beep();
			if (activeLineIndexMain == 0) 
				activeLineIndexMain = lineCount;
			activeLineIndexMain--;
			waitTime = 150;
		} else
		if (bDown)
		{
			menu_beep();
			activeLineIndexMain++;
			if (activeLineIndexMain == lineCount) 
				activeLineIndexMain = 0;			
			waitTime = 150;
		}
	}
}

void reset_globals()
{
	activeLineIndexMain = 0;
}

void main()
{	
	reset_globals();

	while (true)
	{
		if (trainer_switch_pressed())
		{
			menu_beep();
			process_main_menu();
		}

		WAIT(0);
	}
}

void ScriptMain()
{
	srand(GetTickCount());
	main();
}
