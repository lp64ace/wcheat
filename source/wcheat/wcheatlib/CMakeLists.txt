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
	PUBLIC ${DETOURS_INCLUDE_DIRS}
	PUBLIC ${PTHREADS_INCLUDE_DIRS}
	
)

# -----------------------------------------------------------------------------
# Define Source Files

set(SRC
	LIB_assert.h
	LIB_hex.h
	LIB_listbase.h
	LIB_memory.h
	LIB_memory.hh
	LIB_string.h
	LIB_sys_types.h
	LIB_utildefines.h
	LIB_utildefines_variadic.h
	
	algorithm/rabin_karp.c
	algorithm/rabin_karp.h
	
	intern/assert.c
	intern/hex.c
	intern/listbase.c
	intern/memory.c
	intern/memory.cc
	intern/string.c
	intern/utildefines.c
)

# -----------------------------------------------------------------------------
# Define Library Dependencies

set(LIB
	# Internal Library Dependencies
	wcheat::intern::guardedalloc
	
	# External Library Dependencies
	PUBLIC ${PTHREADS_LIBRARIES}
	PUBLIC ${DETOURS_LIBRARIES}
	
)

# -----------------------------------------------------------------------------
# Declare Library

wcheat_add_lib(wcheatlib "${SRC}" "${INC}" "${INC_SYS}" "${LIB}")
add_library(wcheat::source::wcheatlib ALIAS wcheatlib)

# -----------------------------------------------------------------------------
# Define Include Directories (Test)

set(INC
	# Internal Include Directories
	PUBLIC .
	
	# External Include Directories
	
)

# -----------------------------------------------------------------------------
# Define System Include Directories (Test)

set(INC_SYS
	# External System Include Directories
	${PTHREADS_INCLUDE_DIRS}
	${DETOURS_INCLUDE_DIRS}
	
)

# -----------------------------------------------------------------------------
# Define Source Files (Test)

set(TEST
	test/listbase.cc
	test/memory.cc
	test/string.cc
)

# -----------------------------------------------------------------------------
# Define Library Dependencies (Test)

set(LIB
	# Internal Library Dependencies
	wcheat::intern::guardedalloc
	wcheat::source::wcheatlib
	
	# External Library Dependencies
	${PTHREADS_LIBRARIES}
	${DETOURS_LIBRARIES}
	
)

# -----------------------------------------------------------------------------
# Declare Test

wcheat_add_test_executable(wcheatlib "${TEST}" "${INC}" "${INC_SYS}" "${LIB}")
