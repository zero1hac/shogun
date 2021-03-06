# add test case for each generated example
# (not generated yet so have to fake filenames from META_EXAMPLES list)
FOREACH(META_EXAMPLE ${META_EXAMPLES})
    # assume a structure <target_language>/<category>/listing.sg
	get_filename_component(EXAMPLE_NAME ${META_EXAMPLE} NAME_WE)
	IF(${CMAKE_VERSION} VERSION_LESS 2.8.12)
	    get_filename_component(FULL_DIR ${META_EXAMPLE} PATH)
	ELSE()
	    # only supported from >2.8.11
	    get_filename_component(FULL_DIR ${META_EXAMPLE} DIRECTORY)
	ENDIF()
	get_filename_component(EXAMPLE_REL_DIR ${FULL_DIR} NAME)
	set(EXAMPLE_NAME_WITH_DIR "${EXAMPLE_REL_DIR}-${EXAMPLE_NAME}")

    # meta examples have to be generated before executable and test is added
    SET(GENERATED_CPP ${EXAMPLE_REL_DIR}/${EXAMPLE_NAME}.cpp)
    SET(GENERATED_CPP_BINARY ${EXAMPLE_NAME})

    # generate binary in relative directory
    SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXAMPLE_REL_DIR})

    ADD_EXECUTABLE(${GENERATED_CPP_BINARY} ${GENERATED_CPP})
    ADD_DEPENDENCIES(${GENERATED_CPP_BINARY} meta_examples shogun)
    SET_SOURCE_FILES_PROPERTIES(${GENERATED_CPP} PROPERTIES GENERATED 1)
    TARGET_LINK_LIBRARIES(${GENERATED_CPP_BINARY} shogun ${SANITIZER_LIBRARY})
    IF(SANITIZER_FLAGS)
        SET_TARGET_PROPERTIES(${GENERATED_CPP_BINARY} PROPERTIES COMPILE_FLAGS ${SANITIZER_FLAGS})
    ENDIF()
	# Add examples to the dependencies of modular interfaces to make sure
	# nothing will infer with them being build single-threaded.
	IF(SWIG_SINGLE_THREADED)
		FOREACH(SG_MODULAR_INTERFACE_TARGET ${SG_MODULAR_INTERFACE_TARGETS})
			ADD_DEPENDENCIES(${SG_MODULAR_INTERFACE_TARGET} ${GENERATED_CPP_BINARY})
		ENDFOREACH(SG_MODULAR_INTERFACE_TARGET ${SG_MODULAR_INTERFACE_TARGETS})
	ENDIF(SWIG_SINGLE_THREADED)

    LIST(APPEND GENERATED_CPP_EXAMPLES ${GENERATED_CPP_BINARY})

    ADD_TEST(NAME generated_cpp-${EXAMPLE_NAME_WITH_DIR}
             COMMAND ${GENERATED_CPP_BINARY}
             WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE_REL_DIR})

ENDFOREACH()

add_custom_target(build_cpp_meta_examples ALL
    DEPENDS ${GENERATED_CPP_EXAMPLES}
    COMMENT "Compiled generated cpp examples")
