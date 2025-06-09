#!/bin/bash
# this file is used to auto-generate .qm file from .ts file.
# author: shibowen at linuxdeepin.com
CURRENT_DIR=$(pwd)
PROJECT_DIR=$CURRENT_DIR/misc/translations
cd ${PROJECT_DIR}
echo 当前目录：${CURRENT_DIR}
echo 工程目录：${PROJECT_DIR}

if [ -f "/usr/bin/lrelease6" ]; then
    LRELEASE=lrelease6
elif [ -f "/usr/lib/qt6/bin/lrelease" ]; then
    LRELEASE=/usr/lib/qt6/bin/lrelease
elif [ -f "/usr/lib64/qt6/bin/lrelease" ]; then
    LRELEASE=/usr/lib64/qt6/bin/lrelease
else
    echo "lrelease: command not found."
    exit 1
fi

ts_list=(`ls translations/*.ts`)

for ts in "${ts_list[@]}"
do
    printf "\nprocess ${ts}\n"
    ${LRELEASE} "${ts}"
done

cd ${CURRENT_DIR}
