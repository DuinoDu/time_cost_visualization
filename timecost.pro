TEMPLATE = app
QT += core gui widgets charts

contains(TARGET, qml.*) {
    QT += qml quick
}

TARGET = TimeCost
target.path = .

SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h \

INSTALLS += target


LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system -lboost_filesystem -lboost_thread

QMAKE_CXXFLAGS += -pthread
LIBS += -pthread
