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
		BUILD_DUMP_DIR="x86_64_dep"
		ARCH_BIT="64bit"
		ARCH_COMP="gcc"
	elif [ "$ARCH" = "i686" ]; then
		BUILD_DUMP_DIR="i686_dep"
		ARCH_BIT="32bit"
		ARCH_COMP="gcc"
	elif [ "$ARCH" = "armv7l" ]; then
		BUILD_DUMP_DIR="armv7l_dep"
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
		echo "[normal] dependency directory not exists... create! $BUILD_DUMP_DIR"
		mkdir ./$BUILD_DUMP_DIR
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
		echo "[normal] target dependency directory $BUILD_DUMP_DIR already installed"
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

 done_shell () {

	while read line; do VERSION=$line;break;done < ./version.txt
	BUILDTIME=`date +%Y/%m/%d/%H:%M`
	echo $versiontext
	echo "####################################"
	echo "#   live platform dependency make  #"
	echo "####################################"
	echo "arch		: $ARCH"    
        echo "version		: $VERSION"   
	echo "make time		: $BUILDTIME"
        echo "author		: whois-hm"
        echo "git		: https://github.com/whois-hm/live_platform"
 }


#################################################################
#                             exec				#
#################################################################

arch_in
make_dependency
mainline_dependency
done_shell
