#!/bin/bash
export LANG=C

export ALT_BOOTDIR="/usr/jdk1.6.0_45"
export ALT_JDK_IMPORT_PATH="/usr/jdk1.6.0_45"

export ANT_HOME="/home/simomme/002.tool/apache-ant-1.9.4"

export PATH="/usr/lib:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/home/simomme/002.tool/apache-ant-1.9.4:/usr/lib/x86_64-linux-gnu:/usr/lib/gcc/x86_64-linux-gnu/4.6"

#the env variable can not effect, so you need set it manual
export DISABLE_HOTSPOT_OS_VERSION_CHECK=ok

export HOTSPOT_BUILD_JOBS=5
export ALT_OUTPUTDIR="../build/hotspot_debug"
