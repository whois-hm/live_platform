#!/bin/sh
ARCH=$1

VERSION="default"
BUILDTIME="default"
BUILD_DUMP_DIR="none"
THIRDPARTY_LIBRARY_DIR="3rd_librarys"
THIRDPARTY_FFMPEG="ffmpeg"
THIRDPARTY_LIVE555="live555"
THIRDPARTY_MAINLINE="livemedia"
THIRDPARTY_WQ="wq"
ARCH_BIT="32bit"
ARCH_COMP="gcc"

 arch_in () {
# select archtecture from shell first parameter
	if [ "$ARCH" = "x86_64" ]; then
		BUILD_DUMP_DIR="x86_64_buildup"
		ARCH_BIT="64bit"
		ARCH_COMP="gcc"
	elif [ "$ARCH" = "i686" ]; then
		BUILD_DUMP_DIR="i686_buildup"
		ARCH_BIT="32bit"
		ARCH_COMP="gcc"
	elif [ "$ARCH" = "armv7l" ]; then
		BUILD_DUMP_DIR="armv7l_buildup"
		ARCH_BIT="64bit"
		ARCH_COMP="arm"
	else
		echo "[error] cant support archtecture $ARCH done..."
		exit 0
	fi 

	export BUILD_ARCH=$ARCH
 }

 make_dependency () {
# make build dependencies.
# our dependency first ffmpeg    'for audio video codec en/decoding'
#                next  live555   'for rtsp client/server'
#                next  openssl   'for ssl'
#                next  poco      'for http interface'
#                last  workqueue 'for event framework'

	if [ ! -d ./$BUILD_DUMP_DIR ]; then
		echo "[warnning] build up directory not exists... create! $BUILD_DUMP_DIR"
		mkdir ./$BUILD_DUMP_DIR
		echo "[normal] create bin directory"
		mkdir ./$BUILD_DUMP_DIR/bin
		echo "[normal] now uncompressing dependency librarys 'openssl' 'poco' 'ffmpeg' 'live555' 'wq', and arm toolchain"

		echo "---------< openssl >---------"
		tar -xzf $THIRDPARTY_LIBRARY_DIR/openssl_$ARCH_BIT"_"$ARCH_COMP.tar.gz -C ./$BUILD_DUMP_DIR		
		echo "---------< done >---------"

		echo "---------< poco >---------"
		tar -xzf $THIRDPARTY_LIBRARY_DIR/poco_$ARCH_BIT"_"$ARCH_COMP.tar.gz -C ./$BUILD_DUMP_DIR		
		echo "---------< done >---------"

		echo "---------< ffmpeg >---------"
		tar -xzf $THIRDPARTY_LIBRARY_DIR/ffmpeg_$ARCH_BIT"_"$ARCH_COMP.tar.gz -C ./$BUILD_DUMP_DIR 
		echo "---------< done >---------"

		echo "---------< live555 >---------"
		tar -xzf $THIRDPARTY_LIBRARY_DIR/live555_$ARCH_BIT"_"$ARCH_COMP.tar.gz -C ./$BUILD_DUMP_DIR
		echo "---------< done >---------"

		echo "---------< workqueue >---------"
		tar -xzf $THIRDPARTY_LIBRARY_DIR/wq_$ARCH_BIT"_"$ARCH_COMP.tar.gz -C ./$BUILD_DUMP_DIR
		echo "---------< done >---------"
#download compiler if platform in armv7 cpu
		if [ "$ARCH" = "armv7l" ]; then 
			echo "---------< toolchain arm >---------"
			wget -P ./$BUILD_DUMP_DIR http://releases.linaro.org/components/toolchain/binaries/6.5-2018.12/arm-linux-gnueabihf/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar.xz
			xz -d ./$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar.xz
			tar xf ./$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar -C ./$BUILD_DUMP_DIR
			echo "---------< done >---------"
		fi
		echo "[normal] dependency all complete....."
	else
		echo "[normal] target build up directory $BUILD_DUMP_DIR already installed"
	fi 
 }

 mainline_dependency () {
# download our core function
	if [ ! -d ./$THIRDPARTY_MAINLINE ]; then
		echo "[warnning] mainline livemedia repository not found.... clone!"
		git clone "https://github.com/whois-hm/livemedia"
		if [ ! -d ./$THIRDPARTY_MAINLINE ]; then
			echo "[error] mainline livemedia repository clone fail..."
			exit 0
		fi
		echo "[normal] mainline livemedia clone all complete..."
	else 
		echo "[normal] our mainline livemedia already installed"
	fi
 }

 export_compiler () {
# export the target device compiler
	if [ "$ARCH" = "armv7l" ]; then	
		echo "[normal] arch armv7l detection... export arm compliler"
		export AR=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ar
		export AS=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-as
		export LD=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ld
		export RANLIB=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-ranlib
		export CC=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc
		export CXX=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++
		export NM=$(pwd)/$BUILD_DUMP_DIR/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf-nm
	else 
		echo "[normal] arch default host detection... export default gnu"
		export AR=ar 
		export AS=as
		export LD=ld
		export RANLIB=ranlib
		export CC=gcc
		export CXX=g++
		export NM=nm
	fi
 }

 build_clean () {
# ready to copile our object
	rm ./$BUILD_DUMP_DIR/bin/live_http
	rm ./$BUILD_DUMP_DIR/bin/live_unit_rtspserver
	rm ./$BUILD_DUMP_DIR/$ARCH"_live_platform_server".tar.gz
	
	cd live_http
	make clean
	cd ../
	cd live_unit_rtspserver
	make clean
	cd ../
 }
 
 build_live_http () {
# build the 'live_http'
	echo "----------------< live_http >----------------"
	cd live_http
	make
	if [ -e ./live_http ]; then
		cp ./live_http ../$BUILD_DUMP_DIR/bin
		echo "[normal] live_http build ok........."
	else 
		echo "[error] live_http build fail........."
	fi
	make clean
	cd ../
	echo "----------------< done >----------------"
 }

 build_unit_rtspserver () {
# build the live_unit_rtspserver
	echo "----------------< live_unit_rtspserver >----------------"
	cd live_unit_rtspserver
	make
	if [ -e ./live_unit_rtspserver ]; then
		cp ./live_unit_rtspserver ../$BUILD_DUMP_DIR/bin
		echo "[normal] live_unit_rtspserver build ok........."
	else 
		echo "[error] live_unit_rtspserver build fail........."
	fi
	make clean
	cd ../
	echo "----------------< done >----------------"
 }

 can_package () {
#done
	if [ -e ./$BUILD_DUMP_DIR/bin/live_http ] && [ -e ./$BUILD_DUMP_DIR/bin/live_unit_rtspserver ]; then
		echo "[normal] shell build complete.........."	
	else 
		echo "[error] shell exception.........."
		exit 0
	fi
 }

 packing () {
	echo "[normal] now release packing start...!"
	cd ./$BUILD_DUMP_DIR/
	mkdir ./$ARCH"_live_platform_server"
        cp ./bin/live_http ./$ARCH"_live_platform_server"/
	cp ./bin/live_unit_rtspserver ./$ARCH"_live_platform_server"/
	if [ -e ../streamlist_built.txt ]; then
		cp ../streamlist_built.txt ./$BUILD_DUMP_DIR/$ARCH"_live_platform_server"/streamlist.txt
	else
 		echo "[warnning] can't find your streamlist...."
	fi
	tar -cvzf $ARCH"_live_platform_server".tar.gz ./$ARCH"_live_platform_server"/
	rm -r ./$ARCH"_live_platform_server"/
	echo "[normal] now release packing ok...!"
	cd ../
}
 done_shell () {

	while read line; do VERSION=$line;break;done < ./version.txt
	BUILDTIME=`date +%Y/%m/%d/%H:%M`
	echo $versiontext
	echo "####################################"
	echo "#           live platform          #"
	echo "####################################"
	echo "arch		: $ARCH"    
        echo "version		: $VERSION"   
	echo "build time	: $BUILDTIME"
        echo "author		: whois-hm"
        echo "git		: https://github.com/whois-hm/live_platform"
 }


#################################################################
#                             exec				#
#################################################################
echo "[normal] live platform server build up.....start..!"
echo "[normal] architecture found $ARCH...."

arch_in

echo "[normal] target build up directory=$BUILD_DUMP_DIR"

make_dependency

echo "[normal] our mainline livemedia checking...."

mainline_dependency

echo "[normal] now compiler checing....export compiler... $ARCH"

export_compiler

echo "[normal] compiler setup all complete...."
echo "[normal] now build to object servers 'live_http', 'live_unit_rtspserver'"

build_clean

build_live_http

build_unit_rtspserver

can_package

packing

done_shell
