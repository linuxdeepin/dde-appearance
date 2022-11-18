#!/bin/bash

CURRENT_DIR=$(pwd)
PROJECT_DIR=$(dirname $(realpath $(dirname $(realpath $(dirname $(realpath $0))))))
cd ${PROJECT_DIR}
echo 当前目录：${CURRENT_DIR}
echo 工程目录：${PROJECT_DIR}
# 执行目录会切换到工程目录

# dde-appearance
lupdate ${PROJECT_DIR}/src -no-obsolete -no-ui-lines -locations none -ts ${PROJECT_DIR}/misc/translations/translations/dde-appearance_en.ts

# 推送翻译
tx push -s -b m23
cd ${CURRENT_DIR}
