#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "message_protocol_json.h"
#include "order.h"

using namespace msg::ws;
using namespace of;
using json = nlohmann::json;

void TestInvalidField(const json& default_json, const std::string& field_name, const json& invalid_value) {
    json j = default_json;
    j[field_name] = invalid_value;
    std::string json_data = j.dump();

    auto res = parse_order(json_data.data(), 0);

    EXPECT_FALSE(res.is_valid()) << "Expected parsing to fail when '" << field_name << "' is invalid";
    if (!res.is_valid()) {
        std::cout << "Error with '" << field_name << "': " << res.get_error() << std::endl;
    }
}

TEST(OrderTest, ParseJsonToOrder) {
    json j;
    j["symbol"] = "AAPL";
    j["price"] = 150.25;
    j["qty"] = 100;
    j["remaining_qty"] = 50;
    j["order_side"] = "BUY";
    j["order_status"] = "OPEN";
    j["order_type"] = "LIMIT";
    j["client_order_id"] = 12345;
    j["order_id"] = 67890;
    j["acc_id"] = 98765;

    std::string json_data = j.dump();

    auto res = parse_order(json_data.data(), 0);

    if (!res.is_valid()) {
        std::cout << res.get_error() << std::endl;
    }

    ASSERT_TRUE(res.is_valid());
    Order order = res.get_value();

    // Assertions
    EXPECT_EQ(order.symbol(), "AAPL");
    EXPECT_EQ(order.price(), Price(150.25));
    EXPECT_EQ(order.qty(), Quantity(100));
    EXPECT_EQ(order.remaining_qty(), Quantity(50));
    EXPECT_EQ(order.side(), Side::BUY);
    EXPECT_EQ(order.status(), OrderStatus::OPEN);
    EXPECT_EQ(order.type(), OrderType::LIMIT);
    EXPECT_EQ(order.client_order_id(), 12345);
    EXPECT_EQ(order.order_id(), 67890);
    EXPECT_EQ(order.acc_id(), 98765);
}

TEST(OrderTest, ParseJsonToOrder_InvalidFields) {
    json default_json;
    default_json["symbol"] = "AAPL";
    default_json["price"] = 150.25;
    default_json["qty"] = 100;
    default_json["remaining_qty"] = 50;
    default_json["order_side"] = "BUY";
    default_json["order_status"] = "OPEN";
    default_json["order_type"] = "LIMIT";
    default_json["client_order_id"] = 12345;
    default_json["order_id"] = 67890;
    default_json["acc_id"] = 98765;

    TestInvalidField(default_json, "symbol", "");
    TestInvalidField(default_json, "order_side", "INVALID_SIDE");
    TestInvalidField(default_json, "order_status", "INVALID_STATUS");
    TestInvalidField(default_json, "order_type", "INVALID_TYPE");

    json missing_symbol = default_json;
    missing_symbol.erase("symbol");
    std::string json_data = missing_symbol.dump();
    auto res = parse_order(json_data.data(), 0);
    EXPECT_FALSE(res.is_valid()) << "Expected parsing to fail when 'symbol' is missing";
    if (!res.is_valid()) {
        std::cout << "Error: " << res.get_error() << std::endl;
    }
}
