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
add_library(wcheat::source::mwin32 ALIAS mwin32)