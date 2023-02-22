FILE = myfunctions.cpp
!exists($$FILE):system(wget https://raw.githubusercontent.com/DrSmyrke/QT-Libs/master/$$FILE -O $$FILE)
SOURCES += $$FILE

FILE = myfunctions.h
!exists($$FILE):system(wget https://raw.githubusercontent.com/DrSmyrke/QT-Libs/master/$$FILE -O $$FILE)
HEADERS += $$FILE



FILE = qcompressor.cpp
!exists($$FILE):system(wget https://raw.githubusercontent.com/DrSmyrke/QT-Libs/master/$$FILE -O $$FILE)
SOURCES += $$FILE
LIBS += -lz
FILE = qcompressor.h
!exists($$FILE):system(wget https://raw.githubusercontent.com/DrSmyrke/QT-Libs/master/$$FILE -O $$FILE)
HEADERS += $$FILE
