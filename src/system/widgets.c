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

#include "widgets.h"

Widget *getWidgetAt(int x, int y);
static void loadWidgetGroup(char *filename);
static void loadWidgets(void);

static Widget widgetHead;
static Widget *widgetTail;
static Widget *selectedWidget;

void initWidgets(void)
{
	memset(&widgetHead, 0, sizeof(Widget));
	widgetTail = &widgetHead;
	
	loadWidgets();
}

Widget *getWidget(char *name, char *group)
{
	Widget *w;
	
	for (w = widgetHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->name, name) == 0 && strcmp(w->group, group) == 0)
		{
			return w;
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such widget '%s', '%s'", name, group);
	exit(1);

	return NULL;
}

void handleWidgetClick(int x, int y)
{
	Widget *w;
	
	w = getWidgetAt(x, y);

	if (w != NULL)
	{
		if (w->type == WT_SPINNER)
		{
			w->value++;
			
			w->value %= w->numOptions;
		}

		w->clicked = 1;
	}
}

void activeSelected(void)
{
	if (selectedWidget->type == WT_SPINNER)
	{
		selectedWidget->value++;
		
		selectedWidget->value %= selectedWidget->numOptions;
	}

	selectedWidget->clicked = 1;
}

Widget *getWidgetAt(int x, int y)
{
	Widget *w;
	
	for (w = widgetHead.next ; w != NULL ; w = w->next)
	{
		if (w->visible && w->enabled && collision(x, y, 1, 1, w->x - (w->w / 2), w->y, w->w, w->h))
		{
			return w;
		}
	}

	return NULL;
}

void hideAllWidgets(void)
{
	Widget *w;
	
	for (w = widgetHead.next ; w != NULL ; w = w->next)
	{
		w->visible = 0;
	}

	selectedWidget = NULL;
}

void showWidgetGroup(char *group)
{
	Widget *w;
	
	hideAllWidgets();
	
	for (w = widgetHead.next ; w != NULL ; w = w->next)
	{
		if (strcmp(w->group, group) == 0)
		{
			if (selectedWidget == NULL)
			{
				selectedWidget = w;
			}

			w->visible = 1;
		}
	}
}

int wasWidgetClicked(Widget *w)
{
	int wasClicked;
	
	wasClicked = w->clicked;

	w->clicked = 0;

	return wasClicked;
}

static void loadWidgets(void)
{
	char **filenames;
	char path[MAX_FILENAME_LENGTH];
	int count, i;

	filenames = getFileList("data/widgets", &count);

	for (i = 0 ; i < count ; i++)
	{
		sprintf(path, "data/widgets/%s", filenames[i]);

		loadWidgetGroup(path);

		free(filenames[i]);
	}

	free(filenames);
}

static void loadWidgetGroup(char *filename)
{
	cJSON *root, *node;
	char *text;
	Widget *w;

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Loading %s", filename);

	text = readFile(filename);
	root = cJSON_Parse(text);
	
	for (node = root->child ; node != NULL ; node = node->next)
	{
		w = malloc(sizeof(Widget));
		memset(w, 0, sizeof(Widget));
		widgetTail->next = w;
		widgetTail = w;
		
		STRNCPY(w->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->group, cJSON_GetObjectItem(node, "group")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->label, cJSON_GetObjectItem(node, "label")->valuestring, MAX_NAME_LENGTH);
		w->x = cJSON_GetObjectItem(node, "x")->valueint;
		w->y = cJSON_GetObjectItem(node, "y")->valueint;
		w->w = cJSON_GetObjectItem(node, "w")->valueint;
		w->h = cJSON_GetObjectItem(node, "h")->valueint;
		w->type = lookup(cJSON_GetObjectItem(node, "type")->valuestring);
		
		switch (w->type)
		{
			case WT_BUTTON:
				break;
			
			case WT_SPINNER:
				break;
			
			case WT_PLAIN_BUTTON:
				break;
		}
	}
	
	cJSON_Delete(root);
	
	free(text);
}

void destroy(void)
{
}
