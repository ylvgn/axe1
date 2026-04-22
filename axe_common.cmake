include_guard(GLOBAL)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(axe_log)
	message(${ARGN})
endfunction()

function(axe_log_warning)
	message(WARNING ${ARGN}"warning: file ${CMAKE_CURRENT_LIST_FILE}, line ${CMAKE_CURRENT_LIST_LINE}")
endfunction()

function(axe_log_error)
	message(FATAL_ERROR ${ARGN}"error: file ${CMAKE_CURRENT_LIST_FILE}, line ${CMAKE_CURRENT_LIST_LINE}")
endfunction()

function(axe_error_if_not_exist)
	if (NOT ${ARGC} EQUAL 1)
		axe_log_warning("only allow 1 parameter")
	endif()

	set(KEY ${ARGV0})
	set(VALUE ${${KEY}})
	if (NOT EXISTS ${VALUE})
		axe_log_error("path not found: ${KEY}=[${VALUE}]")
	endif()
endfunction()

function(axe_dump_var)
	foreach(i RANGE ${ARGC})
		if (i EQUAL ${ARGC})
			break()
		endif()
		set(KEY ${ARGV${i}})
		set(VALUE ${${KEY}})
		axe_log(STATUS "DUMP_VAR: ${KEY} = [${VALUE}]") # 'STATUS' is compatible for list variable!!
	endforeach()
endfunction()

function(axe_dump_cmake_variables)
# https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        axe_log(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endfunction()

function(axe_write_to_file filename text)
#	axe_log("axe_write_to_file(\"${filename}\")")
	file(GENERATE OUTPUT ${filename} CONTENT "${text}" NEWLINE_STYLE UNIX)	
endfunction()

function(axe_basename out_var_name in_path)
	get_filename_component(basename ${in_path} NAME)
	set(${out_var_name} ${basename} PARENT_SCOPE)
endfunction()

function(axe_string_split out_var_name in_str in_delimiter)
	string(REPLACE ${in_delimiter} ";" out_list ${in_str})
	set(${out_var_name} ${out_list} PARENT_SCOPE)
endfunction()

function(axe_list_at out_var_name in_list index)
# e.g. axe_list_at(out_result my_str_list 1) --> Aware NOT Use This: axe_list_at(out_result ${my_str_list} 1)
	list(GET ${in_list} ${index} out_element)
	set(${out_var_name} ${out_element} PARENT_SCOPE)
endfunction()

function(axe_dirname out_var_name in_path)
	get_filename_component(out_dirname ${in_path} PATH)
	set(${out_var_name} ${out_dirname} PARENT_SCOPE)
endfunction()

function(axe_realpath out_var_name in_path)
	get_filename_component(out_dirname ${in_path} ABSOLUTE)
	set(${out_var_name} ${out_dirname} PARENT_SCOPE)
endfunction()

function(axe_output_copy_dir in_output_name in_src_dir in_dst_dir)
	add_custom_command(
		OUTPUT ${in_output_name}
		COMMAND ${CMAKE_COMMAND} -E copy_directory ${in_src_dir} ${in_dst_dir}
    )
endfunction()

function(axe_output_copy_files in_output_name)
	if (${ARGC} LESS 2)
		axe_log_error(
			"'${in_output_name}' axe_output_copy_files error: without provide <file>... <destination>"
			"\nsyntax:\n\axe_output_copy_files(<output> <file>... <destination>)\n"
			"\nexample:\n\axe_output_copy_files(${in_output_name} foo1.txt foo2.txt bar/my_dst)\n"
		)
	endif()
	add_custom_command(
		OUTPUT ${in_output_name}
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN}
    )
endfunction()

function(axe_output_mkdir in_output_name in_dir)
	add_custom_target(
		OUTPUT ${in_output_name}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${in_dir}
	)
endfunction()

function(axe_add_cmd_post_build target_name)
	add_custom_command(
		TARGET ${target_name} POST_BUILD
		COMMAND ${ARGN}
	)
endfunction()

function(axe_copy_files_post_build target_name)
#	axe_dump_ARGN(${ARGN})
    if (${ARGC} LESS 2)
		axe_log_error(
            "'${target_name}' axe_copy_files_post_build error: without provide <file>... <destination>"
            "\nsyntax:\n\axe_copy_files_post_build(<target> <file>... <destination>)\n"
            "\nexample:\n\axe_copy_files_post_build(${target_name} foo1.txt foo2.txt bar/my_dst)\n"
        )
    endif()
    # add_custom_command(TARGET ${target_name} POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN}
    # )
	axe_add_cmd_post_build(${target_name} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ARGN})
endfunction()

function(axe_copy_dir_post_build target_name src_dir dst_dir)
	add_custom_command(
		TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${src_dir} ${dst_dir}
    )
endfunction()

function(axe_source_group src_path src_files)
	foreach(FILE ${src_files})
		axe_dirname(PARENT_DIR ${FILE}) #get_filename_component(PARENT_DIR "${FILE}" PATH)
		file(RELATIVE_PATH PARENT_DIR ${src_path} ${PARENT_DIR})
		string(REPLACE "/" "\\" GROUP "${PARENT_DIR}")
		set(GROUP "${GROUP}")
		source_group("${GROUP}" FILES "${FILE}")
	endforeach()
endfunction()

function(axe_set_warning_level target_name)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
		# message("axe_set_warning_level MSVC")
		
		# DynamicDebugging
		if(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
			target_link_options(${target_name} PRIVATE /dynamicdeopt)
			target_link_options(${target_name} PRIVATE /INCREMENTAL:NO)
		endif()		
		
		# target_compile_options(${target_name} PRIVATE /Bt)	# show Build Timing
		target_compile_options(${target_name} PRIVATE /utf-8) 	# execution_character_set - otherwise std::format wouldn't check format in compile time for "char"
		target_compile_options(${target_name} PRIVATE /WX)    	#warning treated as error
		target_compile_options(${target_name} PRIVATE /W4)   	#warning level 4
#		target_compile_options(${target_name} PRIVATE /Wall)   	#warning level all
		target_compile_options(${target_name} PRIVATE /we6244)  #warning C6244: local declaration of <variable> hides previous declaration
		target_compile_options(${target_name} PRIVATE /we6246)  #warning C6246: Local declaration of <variable> hides declaration of same name in outer scope
#		target_compile_options(${target_name} PRIVATE /Zp16)    # struct alignment
		
		# re-enable warning disabled by default to level 4
		# VS2017 or later
		target_compile_options(${target_name} PRIVATE /w45038)  # data member 'member1' will be initialized after data member 'member2'
		target_compile_options(${target_name} PRIVATE /w45039)  # 'function': pointer or reference to potentially throwing function passed to extern C function under -EHc. Undefined behavior may occur if this function throws an exception.
		target_compile_options(${target_name} PRIVATE /w45041)  # 'member-name': out-of-line definition for constexpr static data member is not needed and is deprecated in C++17
		target_compile_options(${target_name} PRIVATE /w45042)  # 'function': function declarations at block scope cannot be specified 'inline' in standard C++; remove 'inline' specifier 
#		target_compile_options(${target_name} PRIVATE /w45045)  # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

		# VS2019 or later
		target_compile_options(${target_name} PRIVATE /w45052)  # Keyword 'keyword-name' was introduced in C++ version and requires use of the 'option' command-line option
		target_compile_options(${target_name} PRIVATE /w45204)  # A class with virtual functions has non-virtual trivial destructor
		target_compile_options(${target_name} PRIVATE /w45214)  # applying 'keyword' to an operand with a volatile qualified type is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45215)  # 'function-parameter' a function parameter with a volatile qualified type is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45216)  # 'return-type' a volatile qualified return type is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45217)  # a structured binding declaration that includes volatile is deprecated in C++20
		target_compile_options(${target_name} PRIVATE /w45219)  # implicit conversion from 'type-1' to 'type-2', possible loss of data
		target_compile_options(${target_name} PRIVATE /w45220)  # 'member': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial
		target_compile_options(${target_name} PRIVATE /w45233)  # explicit lambda capture 'identifier' is not used
		target_compile_options(${target_name} PRIVATE /w45240)  # attribute-name': attribute is ignored in this syntactic position  
		target_compile_options(${target_name} PRIVATE /w45243)  # 'type-name': using incomplete class 'class-name' can cause potential one definition rule violation due to ABI limitation
		target_compile_options(${target_name} PRIVATE /w45245)  # 'function': unreferenced function with internal linkage has been removed
		target_compile_options(${target_name} PRIVATE /w45246)  # 'member': the initialization of a subobject should be wrapped in braces
		target_compile_options(${target_name} PRIVATE /w45247)  # Section 'section-name' is reserved for C++ dynamic initialization. Manually creating the section will interfere with C++ dynamic initialization and may lead to undefined behavior
		target_compile_options(${target_name} PRIVATE /w45248)  # Section 'section-name' is reserved for C++ dynamic initialization. Variable manually put into the section may be optimized out and its order relative to compiler generated dynamic initializers is unspecified
		
		# VS2022 or later
		target_compile_options(${target_name} PRIVATE /w45249)  # 'bitfield' of type 'enumeration_name' has named enumerators with values that cannot be represented in the given bit field width of 'bitfield_width'
		target_compile_options(${target_name} PRIVATE /w45250)  # 'function_name': intrinsic function not declared.
		target_compile_options(${target_name} PRIVATE /w45251)  # segment-name changed after including header 
		target_compile_options(${target_name} PRIVATE /w45254)  # language feature 'terse static assert' requires compiler flag '/std:c++17
		target_compile_options(${target_name} PRIVATE /w45256)  # 'enumeration': a non-defining declaration of an enumeration with a fixed underlying type is only permitted as a standalone declaration
		target_compile_options(${target_name} PRIVATE /w45258)  # explicit capture of 'symbol' is not required for this use
		target_compile_options(${target_name} PRIVATE /w45259)  # 'specialized-type': explicit specialization requires 'template <>'
		target_compile_options(${target_name} PRIVATE /w45262)  # implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]] when a break statement is intentionally omitted between cases
		target_compile_options(${target_name} PRIVATE /w45263)  # calling 'std::move' on a temporary object prevents copy elision
		target_compile_options(${target_name} PRIVATE /w45264)  # 'variable-name': 'const' variable is not used
		target_compile_options(${target_name} PRIVATE /w45266)  # 'const' qualifier on return type has no effect
#		target_compile_options(${target_name} PRIVATE /w45267)  # definition of implicit copy constructor/assignment operator for 'type' is deprecated because it has a user-provided assignment operator/copy constructor

		# disable warning
		target_compile_options(${target_name} PRIVATE /wd4100) #warning C4100: unreferenced formal parameter in function
		target_compile_options(${target_name} PRIVATE /wd4127) #warning C4127: conditional expression is constant
		target_compile_options(${target_name} PRIVATE /wd4201) #warning C4201: nonstandard extension used: nameless struct/union
		target_compile_options(${target_name} PRIVATE /wd4251) #Warning C4251 : needs to have dll-interface to be used by clients
		target_compile_options(${target_name} PRIVATE /wd4275) #warning C4275: non dll-interface class 'std::runtime_error' used as base for dll-interface class 'fmt::v10::format_error'
		target_compile_options(${target_name} PRIVATE /wd4702) #warning C4702: unreachable code (seems vc has bug when handle if constexpr() )
		target_compile_options(${target_name} PRIVATE /wd4714) #Warning C4714 : function marked as __forceinline not inlined
		target_compile_options(${target_name} PRIVATE /wd5072) #warning C5072: Address Sanitizer(ASAN) enabled without debug information emission.
	else()
	  	target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	endif()
endfunction()

function(axe_set_unity_build_mode target_name)
	set_target_properties(${target_name} PROPERTIES
							UNITY_BUILD ON
							UNITY_BUILD_MODE BATCH
							UNITY_BUILD_BATCH_SIZE 16)
endfunction()

function(axe_target_source_from_folder target_name src_path)
	get_target_property(target_type ${target_name} TYPE)

	file(GLOB_RECURSE all_files  "${src_path}/src/*.*")
	file(GLOB_RECURSE h_files    "${src_path}/src/*.h")
	file(GLOB_RECURSE cpp_files  "${src_path}/src/*.cpp")
	
	if(CMAKE_CXX_STANDARD EQUAL 20)
		file(GLOB_RECURSE cppm_files "${src_path}/src/*.cppm")
	endif()

	axe_source_group(${src_path} "${all_files}")

	set(other_files ${all_files})
	list(REMOVE_ITEM other_files ${cpp_files} ${h_files})

	target_sources(${target_name} PRIVATE ${h_files})
	target_sources(${target_name} PRIVATE ${cpp_files})
	target_sources(${target_name} PRIVATE ${other_files})

	if(CMAKE_CXX_STANDARD EQUAL 20)
		if ("${target_type}" STREQUAL "INTERFACE_LIBRARY")
		else()
			target_sources(${target_name} PUBLIC 
				FILE_SET cxx_modules 
				TYPE CXX_MODULES 
				FILES ${cppm_files})
		endif()
	endif()

	if(CMAKE_GENERATOR STREQUAL Xcode)
		set_source_files_properties(${cpp_files} PROPERTIES LANGUAGE OBJCXX)
	endif()

endfunction()

function(axe_target_set_header_only_common_properties target_name)
endfunction()

function(axe_target_set_common_properties target_name)
	axe_target_set_header_only_common_properties(${target_name})

	axe_set_warning_level(${target_name})
	axe_set_unity_build_mode(${target_name})

	target_compile_definitions(${target_name} PUBLIC -DAXE_BUILD_${target_name})
	target_compile_definitions(${target_name} PUBLIC 
		$<$<CONFIG:Debug>:AXE_BUILD_CONFIG_Debug>
		$<$<CONFIG:MinSizeRel>:AXE_BUILD_CONFIG_MinSizeRel>
		$<$<CONFIG:Release>:AXE_BUILD_CONFIG_Release>
		$<$<CONFIG:RelWithDebInfo>:AXE_BUILD_CONFIG_RelWithDebInfo>
	)

	target_precompile_headers(${target_name} PRIVATE src/${target_name}-pch.h)

	target_compile_definitions(${target_name} PUBLIC -DUNICODE -D_UNICODE)
endfunction()

# ----------------------- add build target start ----------------------

function(axe_add_header_only_library target_name src_path)
	add_library(${target_name} INTERFACE)
	axe_target_source_from_folder(${target_name} ${src_path})

	target_include_directories(${target_name} INTERFACE src)
	axe_target_set_header_only_common_properties(${target_name})
endfunction()

function(axe_add_library target_name src_path)
	add_library(${target_name} STATIC)
	axe_target_source_from_folder(${target_name} ${src_path})
	target_include_directories(${target_name} PUBLIC src)
	axe_target_set_common_properties(${target_name})
endfunction()

function(axe_add_dynamic_library target_name src_path)	
	add_library(${target_name} SHARED)
	axe_target_source_from_folder(${target_name} ${src_path})
	target_include_directories(${target_name} PUBLIC src)
	axe_target_set_common_properties(${target_name})
endfunction()

function(axe_add_executable target_name src_path)
	add_executable(${target_name})
	axe_target_source_from_folder(${target_name} ${src_path})
	target_include_directories(${target_name} PRIVATE src)
	axe_target_set_common_properties(${target_name})
endfunction()

function(axe_add_gui_executable target_name src_path)
	add_executable(${target_name} WIN32)
	axe_target_source_from_folder(${target_name} ${src_path})

	set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/src/AppIcon.rc")
	set_target_properties(${target_name} PROPERTIES COMPILE_FLAGS "-D_QWE_GUI_EXECUTABLE")

	target_include_directories(${target_name} PRIVATE src)
	axe_target_set_common_properties(${target_name})
endfunction()
# ----------------------- add build target end ----------------------

# ----------------------- vcpkg helper function start ----------------------
function(axe_vcpkg_find_python out_var_name require_python_version)
    axe_error_if_not_exist(Z_VCPKG_ROOT_DIR)
    set(AXE_VCPKG_TOOLS_PYTHON "${Z_VCPKG_ROOT_DIR}/downloads/tools/python")

    if (NOT DEFINED require_python_version)
        axe_log_error("please set 'require_python_version'")
    endif()

#   axe_dump_var(AXE_VCPKG_TOOLS_PYTHON require_python_version)

    File(GLOB src_files RELATIVE ${AXE_VCPKG_TOOLS_PYTHON} "${AXE_VCPKG_TOOLS_PYTHON}/python-*")

    foreach(FILE ${src_files})
        set(ABSOLUTE_FILE "${AXE_VCPKG_TOOLS_PYTHON}/${FILE}")
        if(NOT IS_DIRECTORY ${ABSOLUTE_FILE})
            continue()
        endif()
        axe_basename(out_basename ${FILE})
        axe_string_split(out_str_list ${out_basename} "-")

#       axe_dump_var(out_str_list)
        axe_list_at(out_py_version out_str_list 1)

#       axe_log("found python: ${out_py_version}")
        axe_string_split(out_str_list ${out_py_version} ".")
        axe_list_at(out_py_major_version out_str_list 0)

        if (${out_py_major_version} STREQUAL ${require_python_version})
            set(${out_var_name} ${ABSOLUTE_FILE} PARENT_SCOPE)
            axe_basename(out_basename ${CMAKE_CURRENT_FUNCTION_LIST_FILE})
#           axe_log("[${out_basename}]: choosed python ${ABSOLUTE_FILE}")
            return()
        endif()
    endforeach()
    axe_log_error("python${require_python_version} not found: AXE_VCPKG_TOOLS_PYTHON=${AXE_VCPKG_TOOLS_PYTHON}")
endfunction()
# ----------------------- vcpkg helper function end ----------------------