if(BUILD_CLIENT)
  add_library(xsonrpc SHARED
    client.cpp
  )
endif(BUILD_CLIENT)

add_library(xsonrpc SHARED
  dispatcher.cpp
  jsonformathandler.cpp
  jsonreader.cpp
  jsonwriter.cpp
  request.cpp
  response.cpp
  server.cpp
  util.cpp
  value.cpp
)

install(TARGETS xsonrpc LIBRARY DESTINATION "lib")

target_include_directories(xsonrpc BEFORE PRIVATE
  "${PROJECT_SOURCE_DIR}/include/xsonrpc")

# TinyXML-2
# target_include_directories(xsonrpc PRIVATE ${TINYXML2_INCLUDE_DIR})
# if (TINYXML2_FOUND)
#   target_link_libraries(xsonrpc ${TINYXML2_LIBRARY})
# endif()

# RapidJSON
target_include_directories(xsonrpc PRIVATE
  "${PROJECT_SOURCE_DIR}/3pp/rapidjson/include")

# target_include_directories(xsonrpc PRIVATE
#   "${PROJECT_SOURCE_DIR}/3pp/picojson")

# libmicrohttpd
target_include_directories(xsonrpc PRIVATE ${MICROHTTPD_INCLUDE_DIR})
target_link_libraries(xsonrpc ${MICROHTTPD_LIBRARY})

if(BUILD_CLIENT)
  # cURL
  target_include_directories(xsonrpc PRIVATE ${CURL_INCLUDE_DIR})
  target_link_libraries(xsonrpc ${CURL_LIBRARY})
endif(BUILD_CLIENT)


# Version
target_compile_definitions(xsonrpc PRIVATE
  -DXSONRPC_VERSION="${XSONRPC_VERSION}")
set_target_properties(xsonrpc PROPERTIES VERSION ${XSONRPC_VERSION})
