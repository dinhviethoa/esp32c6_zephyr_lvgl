file(GLOB SOURCES
    led_strip.c
)
SET(INCLUDES . 
    )

target_include_directories(app PRIVATE ${INCLUDES})
target_sources(app PRIVATE ${SOURCES})
