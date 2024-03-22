#include <iostream>
#include <unordered_map>
#include <string>
#include <tuple>

// this would be so much less code with boost/functional/hash.hpp
namespace std{
    namespace
    {
        template <class T>
        inline void hash_combine(std::size_t& seed, T const& v)
        {
            seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        // Recursive template code derived from Matthieu M.
        template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
        struct HashValueImpl
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            HashValueImpl<Tuple, Index-1>::apply(seed, tuple);
            hash_combine(seed, std::get<Index>(tuple));
          }
        };

        template <class Tuple>
        struct HashValueImpl<Tuple,0>
        {
          static void apply(size_t& seed, Tuple const& tuple)
          {
            hash_combine(seed, std::get<0>(tuple));
          }
        };
    }

    template <typename ... TT>
    struct hash<std::tuple<TT...>> 
    {
        size_t
        operator()(std::tuple<TT...> const& tt) const
        {                                              
            size_t seed = 0;                             
            HashValueImpl<std::tuple<TT...> >::apply(seed, tt);    
            return seed;                                 
        }                                              

    };
}

using namespace std;

struct Uri {
    string x;
    string y;
    string z;

    Uri(const string& x, const string& y, const string& z) : x(x), y(y), z(z) {}

    const std::tuple<std::string, std::string, std::string> as_tuple() const { return std::tie(x, y, z); }
};

bool operator==(const Uri& lhs, const Uri& rhs) { return lhs.as_tuple() == rhs.as_tuple(); }

template <>
struct std::hash<Uri>
{
    size_t operator()(const Uri& obj) const
    {
        auto tup = obj.as_tuple();
        return hash<decltype(tup)>()(tup);
    } 
};


int main(int argc, char *argv[])
{
    // unordered_map<Uri, int> m = {
    //     {{ "a", "b", "c" }, 1 },
    //     {{ "a", "b", "d" }, 2 },
    // };

    unordered_map<Uri, int> m;
    int cntr = 0;
    for (auto c1 : { "a_", "b_", "c_" }) {
        for (auto c2 : { "x_" , "y_", "z_" }) {
            for (auto c3 : { "1_", "2_", "3_" }) {
                cout << "c1=" << c1 << " c2=" << c2 << " c3=" << c3 << endl;
                Uri key(c1, c2, c3);
                m[key] = cntr++;
                // m.insert({key, cntr++});
            }
        }
    }

    for (const auto& [k, v] : m) {
        cout << k.x << ", " << k.y << ", " << k.z << " = " << v << endl;
    }
}