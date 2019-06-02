/*
Copyright (C) 2018-2019 Parallel Realities

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
#include "../json/cJSON.h"

extern char *buildFormattedString(const char *format, ...);
extern Entity *createEntity(char *name);
extern int deleteFile(char *path);
extern int fileExists(const char *filename);
extern char **getFileList(const char *dir, int *count);
extern char *getLookupName(const char *prefix, long num);
extern int getPercent(float current, float total);
extern Trophy *getTrophy(char *id);
extern void loadTrophyData(void);
extern int lookup(char *name);
extern char *readFile(const char *filename);
extern char *timeToString(int seconds, int showHours);
extern int writeFile(const char *filename, const char *data);

extern App app;
extern Entity *self;
extern Game game;
extern World world;
