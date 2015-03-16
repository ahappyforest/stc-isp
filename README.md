
[![Build Status](https://travis-ci.org/van9ogh/stc-isp.svg)](https://travis-ci.org/van9ogh/stc-isp)

An STC 8051 ISP programmer for linux
for more information please use -h help


this is a hack from gSTCISP, I convert gtk(2.0) version to command version
maybe, someone are looking for...just try this tools...

just: Make~

limitation:
	1. use TTL to USB to download (PL2303 chip) if your 
	   development board have, Better
	2. for some unknown reason, TTL to RS232 cannot download, 
	   so if you konw, just hack my code.

bash shell function:
you can add this two func to your ~/.bashrc, One key download:

```bash
########################################################
# 一键编译, 完成后在当前目录下生成build文件夹
# one key compile, after this operation, the build dir will generate
stc-cc() {
CUR_PATH=`pwd`
	if [ ! -d $CUR_PATH/build ]; then
		mkdir $CUR_PATH/build
	fi

	cd $CUR_PATH/build
	sdcc -mmcs51 $CUR_PATH/$1
}
```

```bash
# 一键编译&&烧写
# one key compile&&isp, after this operation, the build dir will generate and programmed to STC chip
stc-cisp() {
CUR_PATH=`pwd`
# 首先在当前目录下生成build目录
	if [ ! -d $CUR_PATH/build ]; then
		mkdir $CUR_PATH/build
	fi

# 编译
	sourceFile=$CUR_PATH/build/$1
	cd $CUR_PATH/build
	sdcc -mmcs51 $CUR_PATH/$1
	cd $CUR_PATH
	stcisp -f ${sourceFile%.*}.ihx
}
########################################################
```
