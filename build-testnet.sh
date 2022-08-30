cd build
cmake ..
make
cd awmarket
cleos wallet open -n awnftmakette &&
cleos wallet unlock -n awnftmakette --password PW5KRBc1eqeFSBbUNGzjYLNbx1W8Ag189kAz1nWzvm8FDDdDnshoE &&
cleos -u https://testnet.wax.pink.gg set contract awnftmakette $(pwd) awmarket.wasm awmarket.abi