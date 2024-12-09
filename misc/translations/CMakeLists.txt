# SPDX-FileCopyrightText: 2017 - 2024 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core LinguistTools)

# can use qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES xx xx) from Qt6.7
set(TRANSLATION_LANGUAGES
        af ak am_ET ar ar_EG ast az bg bn bo br ca cgg cs da de de_CH el en en_AU
        en_GB en_US eo es es_MX et eu fa fi fil fr gl_ES he hi_IN hr hu hy id it
        ja kab ka kk km_KH kn_IN ko ku ku_IQ ky ky@Arab la lo lt lv ml mn mr ms
        nb ne nl pa pam pl ps pt pt_BR ro ru sc si sk sl sq sr sv sw ta te th tr
        tzm ug uk ur uz vi zh_CN zh_HK zh_TW
)

set(TRANSLATION_FILES "")
list(APPEND TRANSLATION_FILES "${PROJECT_SOURCE_DIR}/translations/dde-appearance.ts")

foreach(LANG ${TRANSLATION_LANGUAGES})
    list(APPEND TRANSLATION_FILES "${PROJECT_SOURCE_DIR}/translations/dde-appearance_${LANG}.ts")
endforeach()

set_source_files_properties(app_en.ts app_de.ts
    PROPERTIES OUTPUT_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/translations")

# can generate ts and qm file
qt_add_translations(${PLUGIN_NAME}
    TS_FILES ${TRANSLATION_FILES}
    SOURCES ${TRANS_FILE_SOURCE}
    QM_FILES_OUTPUT_VARIABLE TR_QM_FILES
)

## qm files
install(FILES ${TR_QM_FILES} DESTINATION ${CMAKE_INSTALL_DATADIR}/${BIN_NAME}/translations)