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

#include "sound.h"

static void loadSounds(void);

static Mix_Chunk *sounds[SND_MAX];
static Mix_Music *music;

void initSounds(void)
{
	memset(sounds, 0, sizeof(Mix_Chunk*) * SND_MAX);

	music = NULL;

	loadSounds();
}

void playMusic(char *filename, int loop)
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(getFileLocation(filename));

    Mix_PlayMusic(music, (loop) ? -1 : 0);
}

void stopMusic(void)
{
	Mix_HaltMusic();
}

void musicSetPlaying(int playing)
{
	if (music != NULL)
	{
		if (playing)
		{
			Mix_ResumeMusic();
		}
		else
		{
			Mix_PauseMusic();
		}
	}
}

void playSound(int snd, int ch)
{
	Mix_PlayChannel(ch, sounds[snd], 0);
}

int isPlayingMusic(void)
{
	return 0;
}

static Mix_Chunk *loadSound(char *filename)
{
	return Mix_LoadWAV(getFileLocation(filename));
}

static void loadSounds(void)
{
	sounds[SND_SELECT] = loadSound("");
	sounds[SND_BACK] = loadSound("");
	sounds[SND_PISTOL] = loadSound("");
	sounds[SND_MACHINE_GUN] = loadSound("");
	sounds[SND_PLASMA] = loadSound("");
	sounds[SND_THROW] = loadSound("");
	sounds[SND_SPREAD] = loadSound("");
	sounds[SND_SHOTGUN] = loadSound("");
	sounds[SND_LASER] = loadSound("");
	sounds[SND_MISSILE] = loadSound("");
	sounds[SND_RICO_1] = loadSound("");
	sounds[SND_RICO_2] = loadSound("");
	sounds[SND_GRENADE_BOUNCE] = loadSound("");
	sounds[SND_EXPLOSION] = loadSound("");
	sounds[SND_FLESH_HIT] = loadSound("");
	sounds[SND_METAL_HIT] = loadSound("");
	sounds[SND_DEATH_1] = loadSound("");
	sounds[SND_DEATH_2] = loadSound("");
	sounds[SND_DEATH_3] = loadSound("");
	sounds[SND_SPLAT] = loadSound("");
	sounds[SND_DROID_DIE_1] = loadSound("");
	sounds[SND_DROID_DIE_2] = loadSound("");
	sounds[SND_PRESSURE_PLATE] = loadSound("");
	sounds[SND_DOOR_START] = loadSound("");
	sounds[SND_DOOR_FINISH] = loadSound("");
	sounds[SND_CHERRY] = loadSound("");
	sounds[SND_ITEM] = loadSound("");
	sounds[SND_KEY] = loadSound("");
	sounds[SND_WEAPON] = loadSound("");
	sounds[SND_TELEPORT] = loadSound("");
	sounds[SND_APPEAR] = loadSound("");
	sounds[SND_MIA] = loadSound("");
	sounds[SND_WATER_IN] = loadSound("");
	sounds[SND_WATER_OUT] = loadSound("");
	sounds[SND_SLIME] = loadSound("");
	sounds[SND_DROWN] = loadSound("");
	sounds[SND_DENIED] = loadSound("");
	sounds[SND_CONFIRMED] = loadSound("");
	sounds[SND_MISSION_COMPLETE] = loadSound("");
	sounds[SND_HEART_CELL] = loadSound("");
}

void destroySounds(void)
{
	int i;

	for (i = 0 ; i < SND_MAX ; i++)
	{
		if (sounds[i])
		{
			Mix_FreeChunk(sounds[i]);
		}
	}

	if (music != NULL)
	{
		Mix_FreeMusic(music);
	}
}
