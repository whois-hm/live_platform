
ARCH=$(BUILD_ARCH)_buildup
FFMPEG_DIR=	$(PWD)/../$(ARCH)/ffmpeg
LIVE555_DIR=	$(PWD)/../$(ARCH)/live555
WQ_DIR=		$(PWD)/../$(ARCH)/wq
POCO_DIR=	$(PWD)/../$(ARCH)/poco
SSL_DIR=	$(PWD)/../$(ARCH)/openssl
MAINLINE_DIR=	$(PWD)/../livemedia

INC=			-I $(WQ_DIR)/inc -I $(FFMPEG_DIR)/inc -I $(LIVE555_DIR)/inc -I $(POCO_DIR)/include -I $(SSL_DIR)/include -I $(MAINLINE_DIR) 
LIB_DIR= 		-L $(WQ_DIR)/lib -L $(FFMPEG_DIR)/lib -L $(LIVE555_DIR)/lib -L $(POCO_DIR)/lib -L $(SSL_DIR)/lib
ifeq ($(BUILD_ARCH), armv7l)
LIBS=			-lwq -lliveMedia -lgroupsock -lUsageEnvironment -lBasicUsageEnvironment -lavformat -lavformat -lavfilter -lavcodec -lavdevice -lavutil -lpostproc -lswresample -lswscale -lx264 -lPocoUtil -lPocoNet -lPocoXML -lPocoFoundation -lPocoCrypto -lPocoData -lPocoDataSQLite -lPocoEncodings -lPocoJSON -lPocoJWT -lPocoRedis -lPocoZip -ldl -lm -lpthread -lrt -lcrypto -lssl 
else ifeq ($(BUILD_ARCH), x86_64)
LIBS=			-lwq -lliveMedia -lgroupsock -lUsageEnvironment -lBasicUsageEnvironment -lavformat -lavformat -lavfilter -lavcodec -lavdevice -lavutil -lfdk-aac -lmp3lame -lopus -lpostproc -lswresample -lswscale -lx264 -lvpx -lx265 -lyasm -lz -lvorbis -ltheora -ldl -ltheoraenc -ltheoradec -lvorbisenc -lvdpau -lm -lX11 -lva -lva-drm -lva-x11 -lnuma  -lPocoUtil -lPocoNet -lPocoXML -lPocoFoundation -lPocoCrypto -lPocoData -lPocoDataSQLite -lPocoEncodings -lPocoJSON -lPocoJWT -lPocoRedis -lPocoZip -ldl -lm -lpthread -lrt -lcrypto -lssl 
else 
LIBS=			-lwq -lliveMedia -lgroupsock -lUsageEnvironment -lBasicUsageEnvironment -lavformat -lavformat -lavfilter -lavcodec -lavdevice -lavutil -lfdk-aac -lmp3lame -lopus -lpostproc -lswresample -lswscale -lx264 -lvpx -lx265 -lyasm -lz -lvorbis -ltheora -ldl -ltheoraenc -ltheoradec -lvorbisenc -lvdpau -lm -lX11 -lva -lva-x11 -lnuma  -lPocoUtil -lPocoNet -lPocoXML -lPocoFoundation -lPocoCrypto -lPocoData -lPocoDataSQLite -lPocoEncodings -lPocoJSON -lPocoJWT -lPocoRedis -lPocoZip -ldl -lm -lpthread -lrt -lcrypto -lssl 
endif

CXX_SRCS=		$(wildcard *.cpp)
CXX_OBJS=		$(CXX_SRCS:.cpp=.o)


#all : live_http
live_http : $(CXX_OBJS)
	$(CXX) -o live_http $(CXX_OBJS) $(LIB_DIR) $(LIBS)

requesthandler.o : requesthandler.cpp
	$(CXX) -std=c++11 -fpermissive -c -o requesthandler.o requesthandler.cpp $(INC)
requesthandlerfactory.o : requesthandlerfactory.cpp
	$(CXX) -std=c++11 -fpermissive -c -o requesthandlerfactory.o requesthandlerfactory.cpp $(INC)
streaming_list.o : streaming_list.cpp
	$(CXX) -std=c++11 -fpermissive -c -o streaming_list.o streaming_list.cpp $(INC)
main.o : main.cpp
	$(CXX) -std=c++11 -fpermissive -c -o main.o main.cpp $(INC)


#$(CXX_OBJS) : $(CXX_SRCS)
#	$(CXX) -std=c++0x -c -o $(CXX_OBJS) $(CXX_SRCS) $(INC)
clean :
	rm -f *.o
	rm live_http
	
