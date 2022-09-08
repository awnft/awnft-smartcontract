#include <awmarket.hpp>

ACTION awmarket::sellmatch(smatch record)
{
    require_auth(get_self());
    // Tranfer nft đến người mua
    action(permission_level{get_self(), name("active")}, record.mk.quote_nft.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.bidder, record.bid, string("order match | DEX | athenaic.exchange")))
        .send();
    // trả tiền cho người bán
    action(permission_level{get_self(), name("active")}, record.mk.base_token.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.asker, record.ask, string("order match | DEX | athenaic.exchange")))
        .send();
}

ACTION awmarket::buymatch(bmatch record)
{
    require_auth(get_self());
    // Tranfer nft đến người mua
    action(permission_level{get_self(), name("active")}, record.mk.quote_nft.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.asker, record.ask, string("order match | DEX | athenaic.exchange")))
        .send();
    // trả tiền cho người bán
    action(permission_level{get_self(), name("active")}, record.mk.base_token.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.bidder, record.bid, string("order match | DEX | athenaic.exchange")))
        .send();
}
// ACTION awmarket::ban(vector<name> accounts) {}

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
ACTION awmarket::openmarket(name base_con, symbol base_sym, uint32_t quote_t_id, name quote_con, asset min_sell, uint8_t min_buy, uint8_t fee)
{
    require_auth(OWNER);
    auto markets = market_s(get_self(), get_self().value);
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
ACTION awmarket::closemarket(uint64_t market_id)
{
    require_auth(OWNER);
    auto markets = market_s(get_self(), get_self().value);
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
ACTION awmarket::setmfrozen(uint64_t market_id, uint64_t frozen)
{
    require_auth(get_self());
    auto markets = market_s(get_self(), get_self().value);
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
ACTION awmarket::setmfee(uint64_t market_id, uint8_t fee)
{
    require_auth(get_self());
    auto markets = market_s(get_self(), get_self().value);
    auto market = markets.find(market_id);

    if (market != markets.end())
    {
        markets.modify(market, get_self(), [&](auto &v)
                       { v.fee = fee; });
    }
}

ACTION awmarket::matchassets(name from, name to, vector<uint64_t> asset_ids, std::string memo)
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
        check(tmp.size() == 3, "memo not match pattern");
        uint64_t market_id = std::stoi(tmp.at(1));
        auto markets = market_s(get_self(), get_self().value);
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
        // get buyorder
        auto buyorders = buy_order_s(get_self(), market_id);

        uint64_t ask_amount = std::stoi(tmp.at(2));
        eosio::asset sell_quantity(ask_amount, market->base_token.sym);
        auto askquantity = sell_quantity;
        // check match asset
        // Check order mask
        auto bidorder = buyorders.get_index<name("bidorder")>();
        uint8_t asksize = asset_ids.size();
        for (auto &order : bidorder)
        {
            if (asksize >= order.bid)
            {
                // log buymatch
                smatch match_ = {order.id, order.bid, from, order.ask, order.account, bmarket, now()};
                action(permission_level{get_self(), name("active")}, get_self(),
                       name("buymatch"),
                       std::make_tuple(match_))
                    .send();
                // delete order success
                buyorders.erase(order);
                bid_quantity -= order.ask;
            }
            else
            {
            }
        }
        //  if match
        if (asset_ids.size() > 0)
        {
            /* code */
        }
        else
        {
            auto sellorders = sell_order_s(get_self(), market_id);
            sellorder sell_receipt = {
                sellorders.available_primary_key(),
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
}

ACTION awmarket::matchnfts(name from, name to, asset quantity, std::string memo)
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
        check(tmp.size() == 3, "memo not match pattern");
        // get market id
        uint64_t market_id = std::stoi(tmp.at(1));
        // find market
        auto markets = market_s(get_self(), get_self().value);
        auto market_ = markets.find(market_id);
        check(market_ != markets.end(), "not found market");
        check(!market_->frozen, "market is frozen");
        check(market_->base_token.sym == quantity.symbol, "symbol not correct");
        uint8_t buy_ask = std::stoi(tmp.at(2));
        check(buy_ask >= market_->min_buy, "min buy not enough");

        // get sellorder
        auto sellorders = sell_order_s(get_self(), market_id);

        // Check order mask
        auto askorder = sellorders.get_index<name("askorder")>();
        auto bid_quantity = quantity;
        market bmarket = {market_->id, market_->base_token, market_->quote_nft, market_->min_sell, market_->min_buy, market_->fee, market_->frozen, market_->timestamp};
        for (auto &order : askorder)
        {
            if (bid_quantity >= order.ask)
            {
                // log buymatch
                bmatch match_ = {order.id, order.bid, from, order.ask, order.account, bmarket, now()};
                action(permission_level{get_self(), name("active")}, get_self(),
                       name("sellmatch"),
                       std::make_tuple(match_))
                    .send();
                // delete order success
                sellorders.erase(order);
                bid_quantity -= order.ask;
            }
            else
            {
                // Case này vào trường hợp bid còn 1 => break match
                if (order.bid.size() == 1)
                {
                    break;
                }
                // Trường hợp có nfts với giá phù hợp trong lô => tách lô ra nft lẻ để match
                uint8_t bidsize = order.bid.size();
                uint8_t sizeremaning = ceil(bid_quantity.amount / (order.ask.amount / bidsize));
                if (sizeremaning > 0)
                {
                    vector<uint64_t> bidorder;
                    vector<uint64_t> bidremaning;
                    uint8_t i = 0;
                    for (auto &b : order.bid)
                    {
                        if (i < sizeremaning)
                        {
                            bidorder.push_back(b);
                        }
                        else
                        {
                            bidremaning.push_back(b);
                        }
                        i++;
                    }

                    if (bidorder.size() > 0)
                    {
                        eosio::asset ask_order(sizeremaning * (order.ask.amount / bidsize), market_->base_token.sym);
                        bmatch match_ = {order.id, bidorder, from, ask_order, order.account, bmarket, now()};
                        action(permission_level{get_self(), name("active")}, get_self(),
                               name("sellmatch"),
                               std::make_tuple(match_))
                            .send();
                        // Xóa order cũ, Thêm lại bản ghi mới sau khi đã match.
                        bid_quantity -= ask_order;
                        sellorders.erase(order);
                        sellorders.emplace(get_self(), [&](auto &v)
                                           {
                                            v.id = order.id;
                                            v.account = order.account;
                                            v.ask = order.ask - ask_order;
                                            v.bid = bidremaning;
                                            v.timestamp = order.timestamp; });
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }

        // if match
        if (quantity != bid_quantity)
        {
            // Trả tiền dư
            if (bid_quantity.amount > 0)
            {
                // Tranfer asset to order account
                action(permission_level{get_self(), name("active")}, market_->base_token.contract,
                       name("transfer"),
                       std::make_tuple(get_self(), from, bid_quantity, string("Refund | DEX | athenaic.exchange")))
                    .send();
            }
        }
        else
        {
            // get buyorder
            auto buyorders = buy_order_s(get_self(), market_id);

            // else match
            buyorder buy_receipt = {buyorders.available_primary_key(), from, buy_ask, quantity, now()};
            action(permission_level{get_self(), name("active")}, get_self(),
                   name("buyreceipt"),
                   std::make_tuple(market_id, buy_receipt))
                .send();
        }
    }
}

ACTION awmarket::sellreceipt(uint64_t market_id, sellorder order)
{
    require_auth(get_self());
    auto sellorders = sell_order_s(get_self(), market_id);
    sellorders.emplace(get_self(), [&](auto &v)
                       {
        v.id = order.id;
        v.account = order.account;
        v.ask = order.ask;
        v.bid = order.bid;
        v.timestamp = order.timestamp; });
}

ACTION awmarket::buyreceipt(uint64_t market_id, buyorder order)
{
    require_auth(get_self());
    auto buyorders = buy_order_s(get_self(), market_id);
    buyorders.emplace(get_self(), [&](auto &v)
                      {
        v.id = order.id;
        v.account = order.account;
        v.ask = order.ask;
        v.bid = order.bid;
        v.timestamp = order.timestamp; });
}