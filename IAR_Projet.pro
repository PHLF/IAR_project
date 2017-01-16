TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += Controleur/main.cpp \
           Controleur/LightSim.cpp \
           Modele/Agents/Agent.cpp \
           Modele/Agents/Predator.cpp \
           Modele/Agents/Prey.cpp \
           Modele/Agents/Retina.cpp \
           Modele/Environnement/Environment.cpp \
           Vue/FenetrePrincipale.cpp

HEADERS += Controleur/LightSim.h \
           Modele/Agents/Agent.h \
           Modele/Agents/Predator.h \
           Modele/Agents/Prey.h \
           Modele/Agents/Retina.h \
           Modele/Environnement/Environment.h \    
           Modele/Environnement/utils.h \
           Vue/FenetrePrincipale.h \
           Divers/Globals.h \
           Vue/SDLWrappers.h

LIBS += -lSDL2
