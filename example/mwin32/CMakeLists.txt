# -----------------------------------------------------------------------------
# Define Include Directories

set(INC
	# Internal Include Directories
	PUBLIC .
	
	# External Include Directories
	
)

# -----------------------------------------------------------------------------
# Define System Include Directories

set(INC_SYS
	# External System Include Directories
	
)

# -----------------------------------------------------------------------------
# Define Source Files

set(SRC
	main.cc
	
	intern/Log.c
	intern/Log.h

	detour/CNetStream.cc
	detour/CNetStream.hh
	detour/CPythonChat.cc
	detour/CPythonChat.hh
	detour/CPythonEventManager.cc
	detour/CPythonEventManager.hh
	detour/CPythonNetworkStream.cc
	detour/CPythonNetworkStream.hh
	detour/IAbstractPlayer.cc
	detour/IAbstractPlayer.hh
)

# -----------------------------------------------------------------------------
# Define Library Dependencies

set(LIB
	# Internal Library Dependencies
	wcheat::intern::guardedalloc
	wcheat::source::wcheatlib
	
	# External Library Dependencies
	
)

# -----------------------------------------------------------------------------
# Declare Library

wcheat_add_dll(mwin32 "${SRC}" "${INC}" "${INC_SYS}" "${LIB}")
add_library(wcheat::example::mwin32 ALIAS mwin32)

# -----------------------------------------------------------------------------
# DLL Memory

get_filename_component(MWIN32_SRC ${CMAKE_CURRENT_BINARY_DIR}/mwin32.dll.c REALPATH)
get_filename_component(MWIN32_PATH ${MWIN32_SRC} PATH)

add_custom_command(
	OUTPUT
		${MWIN32_SRC}
	COMMAND
		${CMAKE_COMMAND} -E make_directory ${MWIN32_PATH}
	COMMAND
		"$<TARGET_FILE:datatoc>" --src "$<TARGET_FILE:mwin32>" --bin ${MWIN32_SRC}
	DEPENDS
		$<TARGET_FILE:mwin32>
		wcheat::source::datatoc
)

# -----------------------------------------------------------------------------
# Define Include Directories

set(INC
	# Internal Include Directories
	PUBLIC .
	
	# External Include Directories
	
)

# -----------------------------------------------------------------------------
# Define System Include Directories

set(INC_SYS
	# External System Include Directories
	
)

# -----------------------------------------------------------------------------
# Define Source Files

set(SRC
	${MWIN32_SRC}

	api.cc

	intern/Log.c
	intern/Log.h
)

# -----------------------------------------------------------------------------
# Define Library Dependencies

set(LIB
	# Internal Library Dependencies
	wcheat::intern::guardedalloc
	wcheat::source::wcheatlib
	
	# External Library Dependencies
	
)

# -----------------------------------------------------------------------------
# Declare Library

add_executable(mwin32api ${SRC})

wcheat_target_link_libraries(mwin32api "${LIB}")
wcheat_target_include_dirs(mwin32api "${INC}")
wcheat_project_group(mwin32api "${SRC}")
