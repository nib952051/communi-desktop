######################################################################
# Communi
######################################################################

TARGET = uchardetplugin
DESTDIR = ../../../plugins/communi

SOURCES += plugin.cpp
include(../../3rdparty/uchardet-0.0.1/uchardet.pri)

contains(MEEGO_EDITION,harmattan) {
    !no_rpath:QMAKE_RPATHDIR += /opt/communi/lib
    COMMUNI_INSTALL_PLUGINS = /opt/communi/plugins/communi
} else {
    COMMUNI_INSTALL_PLUGINS = $$[QT_INSTALL_PLUGINS]/communi
}

target.path = $$COMMUNI_INSTALL_PLUGINS
INSTALLS += target

symbian {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = NetworkServices
    # TODO: TARGET.UID3 = 0xFFFFFFFF

    target.sources = $${TARGET}.dll
    target.path = $$QT_PLUGINS_BASE_DIR/communi

    DEPLOYMENT += target
}

include(../plugins.pri)
