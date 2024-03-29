cmake_minimum_required(VERSION 3.5.0)
project(quail-server VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEPS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(PACKAGE_PATH ${DEPS_PATH}/package)
set(CMAKE_PREFIX_PATH ${DEPS_PATH})
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # -fPIC

set(QUICHE_PATH ${PACKAGE_PATH}/quiche/)

add_subdirectory(${PACKAGE_PATH}/googleurl)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL CONFIG REQUIRED)
find_package(protobuf CONFIG REQUIRED)

# proto
set(PROTO_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${PROTO_BINARY_DIR})

function(addProtobufLib libname protopath importdir)
  message(addProtobufLib: ${libname} ${protopath})

  add_library(${libname} OBJECT ${protopath})
  target_link_libraries(${libname} PUBLIC protobuf::libprotobuf)

  target_include_directories(${libname} PUBLIC $<BUILD_INTERFACE:${PROTO_BINARY_DIR}>)
    
  protobuf_generate(
      TARGET ${libname}
      IMPORT_DIRS ${importdir}
      PROTOC_OUT_DIR ${PROTO_BINARY_DIR})
  
endfunction()

function(jsonGet inputJson inputKey outputList)
    if(ARGN)
        set(prefixPath ${ARGN})
    endif(ARGN)
    
    string(JSON inputArray GET ${inputJson} ${inputKey})
    string(JSON length LENGTH ${inputArray})
    math(EXPR length "${length} - 1")
    foreach(IDX RANGE "${length}")
        string(JSON element GET ${inputArray} ${IDX})
        set(${outputList}  ${${outputList}} "${prefixPath}${element}")
        # message("Element: ${IDX} ${element}")
    endforeach()
    set(${outputList} ${${outputList}} PARENT_SCOPE)
endfunction()


##

addProtobufLib(cached_network_parameters_proto_lib ${QUICHE_PATH}quiche/quic/core/proto/cached_network_parameters.proto ${QUICHE_PATH})
addProtobufLib(source_address_token_proto_lib ${QUICHE_PATH}quiche/quic/core/proto/source_address_token.proto ${QUICHE_PATH})
addProtobufLib(crypto_server_config_proto_lib ${QUICHE_PATH}quiche/quic/core/proto/crypto_server_config.proto ${QUICHE_PATH})
addProtobufLib(quic_trace_proto_lib ${PACKAGE_PATH}/quic-trace/quic_trace/quic_trace.proto ${PACKAGE_PATH}/quic-trace)


##

file(READ ${QUICHE_PATH}build/source_list.json SOURCE_LIST_JSON)

set(quiche_core_hdrs "")
jsonGet(${SOURCE_LIST_JSON} "quiche_core_hdrs" quiche_core_hdrs ${QUICHE_PATH})

set(quiche_core_srcs "")
jsonGet(${SOURCE_LIST_JSON} "quiche_core_srcs" quiche_core_srcs ${QUICHE_PATH})

set(quiche_tool_support_hdrs "")
jsonGet(${SOURCE_LIST_JSON} "quiche_tool_support_hdrs" quiche_tool_support_hdrs ${QUICHE_PATH})

set(quiche_tool_support_srcs "")
jsonGet(${SOURCE_LIST_JSON} "quiche_tool_support_srcs" quiche_tool_support_srcs ${QUICHE_PATH})

set(io_tool_support_hdrs "")
jsonGet(${SOURCE_LIST_JSON} "io_tool_support_hdrs" io_tool_support_hdrs ${QUICHE_PATH})

set(io_tool_support_srcs "")
jsonGet(${SOURCE_LIST_JSON} "io_tool_support_srcs" io_tool_support_srcs ${QUICHE_PATH})

set(default_platform_impl_tool_support_hdrs "")
jsonGet(${SOURCE_LIST_JSON} "default_platform_impl_tool_support_hdrs" default_platform_impl_tool_support_hdrs ${QUICHE_PATH})

set(default_platform_impl_tool_support_srcs "")
jsonGet(${SOURCE_LIST_JSON} "default_platform_impl_tool_support_srcs" default_platform_impl_tool_support_srcs ${QUICHE_PATH})


set(default_platform_impl_hdrs 
  "default/quiche_platform_impl/quiche_bug_tracker_impl.h"
  "default/quiche_platform_impl/quiche_client_stats_impl.h"
  "default/quiche_platform_impl/quiche_containers_impl.h"
  "default/quiche_platform_impl/quiche_event_loop_impl.h"
  "default/quiche_platform_impl/quiche_export_impl.h"
  "default/quiche_platform_impl/quiche_flag_utils_impl.h"
  "default/quiche_platform_impl/quiche_flags_impl.h"
  "default/quiche_platform_impl/quiche_header_policy_impl.h"
  "default/quiche_platform_impl/quiche_iovec_impl.h"
  "default/quiche_platform_impl/quiche_logging_impl.h" # modified
  "default/quiche_platform_impl/quiche_lower_case_string_impl.h"
  "default/quiche_platform_impl/quiche_mem_slice_impl.h"
  "default/quiche_platform_impl/quiche_mutex_impl.h"
  "default/quiche_platform_impl/quiche_prefetch_impl.h"
  "default/quiche_platform_impl/quiche_reference_counted_impl.h"
  "default/quiche_platform_impl/quiche_server_stats_impl.h"
  "default/quiche_platform_impl/quiche_stack_trace_impl.h"
  "default/quiche_platform_impl/quiche_testvalue_impl.h"
  "default/quiche_platform_impl/quiche_time_utils_impl.h"
  "default/quiche_platform_impl/quiche_udp_socket_platform_impl.h"
  "default/quiche_platform_impl/quiche_url_utils_impl.h"
)

set(default_platform_impl_srcs 
  "default/quiche_platform_impl/quiche_logging_impl.cc" # modified
  "default/quiche_platform_impl/quiche_flags_impl.cc"
  "default/quiche_platform_impl/quiche_mutex_impl.cc"
  "default/quiche_platform_impl/quiche_stack_trace_impl.cc"
  "default/quiche_platform_impl/quiche_time_utils_impl.cc"
  "default/quiche_platform_impl/quiche_url_utils_impl.cc"
)

##

## for quiche
include_directories(
  ${QUICHE_PATH}
  default
)


##

add_library(quiche_platform_default_tools
  ${default_platform_impl_tool_support_hdrs}
  ${default_platform_impl_tool_support_srcs}
)

target_link_libraries(quiche_platform_default_tools PUBLIC
  quiche_core)

##
add_library(quiche_platform_default
  ${default_platform_impl_hdrs}
  ${default_platform_impl_srcs}
)

target_link_libraries(quiche_platform_default
  PUBLIC
  absl::synchronization
  googleurl
)

##

add_library(quiche_tool_support
  ${quiche_tool_support_hdrs} ${quiche_tool_support_srcs}
)

target_link_libraries(quiche_tool_support PUBLIC 
quiche_platform_default_tools
)


##

add_library(io_tool_support
  ${io_tool_support_hdrs}
  ${io_tool_support_srcs}
)

target_link_libraries(io_tool_support PUBLIC
  quiche_tool_support
  quiche_platform_default_tools

  OpenSSL::Crypto
  absl::strings
)

##

add_library(quiche_core
  ${quiche_core_hdrs} ${quiche_core_srcs}
)

pkg_check_modules(ZLIB REQUIRED IMPORTED_TARGET zlib)

target_link_libraries(quiche_core PUBLIC 
  quiche_platform_default

  cached_network_parameters_proto_lib
  crypto_server_config_proto_lib
  source_address_token_proto_lib
  quic_trace_proto_lib
  
  OpenSSL::Crypto OpenSSL::SSL PkgConfig::ZLIB
)

# message(STATUS ${quiche_core_hdrs})
# set(B_list "")
# jsonGet(${MY_JSON_STRING} "b" B_list)



add_library(${PROJECT_NAME} SHARED
    src/quit_converter.h
    src/quit_converter.cc
  
    src/api/quail_server.h
    src/api/quail_server.cc
    src/api/quail_transport.h
    src/api/quail_transport.cc
    src/api/quail_transport_impl.h
    src/api/quail_transport_impl.cc

    src/core/quail_server_backend.h
    src/core/quail_server_backend.cc

    src/impl/quail_server_impl.h
    src/impl/quail_server_impl.cc
    src/impl/quail_proof_providers_impl.h
    src/impl/quail_proof_providers_impl.cc

    src/api/quit_transport_stream.h
    src/api/quit_transport_stream.cc

    src/quit_server.h
    src/quit_server.cc
    src/quit_dispatcher.h
    src/quit_dispatcher.cc
    src/quit_server_session.h
    src/quit_server_session.cc
    src/quit_server_stream.h
    src/quit_server_stream.cc
    src/quit_web_transport_visitors.h)

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  io_tool_support # ok
  quiche_platform_default
  quiche_platform_default_tools
  quiche_tool_support)


target_include_directories(${PROJECT_NAME}
  PUBLIC
  src
)


