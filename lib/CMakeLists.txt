set(NAN3_SOURCES
    reflection.cpp)

# Create a library corresponding to 'plugin'.
add_llvm_library(nan3 MODULE ${NAN3_SOURCES} PLUGIN_TOOL clang)

# Configure include directories for 'plugin'.
target_include_directories(nan3 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")

target_link_libraries(nan3
    PRIVATE
        clangAST
        clangBasic
        clangFrontend
        clangLex
        LLVMSupport
        # On Darwin (unlike on Linux), undefined symbols in shared objects are not
        # allowed at the end of the link-edit. The plugins defined here:
        #  - _are_ shared objects
        #  - reference symbols from LLVM shared libraries, i.e. symbols which are
        #    undefined until those shared objects are loaded in memory (and hence
        #    _undefined_ during static linking)
        # The build will fail with errors like this:
        #    "Undefined symbols for architecture x86_64"
        # with various LLVM symbols being undefined. Since those symbols are later
        # loaded and resolved at runtime, these errors are false positives.
        # This behaviour can be modified via the '-undefined' OS X linker flag as
        # follows.
        "$<$<PLATFORM_ID:Darwin>:-undefined dynamic_lookup>"
)