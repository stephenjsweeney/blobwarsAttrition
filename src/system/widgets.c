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

static void loadWidgetGroup(char *filename);
static void loadWidgets(void);
static void createWidgetOptions(Widget *w, char *options);
static void selectWidget(int dir);

static Widget widgets[MAX_WIDGETS];
static Widget *selectedWidget;
static int widgetIndex;
static int numWidgets;

void initWidgets(void)
{
	memset(widgets, 0, sizeof(Widget) * MAX_WIDGETS);

	numWidgets = 0;
	
	loadWidgets();
}

void doWidgets(void)
{
	if (app.keyboard[SDL_SCANCODE_UP])
	{
		selectWidget(-1);

		app.keyboard[SDL_SCANCODE_UP] = 0;
	}

	if (app.keyboard[SDL_SCANCODE_DOWN])
	{
		selectWidget(1);

		app.keyboard[SDL_SCANCODE_DOWN] = 0;
	}

	if (app.keyboard[SDL_SCANCODE_LEFT] && selectedWidget->type == WT_SPINNER)
	{
	}

	if (app.keyboard[SDL_SCANCODE_RIGHT] && selectedWidget->type == WT_SPINNER)
	{
	}
}

void drawWidgets(void)
{
	int i;
	Widget *w;

	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
		if (w->visible)
		{
			switch (w->type)
			{
				case WT_BUTTON:
					break;

				case WT_PLAIN_BUTTON:
					break;

				case WT_SPINNER:
					break;

				case WT_INPUT:
					break;
			}
		}
	}
}

static void selectWidget(int dir)
{
	do
	{
		widgetIndex += dir;

		if (widgetIndex < 0)
		{
			widgetIndex = numWidgets - 1;
		}

		if (widgetIndex >= numWidgets)
		{
			widgetIndex = 0;
		}

		selectedWidget = &widgets[widgetIndex];

	} while (!selectedWidget->enabled && !selectedWidget->visible);
}

Widget *getWidget(char *name, char *group)
{
	int i;
	Widget *w;
	
	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
		if (strcmp(w->name, name) == 0 && strcmp(w->group, group) == 0)
		{
			return w;
		}
	}
	
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "No such widget '%s', '%s'", name, group);
	exit(1);

	return NULL;
}

void hideAllWidgets(void)
{
	int i;
	
	for (i = 0 ; i < numWidgets ; i++)
	{
		widgets[i].visible = 0;
	}

	selectedWidget = NULL;
}

void showWidgetGroup(char *group)
{
	int i;
	Widget *w;
	
	hideAllWidgets();
	
	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
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
		if (++numWidgets >= MAX_WIDGETS)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Out of widget space.");
			exit(1);
		}

		w = &widgets[numWidgets];
		
		STRNCPY(w->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->group, cJSON_GetObjectItem(node, "group")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->label, cJSON_GetObjectItem(node, "label")->valuestring, MAX_NAME_LENGTH);
		w->x = cJSON_GetObjectItem(node, "x")->valueint;
		w->y = cJSON_GetObjectItem(node, "y")->valueint;
		w->type = lookup(cJSON_GetObjectItem(node, "type")->valuestring);
		
		switch (w->type)
		{
			case WT_SPINNER:
				createWidgetOptions(w, cJSON_GetObjectItem(node, "options")->valuestring);
				break;
			
			default:
				break;
		}
	}
	
	cJSON_Delete(root);
	
	free(text);
}

static void createWidgetOptions(Widget *w, char *options)
{
	int i;
	char *option;
	
	w->numOptions = 1;

	for (i = 0 ; i < strlen(options) ; i++)
	{
		if (options[i] == '|')
		{
			w->numOptions++;
		}
	}

	w->options = malloc(w->numOptions * sizeof(char*));

	i = 0;
	option = strtok(options, "|");
	while (option)
	{
		w->options[i] = malloc(strlen(option) + 1);
		strcpy(w->options[i], option);
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "widget.option[%d] = %s", i, option);

		option = strtok(NULL, "|");

		i++;
	}
}

void destroy(void)
{
}
