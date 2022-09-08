cleos wallet open -n awautoprotes &&
cleos wallet unlock -n awautoprotes --password PW5K5JvVmsL18BrwUbTqgvLgCAaBWXid1m28V8XiN1oLXPUYxpeyC
# cleos wallet open -n awautoprotes &&
# cleos wallet unlock -n awautoprotes --password PW5K5JvVmsL18BrwUbTqgvLgCAaBWXid1m28V8XiN1oLXPUYxpeyC
cleos -u https://testnet.wax.pink.gg push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "eosio.token",
      "name": "transfer",
      "data": {
        "from": "awautoprotes",
        "to": "awnftmakette",
        "quantity": "20.00000000 WAX",
        "memo": "awnftmarket#0#10"
      },
      "authorization": [
        {
          "actor": "awautoprotes",
          "permission": "owner"
        }
      ]
    }
  ]
}'