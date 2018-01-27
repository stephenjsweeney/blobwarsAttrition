VERSION = 0.1
REVISION = $(shell git rev-list HEAD 2>/dev/null | wc -l)
LOCALE_MO = $(patsubst %.po,%.mo,$(wildcard locale/*.po))

SEARCHPATH += src 
SEARCHPATH += src/combat 
SEARCHPATH += src/game 
SEARCHPATH += src/hub 
SEARCHPATH += src/system 
SEARCHPATH += src/util 
SEARCHPATH += src/widgets 
SEARCHPATH += src/world
SEARCHPATH += src/world/entities 
SEARCHPATH += src/world/entities/blobs
SEARCHPATH += src/world/entities/boss
SEARCHPATH += src/world/entities/cannons
SEARCHPATH += src/world/entities/decoration
SEARCHPATH += src/world/entities/items
SEARCHPATH += src/world/entities/misc
SEARCHPATH += src/world/entities/structures

vpath %.c $(SEARCHPATH)
vpath %.h $(SEARCHPATH)

DEPS += defs.h structs.h

OBJS += atlas.o
OBJS += battery.o blaze.o bob.o boss.o blobBoss.o
OBJS += camera.o cannon.o cardReader.o cell.o cherry.o combat.o consumable.o
OBJS += debris.o destructable.o door.o draw.o
OBJS += effects.o entities.o exit.o explosions.o eyeDroidCommander.o
OBJS += fleshChunk.o frost.o
OBJS += game.o
OBJS += heart.o horizontalDoor.o hub.o hud.o
OBJS += init.o infoPoint.o input.o io.o item.o items.o
OBJS += key.o keycard.o
OBJS += lookup.o
OBJS += main.o map.o maths.o mia.o
OBJS += objectives.o
OBJS += particles.o player.o
OBJS += quadtree.o
OBJS += sound.o sprites.o
OBJS += tankCommander.o tankTrack.o teeka.o text.o textures.o title.o triggers.o
OBJS += unit.o util.o
OBJS += weapons.o weaponPickup.o widgets.o

# top-level rule to create the program.
all: $(PROG) $(LOCALE_MO)

# compiling other source files.
%.o: %.c %.h $(DEPS)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $<
	
%.mo: %.po
	msgfmt -c -o $@ $<
	
# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS) $(PROG) $(LOCALE_MO)
