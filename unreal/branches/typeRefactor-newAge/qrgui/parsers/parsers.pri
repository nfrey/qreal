# Креатор не умеет показывать файлы глубже одного уровня в иерархии инклудов
# include (generators/xmi/xmi.pri)
# include (generators/hascol/hascol.pri)
# Креатор не умеет показывать файлы глубже одного уровня в иерархии инклудов
# include (generators/xmi/xmi.pri)
# include (generators/hascol/hascol.pri)
# Хаскол
HEADERS += parsers/hascol/hascolParser.h \
	parsers/xml/treeElement.h \
    parsers/xml/MetaModelParser.h \
	parsers/xml/metaElementImpl.h \
	../qrxc/scalableItem.h\
	../qrxc/scalableCoordinate.h\

SOURCES += parsers/hascol/hascolParser.cpp \
	parsers/xml/treeElement.cpp \
    parsers/xml/MetaModelParser.cpp \
	parsers/xml/metaElementImpl.cpp \
	../qrxc/scalableItem.cpp\
	../qrxc/scalableCoordinate.cpp\

HEADERS += parsers/xml/xmlParser.h
SOURCES += parsers/xml/xmlParser.cpp # Креатор не умеет показывать файлы глубже одного уровня в иерархии инклудов

