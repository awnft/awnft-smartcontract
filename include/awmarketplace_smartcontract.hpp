#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>

using namespace eosio;
using namespace std;

CONTRACT awmarketplace_smartcontract : public contract
{
public:
   using contract::contract;
   static constexpr name ATOMICASSETS_ACCOUNT = name("atomicassets");

   struct s_order
   {
      uint64_t id;
      name account;
      asset ask;
      uint16_t bid;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };

   struct b_order
   {
      uint64_t id;
      name account;
      uint16_t ask;
      asset bid;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };

   ACTION sellreceipt(uint64_t market_id, s_order sell_order);
   ACTION buyreceipt(uint64_t market_id, b_order buy_order);
   ACTION sellmatch();
   ACTION buymatch();
   ACTION ban(vector<name> accounts);
   ACTION openmarket(name base_con, symbol base_sym, uint32_t quote_t_id, name quote_con, asset min_buy, uint16_t min_sell, uint8_t fee);
   ACTION closemarket(uint64_t market_id);
   ACTION setmfrozen(uint64_t market_id, uint64_t frozen);
   ACTION setmfee(uint64_t market_id, uint8_t fee);

   [[eosio::on_notify("atomicassets::transfer")]] void validassets(name from, name to, vector<uint64_t> asset_ids, std::string memo);

private:
   /*Table*/
   TABLE buy_order_t
   {
      uint64_t id;
      name account;
      uint16_t ask;
      asset bid;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };
   typedef multi_index<name("buyorder"), buy_order_t> buy_order_s;

   TABLE sell_order_t
   {
      uint64_t id;
      name account;
      asset ask;
      uint16_t bid;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };
   typedef multi_index<name("sellorder"), sell_order_t> sell_order_s;

   TABLE ban_t
   {
      vector<name> accounts;
   };

   struct nft_s
   {
      uint64_t id;
      uint32_t template_id;
      uint16_t quantity;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   struct token_s
   {
      uint64_t id;
      asset quantity;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   struct base_token_s
   {
      uint64_t id;
      symbol sym;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   struct quote_nft_s
   {
      uint64_t id;
      uint32_t template_id;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   TABLE market_t
   {
      uint64_t id;
      base_token_s base_token;
      quote_nft_s quote_nft;
      asset min_buy;
      uint16_t min_sell;
      uint8_t fee;
      bool frozen;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };
   typedef eosio::multi_index<name("markets"), market_t> market_s;
   market_s markets = market_s(get_self(), get_self().value);

   TABLE pair_t
   {
      uint64_t id;
      nft_s nft_pool;
      token_s token_pool;
      uint32_t timestamp;
      uint8_t fee;
      uint64_t primary_key() const { return id; };
   };
   typedef eosio::multi_index<name("pairs"), pair_t> pair_s;

   uint32_t now()
   {
      return (uint32_t)(eosio::current_time_point().sec_since_epoch());
   }

   // Scope: owner
   struct assets_s
   {
      uint64_t asset_id;
      name collection_name;
      name schema_name;
      int32_t template_id;
      name ram_payer;
      vector<asset> backed_tokens;
      vector<uint8_t> immutable_serialized_data;
      vector<uint8_t> mutable_serialized_data;

      uint64_t primary_key() const { return asset_id; };
   };

   typedef multi_index<name("assets"), assets_s> assets_t;

   assets_t get_assets(name acc)
   {
      return assets_t(ATOMICASSETS_ACCOUNT, acc.value);
   }

   // Scope: template
   struct templates_s
   {
      int32_t template_id;
      name schema_name;
      bool transferable;
      bool burnable;
      uint32_t max_supply;
      uint32_t issued_supply;
      vector<uint8_t> immutable_serialized_data;

      uint64_t primary_key() const { return (uint64_t)template_id; }
   };

   typedef multi_index<name("templates"), templates_s> templates_t;

   templates_t get_templates(name collection_name)
   {
      return templates_t(ATOMICASSETS_ACCOUNT, collection_name.value);
   }
};