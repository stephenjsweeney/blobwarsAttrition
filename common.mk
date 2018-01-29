VERSION = 0.1
REVISION = $(shell git rev-list HEAD 2>/dev/null | wc -l)
LOCALE_MO = $(patsubst %.po,%.mo,$(wildcard locale/*.po))

SEARCHPATH += src 
SEARCHPATH += src/combat 
SEARCHPATH += src/entities 
SEARCHPATH += src/entities/blobs
SEARCHPATH += src/entities/boss
SEARCHPATH += src/entities/cannons
SEARCHPATH += src/entities/decoration
SEARCHPATH += src/entities/items
SEARCHPATH += src/entities/misc
SEARCHPATH += src/entities/structures
SEARCHPATH += src/entities/traps
SEARCHPATH += src/game
SEARCHPATH += src/hub
SEARCHPATH += src/json
SEARCHPATH += src/system
SEARCHPATH += src/test
SEARCHPATH += src/util 
SEARCHPATH += src/widgets 
SEARCHPATH += src/world

vpath %.c $(SEARCHPATH)
vpath %.h $(SEARCHPATH)

DEPS += defs.h structs.h

OBJS += atlas.o atlasTest.o
OBJS += battery.o blaze.o bob.o boss.o blobBoss.o
OBJS += camera.o cannon.o cardReader.o cell.o cherry.o combat.o consumable.o
OBJS += debris.o destructable.o door.o draw.o
OBJS += effects.o entities.o exit.o explosions.o eyeDroidCommander.o
OBJS += fleshChunk.o frost.o
OBJS += game.o
OBJS += heart.o horizontalDoor.o horizontalLaserTrap.o hub.o hud.o
OBJS += i18n.o init.o infoPoint.o input.o io.o item.o items.o itemPad.o
OBJS += cJSON.o
OBJS += key.o keycard.o
OBJS += laserTrap.o lift.o lookup.o
OBJS += main.o map.o maths.o mia.o
OBJS += objectives.o
OBJS += particles.o player.o powerPoint.o powerPool.o pressurePlate.o pushBlock.o
OBJS += quadtree.o
OBJS += sound.o sprites.o
OBJS += tankCommander.o tankTrack.o teeka.o teleporter.o text.o textures.o title.o triggers.o
OBJS += unit.o util.o
OBJS += weapons.o weaponPickup.o widgets.o world.o worldLoader.o

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
