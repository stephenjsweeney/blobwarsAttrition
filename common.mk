VERSION = 0.7
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

_OBJS += atlas.o aquaBlob.o
_OBJS += battery.o blaze.o bob.o boss.o blobBoss.o bullet.o
_OBJS += camera.o cannon.o cardReader.o cell.o cherry.o combat.o controls.o consumable.o credits.o
_OBJS += debris.o destructable.o door.o draw.o
_OBJS += effects.o ending.o entities.o entity.o entityFactory.o exit.o explosions.o eyeDroid.o eyeDroidCommander.o evilBlob.o
_OBJS += fleshChunk.o frost.o
_OBJS += game.o grenade.o grenadeBlob.o grenadeDroid.o
_OBJS += heart.o horizontalDoor.o horizontalLaserTrap.o hub.o hud.o
_OBJS += i18n.o init.o infoPoint.o input.o io.o item.o items.o itemPad.o
_OBJS += cJSON.o
_OBJS += key.o keycard.o
_OBJS += laser.o laserBlob.o laserDroid.o laserTrap.o lift.o lookup.o
_OBJS += machineGunBlob.o machineGunDroid.o main.o map.o maths.o mia.o missile.o
_OBJS += objectives.o options.o
_OBJS += particles.o player.o plasmaBlob.o plasmaDroid.o pistolBlob.o pistolDroid.o postMission.o powerPoint.o powerPool.o pressurePlate.o pushBlock.o
_OBJS += quadtree.o
_OBJS += radar.o
_OBJS += savepng.o shotgunBlob.o shotgunDroid.o sound.o spreadGunBlob.o spreadGunDroid.o sprites.o stats.o structures.o
_OBJS += tankCommander.o tankTrack.o teeka.o teleporter.o text.o textures.o title.o transition.o triggers.o trophies.o
_OBJS += unit.o util.o
_OBJS += weapons.o weaponPickup.o widgets.o world.o worldLoader.o worldSaver.o worldTest.o

OBJS = $(patsubst %,$(OUT)/%,$(_OBJS))

# top-level rule to create the program.
all: $(PROG) $(LOCALE_MO)

# compiling other source files.
$(OUT)/%.o: %.c %.h $(DEPS)
	@mkdir -p $(OUT)
	$(CC) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	
%.mo: %.po
	msgfmt -c -o $@ $<
	
# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) $(OBJS) $(PROG) $(LOCALE_MO)
