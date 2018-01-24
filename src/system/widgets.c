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

static Widget widgetHead;
static Widget *widgetTail;
static Widget *selectedWidget;

void initWidgets(void)
{
	memset(&widgetHead, 0, sizeof(Widget));
	widgetTail = &widgetHead;
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
			
			w->value %= MAX_WIDGET_OPTIONS;
		}

		w->clicked = 1;
	}
}

void activeSelected(void)
{
	if (selectedWidget->type == WT_SPINNER)
	{
		selectedWidget->value++;
		
		selectedWidget->value %= MAX_WIDGET_OPTIONS;
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

void hideAll(void)
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
	
	hideAll();
	
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

void loadWidgets(char *filename)
{
	
}

void destroy(void)
{
}
