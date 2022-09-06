cleos wallet open -n awnftmakette &&
cleos wallet unlock -n awnftmakette --password PW5KRBc1eqeFSBbUNGzjYLNbx1W8Ag189kAz1nWzvm8FDDdDnshoE &&
cleos -u https://testnet.wax.pink.gg push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "awnftmakette",
      "name": "openmarket",
      "data": {
        "base_con": "eosio.token",
        "base_sym": "8,WAX",
        "quote_t_id": 19553,
        "quote_con": "atomicassets",
        "min_buy": 1,
        "min_sell": "0.00000001 WAX",
        "fee": 0
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