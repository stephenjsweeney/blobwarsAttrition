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

#include "unixInit.h"
 
void createSaveFolder(void)
{
	char *userHome;
	char dir[MAX_FILENAME_LENGTH];
	int i;
	
	userHome = getenv("HOME");
	
	if (!userHome)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Unable to determine user save folder.");
		exit(1);
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "User home = %s", userHome);
	
	for (i = 0 ; i < MAX_SAVE_SLOTS ; i++)
	{
		sprintf(dir, "%s/.local/share/blobwarsAttrition/%d", userHome, i);
		if (mkdir(dir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH) != 0 && errno != EEXIST)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Failed to create save dir '%s'.", dir);
			exit(1);
		}
	}
	
	sprintf(app.saveDir, "%s/.local/share/blobwarsAttrition", userHome);
}

void createScreenshotFolder(void)
{
	mkdir("/tmp/blobwarsAttrition", S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
	
	dev.screenshotFolder = "/tmp/blobwarsAttrition";
}
