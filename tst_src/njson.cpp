#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <tuple>
#include <sstream>
#include <boost/functional/hash.hpp>
#include <nlohmann/json.hpp>

typedef std::vector<uint8_t> Opaque;

std::ostream& operator<<(std::ostream& os, const Opaque& data)
{
    std::stringstream ss;
    ss << "Opaque[";
    for (auto c : data) ss << ' ' << int(c);
    ss << " ]";
    os << ss.str();
    return os;
}

struct Uri {
    std::string x;
    std::string y;
    std::string z;

    const std::tuple<std::string, std::string, std::string> as_tuple() const { return std::tie(x, y, z); }

    Uri(const std::string& x, const std::string& y, const std::string& z) : x(x), y(y), z(z) {}

    Uri(const Opaque& data)
    {
        using namespace std;
        using nlohmann::json;
        auto j = json::from_cbor(data);
        // auto j = json::from_msgpack(data);
        x = j[0].get<string>();
        y = j[1].get<string>();
        z = j[2].get<string>();
    }

    Opaque serialize() const
    {
        using nlohmann::json;
        return json::to_cbor(json{x, y, z});
        // return json::to_msgpack(json{x, y, z});
    }

    std::string repr() const
    {
        std::stringstream ss;
        ss << "Uri( \"" << x << "\", \"" << y << "\", " << z << "\" )";
        return ss.str();    
    }
};

std::ostream& operator<<(std::ostream& os, const Uri& uri) { return os << uri.repr(); }

bool operator==(const Uri& lhs, const Uri& rhs) { return lhs.as_tuple() == rhs.as_tuple(); }

template <>
struct std::hash<Uri>{ std::size_t operator()(const Uri & key) const { return boost::hash_value(key.as_tuple()); } };

using namespace std;

int main(int argc, char *argv[])
{
    unordered_map<Uri, int> m;
    int cntr = 0;
    for (auto c1 : { "a_", "b_", "c_" }) {
        for (auto c2 : { "x_" , "y_", "z_" }) {
            for (auto c3 : { "1_", "2_", "3_" }) {
                Uri key(c1, c2, c3);
                auto data = key.serialize();
                cout << "c1=" << c1 << " c2=" << c2 << " c3=" << c3 << ' ' << data << ' ' << Uri(data) << endl;
                m[key] = cntr++;
            }
        }
    }

    for (const auto& [k, v] : m) {
        cout << k.x << ", " << k.y << ", " << k.z << " = " << v << endl;
    }
}