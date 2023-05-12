FILE(
    GLOB_RECURSE SRC_LIST 
    ./*.c
)

# Exe output path
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR/bin})

ADD_EXECUTABLE(screenshot ${SRC_LIST})

TARGET_LINK_LIBRARIES(
    screenshot 
    libpng.so;
)