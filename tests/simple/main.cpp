#include <clickhouse/client.h>
#include <clickhouse/type_parser.h>

#include <iostream>

using namespace clickhouse;
using namespace std;

inline void PrintBlock(const Block& block) {
    for (Block::Iterator bi(block); bi.IsValid(); bi.Next()) {
        std::cout << bi.Name() << " ";
    }
    std::cout << std::endl;

    for (size_t i = 0; i < block.GetRowCount(); ++i) {
        std::cout << (*block[0]->As<ColumnUInt64>())[i] << " "
                  << (*block[1]->As<ColumnString>())[i] << "\n";
    }
}

int main() {
    Client client(ClientOptions().SetHost("localhost"));

    try {
        /// Create a table.
        client.Execute("CREATE TABLE IF NOT EXISTS test.client (id UInt64, name String) ENGINE = Memory");

        /// Insert some values.
        {
            Block b;

            auto id = std::make_shared<ColumnUInt64>();
            id->Append(1);
            id->Append(7);

            auto name = std::make_shared<ColumnString>();
            name->Append("one");
            name->Append("seven");

            b.AppendColumn("id"  , id);
            b.AppendColumn("name", name);

            client.Insert("test.client", b);
        }

        /// Select values inserted in the previous step.
        client.Select("SELECT id, name FROM test.client", [](const Block& block)
            {
                PrintBlock(block);
            }
        );

        /// Delete table.
        client.Execute("DROP TABLE test.client");
    } catch (const std::exception& e) {
        std::cerr << "exception : " << e.what() << std::endl;
    }

    return 0;
}
