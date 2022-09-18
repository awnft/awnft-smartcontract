cd build
cmake ..
make
cd awmarket
cleos wallet open -n awmarketmain &&
cleos wallet unlock -n awmarketmain --password PW5KGTzZg91s9eGUdsYCaNajJtrZUGrqH51yyijMnnQZqh6DfXomo &&
# cleos wallet unlock -n awmarketmain --password PW5KFmeioziq7bWEaJ8z651Hu1333hW9tPJwDzuHAnKHKqyKgKLwR &&
cleos -u https://wax.greymass.com set contract awmarketmain $(pwd) awmarket.wasm awmarket.abi