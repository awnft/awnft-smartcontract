#include <awmarketplace_smartcontract.hpp>

ACTION awmarketplace_smartcontract::sellmatch(s_match_record record)
{
}
ACTION awmarketplace_smartcontract::buymatch(b_match_record record)
{
}
ACTION awmarketplace_smartcontract::ban(vector<name> accounts) {}

/**
 * @brief open market
 *
 * @param base_con
 * @param base_sym
 * @param quote_t_id
 * @param quote_con
 * @param min_buy
 * @param min_sell
 * @param fee
 * @return ACTION
 */
ACTION awmarketplace_smartcontract::openmarket(name base_con, symbol base_sym, uint32_t quote_t_id, name quote_con, asset min_buy, uint16_t min_sell, uint8_t fee)
{
    require_auth(get_self());
    market_t markets(get_self(), get_self().value);
    markets.emplace(get_self(), [&](auto &v)
                    { v.base_token.contract = base_con;
                    v.base_token.sym = base_sym;
                    v.quote_nft.template_id = quote_t_id;
                    v.quote_nft.contract = quote_con;
                    v.min_buy = min_buy;
                    v.min_sell = min_sell;
                    v.fee = fee;
                    v.frozen = false;
                     v.timestamp = now(); });
}

/**
 * @brief close market
 *
 * @param market_id
 * @return ACTION
 */
ACTION awmarketplace_smartcontract::closemarket(uint64_t market_id)
{
    require_auth(get_self());
    market_t markets(get_self(), get_self().value);
    auto it = markets.find(market_id);
    if (it != markets.end())
    {
        it = markets.erase(it);
    }
}

/**
 * @brief set frozen for market
 *
 * @param market_id
 * @param frozen
 * @return ACTION
 */
ACTION awmarketplace_smartcontract::setmfrozen(uint64_t market_id, uint64_t frozen)
{
    require_auth(get_self());
    market_t markets(get_self(), get_self().value);
    auto market = markets.find(market_id);

    if (market != markets.end())
    {
        markets.modify(market, get_self(), [&](auto &v)
                       { v.frozen = frozen; });
    }
}

/**
 * @brief Set fee for market
 *
 * @param market_id
 * @param fee
 * @return ACTION
 */
ACTION awmarketplace_smartcontract::setmfee(uint64_t market_id, uint8_t fee)
{
    require_auth(get_self());
    market_t markets(get_self(), get_self().value);
    auto market = markets.find(market_id);

    if (market != markets.end())
    {
        markets.modify(market, get_self(), [&](auto &v)
                       { v.fee = fee; });
    }
}

ACTION awmarketplace_smartcontract::matchassets(name from, name to, vector<uint64_t> asset_ids, std::string memo)
{
    // check xem asset chuyển vào chợ có đúng không?
    if (memo.find("awnftmarket") != std::string::npos)
    {
        char delim = '#';
        size_t start;
        size_t end = 0;
        vector<string> tmp;
        while ((start = memo.find_first_not_of(delim, end)) !=
               string::npos)
        {
            end = memo.find(delim, start);
            tmp.push_back(memo.substr(start, end - start));
        }
        check(tmp.size() == 3, "memo not match partner");
        uint64_t market_id = std::stoi(tmp.at(1));
        market_t markets(get_self(), get_self().value);
        auto market = markets.find(market_id);
        if (market != markets.end())
        {
            assets_t own_assets = get_assets(get_self());
            for (auto &asset_id : asset_ids)
            {
                auto guest_asset = own_assets.find(asset_id);
                check(guest_asset != own_assets.end(), "asset not found");
                check(market->quote_nft.template_id == guest_asset->template_id, "template_id not match market");
            }
        }

        // check match asset
        //  if match

        // else match
        uint64_t ask_amount = std::stoi(tmp.at(2));
        eosio::asset sell_quantity(ask_amount, eosio::symbol("TLM", 4));
        sell_order_t sellorder(get_self(), market_id);
        sell_order_s sell_receipt = {
            sellorder.available_primary_key(),
            from,
            sell_quantity,
            asset_ids,
            now()};
        action(permission_level{get_self(), name("active")}, get_self(),
               name("sellreceipt"),
               std::make_tuple(market_id, sell_receipt))
            .send();
    }
}

ACTION awmarketplace_smartcontract::matchnfts(name from, name to, asset quantity, std::string memo)
{
    if (memo.find("awnftmarket") != std::string::npos)
    {
        char delim = '#';
        size_t start;
        size_t end = 0;
        vector<string> tmp;
        while ((start = memo.find_first_not_of(delim, end)) !=
               string::npos)
        {
            end = memo.find(delim, start);
            tmp.push_back(memo.substr(start, end - start));
        }
        check(tmp.size() == 3, "memo not match partner");
        uint64_t market_id = std::stoi(tmp.at(1));
        market_t markets(get_self(), get_self().value);
        auto market = markets.find(market_id);
        buy_order_t buyorder(get_self(), market_id);
        sell_order_t sellorder(get_self(), market_id);
        // if match
        auto bid_quantity = quantity;
        eosio::asset sell_quantity_zero(0, eosio::symbol("TLM", 4));
        auto match_sellorders = sellorder.range(
            {sell_quantity_zero, ask},
            {quantity, ask});
        if (match_sellorders.size() > 0)
        {
            for (auto &order : match_sellorders)
            {
                // Tranfer nft đến người mua
                action(permission_level{get_self(), name("active")}, ATOMICASSETS_ACCOUNT,
                       name("transfer"),
                       std::make_tuple(get_self(), from, order->bid, string("")))
                    .send();
                // trả tiền cho người bán
                action(permission_level{get_self(), name("active")}, ALIEN_WORLDS,
                       name("transfer"),
                       std::make_tuple(get_self(), order->account, order->ask, string("match order")))
                    .send();
                bid_quantity -= order->ask;
                // delete order success
                sellorder.erase(order);
            }

            // Trả tiền dư
            if (bid_quantity.amount > 0)
            {
                // Tranfer asset to order account
                action(permission_level{get_self(), name("active")}, ALIEN_WORLDS,
                       name("transfer"),
                       std::make_tuple(get_self(), from, bid_quantity, string("refund")))
                    .send();
            }
        }
        else
        {
            // else match
            uint16_t ask = std::stoi(tmp.at(2));
            buy_order_s buy_receipt = {buyorder.available_primary_key(), from, ask, quantity, now()};
            action(permission_level{get_self(), name("active")}, get_self(),
                   name("buyreceipt"),
                   std::make_tuple(market_id, buy_receipt))
                .send();
        }
    }
}

ACTION awmarketplace_smartcontract::sellreceipt(uint64_t market_id, sell_order_s sell_order)
{
    require_auth(get_self());
    sell_order_t sellorder(get_self(), market_id);
    sellorder.emplace(get_self(), [&](auto &v)
                      {
        v.id = sell_order.id;
        v.account = sell_order.account;
        v.ask = sell_order.ask;
        v.bid = sell_order.bid;
        v.timestamp = sell_order.timestamp; });
}

ACTION awmarketplace_smartcontract::buyreceipt(uint64_t market_id, buy_order_s buy_order)
{
    require_auth(get_self());
    buy_order_t buyorder(get_self(), market_id);
    buyorder.emplace(get_self(), [&](auto &v)
                     {
        v.id = buy_order.id;
        v.account = buy_order.account;
        v.ask = buy_order.ask;
        v.bid = buy_order.bid;
        v.timestamp = buy_order.timestamp; });
}