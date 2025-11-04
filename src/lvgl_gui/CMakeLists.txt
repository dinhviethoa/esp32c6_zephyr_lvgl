file(GLOB SOURCES
    screens/*.c
    components/*.c
    ui.c
    ui_helpers.c
)
SET(INCLUDES .
    screens
    )

target_include_directories(app PRIVATE ${INCLUDES})
target_sources(app PRIVATE ${SOURCES})
