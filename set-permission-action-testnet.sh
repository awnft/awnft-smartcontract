# cleos wallet open -n autoproloans &&
# cleos wallet unlock -n autoproloans --password PW5JtUvcsHXAPA7inBcYVFLwLHnH73d2SVKbqzMgsoPWvAUC8g4z8
# cleos wallet open -n awgatekeeper &&
# cleos wallet unlock -n awgatekeeper --password PW5JVKGBBTmTghokrHkt1D1Ay8R3zBNRtrsDz4EJTrAAu5XUd5BsX
cleos wallet open -n awnftmakette &&
cleos wallet unlock -n awnftmakette --password PW5KRBc1eqeFSBbUNGzjYLNbx1W8Ag189kAz1nWzvm8FDDdDnshoE &&
cleos -u https://testnet.waxsweden.org push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio",
      "name": "updateauth",
      "data": {
        "account": "awnftmakette",
        "permission": "active",
        "parent": "owner",
        "auth": {
          "threshold": 1,
          "keys": [
            {
              "key": "PUB_K1_6btvA4xTLuT6FRmzY6gmQ3MpPFMqGGNXXpE8zE9GYeWq4xFGm7",
              "weight": 1
            }
          ],
          "accounts": [
            {
              "permission": {
                "actor": "awnftmakette",
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
          "actor": "awnftmakette",
          "permission": "owner"
        }
      ]
    }
  ]
}'