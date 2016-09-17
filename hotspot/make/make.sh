#!/bin/bash
source ../compile.sh
make jvmg jvmg1 2>&1|tee ../../build/hotspot_build_debug.log 
