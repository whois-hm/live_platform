#!/bin/sh

if [ -d ./livemedia ]; then
	echo core mainline repository 'livemedia' exist.
	echo you want change code commit or delete force.
	read -p "press y if you want commit other delete force press other:" yn
	if [ "$yn" = "y" ]; then
		echo "ok... commit livemedia repository first"
		exit 0
	else
		read -p "really livemediarepository delete force?:" retryyn
		if [ "$retryyn" = "y" ]; then
			echo "ok... delete force.."
			rm -r ./livemedia			
		else
			echo "ok... commit livemedia repository first"
			exit 0
		fi
	fi
	
fi
echo "entering livehttp"
cd ./live_http
make clean
echo "leave livehttp"

cd ../

echo "entering live_unit_rtspserver"
cd ./live_unit_rtspserver
make clean
echo "leave live_unit_rtspserver"
cd ../

rm ./streamlist_built.txt
rm -r *_buildup

echo "clean all complete.."
