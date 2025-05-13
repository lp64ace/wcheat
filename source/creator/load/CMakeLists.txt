# -----------------------------------------------------------------------------
# Define Include Directories

set(INC
	# Internal Include Directories
	PUBLIC .
	
	# External Include Directories
	
)

# -----------------------------------------------------------------------------
# Define Source Files

set(SRC
	main.c
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

add_executable(load "${SRC}")

wcheat_target_link_libraries(load "${LIB}")
wcheat_target_include_dirs(load "${INC}")