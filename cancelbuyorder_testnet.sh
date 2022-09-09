cleos wallet open -n awautoprotes &&
cleos wallet unlock -n awautoprotes --password PW5K5JvVmsL18BrwUbTqgvLgCAaBWXid1m28V8XiN1oLXPUYxpeyC
# cleos wallet open -n awautoprotes &&
# cleos wallet unlock -n awautoprotes --password PW5K5JvVmsL18BrwUbTqgvLgCAaBWXid1m28V8XiN1oLXPUYxpeyC
cleos -u https://testnet.wax.pink.gg push transaction '{
  "delay_sec": 0,
  "max_cpu_usage_ms": 0,
  "actions": [
    {
      "account": "awnftmakette",
      "name": "cancelbuy",
      "data": {
        "executor": "awautoprotes",
        "market_id": 0,
        "order_id": 3
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