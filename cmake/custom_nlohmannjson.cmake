# Download Niels Lohmann's JSON single header at configure time
file(DOWNLOAD
	https://raw.githubusercontent.com/nlohmann/json/v3.9.1/single_include/nlohmann/json.hpp
	${CMAKE_BINARY_DIR}/nlohmannjson-src/include/nlohmann/json.hpp
	STATUS result)

list(GET result 0 result_code)
if(result_code)
	message(FATAL_ERROR "Cannot download nlohmannjson header file")
else()
	target_include_directories(${NCPROJECT_EXE_NAME} PRIVATE ${CMAKE_BINARY_DIR}/nlohmannjson-src/include)
endif()
