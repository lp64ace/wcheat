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
	datatoc.c
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
# Declare Executable

add_executable(datatoc ${SRC})
add_executable(wcheat::source::datatoc ALIAS datatoc)

wcheat_target_link_libraries(datatoc "${LIB}")
wcheat_target_include_dirs(datatoc "${INC}")
