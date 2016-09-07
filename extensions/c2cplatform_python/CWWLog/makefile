include $(C2C_BASE_PATH)/comm/makefile.comm.v2
CXX     = g++
CFLAGS  = -g -O -Wall -fPIC -Wno-write-strings
INSTALL_PATH = ../bbcplatform
INC_EXPORT_PATH  = $(C2C_BASE_PATH)/comm/export_include
INCLUDE = -I. -I$(INC_EXPORT_PATH) -I$(INC_EXPORT_PATH)/c2cplatform -I/usr/include/python2.6/
LIB_SET =  -L$(LIB_EXPORT_PATH) -llibrary_log2 -llogclient -llibrary_the3_tinyxml
OUTPUT = CWWLog.so

all: $(OUTPUT)

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $^ 

.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^ $(APP_LIB)

$(OUTPUT): CWWLog_python_api.o
	$(CXX) $(CFLAGS) -shared -o $@ $^ ${LIB_SET}

clean:
	rm -f *.o *.so *.~ *.bak $(OUTPUT)

install:
	cp -f $(OUTPUT) $(INSTALL_PATH)

