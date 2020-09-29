#!/bin/sh
ARCH=$1

BUILD_DUMP_DIR="none"
THIRDPARTY_LIBRARY_DIR="3rd_librarys"
THIRDPARTY_FFMPEG="ffmpeg"
THIRDPARTY_LIVE555="live555"
THIRDPARTY_MAINLINE="livemedia"
THIRDPARTY_WQ="wq"
ARCH_BIT="32bit"
ARCH_COMP="gcc"

echo "[normal] live platform server build up.....start..!"
echo "[normal] architecture found $ARCH...."

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

echo "[normal] target build up directory=$BUILD_DUMP_DIR"
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

	if [ "$ARCH" = "armv7l" ]; then
		echo "---------< toolchain arm >---------"
		xz -d toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar.xz
		tar xf toolchain/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar -C ./$BUILD_DUMP_DIR
		echo "---------< done >---------"
	fi
	echo "[normal] dependency all complete....."
else
	echo "[normal] target build up directory $BUILD_DUMP_DIR already installed"
fi

echo "[normal] our mainline livemedia checking...."
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

echo "[normal] now compiler checing....export compiler... $ARCH"
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
echo "[normal] compiler setup all complete...."
echo "[normal] now build to object servers 'live_http', 'live_unit_rtspserver'"
rm ./$BUILD_DUMP_DIR/bin/live_http
rm ./$BUILD_DUMP_DIR/bin/live_unit_rtspserver

echo "----------------< live_http >----------------"
cd live_http
make clean
make
if [ -e ./live_http ]; then
	cp ./live_http ../$BUILD_DUMP_DIR/bin
	echo "[normal] live_http build ok........."
else 
	echo "[error] live_http build fail........."
fi
make clean
echo "----------------< done >----------------"

echo "----------------< live_unit_rtspserver >----------------"
cd ../live_unit_rtspserver
make clean
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
if [ -e ./$BUILD_DUMP_DIR/bin/live_http ] && [ -e ./$BUILD_DUMP_DIR/bin/live_unit_rtspserver ]; then
	echo "[normal] shell all complete.........."	
else 
	echo "[error] shell all complete.........."
fi




