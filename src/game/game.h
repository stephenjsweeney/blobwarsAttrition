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
#include "../json/cJSON.h"

extern Entity *createEntity(char *name);
extern int fileExists(const char *filename);
extern char *getLookupName(const char *prefix, long num);
extern Trophy *getTrophy(char *id);
extern int lookup(char *name);
extern char *readFile(const char *filename);
extern int writeFile(const char *filename, const char *data);

extern App app;
extern Entity *self;
extern Game game;
extern World world;

