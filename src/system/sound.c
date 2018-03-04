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

void loadMusic(char *filename)
{
	if (music != NULL)
	{
		Mix_HaltMusic();
		Mix_FreeMusic(music);
		music = NULL;
	}

	music = Mix_LoadMUS(getFileLocation(filename));
}

void playMusic(int loop)
{
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

void pauseSound(int pause)
{
	if (pause)
	{
		Mix_Pause(-1);
	}
	else
	{
		Mix_Resume(-1);
	}
}

void playBattleSound(int snd, int channel, int x, int y)
{
	float distance, bearing, vol;

	distance = getDistance(world.bob->x, world.bob->y, x, y);

	if (distance <= MAX_BATTLE_SOUND_DISTANCE)
	{
		channel = Mix_PlayChannel(channel, sounds[snd], 0);
		
		if (channel != -1)
		{
			vol = 255;
			vol /= MAX_BATTLE_SOUND_DISTANCE;
			vol *= distance;

			if (distance >= MIN_BATTLE_SOUND_DISTANCE)
			{
				bearing = 360 - getAngle(x, y, world.bob->x, world.bob->y);
				
				Mix_SetPosition(channel, (Sint16)bearing, (Uint8)vol);
			}
			else
			{
				Mix_SetDistance(channel, vol);
			}
		}
	}
}

int isPlayingMusic(void)
{
	return Mix_PlayingMusic();
}

static Mix_Chunk *loadSound(char *filename)
{
	return Mix_LoadWAV(getFileLocation(filename));
}

static void loadSounds(void)
{
	sounds[SND_MENU_BACK] = loadSound("sound/50557__broumbroum__sf3-sfx-menu-back.ogg");
	sounds[SND_MENU_SELECT] = loadSound("sound/50561__broumbroum__sf3-sfx-menu-select.ogg");
	sounds[SND_MENU_NAV] = loadSound("sound/146721__fins__menu-click.ogg");
	
	sounds[SND_PISTOL] = loadSound("sound/Gun_44magnum-freesoundeffects.ogg");
	sounds[SND_MACHINE_GUN] = loadSound("sound/67020__ls__submachinegun2.ogg");
	sounds[SND_PLASMA] = loadSound("sound/93017__cosmicd__60.ogg");
	sounds[SND_THROW] = loadSound("sound/60009__qubodup__swosh-22.ogg");
	sounds[SND_GRENADE_BOUNCE] = loadSound("sound/126531__sinkhole7__tink3d.ogg");
	sounds[SND_SPREAD] = loadSound("sound/ray_gun-Mike_Koenig-1169060422.ogg");
	sounds[SND_SHOTGUN] = loadSound("sound/10 Guage Shotgun-SoundBible.com-74120584.ogg");
	sounds[SND_LASER] = loadSound("sound/Laser_Cannon-Mike_Koenig-797224747.ogg");
	sounds[SND_MISSILE] = loadSound("sound/51468__smcameron__missile-launch-2.ogg");
	sounds[SND_RICO_1] = loadSound("sound/148840__cedarstudios__ricochet1.ogg");
	sounds[SND_RICO_2] = loadSound("sound/148840__cedarstudios__ricochet2.ogg");
	sounds[SND_METAL_HIT] = loadSound("sound/170959__timgormly__metal-clang.ogg");
	sounds[SND_FLESH_HIT] = loadSound("sound/104183__ekokubza123__punch.ogg");
	sounds[SND_EXPLOSION] = loadSound("sound/bomb-freesoundeffects.ogg");
	sounds[SND_DEATH_1] = loadSound("sound/219858__adriancalzon__scream-01.ogg");
	sounds[SND_DEATH_2] = loadSound("sound/220289__gabrielaupf__screaming-male-painii.ogg");
	sounds[SND_DEATH_3] = loadSound("sound/180310__jorickhoofd__sad-cry-5.ogg");
	sounds[SND_SPLAT] = loadSound("sound/81042__rock-savage__blood-hitting-window.ogg");
	sounds[SND_DROID_DIE_1] = loadSound("sound/153445__lukechalaudio__8bit-robot-sound.ogg");
	sounds[SND_DROID_DIE_2] = loadSound("sound/153446__lukechalaudio__8bit-robot-sound-2.ogg");
	sounds[SND_DOOR_START] = loadSound("sound/12906__swelk__hydraul1-1.ogg");
	sounds[SND_DOOR_FINISH] = loadSound("sound/12906__swelk__hydraul1-2.ogg");
	sounds[SND_PRESSURE_PLATE] = loadSound("sound/151578__d-w__door-handle-jiggle-04.ogg");
	sounds[SND_CHERRY] = loadSound("sound/151233__owlstorm__gulp-2.ogg");
	sounds[SND_ITEM] = loadSound("sound/88275__s-dij__gbc-reload-06.ogg");
	sounds[SND_WEAPON] = loadSound("sound/Shotgun-SoundBible.com-862990674.ogg");
	sounds[SND_KEY] = loadSound("sound/mortice_key_drop_on_concrete_floor.ogg");
	sounds[SND_TELEPORT] = loadSound("sound/83301__silencer1337__item-respawn.ogg");
	sounds[SND_APPEAR] = loadSound("sound/178350__andromadax24__s-teleport-05.ogg");
	sounds[SND_MIA] = loadSound("sound/116505__owdeo__transporter-beam.ogg");
	sounds[SND_WATER_IN] = loadSound("sound/110393__soundscalpel-com__water-splash.ogg");
	sounds[SND_WATER_OUT] = loadSound("sound/50790__smcameron__medium-splash8.ogg");
	sounds[SND_SLIME] = loadSound("sound/181490__toiletrolltube__120110-15-hot-plate-cold-water-5.ogg");
	sounds[SND_DROWN] = loadSound("sound/56271__q-k__water-gulp.ogg");
	sounds[SND_DENIED] = loadSound("sound/142608__autistic-lucario__error.ogg");
	sounds[SND_CONFIRMED] = loadSound("sound/39028__wildweasel__cardlock-open.ogg");
	sounds[SND_HEART_CELL] = loadSound("sound/162467__kastenfrosch__gotitem.ogg");
	sounds[SND_ELECTRIC_HIT] = loadSound("sound/46501__phreaksaccount__welder1.ogg");
	sounds[SND_ITEM_PAD] = loadSound("sound/319996__kenrt__ratchet.ogg");
	sounds[SND_POWER_POOL] = loadSound("sound/235737__copyc4t__tf-power-tools.ogg");
	
	sounds[SND_MISSION_COMPLETE] = loadSound("sound/113989__kastenfrosch__gewonnen.ogg");
	sounds[SND_TROPHY] = loadSound("sound/278142__ricemaster__effect-notify.ogg");
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
