include $(C2C_BASE_PATH)/comm/makefile.comm.v2
CXX     = g++
CFLAGS  = -g -O -Wall -fPIC 
INC_EXPORT_PATH  = $(C2C_BASE_PATH)/comm/export_include
INCLUDE = -I. -I$(INC_EXPORT_PATH) -I$(INC_EXPORT_PATH)/c2cplatform/component/configcenter -I/usr/include/python2.6/
LIB_SET = -L$(LIB_EXPORT_PATH) -llibrary_module_invoke_api -llogclient -lcomponent_configcenter_4daemon_multiport -lcomponent_configcenter_set
OUTPUT = confg4set.so

all: $(OUTPUT)

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $^ 

.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^ $(APP_LIB)

$(OUTPUT): confg4set_python_api.o
	$(CXX) $(CFLAGS) -shared -o $@ $^ ${LIB_SET}

clean:
	rm -f *.o *.so *.~ *.bak $(OUTPUT)

install:
	cp -f $(OUTPUT) /usr/local/lib

