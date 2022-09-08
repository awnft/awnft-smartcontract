#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/system.hpp>
#include <math.h>

using namespace eosio;
using namespace std;

CONTRACT awmarket : public contract
{
public:
   using contract::contract;
   static constexpr name ATOMICASSETS_ACCOUNT = name("atomicassets");
   static constexpr name ALIEN_WORLDS = name("alien.worlds");
   static constexpr name OWNER = name("awnftmakette");

   struct [[eosio::table]] sellorder
   {
      uint64_t id;
      name account;
      asset ask;
      vector<uint64_t> bid;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
      uint64_t askorder() const
      {
         return ask.symbol.code().raw();
      }
   };

   struct [[eosio::table]] buyorder
   {
      uint64_t id;
      name account;
      uint8_t ask;
      asset bid;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
      uint64_t bidorder() const
      {
         return bid.symbol.code().raw();
      }
   };

   struct nft
   {
      uint64_t id;
      int32_t template_id;
      uint16_t quantity;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   struct token
   {
      uint64_t id;
      asset quantity;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   struct basetoken
   {
      uint64_t id;
      symbol sym;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   struct quotenft
   {
      uint64_t id;
      int32_t template_id;
      name contract;
      uint64_t primary_key() const { return id; };
   };

   /**
    * @brief Struct market
    *
    */
   struct [[eosio::table]] market
   {
      uint64_t id;
      basetoken base_token;
      quotenft quote_nft;
      asset min_sell;
      uint8_t min_buy;
      uint8_t fee;
      bool frozen;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };

   /**
    * @brief Struct buy match record
    *
    */
   struct bmatch
   {
      uint64_t id;
      vector<uint64_t> ask;
      name asker;
      asset bid;
      name bidder;
      market mk;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };

   /**
    * @brief Struct buy match record
    *
    */
   struct smatch
   {
      uint64_t id;
      asset ask;
      name asker;
      vector<uint64_t> bid;
      name bidder;
      market mk;
      uint32_t timestamp;
      uint64_t primary_key() const { return id; };
   };

   ACTION buymatch(bmatch record);
   ACTION sellmatch(smatch record);
   ACTION sellreceipt(uint64_t market_id, sellorder order);
   ACTION buyreceipt(uint64_t market_id, buyorder order);
   // ACTION ban(vector<name> accounts);
   ACTION openmarket(name base_con, symbol base_sym, uint32_t quote_t_id, name quote_con, asset min_sell, uint8_t min_buy, uint8_t fee);
   ACTION closemarket(uint64_t market_id);
   ACTION setmfrozen(uint64_t market_id, uint64_t frozen);
   ACTION setmfee(uint64_t market_id, uint8_t fee);
   [[eosio::on_notify("atomicassets::transfer")]] void matchassets(name from, name to, vector<uint64_t> asset_ids, std::string memo);
   [[eosio::on_notify("eosio.token::transfer")]] void matchnfts(name from, name to, asset quantity, std::string memo);

   uint32_t now()
   {
      return (uint32_t)(eosio::current_time_point().sec_since_epoch());
   }

private:
   /*Table*/
   typedef multi_index<name("sellorders"), sellorder, indexed_by<name("askorder"), const_mem_fun<sellorder, uint64_t, &sellorder::askorder>>> sell_order_s;

   typedef multi_index<name("buyorders"), buyorder, indexed_by<name("bidorder"), const_mem_fun<buyorder, uint64_t, &buyorder::bidorder>>> buy_order_s;

   typedef multi_index<name("markets"), market> market_s;

   TABLE pair_t
   {
      uint64_t id;
      nft nft_pool;
      token token_pool;
      uint32_t timestamp;
      uint8_t fee;
      uint64_t primary_key() const { return id; };
   };
   typedef eosio::multi_index<name("pairs"), pair_t> pair_s;

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