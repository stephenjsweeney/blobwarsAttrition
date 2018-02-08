VERSION = 0.2
REVISION = $(shell git rev-list HEAD 2>/dev/null | wc -l)
LOCALE_MO = $(patsubst %.po,%.mo,$(wildcard locale/*.po))

SEARCHPATH += src 
SEARCHPATH += src/combat 
SEARCHPATH += src/entities 
SEARCHPATH += src/entities/blobs
SEARCHPATH += src/entities/boss
SEARCHPATH += src/entities/bullets
SEARCHPATH += src/entities/cannons
SEARCHPATH += src/entities/decoration
SEARCHPATH += src/entities/evilBlobs
SEARCHPATH += src/entities/eyeDroids
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

OBJS += atlas.o atlasTest.o aquaBlob.o
OBJS += battery.o blaze.o bob.o boss.o blobBoss.o bullet.o
OBJS += camera.o cannon.o cardReader.o cell.o cherry.o combat.o controls.o consumable.o
OBJS += debris.o destructable.o door.o draw.o
OBJS += effects.o ending.o entities.o entity.o entityFactory.o exit.o explosions.o eyeDroid.o eyeDroidCommander.o evilBlob.o
OBJS += fleshChunk.o frost.o
OBJS += game.o grenade.o grenadeBlob.o grenadeDroid.o
OBJS += heart.o horizontalDoor.o horizontalLaserTrap.o hub.o hud.o
OBJS += i18n.o init.o infoPoint.o input.o io.o item.o items.o itemPad.o
OBJS += cJSON.o
OBJS += key.o keycard.o
OBJS += laser.o laserTrap.o lift.o lookup.o
OBJS += machineGunBlob.o main.o map.o maths.o mia.o missile.o
OBJS += objectives.o
OBJS += particles.o player.o pistolBlob.o pistolDroid.o powerPoint.o powerPool.o pressurePlate.o pushBlock.o
OBJS += quadtree.o
OBJS += sound.o sprites.o structures.o
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
