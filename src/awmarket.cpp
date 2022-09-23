#include <awmarket.hpp>

ACTION awmarket::sellmatch(smatch record)
{
    require_auth(get_self());
}

ACTION awmarket::buymatch(bmatch record)
{
    require_auth(get_self());
}

void awmarket::matchbtransfer(bmatch record)
{
    // Tranfer nft đến người mua
    action(permission_level{get_self(), name("active")}, record.mk.quote_nft.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.asker, record.ask, string("order match | DEX | athenaic.io")))
        .send();
    // trả tiền cho người bán
    auto quantity = record.bid;
    if (record.mk.fee > 0)
    {
        // fee
        auto fee_quantity = (record.bid * record.mk.fee) / 100;
        quantity -= fee_quantity;
    }

    action(permission_level{get_self(), name("active")}, record.mk.base_token.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.bidder, quantity, string("order match | DEX | athenaic.io")))
        .send();
}

void awmarket::matchstransfer(smatch record)
{
    // Tranfer nft đến người bán
    action(permission_level{get_self(), name("active")}, record.mk.quote_nft.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.bidder, record.bid, string("order match | DEX | athenaic.io")))
        .send();
    // trả tiền cho người mua
    auto quantity = record.ask;
    if (record.mk.fee > 0)
    {
        // fee
        auto fee_quantity = (record.ask * record.mk.fee) / 100;
        quantity -= fee_quantity;
    }
    action(permission_level{get_self(), name("active")}, record.mk.base_token.contract,
           name("transfer"),
           std::make_tuple(get_self(), record.asker, quantity, string("order match | DEX | athenaic.io")))
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
                    { 
                    v.id = markets.available_primary_key();
                    v.base_token.contract = base_con;
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
        auto market_ = markets.find(market_id);
        check(market_ != markets.end(), "not found market");
        check(!market_->frozen, "market is frozen");
        assets_t own_assets = get_assets(get_self());
        for (auto &asset_id : asset_ids)
        {
            auto guest_asset = own_assets.find(asset_id);
            check(guest_asset != own_assets.end(), "asset not found");
            check(market_->quote_nft.template_id == guest_asset->template_id, "template_id not match market");
        }
        // get sellorder
        auto sellorders = sell_order_s(get_self(), market_id);

        // get buyorder
        auto buyorders = buy_order_s(get_self(), market_id);

        uint64_t ask_amount = std::stoi(tmp.at(2));
        eosio::asset sell_quantity(ask_amount, market_->base_token.sym);
        check(sell_quantity >= market_->min_sell, "min sell not enough");
        // check match asset
        // Check order mask
        auto bidorder = buyorders.get_index<name("bidorder")>();
        uint8_t asksize_org = asset_ids.size();
        uint8_t asksize = asset_ids.size();
        market smarket = {market_->id, market_->base_token, market_->quote_nft, market_->min_sell, market_->min_buy, market_->fee, market_->frozen, market_->timestamp};
        asset quantity_remaining;
        auto sellprice = sell_quantity / asksize;
        vector<uint64_t> delid;
        for (auto order = bidorder.rbegin(); order != bidorder.rend(); order++)
        {
            if (sellprice.amount > order->unit_price)
            {
                break;
            }

            if (asksize == 0)
            {
                break;
            }

            // Check số lượng nft hiện tại bán ít hơn hoặc bằng số của ô mua thì khớp 100% lệnh này => cập nhật lại số còn lại cho ô mua
            if (asksize <= order->ask)
            {
                if (asksize < order->ask)
                {
                    // log sellmatch
                    asset bid_order_quantity(order->unit_price * asksize, market_->base_token.sym);
                    smatch match_ = {order->id, bid_order_quantity, from, asset_ids, order->account, smarket, now()};
                    action(permission_level{get_self(), name("active")}, get_self(),
                           name("sellmatch"),
                           std::make_tuple(match_))
                        .send();
                    matchstransfer(match_);
                    // Cập nhật lại số lượng cho ô mua
                    //  update lại
                    auto bidremaining = order->bid - bid_order_quantity;
                    auto sizeremaning = order->ask - asksize;
                    auto bidremaining_price = bidremaining / sizeremaning;
                    uint64_t bidremaining_amount = bidremaining_price.amount;
                    auto itr = buyorders.find(order->id);
                    check(itr != buyorders.end(), "order does not exist in table");
                    buyorders.modify(itr, get_self(), [&](auto &v)
                                     {
                                            v.ask = sizeremaning;
                                            v.bid = bidremaining;
                                            v.unit_price = bidremaining_amount; });
                }
                else
                {
                    // log sellmatch
                    smatch match_ = {order->id, order->bid, from, asset_ids, order->account, smarket, now()};
                    action(permission_level{get_self(), name("active")}, get_self(),
                           name("sellmatch"),
                           std::make_tuple(match_))
                        .send();
                    matchstransfer(match_);
                    delid.push_back(order->id);
                }
                asksize = 0;
            }
            else
            {
                // số nft hiện tại lớn hơn buyorder => cập nhật lại số dư, đồng thời xóa buyorder
                auto i = 0;
                vector<uint64_t> a_ids;
                for (auto &a_id : asset_ids)
                {
                    if (i < order->ask)
                    {
                        a_ids.push_back(a_id);
                        asset_ids.erase(asset_ids.begin() + i);
                    }
                    else
                    {
                        break;
                    }
                    i++;
                }

                // log sellmatch
                smatch match_ = {order->id, order->bid, from, a_ids, order->account, smarket, now()};
                action(permission_level{get_self(), name("active")}, get_self(),
                       name("sellmatch"),
                       std::make_tuple(match_))
                    .send();

                matchstransfer(match_);
                // Cập nhật số lượng ask
                asksize -= order->ask;
                delid.push_back(order->id);
            }
        }

        // delete buy order
        if (delid.size() > 0)
        {
            for (auto &id : delid)
            {
                // delete order success
                auto itr = buyorders.find(id);
                check(itr != buyorders.end(), "order does not exist in table");
                itr = buyorders.erase(itr);
            }
        }

        //  Còn lại không match được hoặc chưa map đc tý nào thì ghi vào sell order
        if (asksize > 0)
        {
            uint64_t unit_price = sellprice.amount;
            uint8_t remaining_size = asksize_org - asksize;
            auto selled_quantity = remaining_size * sellprice;
            sellorder sell_receipt = {
                sellorders.available_primary_key(),
                from,
                sell_quantity - selled_quantity,
                asset_ids,
                unit_price,
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
        // get buyorder
        auto buyorders = buy_order_s(get_self(), market_id);
        // get sellorder
        auto sellorders = sell_order_s(get_self(), market_id);

        // Check order mask
        auto askorder = sellorders.get_index<name("askorder")>();
        auto bid_quantity = quantity;
        auto buyprice = quantity / buy_ask;
        market bmarket = {market_->id, market_->base_token, market_->quote_nft, market_->min_sell, market_->min_buy, market_->fee, market_->frozen, market_->timestamp};
        vector<uint64_t> delid;
        for (auto &order : askorder)
        {
            auto bidsize = order.bid.size();
            if (buyprice.amount < order.unit_price)
            {
                break;
            }

            if (buy_ask == 0)
            {
                break;
            }

            // nếu người bán, bán rẻ hơn giá cần mua => múc
            if (buy_ask >= bidsize)
            {
                // buy match
                bmatch match_ = {order.id, order.bid, from, order.ask, order.account, bmarket, now()};
                action(permission_level{get_self(), name("active")}, get_self(),
                       name("buymatch"),
                       std::make_tuple(match_))
                    .send();
                matchbtransfer(match_);
                buy_ask -= bidsize;
                bid_quantity -= order.ask;

                // xóa sellorder
                delid.push_back(order.id);
            }
            else
            {
                // Tách nft để bán
                auto i = 0;
                vector<uint64_t> a_ids;
                vector<uint64_t> asset_remaning;
                for (auto &a_id : order.bid)
                {
                    if (i < buy_ask)
                    {
                        a_ids.push_back(a_id);
                    }
                    else
                    {
                        asset_remaning.push_back(a_id);
                    }
                    i++;
                }
                // buy match
                asset a_unit_price(order.unit_price, market_->base_token.sym);
                auto sellaccept = a_unit_price * buy_ask;
                bmatch match_ = {order.id, a_ids, from, sellaccept, order.account, bmarket, now()};
                action(permission_level{get_self(), name("active")}, get_self(),
                       name("buymatch"),
                       std::make_tuple(match_))
                    .send();
                matchbtransfer(match_);
                buy_ask = 0;
                bid_quantity -= sellaccept;
                // update lại sellorder
                auto askremaining = order.ask - sellaccept;
                auto askremaining_price = askremaining / asset_remaning.size();
                uint64_t askremaining_amount = askremaining_price.amount;
                sellorders.modify(order, get_self(), [&](auto &v)
                                  {
                                            v.ask = askremaining;
                                            v.bid =  asset_remaning;
                                            v.unit_price = askremaining_amount; });
                break;
            }
        }

        // del sellorder
        if (delid.size() > 0)
        {
            for (auto &id : delid)
            {
                auto itr = sellorders.find(id);
                check(itr != sellorders.end(), "order does not exist in table");
                itr = sellorders.erase(itr);
            }
        }

        // if match
        if (quantity > bid_quantity)
        {
            // Mua hết rồi thì trả tiền dư
            if (buy_ask == 0)
            {
                if (bid_quantity.amount > 0)
                {
                    // Tranfer asset to order account
                    action(permission_level{get_self(), name("active")}, market_->base_token.contract,
                           name("transfer"),
                           std::make_tuple(get_self(), from, bid_quantity, string("refund | DEX | athenaic.io")))
                        .send();
                }
            }
            else
            {
                // Còn thì cho vào biên nhận để match lần sau
                uint64_t unit_price = buyprice.amount;
                buyorder buy_receipt = {
                    buyorders.available_primary_key(),
                    from,
                    buy_ask,
                    bid_quantity,
                    unit_price,
                    now()};
                action(permission_level{get_self(), name("active")}, get_self(),
                       name("buyreceipt"),
                       std::make_tuple(market_id, buy_receipt))
                    .send();
            }
        }
        else
        {
            // else match
            uint64_t unit_price = buyprice.amount;
            buyorder buy_receipt = {buyorders.available_primary_key(), from, buy_ask, quantity, unit_price, now()};
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
        v.unit_price = order.unit_price;
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
        v.unit_price = order.unit_price;
        v.timestamp = order.timestamp; });
}

ACTION awmarket::cancelbuy(name executor, uint64_t market_id, uint64_t order_id)
{
    require_auth(executor);
    // find market
    auto markets = market_s(get_self(), get_self().value);
    auto market_ = markets.find(market_id);
    check(market_ != markets.end(), "not found market");
    auto buyorders = buy_order_s(get_self(), market_id);
    auto cancelorder = buyorders.find(order_id);
    check(cancelorder != buyorders.end(), "not found order");
    check(executor == cancelorder->account, "not owner");
    // trả tiền cho người mua
    action(permission_level{get_self(), name("active")}, market_->base_token.contract,
           name("transfer"),
           std::make_tuple(get_self(), cancelorder->account, cancelorder->bid, string("cancel order | DEX | athenaic.io")))
        .send();
    buyorders.erase(cancelorder);
}

ACTION awmarket::cancelsell(name executor, uint64_t market_id, uint64_t order_id)
{
    require_auth(executor);
    // find market
    auto markets = market_s(get_self(), get_self().value);
    auto market_ = markets.find(market_id);
    check(market_ != markets.end(), "not found market");
    auto sellorders = sell_order_s(get_self(), market_id);
    auto cancelorder = sellorders.find(order_id);
    check(cancelorder != sellorders.end(), "not found order");
    check(executor == cancelorder->account, "not owner");
    // trả nft cho người bán
    action(permission_level{get_self(), name("active")}, market_->quote_nft.contract,
           name("transfer"),
           std::make_tuple(get_self(), cancelorder->account, cancelorder->bid, string("cancel order | DEX | athenaic.io")))
        .send();
    sellorders.erase(cancelorder);
}