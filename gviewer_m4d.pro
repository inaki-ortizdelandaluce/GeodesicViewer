## It should not be necessary to change
## the TOP_DIR variable
TOP_DIR = $$PWD

## Do not change this line
HOME = $$system(echo $HOME)

## This path should point to the location
## where you have copied the Motion4D library.
M4D_DIR      = $$TOP_DIR/../Motion4D/

## If your Gnu Scientific Library is not
## installed in the standard paths, please
## fix this variable accordingly.
GSL_DIR      = #/usr/local/gsl/1.15

## Do not change these lines
M4D_SRC_DIR  = $$M4D_DIR/src
M4D_LIB_DIR  = $$M4D_DIR/lib
GSL_LIB_DIR  = $$GSL_DIR/lib


## You should not need this...
#DEFINES += GL_GLEXT_PROTOTYPES


## If you have a big screen
DEFINES += USE_BIG_SCREEN

## Shows time needed to integrate
## geodesic equation
#DEFINES += SHOW_CALC_TIME


## Show status tips
#DEFINES += SHOW_STATUS_TIPS


## Use Dormand-Prince integrators
## (experimental)
#DEFINES += USE_DP_INT


## If you have LUA available, you can
## uncomment the following lines
## (experimental)
#LUA_DIR     = /usr/local/lua/5.3
#LUA_LIB_DIR = $$LUA_DIR/lib
#M4D_LUA_DIR = $$M4D_DIR/src/lua

#INCLUDEPATH += $$LUA_DIR/include
#DEFINES += HAVE_LUA
#LIBS += -L$$LUA_LIB_DIR -llua -ldl

#HEADERS += $$M4D_LUA_HEADERS
#SOURCES += $$M4D_LUA_SOURCES


## DO NOT CHANGE THIS LINE
include( gviewer_m4d.pri )

