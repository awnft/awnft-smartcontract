cleos wallet open -n awnftmakette &&
cleos wallet unlock -n awnftmakette --password PW5KRBc1eqeFSBbUNGzjYLNbx1W8Ag189kAz1nWzvm8FDDdDnshoE &&
cleos -u https://testnet.wax.pink.gg push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "awnftmakette",
      "name": "closemarket",
      "data": {
        "market_id": 0
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