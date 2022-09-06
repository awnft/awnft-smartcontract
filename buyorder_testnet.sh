cleos wallet open -n atgatekeeper &&
cleos wallet unlock -n atgatekeeper --password PW5KUS9ENT3aTTBHBFUUNJZWfoFBYvnrZWD6Y9KRnmhF8jNRE3ndB
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
        "from": "atgatekeeper",
        "to": "awnftmakette",
        "quantity": "5.00000000 WAX",
        "memo": "awnftmarket#0#1"
      },
      "authorization": [
        {
          "actor": "atgatekeeper",
          "permission": "owner"
        }
      ]
    }
  ]
}'