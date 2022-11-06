LINK_TARGET = pvnswitch.exe

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

OBJS = pvnswitch.o stdafx.o BtnLifeCicle.o BufferHelper.o messages.o windowshooks.o resources.o

REBUILDABLES = $(OBJS) $(LINK_TARGET)

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g -O0 -DWIN32 -D_DEBUG -D_CONSOLE -D_LIB -finput-charset=cp1251
	LDFLAGS += -g -DWIN32 -D_DEBUG -D_CONSOLE -D_LIB
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O3 -DWIN32 -D_WINDOWS -DNDEBUG -finput-charset=cp1251
        LDFLAGS += -mwindows
else
    $(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all:	$(LINK_TARGET)

$(LINK_TARGET) : $(OBJS)
	$(CXX) -static $(LDFLAGS) -o $@ $^

%.o:	$(PROJECT_ROOT)%.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

resources.o : $(PROJECT_ROOT)resources.rc
	windres $(PROJECT_ROOT)resources.rc -o resources.o

clean:
	rm -f $(REBUILDABLES)
	
pvnswitch.o : $(PROJECT_ROOT)pvnswitch.h
stdafx.o : $(PROJECT_ROOT)stdafx.h
BtnLifeCicle.o : $(PROJECT_ROOT)BtnLifeCicle.h
BufferHelper.o : $(PROJECT_ROOT)BufferHelper.h 
messages.o : $(PROJECT_ROOT)messages.h
windowshooks.o : $(PROJECT_ROOT)windowshooks.h