#include <gtest/gtest.h>


#include "hashed_linked_list.h"

using namespace of;

struct Person {
    Person(int street_number, const std::string &street) : street_number(street_number), street(street) {}

    int street_number;
    std::string street;
};

using Name = std::string;

class TestHashedLinkedList : public ::testing::Test {
protected:
    HashLinkedList<Name, Person> ds;

    std::string Crown = "Crown";
    std::string Queen = "Queen";
    std::string King = "King";

    std::string Brian = "Brian";
    Person BrianData{50, Crown};

    std::string Geoff = "Geoff";
    Person GeoffData{43, Queen};

    std::string Rodney = "Rodney";
    Person RodneyData{100, King};
};

TEST_F(TestHashedLinkedList, insert) {
    ds.push(Brian, BrianData);
    ASSERT_EQ(ds.front().street_number, 50);
    ASSERT_EQ(ds.front().street, Crown);
}

TEST_F(TestHashedLinkedList, pop) {
    ds.push(Brian, BrianData);
    ds.push(Geoff, GeoffData);
    ASSERT_EQ(ds.front().street_number, 50);
    ASSERT_EQ(ds.front().street, Crown);
    ds.pop();
    ASSERT_EQ(ds.front().street_number, 43);
    ASSERT_EQ(ds.front().street, Queen);
}

TEST_F(TestHashedLinkedList, remove_middle) {
    ds.push(Brian, BrianData);
    ds.push(Geoff, GeoffData);
    ds.push(Rodney, RodneyData);

    ds.remove(Geoff);

    ASSERT_EQ(ds.front().street_number, 50);
    ASSERT_EQ(ds.front().street, Crown);
    ds.pop();
    ASSERT_EQ(ds.front().street_number, 100);
    ASSERT_EQ(ds.front().street, King);
}

TEST_F(TestHashedLinkedList, remove_front) {
    ds.push(Brian, BrianData);
    ds.push(Geoff, GeoffData);
    ds.push(Rodney, RodneyData);
    ds.remove(Brian);
    ASSERT_EQ(ds.front().street_number, 43);
    ASSERT_EQ(ds.front().street, Queen);
    ds.pop();
    ASSERT_EQ(ds.front().street_number, 100);
    ASSERT_EQ(ds.front().street, King);
}

TEST_F(TestHashedLinkedList, remove_back) {
    ds.push(Brian, BrianData);
    ds.push(Geoff, GeoffData);
    ds.push(Rodney, RodneyData);
    ds.remove(Rodney);
    ASSERT_EQ(ds.front().street_number, 50);
    ASSERT_EQ(ds.front().street, Crown);
    ds.pop();
    ASSERT_EQ(ds.front().street_number, 43);
    ASSERT_EQ(ds.front().street, Queen);
}

TEST_F(TestHashedLinkedList, size) {
    ASSERT_TRUE(ds.empty());
    ds.push(Brian, BrianData);
    ASSERT_FALSE(ds.empty());
    ASSERT_EQ(ds.size(), 1);
    ds.push(Geoff, GeoffData);
    ds.push(Rodney, RodneyData);
    ASSERT_EQ(ds.size(), 3);
    ds.remove(Brian);
    ds.remove(Rodney);
    ASSERT_EQ(ds.size(), 1);
    ds.pop();
    ASSERT_TRUE(ds.empty());
}

TEST_F(TestHashedLinkedList, iterate) {
    std::vector<std::pair<Name, Person>> person_vec{{Brian, BrianData}, {Geoff, GeoffData}, {Rodney, RodneyData}};
    for (auto &[name, person]: person_vec) {
        ds.push(name, person);
    }

    for (auto &r: ds) {
        ASSERT_EQ(r.item.street, ds.front().street);
        ds.pop();
    }
}

TEST_F(TestHashedLinkedList, contains) {
    ASSERT_FALSE(ds.contains(Geoff));
    ASSERT_FALSE(ds.contains(Rodney));
    ds.push(Geoff, GeoffData);
    ds.push(Rodney, RodneyData);
    ASSERT_TRUE(ds.contains(Rodney));
    ASSERT_TRUE(ds.contains(Geoff));
    ds.remove(Rodney);
    ASSERT_FALSE(ds.contains(Rodney));
    ASSERT_FALSE(ds.contains(Brian));
}

// TEST_F(TestHashedLinkedList, destructor) {
//     auto ds_ptr = new HashLinkedList<Name, Person>();
//     ds_ptr->push(Geoff, GeoffData);
//     ds_ptr->push(Rodney, RodneyData);
//     ds_ptr->~HashLinkedList<Name, Person>();
//     ASSERT_TRUE(ds_ptr->empty());
//     ASSERT_FALSE(ds_ptr->contains(Geoff));
// }
