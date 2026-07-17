/**
 * @file main.cpp
 */
#include <iostream>

#include "db/connection.h"

int main(int argc, char* argv[])
{
    std::ios::sync_with_stdio(false);

    pqxx::connection conn = db::connect();
    pqxx::work txn(conn);

    return 0;
}
