CXXFLAGS = -g -c -O6 -Ofast  -funroll-loops -funroll-all-loops -fomit-frame-pointer -fno-exceptions -fno-rtti -ffast-math -DOSMESA -DGLM_FORCE_RADIANS -mtune=i486 -march=i486 -I../noudar-core/noudar-core/include -Iincludes -I../NullSoundSystem -I../noudar-rendering -I../gles2-renderer/ -IDOS-version -I../glm -I../nanovg/src -Imesa -I../stb -march=i486 -I/usr/local/include/GL -fpermissive

LDFLAGS = -lGLU -Lmesa -lOSMesa -L/usr/local/lib -I/usr/include/GL 

CXX = i586-pc-msdosdjgpp-g++
CC = i586-pc-msdosdjgpp-gcc

OBJS = DOS-version/main.o \
   ../noudar-core/noudar-core/src/Vec2i.o \
   ../noudar-core/noudar-core/src/CActor.o \
    ../noudar-core/noudar-core/src/CMonster.o \
    ../noudar-core/noudar-core/src/CMonsterGenerator.o \
    ../noudar-core/noudar-core/src/CTeam.o \
    ../noudar-core/noudar-core/src/CMap.o \
    ../noudar-core/noudar-core/src/CRandomWorldGenerator.o \
    ../noudar-core/noudar-core/src/Common.o \
    ../noudar-core/noudar-core/src/CHuntApproach.o \
    ../noudar-core/noudar-core/src/CCharacter.o \
    ../noudar-core/noudar-core/src/CCharacterArchetype.o \
    ../noudar-core/noudar-core/src/CDoorway.o \
    ../noudar-core/noudar-core/src/IFileLoaderDelegate.o \
    ../noudar-core/noudar-core/src/IMapElement.o \
    ../noudar-core/noudar-core/src/CGame.o \
    ../noudar-core/noudar-core/src/CItem.o \
    ../noudar-core/noudar-core/src/CStorageItem.o \
    ../noudar-core/noudar-core/src/CPlainFileLoader.o \
    ../noudar-core/noudar-core/src/CGameDelegate.o \
    ../noudar-core/noudar-core/src/commands/IGameCommand.o \
    ../noudar-core/noudar-core/src/commands/CMoveActorCommand.o \
    ../noudar-core/noudar-core/src/commands/CLoadNewLevelCommand.o \
    ../noudar-core/noudar-core/src/commands/CTurnActorCommand.o \
    ../noudar-core/noudar-core/src/commands/CQuitCommand.o \
    ../noudar-core/noudar-core/src/commands/CNullCommand.o \
    ../noudar-core/noudar-core/src/commands/CEndTurnCommand.o \
    ../noudar-core/noudar-core/src/commands/CPickItemCommand.o \
    ../noudar-core/noudar-core/src/commands/CDropItemCommand.o \
    ../noudar-core/noudar-core/src/commands/CUseCurrentItemCommand.o \
    ../noudar-core/noudar-core/src/commands/CCycleNextItemCommand.o \
    ../noudar-core/noudar-core/src/commands/CCyclePreviousItemCommand.o \
        ../noudar-rendering/DungeonGL11Renderer.o \
        ../noudar-rendering/RenderingJobSnapshotAdapter.o \
        ../noudar-rendering/VisibilityStrategy.o \
	../noudar-rendering/GameNativeAPI.o \
        ../noudar-rendering/NoudarDungeonSnapshot.o \
        ../noudar-rendering/SplatAnimation.o \
        ../noudar-rendering/CTile3DProperties.o \
        ../noudar-rendering/NoudarGLES2Bridge.o \
        DOS-version/WindowOperationsDOS.o \
	 ../gles2-renderer/LoadPNG.o \
        ../gles2-renderer/Logger.o \
        ../gles2-renderer/Trig.o \
        ../gles2-renderer/Camera.o \
        ../gles2-renderer/CLerp.o \
        ../gles2-renderer/TrigBatch.o \
        ../gles2-renderer/VBORenderingJob.o \
        ../gles2-renderer/NativeBitmap.o \
        ../NullSoundSystem/SoundClip.o \
        ../NullSoundSystem/SoundEmitter.o \
        ../NullSoundSystem/SoundListener.o \
        ../NullSoundSystem/SoundUtils.o

TARGET = NOUDAR.exe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDFLAGS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm *~
