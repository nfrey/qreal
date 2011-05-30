# XMI
HEADERS += generators/xmi/xmiHandler.h \
    generators/editorGenerator/metaEditorGenerator.h

SOURCES += generators/xmi/xmiHandler.cpp \
    generators/editorGenerator/metaEditorGenerator.cpp

# JAVA
HEADERS += generators/java/javaHandler.h \

SOURCES += generators/java/javaHandler.cpp \

# Генератор хаскола
HEADERS += generators/hascol/hascolGenerator.h \

SOURCES += generators/hascol/hascolGenerator.cpp \


HEADERS += generators/editorGenerator/editorGenerator.h \

SOURCES += generators/editorGenerator/editorGenerator.cpp \


# Креатор не умеет показывать файлы глубже одного уровня в иерархии инклудов
# include (generators/xmi/xmi.pri)

# include (generators/hascol/hascol.pri)
