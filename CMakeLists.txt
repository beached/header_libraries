cmake_minimum_required( VERSION 3.12 ) 

set( DAW_HEADER_LIBRARIES_VERSION 1.0.0 )
set( DAW_HEADER_LIBRARIES_ABI_VERSION 1 )

project( "daw_header_libraries"
	VERSION "${DAW_HEADER_LIBRARIES_VERSION}" 
	DESCRIPTION "Various headers"
	HOMEPAGE_URL "http://github.com/beached/header_libraries"
	LANGUAGES C CXX)

find_package( Threads )

option( DAW_ENABLE_TESTING "Build unit tests"  OFF )

if( DAW_ENABLE_TESTING )
	enable_testing( )
	add_subdirectory( tests )
endif( )	

set(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
include(GNUInstallDirs)


add_library( ${PROJECT_NAME} INTERFACE ) 
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_NAME}
	INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION
  ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                 VERSION ${PROJECT_VERSION}
                                 COMPATIBILITY SameMajorVersion)

install(EXPORT ${PROJECT_NAME}_Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE daw::
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include DESTINATION include)
			
