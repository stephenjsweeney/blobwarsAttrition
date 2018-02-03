/*
Copyright (C) 2018 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "hud.h"

static int messageTime;
static char message[MAX_DESCRIPTION_LENGTH];
static int messageType;

static char infoMessage[MAX_DESCRIPTION_LENGTH];

void initHud(void)
{
	messageTime = FPS * 2;
	messageType = MSG_STANDARD;
}

void doHud(void)
{
	messageTime--;
	
	if (messageTime <= 0)
	{
		messageType = MSG_STANDARD;
		
		messageTime = 0;
	}
}

void drawHud(void)
{
	drawText(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 25, 14, TA_CENTER, colors.white, "Bob [%.0f, %.0f]", world.bob->x / MAP_TILE_SIZE, world.bob->y / MAP_TILE_SIZE);
}

void setGameplayMessage(int newMessageType, const char *format, ...)
{
	char newMessage[MAX_DESCRIPTION_LENGTH];
	va_list args;

	memset(&newMessage, '\0', sizeof(newMessage));

	va_start(args, format);
	vsprintf(newMessage, format, args);
	va_end(args);
	
	if (newMessageType >= messageType && newMessage != NULL)
	{
		STRNCPY(message, newMessage, MAX_DESCRIPTION_LENGTH);
		messageType = newMessageType;
		messageTime = FPS * 3;
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%s", message);
	}
}

void showInfoMessage(char *newInfoMessage)
{
	STRNCPY(infoMessage, newInfoMessage, MAX_DESCRIPTION_LENGTH);

	showWidgetGroup("ok");
}
