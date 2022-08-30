project(awmarket)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( awmarket awmarket awmarket.cpp )
target_include_directories( awmarket PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( awmarket ${CMAKE_SOURCE_DIR}/../ricardian )