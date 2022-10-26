LINK_TARGET = pvnswitch.exe

OBJS = pvnswitch.o stdafx.o BtnLifeCicle.o BufferHelper.o messages.o windowshooks.o resources.o

REBUILDABLES = $(OBJS) $(LINK_TARGET)

clean :
	rm -f $(REBUILDABLES)
	echo Clean done

all : $(LINK_TARGET)
	echo All done

$(LINK_TARGET) : $(OBJS)
	g++ -g -static -o $@ $^

%.o : %.cpp
	g++ -DWIN32 -D_DEBUG -D_CONSOLE -D_LIB -finput-charset=cp1251 -g -o $@ -c $<

resources.o : resources.rc
	windres resources.rc -o resources.o

pvnswitch.o : pvnswitch.h
stdafx.o : stdafx.h
BtnLifeCicle.o : BtnLifeCicle.h
BufferHelper.o : BufferHelper.h 
messages.o : messages.h
windowshooks.o : windowshooks.h
resources.o : resources.rc