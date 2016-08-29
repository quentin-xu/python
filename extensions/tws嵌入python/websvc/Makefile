include $(C2C_BASE_PATH)/comm/makefile.comm.v2

INCLUDE = -I../src \
	-I$(INC_EXPORT_PATH)/ \
	-I$(INC_EXPORT_PATH)/libc2c \
	-I$(INC_EXPORT_PATH)/libtlib \
	-I$(INC_EXPORT_PATH)/app_platform \
	-I$(INC_EXPORT_PATH)/c2cplatform \
	-I$(INC_EXPORT_PATH)/c2cplatform/library/the3/ \

LIB = -L$(LIB_EXPORT_PATH) \
      -L../../../lib/ -llibrary_algorithm -ltinyxml_pl_inner -llibrary_xml -lxml2\
	  -lapcgibase \
	  -lcomponent_configcenter_set -llibrary_module_invoke_api \
	  -llibrary_log2 -llogclient $(CGI_WEBFRAMEWORK3_LIB) $(CGI_APCGIBASE_LIB)\
	  -llibrary_rewrite -llibrary_the3_pcre -llibrary_the3_itil -llibrary_the3_tinyxml \
	  -llibrary_the3_jsoncpp_060 -lc2cplatform_chdog_limit \
	  -lcomponent_error $(CGI_TEMPLATE_LIB) -llibrary_util

VPATH=../lib:../src	

OUTPUT  = python_svc.so

SOURCES = python_svc_adaptor.cpp \
		  python_svc.cpp 

OBJECTS=$(SOURCES:.cpp=.o)
		  
all:$(OUTPUT) 

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -c $(filter %.cpp, $^) 

.o:
	$(CXX) $(CFLAGS) $(INCLUDE) -o $@ $^

$(OUTPUT): $(OBJECTS)
	$(CXX) $(CFLAGS) -shared  -o $(OUTPUT) $^ $(LIB)

clean:
	rm -f *.o *.~ *.bak
	rm -f $(OUTPUT)

dep:
	mkdep -p$(VPATH) $(INCLUDE) $(SOURCES) > .depend

ifneq ($(wildcard .depend),)
include .depend
endif


