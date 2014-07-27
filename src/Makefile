GCC = nspire-g++
LD = nspire-ld
#WIth -DALLOW_ALL_LEVELS you get 2000 levels! :)
GCCFLAGS = -Os -Wall -W -marm -Wno-unused-parameter -I include #-DALLOW_ALL_LEVELS
LDFLAGS = -L lib -lndls -lBinary -lArchive
OBJCOPY := "$(shell which arm-elf-objcopy 2>/dev/null)"
ifeq (${OBJCOPY},"")
	OBJCOPY := arm-none-eabi-objcopy
endif
EXE = nBinary-sdl.tns
OBJS = $(patsubst %.cpp,%.o,$(wildcard *.cpp))
vpath %.tns $(DISTDIR)

all: prepare $(EXE)

#Because JGit does not do symbolic links and TeamCity uses JGit
prepare:
	mkdir include
	mkdir include/ArchiveLib
	mkdir include/BinaryLib
	mkdir lib
	cp -v ArchiveLib/include/ArchiveLib.h include/ArchiveLib
	cp -v ArchiveLib/libArchive.a lib
	cp -v BinaryLib/include/BinaryLib.h include/BinaryLib
	cp -v BinaryLib/libBinary.a lib

%.o: %.cpp
	$(GCC) $(GCCFLAGS) -c $<

$(EXE): $(OBJS)
	$(LD) $^ -o $(@:.tns=.elf) $(LDFLAGS)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $@

clean:
	rm -rf include
	rm -rf lib
	rm -f *.o *.elf
	rm -f $(EXE)
