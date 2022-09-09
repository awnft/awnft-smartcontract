cleos wallet open -n awmarketmain &&
# cleos wallet unlock -n awmarketmain --password PW5KGTzZg91s9eGUdsYCaNajJtrZUGrqH51yyijMnnQZqh6DfXomo &&
cleos wallet unlock -n awmarketmain --password PW5KFmeioziq7bWEaJ8z651Hu1333hW9tPJwDzuHAnKHKqyKgKLwR &&
cleos -u https://wax.greymass.com push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "awmarketmain",
      "name": "openmarket",
      "data": {
        "base_con": "alien.worlds",
        "base_sym": "4,TLM",
        "quote_t_id": 19553,
        "quote_con": "atomicassets",
        "min_buy": 1,
        "min_sell": "0.0001 TLM",
        "fee": 0
      },
      "authorization": [
        {
          "actor": "awmarketmain",
          "permission": "owner"
        }
      ]
    }
  ]
}'