# Provide installation directories


if (NOT INSTALLATION_PREFIX)
    set(INSTALLATION_PREFIX ${CMAKE_INSTALL_PREFIX})  # default to /usr/local
endif()
set(PROJECT_INSTALL_DIR ${INSTALLATION_PREFIX}/${PROJECT_NAME_LOWERCASE})

set(INCLUDE_INSTALL_DIR ${PROJECT_INSTALL_DIR}/include)
set(CMAKE_INSTALL_DIR ${PROJECT_INSTALL_DIR}/cmake)
set(LIBRARY_INSTALL_DIR ${PROJECT_INSTALL_DIR}/lib)
set(BIN_INSTALL_DIR ${PROJECT_INSTALL_DIR}/bin)
