QT -= gui

TEMPLATE = app

CONFIG *= cmdline

DEFINES += PROJECT_TOM

SOURCES += $$PWD/main.cpp

# Database migrations
include($$PWD/database/migrations.pri)
# Database seeders
include($$PWD/database/seeders.pri)

# Auto-configure TinyORM library for the migrations purposes 🔥
include($$TINY_MAIN_DIR/TinyORM/qmake/tom.pri)
