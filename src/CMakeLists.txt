configure_file(config.h.in config.h)

add_executable(GoLpp golwin.cpp golarea.cpp main.cpp)

target_include_directories(GoLpp PUBLIC "${PROJECT_BINARY_DIR}" ${GTKMM_INCLUDE_DIRS})

target_link_libraries(GoLpp Threads::Threads ${GTKMM_LIBRARIES})

install(TARGETS GoLpp DESTINATION bin)

