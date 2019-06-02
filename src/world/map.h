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

extern void blitRect(SDL_Texture *texture, int x, int y, SDL_Rect *srcRect, int center);
extern Atlas *getImageFromAtlas(char *filename);
extern Texture *getTexture(const char *filename);
extern float limit(float i, float a, float b);
extern char *readCompressedFile(const char *filename);
extern int rrnd(int low, int high);

extern App app;
extern Camera camera;
extern Game game;
extern World world;
