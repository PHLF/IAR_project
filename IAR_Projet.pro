TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS_RELEASE += -O3

SOURCES += Controller/main.cpp \
           Controller/LightSim.cpp \
           Controller/LocalThreadSim.cpp \
           Model/Agents/Agent.cpp \
           Model/Agents/Predator.cpp \
           Model/Agents/Prey.cpp \
           Model/Agents/Retina.cpp \
           Model/Environment/Environment.cpp \
           Model/Evo/MarkovBrain.cpp \
           View/MainView.cpp\
           Misc/Utils.cpp

HEADERS += Controller/LightSim.h \
           Controller/LocalThreadSim.h \
           Model/Agents/Agent.h \
           Model/Agents/Predator.h \
           Model/Agents/Prey.h \
           Model/Agents/Retina.h \
           Model/Environment/Environment.h \
           Model/Evo/MarkovBrain.h \
           View/SDLWrappers.h \
           View/MainView.h \
           Misc/Globals.h \
           Misc/Utils.h

LIBS += -lSDL2 -lpthread
