
#include "trading_engine.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace
{
    vector<string> split(const string& line, char delimiter)
    {
        vector<string> parts;
        stringstream stream(line);
        string part;

        while (getline(stream, part, delimiter))
        {
            parts.push_back(part);
        }

        return parts;
    }
}

int main()
{
    TradingEngine engine;
    string line;

    cout.setf(ios::unitbuf);

    while (getline(cin, line))
    {
        if (line.empty())
        {
            continue;
        }

        const auto parts = split(line, '|');
        const string& command = parts[0];

        try
        {
            if (command == "CREATE_TRADER" && parts.size() == 3)
            {
                engine.createTrader(parts[1], stod(parts[2]), cout);
            }
            else if (command == "RESTORE_TRADER" && parts.size() == 4)
            {
                const bool ok = engine.restoreTrader(
                    stoi(parts[1]), parts[2], stod(parts[3])
                );

                if (ok)
                {
                    cout << "RESTORE_OK\nEND\n";
                }
                else
                {
                    cout << "ERROR|Restore failed\nEND\n";
                }
            }
            else if (command == "SEED_HOLDING" && parts.size() == 4)
            {
                const bool ok = engine.seedHolding(
                    stoi(parts[1]), parts[2], stoi(parts[3])
                );

                if (ok)
                {
                    cout << "HOLDING_OK\nEND\n";
                }
                else
                {
                    cout << "ERROR|Unable to add holding\nEND\n";
                }
            }
            
            else if (command == "ALLOCATE_HOLDING" && parts.size() == 5)
            {
                engine.allocateHolding(
                    stoi(parts[1]),
                    parts[2],
                    stod(parts[3]),
                    stoi(parts[4]),
                    cout
                );
            }
            else if (command == "ADD" && parts.size() == 6)
            {
                if (parts[2].empty())
                {
                    cout << "ERROR|Missing order side\nEND\n";
                }
                else
                {
                    engine.addOrder(
                        stoi(parts[1]),
                        parts[2][0],
                        parts[3],
                        stod(parts[4]),
                        stoi(parts[5]),
                        cout
                    );
                }
            }
            else if (command == "STATUS" && parts.size() == 2)
            {
                engine.status(parts[1], cout);
            }
            else if (command == "ACCOUNT" && parts.size() == 2)
            {
                engine.account(stoi(parts[1]), cout);
            }
            else if (command == "CANCEL" && parts.size() == 3)
            {
                engine.cancelOrder(
                    stoi(parts[1]), stoi(parts[2]), cout
                );
            }
            else if (command == "EXIT")
            {
                break;
            }
            else
            {
                cout << "ERROR|Unknown or malformed command\nEND\n";
            }
        }
        catch (const exception& ex)
        {
            string message = ex.what();

            for (char& ch : message)
            {
                if (ch == '|')
                {
                    ch = '/';
                }
            }

            cout << "ERROR|" << message << "\nEND\n";
        }
    }

    return 0;
}
