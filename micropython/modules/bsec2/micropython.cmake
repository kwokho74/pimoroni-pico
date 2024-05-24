# Create an INTERFACE library for our C module.
add_library(usermod_bsec2 INTERFACE)

# Add our source files to the lib
target_sources(usermod_bsec2 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/bsec_wrapper.c
)

# Add the current directory as an include directory.
target_include_directories(usermod_bsec2 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Define an imported static library target named "algobsec".
# This allows CMake to know about the external precompiled library.
add_library(algobsec STATIC IMPORTED)

# Set the properties for the imported library target, specifically the location of the precompiled library file.
# ${CMAKE_CURRENT_LIST_DIR} is the directory containing this CMake file, ensuring the correct path to the library.
set_target_properties(algobsec PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/libalgobsec.a)

# Link the usermod target with the INTERFACE library and the imported static library.
# This ensures that when building the firmware, both the usermod_bsec2 sources and the algobsec library are included.
target_link_libraries(usermod INTERFACE usermod_bsec2 algobsec)