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
static void updateWidgetValue(int dir);
static void handleInputWidget(void);

static Widget widgets[MAX_WIDGETS];
static Widget *selectedWidget;
static int widgetIndex;
static int numWidgets;
static Atlas *left;
static Atlas *right;
static Texture *atlasTexture;
static SDL_Rect frame;

void initWidgets(void)
{
	memset(widgets, 0, sizeof(Widget) * MAX_WIDGETS);

	numWidgets = 0;
	
	selectedWidget = NULL;
	
	loadWidgets();
	
	atlasTexture = getTexture("gfx/atlas/atlas.png");
	left = getImageFromAtlas("gfx/ui/left.png");
	right = getImageFromAtlas("gfx/ui/right.png");
}

void doWidgets(void)
{
	if (!app.awaitingWidgetInput)
	{
		if (isControl(CONTROL_UP) || app.keyboard[SDL_SCANCODE_UP])
		{
			selectWidget(-1);

			app.keyboard[SDL_SCANCODE_UP] = 0;
			clearControl(CONTROL_UP);
		}

		if (isControl(CONTROL_DOWN) || app.keyboard[SDL_SCANCODE_DOWN])
		{
			selectWidget(1);

			app.keyboard[SDL_SCANCODE_DOWN] = 0;
			clearControl(CONTROL_DOWN);
		}

		if (isControl(CONTROL_LEFT) || app.keyboard[SDL_SCANCODE_LEFT])
		{
			updateWidgetValue(-1);
		}

		if (isControl(CONTROL_RIGHT) || app.keyboard[SDL_SCANCODE_RIGHT])
		{
			updateWidgetValue(1);
		}

		if (isControl(CONTROL_FIRE) || app.keyboard[SDL_SCANCODE_RETURN] || app.keyboard[SDL_SCANCODE_SPACE])
		{
			if (selectedWidget->type != WT_INPUT)
			{
				selectedWidget->action();
			}
			else if (!isControl(CONTROL_FIRE))
			{
				app.awaitingWidgetInput = 1;
				app.lastKeyPressed = 0;
				app.lastButtonPressed = -1;
			}
			
			app.keyboard[SDL_SCANCODE_SPACE] = app.keyboard[SDL_SCANCODE_RETURN] = 0;
			clearControl(CONTROL_FIRE);
		}
	}
	else
	{
		handleInputWidget();
	}
}

static void updateWidgetValue(int dir)
{
	if (selectedWidget->type == WT_SLIDER)
	{
		selectedWidget->value[0] = limit(selectedWidget->value[0] + dir, selectedWidget->minValue, selectedWidget->maxValue);
		selectedWidget->action();
	}
	else if (selectedWidget->type == WT_SPINNER)
	{
		selectedWidget->value[0] = limit(selectedWidget->value[0] + dir, 0, selectedWidget->numOptions - 1);
		selectedWidget->action();
		app.keyboard[SDL_SCANCODE_LEFT] = app.keyboard[SDL_SCANCODE_RIGHT] = 0;
		clearControl(CONTROL_LEFT);
		clearControl(CONTROL_RIGHT);
		playSound(SND_MENU_SELECT, 0);
	}
}

static void handleInputWidget(void)
{
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		app.awaitingWidgetInput = 0;
	}
	else if (app.keyboard[SDL_SCANCODE_BACKSPACE])
	{
		selectedWidget->value[0] = 0;
		selectedWidget->value[1] = -1;
		
		app.awaitingWidgetInput = 0;
	}
	else if (app.lastKeyPressed != 0 || app.lastButtonPressed != -1)
	{
		if (app.lastKeyPressed != 0)
		{
			selectedWidget->value[0] = app.lastKeyPressed;
		}
		
		if (app.lastButtonPressed != -1)
		{
			selectedWidget->value[1] = app.lastButtonPressed;
		}
		
		app.awaitingWidgetInput = 0;
	}
}

void drawWidgets(void)
{
	int i, j, x, tw, th, outline;
	Widget *w;

	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
		if (w->visible)
		{
			if (w != selectedWidget)
			{
				drawRect(w->x, w->y, w->w, w->h, 0, 0, 0, 255);
				drawOutlineRect(w->x, w->y, w->w, w->h, 0, 128, 0, 255);
			}
			else
			{
				drawRect(w->x, w->y, w->w, w->h, 0, 128, 0, 255);
				drawOutlineRect(w->x, w->y, w->w, w->h, 0, 255, 0, 255);
			}
			
			drawText(w->x + w->w / 2, w->y + 2, 24, TA_CENTER, colors.white, w->label);
			
			outline = (w == selectedWidget) ? 255 : 192;
			
			switch (w->type)
			{
				case WT_SLIDER:
					drawRect(w->x + w->w + 25, w->y, 500 * (w->value[0] * 1.0 / w->maxValue), 40, 0, 128, 0, 255);
					drawOutlineRect(w->x + w->w + 25, w->y, 500, 40, 0, outline, 0, 255);
					break;

				case WT_SPINNER:
					x = w->x + w->w + 25;
					for (j = 0 ; j < w->numOptions ; j++)
					{
						textSize(w->options[j], 24, &tw, &th);
						
						tw += 25;
						
						if (j == w->value[0])
						{
							drawRect(x, w->y, tw, w->h, 0, 128, 0, 255);
							drawOutlineRect(x, w->y, tw, w->h, 0, outline, 0, 255);
						}
						
						drawText(x + tw / 2, w->y + 2, 24, TA_CENTER, colors.white, w->options[j]);
						
						x += tw + 25;
					}
					break;

				case WT_INPUT:
					x = w->x + w->w + 25;
					drawRect(x, w->y, 200, w->h, 0, 0, 0, 255);
					drawOutlineRect(x, w->y, 200, w->h, 0, outline, 0, 255);
					
					if (app.awaitingWidgetInput && w == selectedWidget)
					{
						drawText(x + 100, w->y + 2, 24, TA_CENTER, colors.white, "...");
					}
					else if (w->value[0] != -1 && w->value[1] != -1)
					{
						drawText(x + 100, w->y + 2, 24, TA_CENTER, colors.white, "%s or Btn %d", SDL_GetScancodeName(w->value[0]), w->value[1]);
					}
					else if (w->value[0] != -1)
					{
						drawText(x + 100, w->y + 2, 24, TA_CENTER, colors.white, "%s", SDL_GetScancodeName(w->value[0]));
					}
					else if (w->value[1] != -1)
					{
						drawText(x + 100, w->y + 2, 24, TA_CENTER, colors.white, "Btn %d", w->value[1]);
					}
					break;
			}
			
			if (w->disabled)
			{
				drawRect(w->x, w->y, w->w, w->h, 0, 0, 0, 160);
			}
		}
	}
}

void drawWidgetFrame(void)
{
	drawRect(frame.x, frame.y, frame.w, frame.h, 0, 0, 0, 192);
	
	drawOutlineRect(frame.x, frame.y, frame.w, frame.h, 255, 255, 255, 255);
}

static void selectWidget(int dir)
{
	int oldWidgetIndex, valid;
	
	oldWidgetIndex = widgetIndex;
	valid = 0;
	
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
		
		valid = selectedWidget->visible && !selectedWidget->disabled;

	} while (!valid);
	
	if (oldWidgetIndex != widgetIndex)
	{
		playSound(SND_MENU_NAV, 0);
	}
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

void setSelectedWidget(char *name, char *group)
{
	Widget *w;
	int i;
	
	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
		if (strcmp(w->name, name) == 0 && strcmp(w->group, group) == 0)
		{
			widgetIndex = i;
			selectedWidget = w;
			return;
		}
	}
}

Widget *getSelectedWidget(void)
{
	return selectedWidget;
}

Widget *selectWidgetAt(int x, int y)
{
	Widget *w;
	int i;

	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
		if (w->visible && collision(w->x, w->y, w->w, w->h, x, y, 1, 1))
		{
			if (w != selectedWidget)
			{
				playSound(SND_MENU_NAV, 0);
			}
			
			widgetIndex = i;
			selectedWidget = w;
			return w;
		}
	}
	
	selectedWidget = NULL;
	
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
	
	frame.x = frame.y = frame.w = frame.h = 0;
}

void showWidgetGroup(char *group)
{
	int i;
	Widget *w;
	
	hideAllWidgets();
	
	frame.x = frame.y = SCREEN_WIDTH;
	frame.w = frame.h = 0;
	
	for (i = 0 ; i < numWidgets ; i++)
	{
		w = &widgets[i];
		
		if (strcmp(w->group, group) == 0)
		{
			if (selectedWidget == NULL)
			{
				selectedWidget = w;
				widgetIndex = i;
			}

			w->visible = 1;
			
			frame.x = MIN(w->x - 25, frame.x);
			frame.y = MIN(w->y - 25, frame.y);
			frame.w = MAX(w->w + 50, frame.w);
			frame.h = MAX(w->y + w->h + 25, frame.h);

		}
	}
	
	
	frame.h -= frame.y;
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
		if (numWidgets >= MAX_WIDGETS)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Out of widget space.");
			exit(1);
		}

		w = &widgets[numWidgets];
		
		STRNCPY(w->name, cJSON_GetObjectItem(node, "name")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->group, cJSON_GetObjectItem(node, "group")->valuestring, MAX_NAME_LENGTH);
		STRNCPY(w->label, _(cJSON_GetObjectItem(node, "label")->valuestring), MAX_NAME_LENGTH);
		w->x = cJSON_GetObjectItem(node, "x")->valueint;
		w->y = cJSON_GetObjectItem(node, "y")->valueint;
		w->w = cJSON_GetObjectItem(node, "w")->valueint;
		w->h = cJSON_GetObjectItem(node, "h")->valueint;
		w->type = lookup(cJSON_GetObjectItem(node, "type")->valuestring);
		
		if (w->x == -1)
		{
			w->x = (SCREEN_WIDTH - w->w) / 2;
		}
		
		switch (w->type)
		{
			case WT_SPINNER:
				createWidgetOptions(w, cJSON_GetObjectItem(node, "options")->valuestring);
				break;
				
			case WT_SLIDER:
				w->minValue = cJSON_GetObjectItem(node, "minValue")->valueint;
				w->maxValue = cJSON_GetObjectItem(node, "maxValue")->valueint;
				break;
				
			case WT_INPUT:
				break;
			
			default:
				break;
		}
		
		numWidgets++;
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
	option = _(strtok(options, "|"));
	while (option)
	{
		w->options[i] = malloc(strlen(option) + 1);
		strcpy(w->options[i], option);
		
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG, "widget.option[%d] = %s", i, option);

		option = _(strtok(NULL, "|"));

		i++;
	}
}

void destroy(void)
{
}
