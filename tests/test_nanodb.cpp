#include <gtest/gtest.h>

#include "nanodb/nanodb.hpp"

TEST(NanoDB, HandlesMultipleBlocks) {
    NanoDB db(2);

    db.insert(1000, 10.0);
    db.insert(1001, 11.0);
    db.insert(1002, 12.0);
    db.insert(1003, 13.0);
    db.insert(1004, 14.0);

    ASSERT_EQ(db.getBlockCount(), 3);

    auto queryResult = db.query(1001, 1003);

    ASSERT_EQ(queryResult.size(), 3);
    EXPECT_EQ(queryResult[0].first, 1001);
    EXPECT_DOUBLE_EQ(queryResult[0].second, 11.0);
    EXPECT_EQ(queryResult[1].first, 1002);
    EXPECT_DOUBLE_EQ(queryResult[1].second, 12.0);
    EXPECT_EQ(queryResult[2].first, 1003);
    EXPECT_DOUBLE_EQ(queryResult[2].second, 13.0);
};

TEST(NanoDB, SaveAndLoad) {
    NanoDB db1;

    db1.insert(1000, 10.0);
    db1.insert(1001, 11.0);
    db1.insert(1002, 12.0);

    std::string filename = "test_db.nano";
    db1.save(filename);

    NanoDB db2;
    db2.load(filename);

    auto queryResult = db2.query(1000, 1002);

    ASSERT_EQ(queryResult.size(), 3);
    EXPECT_EQ(queryResult[0].first, 1000);
    EXPECT_DOUBLE_EQ(queryResult[0].second, 10.0);
    EXPECT_EQ(queryResult[1].first, 1001);
    EXPECT_DOUBLE_EQ(queryResult[1].second, 11.0);
    EXPECT_EQ(queryResult[2].first, 1002);
    EXPECT_DOUBLE_EQ(queryResult[2].second, 12.0);
};