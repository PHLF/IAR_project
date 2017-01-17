TEMPLATE = app
CONFIG += console
CONFIG += c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += Controleur/main.cpp \
           Controleur/LightSim.cpp \
           Modele/Agents/Agent.cpp \
           Modele/Agents/Predator.cpp \
           Modele/Agents/Prey.cpp \
           Modele/Agents/Retina.cpp \
           Modele/Environnement/Environment.cpp \
           Vue/FenetrePrincipale.cpp \
           Divers/Utils.cpp

HEADERS += Controleur/LightSim.h \
           Modele/Agents/Agent.h \
           Modele/Agents/Predator.h \
           Modele/Agents/Prey.h \
           Modele/Agents/Retina.h \
           Modele/Environnement/Environment.h \
           Vue/FenetrePrincipale.h \           
           Vue/SDLWrappers.h \
           Divers/Globals.h \
           Divers/Utils.h

LIBS += -lSDL2
