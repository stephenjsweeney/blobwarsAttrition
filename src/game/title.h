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

#include "../common.h"

enum
{
	SA_NONE,
	SA_LOAD,
	SA_NEW,
	SA_DELETE
};

extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern char *buildFormattedString(const char *format, ...);
extern void deleteSaveSlot(int slot);
extern void destroyGame(void);
extern void doWidgets(void);
extern void drawText(int x, int y, int size, int align, SDL_Color c, const char *format, ...);
extern void drawWidgets(void);
extern void endSectionTransition(void);
extern int fileExists(const char *filename);
extern long getFileModTime(char *filename);
extern Atlas *getImageFromAtlas(char *filename);
extern char *getSaveWidgetLabel(char *filename);
extern Widget *getSelectedWidget(void);
extern Texture *getTexture(const char *filename);
extern Widget *getWidget(char *name, char *group);
extern void initCredits(int playMusic);
extern void initHub(void);
extern void initOptions(void (*callback)(void));
extern void loadGame(int slot);
extern void loadMusic(char *filename);
extern void newGame(void);
extern void playMusic(int loop);
extern void saveGame(int isTempFile);
extern void setSelectedWidget(char *name, char *group);
extern void showWidgetGroup(char *group);
extern void startSectionTransition(void);
extern void stopMusic(void);

extern App app;
extern Colors colors;
extern Game game;
