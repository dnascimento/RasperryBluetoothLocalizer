INCLUDE_DIR=include
#CC=arm-unknown-linux-gnu-gcc
CC=gcc
CFLAGS=-fPIC -g -L../includefred

#We can only change the name of this folder. We cannot put it into another directory
#and it must end with a slash '/'
BUILD_DIR=build/


all: 
		mkdir -p $(BUILD_DIR)
		make -C spotter/
		make -C spotter/ plugins

deploy: all
		mv $(BUILD_DIR) ~/tese_build
		cp -r ../lib/spot_plugins ~/tese_build/
		cp -r ../config ~/tese_build/

clean:
		rm -rf ~/tese_build/
		make -C spotter/ clean
		make -C manager/ clean

redo: clean deploy

update: all
		mv $(BUILD_DIR)* ~/tese_build
		cp -r ../lib/spot_plugins ~/tese_build/
		
