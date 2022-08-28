project(awmarketplace_smartcontract)

set(EOSIO_WASM_OLD_BEHAVIOR "Off")
find_package(eosio.cdt)

add_contract( awmarketplace_smartcontract awmarketplace_smartcontract awmarketplace_smartcontract.cpp )
target_include_directories( awmarketplace_smartcontract PUBLIC ${CMAKE_SOURCE_DIR}/../include )
target_ricardian_directory( awmarketplace_smartcontract ${CMAKE_SOURCE_DIR}/../ricardian )