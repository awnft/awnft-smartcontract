#include <awmarketplace_smartcontract.hpp>

ACTION awmarketplace_smartcontract::sellmatch()
{
}
ACTION awmarketplace_smartcontract::buymatch() {}
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
    auto market = markets.find(market_id);

    if (market != markets.end())
    {
        markets.modify(market, get_self(), [&](auto &v)
                       { v.fee = fee; });
    }
}

ACTION awmarketplace_smartcontract::validassets(name from, name to, vector<uint64_t> asset_ids, std::string memo)
{
    //check xem asset chuyển vào chợ có đúng không?
    if (memo.find("alien.worlds marketplace") != std::string::npos)
    {
        vector<assets_s> assets;
        assets_t own_assets = get_assets(get_self());

    }
}

ACTION awmarketplace_smartcontract::sellreceipt(uint64_t market_id, s_order sell_order)
{
}

ACTION awmarketplace_smartcontract::buyreceipt(uint64_t market_id, b_order buy_order)
{
}