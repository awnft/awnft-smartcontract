cleos wallet open -n awmarketmain &&
# cleos wallet unlock -n awmarketmain --password PW5KGTzZg91s9eGUdsYCaNajJtrZUGrqH51yyijMnnQZqh6DfXomo &&
cleos wallet unlock -n awmarketmain --password PW5KFmeioziq7bWEaJ8z651Hu1333hW9tPJwDzuHAnKHKqyKgKLwR &&
cleos -u https://wax.greymass.com push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "updateauth",
      "data": {
        "account": "awmarketmain",
        "permission": "active",
        "parent": "owner",
        "auth": {
          "threshold": 1,
          "keys": [
            {
              "key": "PUB_K1_5vY8UdAaYDjWScGY7rLKnMBHH5PfBfY2cHZZG7UYRyuf5reBNJ",
              "weight": 1
            }
          ],
          "accounts": [
            {
              "permission": {
                "actor": "awmarketmain",
                "permission": "eosio.code"
              },
              "weight": 1
            }
          ],
          "waits": []
        }
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