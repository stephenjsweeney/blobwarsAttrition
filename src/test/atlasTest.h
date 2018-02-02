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

#define CAMERA_SCROLL_SPEED 16

extern void initWorld(void);
extern void initMap(void);
extern void initHub(void);
extern void initGame(void);
extern void initEntities(void);
extern void loadWorld(char *filename);
extern void drawMap(void);
extern void drawEntities(int plane);
extern void cameraTrack(Entity *e);
extern void doEntities(void);
extern void doParticles(void);
extern void doWorld(void);
extern Entity *findEntity(char *name);
extern void clipCamera(void);
extern void blitScaled(SDL_Texture *texture, int x, int y, int w, int h, int center);
extern Texture *getTexture(const char *filename);

extern App app;
extern Camera camera;
extern World world;
