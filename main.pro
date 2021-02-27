VPATH += $$system(pwd)/src/gui
INCLUDEPATH += $$system(pwd)/src $$system(pwd)/src/gui
include(src/gui/Astree.pro)
DESTDIR=.
CONFIG(release, debug|release):TARGET = astree

unix:!macx {
  isEmpty(PREFIX) {
    PREFIX = /usr
  }
  target.path = $$PREFIX/bin
  desktop.path = $$PREFIX/share/applications
  desktop.files = astree.desktop
  icon.target = $$OUT_PWD/astree.png
  icon.path = $(INSTALL_ROOT)/$$PREFIX/share/pixmaps
  icon.commands = $(COPY_FILE) Astree.png $$icon.target \
    && $(MKDIR) $$icon.path \
    && $(INSTALL_FILE) $$icon.target $$icon.path
  appdata.path = $$PREFIX/share/appdata
  appdata.files = astree.appdata.xml
  INSTALLS += target desktop icon appdata
}
