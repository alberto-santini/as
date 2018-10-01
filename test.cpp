//
// Created by alberto on 15/10/17.
//

#include <gtest/gtest.h>

#include <boost/graph/adjacency_list.hpp>

#include "src/and_die.h"
#include "src/console.h"
#include "src/containers.h"
#include "src/graph.h"
#include "src/mwis.h"
#include "src/numeric.h"
#include "src/geometry.h"
#include "src/max_clique.h"
#include "src/random.h"
#include "src/string.h"
#include "src/combinatorial.h"
#include "src/tsplib.h"
#include "src/discorde.h"
#include "src/mtz.h"
#include "src/repeat.h"

namespace {
    using namespace as;

    TEST(AndDieTest, AndDieShouldExit) {
        ASSERT_DEATH({ std::cout << as::and_die(); }, "");
    }

    TEST(ConsoleTest, PrintNotice) {
        using namespace as::console;

        ::testing::internal::CaptureStdout();
        std::cout << notice << "A notice\n";

        const std::string out = ::testing::internal::GetCapturedStdout();

        ASSERT_EQ(out, "\x1b[32m[o] \x1b[39mA notice\n");
    }

    TEST(ConsoleTest, PrintWarning) {
        using namespace as::console;

        ::testing::internal::CaptureStdout();
        std::cout << warning << "A warning\n";

        const std::string out = ::testing::internal::GetCapturedStdout();

        ASSERT_EQ(out, "\x1b[33m[*] \x1b[39mA warning\n");
    }

    TEST(ConsoleTest, PrintError) {
        using namespace as::console;

        ::testing::internal::CaptureStdout();
        std::cout << error << "An error\n";

        const std::string out = ::testing::internal::GetCapturedStdout();

        ASSERT_EQ(out, "\x1b[31m[!] \x1b[39mAn error\n");
    }

    TEST(ContainersTest, VectorContains) {
        using namespace as::containers;

        const std::vector<int> v = {0, 0, 1, 2, 5};

        ASSERT_TRUE(contains(v, 0));
        ASSERT_FALSE(contains(v, 4));
    }

    TEST(ContainersTest, SetContains) {
        using namespace as::containers;

        const std::set<int> w = {0, 0, 1, 2, 5};

        ASSERT_TRUE(contains(w, 0));
        ASSERT_FALSE(contains(w, 4));
    }

    TEST(ContainersTest, JoinAndPrintVector) {
        using namespace as::containers;

        const std::vector<int> v = {0, 1, 2};

        ::testing::internal::CaptureStdout();
        join_and_print(v);

        ASSERT_EQ(::testing::internal::GetCapturedStdout(), "0, 1, 2\n");
    }

    TEST(ContainersTest, JoinAndPrintMap) {
        using namespace as::containers;

        const std::map<char, int> m = {{'a', 1}, {'b', 2}};

        ::testing::internal::CaptureStdout();
        join_and_print(m);

        ASSERT_EQ(::testing::internal::GetCapturedStdout(), "a: 1, b: 2\n");
    }

    TEST(ContainersTest, JoinAndPrintOtherStream) {
        using namespace as::containers;

        const std::vector<int> v = {1, 2, 3};

        ::testing::internal::CaptureStderr();
        join_and_print(v, std::cerr);

        ASSERT_EQ(::testing::internal::GetCapturedStderr(), "1, 2, 3\n");
    }

    TEST(ContainersTest, JoinAndPrintOtherSeparator) {
        using namespace as::containers;

        const std::vector<int> v = {1, 2, 3};

        ::testing::internal::CaptureStdout();
        join_and_print(v, "; ");

        ASSERT_EQ(::testing::internal::GetCapturedStdout(), "1; 2; 3\n");
    }

    TEST(ContainerTest, EraseSwap) {
        using namespace as::containers;

        std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7 };
        const std::set<int> w = { 1, 3, 5, 7 };
        const std::set<int> x = {};

        const auto is_even = [] (const int& n) -> bool { return n % 2 == 0; };
        const auto is_odd = [] (const int& n) -> bool { return n % 2 != 0; };

        swap_erase(v, is_even);

        ASSERT_EQ(std::set<int>(v.begin(), v.end()), w);

        swap_erase(v, is_odd);

        ASSERT_EQ(std::set<int>(v.begin(), v.end()), x);
    }

    TEST(ContainerTest, EraseSwapEdgeCases) {
        using namespace as::containers;

        std::vector<int> v = { 1, 2, 3 };
        std::vector<int> x = {};
        const std::vector<int> w = { 1, 2, 3 };
        const std::vector<int> y = {};

        const auto always = [] (const int& n) -> bool { return true; };
        const auto never = [] (const int& n) -> bool { return false; };

        swap_erase(v, never);

        ASSERT_EQ(v, w);

        swap_erase(v, always);

        ASSERT_EQ(v, y);

        swap_erase(x, never);

        ASSERT_EQ(x, y);

        swap_erase(x, always);

        ASSERT_EQ(x, y);
    }

    class CliqueTest : public ::testing::Test {
    public:
        struct VertexProperty { float weight; };

        boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> u;
        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> d;
        boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty> w;

        CliqueTest() {
            // Build a directed and an undirected triangle with an attached vertex
            for(auto i = 0u; i < 4u; ++i) {
                boost::add_vertex(u);
                boost::add_vertex(d);
                boost::add_vertex({std::pow(2.0f, 1.0f * i)}, w);
            }

            boost::add_edge(0u, 1u, u);
            boost::add_edge(0u, 1u, d);
            boost::add_edge(0u, 1u, w);

            boost::add_edge(1u, 2u, u);
            boost::add_edge(1u, 2u, d);
            boost::add_edge(1u, 2u, w);

            boost::add_edge(2u, 0u, u);
            boost::add_edge(2u, 0u, d);
            boost::add_edge(2u, 0u, w);

            boost::add_edge(0u, 3u, u);
            boost::add_edge(0u, 3u, d);
            boost::add_edge(0u, 3u, w);
        }
    };

    TEST_F(CliqueTest, MipClique) {
        using namespace as::max_clique;

        const auto clique_u = solve_with_mip(u);
        const auto clique_d = solve_with_mip(d);
        const auto clique_w = solve_with_mip(w);
        const std::vector<unsigned long> expected = { 0u, 1u, 2u };
        const std::vector<unsigned long> weighted_expected = { 0u, 3u };

        ASSERT_EQ(clique_u, expected);
        ASSERT_EQ(clique_d, expected);
        ASSERT_EQ(clique_w, weighted_expected);
    }

    TEST_F(CliqueTest, PmcClique) {
        using namespace as::max_clique;

        const auto clique_u = solve_with_pmc(u);
        const auto clique_d = solve_with_pmc(d);
        const std::unordered_set<unsigned long> expected = { 0u, 1u, 2u };

        ASSERT_EQ(std::unordered_set<unsigned long>(clique_u.begin(), clique_u.end()), expected);
        ASSERT_EQ(std::unordered_set<unsigned long>(clique_d.begin(), clique_d.end()), expected);
    }

    class GraphTest : public ::testing::Test {
    public:
        boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> u;
        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> d;

        GraphTest() {
            // Build 2 squares
            for(auto i = 0u; i < 4u; ++i) {
                boost::add_vertex(u);
                boost::add_vertex(d);
            }
            for(auto i = 0u; i < 4u; ++i) {
                boost::add_edge(i, (i + 1) % 4u, u);
                boost::add_edge(i, (i + 1) % 4u, d);
            }
        }
    };

    TEST_F(GraphTest, IncidentUndirected) {
        using namespace as::graph;

        const auto ue1 = boost::edge(0u, 1u, u).first;
        const auto ue2 = boost::edge(1u, 2u, u).first;
        const auto ue3 = boost::edge(2u, 3u, u).first;

        ASSERT_TRUE(incident_to_the_same_vertex(ue1, ue2, u));
        ASSERT_FALSE(incident_to_the_same_vertex(ue1, ue3, u));
    }

    TEST_F(GraphTest, IsExtremeUndirected) {
        using namespace as::graph;

        const auto ue1 = boost::edge(0u, 1u, u).first;

        ASSERT_TRUE(is_extreme(0u, ue1, u));
        ASSERT_TRUE(is_extreme(1u, ue1, u));
        ASSERT_FALSE(is_extreme(2u, ue1, u));
    }

    TEST_F(GraphTest, IsExtremeDirected) {
        using namespace as::graph;

        const auto de1 = boost::edge(0u, 1u, d).first;

        ASSERT_TRUE(is_extreme(0u, de1, d));
        ASSERT_TRUE(is_extreme(1u, de1, d));
        ASSERT_FALSE(is_extreme(2u, de1, d));
    }

    TEST_F(GraphTest, OtherExtemeUndirected) {
        using namespace as::graph;

        const auto ue1 = boost::edge(0u, 1u, u).first;

        ASSERT_EQ(other_extreme(0u, ue1, u), 1u);
        ASSERT_EQ(other_extreme(1u, ue1, u), 0u);
    }

    TEST_F(GraphTest, OtherExtremeDirected) {
        using namespace as::graph;

        const auto de1 = boost::edge(0u, 1u, d).first;

        ASSERT_TRUE(is_extreme(0u, de1, d));
        ASSERT_TRUE(is_extreme(1u, de1, d));
        ASSERT_FALSE(is_extreme(2u, de1, d));
    }

    TEST_F(GraphTest, VertexComplement) {
        using namespace as::graph;

        const std::vector<unsigned long> v = { 0ul, 1ul };
        const std::vector<unsigned long> w = { 2ul, 3ul };

        ASSERT_EQ(vertex_complement(v, u), w);
        ASSERT_EQ(vertex_complement(v, d), w);
    }

    TEST_F(GraphTest, AcyclicOrientationSimple) {
        using namespace as::graph;

        const auto dir = acyclic_orientation(u);

        ASSERT_EQ(boost::num_vertices(dir), 4u);
        ASSERT_TRUE(boost::edge(0u, 1u, dir).second);
        ASSERT_TRUE(boost::edge(0u, 3u, dir).second);
        ASSERT_TRUE(boost::edge(1u, 2u, dir).second);
        ASSERT_TRUE(boost::edge(2u, 3u, dir).second);
        ASSERT_FALSE(boost::edge(3u, 0u, dir).second);
    }

    TEST_F(GraphTest, AcyclicOrientationWithProperties) {
        using namespace as::graph;

        boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, int, int, int, boost::vecS> und;
        for(auto i = 0u; i < 4u; ++i) {
            boost::add_vertex(static_cast<int>(i), und);
        }
        for(auto i = 0u; i < 4u; ++i) {
            boost::add_edge(i, (i + 1) % 4u, static_cast<int>(i * 5 + 1), und);
        }
        und[boost::graph_bundle] = 1000;

        const auto dir = acyclic_orientation(und);

        for(auto i = 0u; i < 4u; ++i) {
            EXPECT_EQ(und[i], dir[i]);
        }

        const auto& e1 = boost::edge(0u, 1u, und).first;
        const auto& [a1, t1] = boost::edge(0u, 1u, dir);

        ASSERT_TRUE(t1);
        ASSERT_EQ(und[e1], dir[a1]);

        ASSERT_TRUE(boost::edge(3u, 0u, und).second);
        ASSERT_FALSE(boost::edge(3u, 0u, dir).second);
    }

    TEST_F(GraphTest, AcyclicOrientationWithOrderCustom) {
        using namespace as::graph;

        const auto dir = acyclic_orientation(u, std::greater<>{});

        ASSERT_EQ(boost::num_vertices(dir), 4u);
        ASSERT_TRUE(boost::edge(1u, 0u, dir).second);
        ASSERT_TRUE(boost::edge(3u, 0u, dir).second);
        ASSERT_TRUE(boost::edge(2u, 1u, dir).second);
        ASSERT_TRUE(boost::edge(3u, 2u, dir).second);
        ASSERT_FALSE(boost::edge(0u, 3u, dir).second);
    }

    TEST_F(GraphTest, AcyclicOrientationWithOrderWithProperties) {
        using namespace as::graph;

        boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS, int, int, int, boost::vecS> und;
        for(auto i = 0u; i < 4u; ++i) {
            boost::add_vertex(static_cast<int>(i), und);
        }
        for(auto i = 0u; i < 4u; ++i) {
            boost::add_edge(i, (i + 1) % 4u, static_cast<int>(i * 5 + 1), und);
        }
        und[boost::graph_bundle] = 1000;

        const auto dir = acyclic_orientation(und);

        for(auto i = 0u; i < 4u; ++i) {
            EXPECT_EQ(und[i], dir[i]);
        }

        const auto& e1 = boost::edge(0u, 1u, und).first;
        const auto& [a1, t1] = boost::edge(0u, 1u, dir);

        ASSERT_TRUE(t1);
        ASSERT_EQ(und[e1], dir[a1]);

        ASSERT_TRUE(boost::edge(3u, 0u, und).second);
        ASSERT_FALSE(boost::edge(3u, 0u, dir).second);
    }

    TEST_F(GraphTest, AreConnected) {
        using namespace as::graph;

        ASSERT_TRUE(are_connected(0u, 1u, u));
        ASSERT_TRUE(are_connected(1u, 0u, u));
        ASSERT_TRUE(are_connected(0u, 1u, d));
        ASSERT_TRUE(are_connected(1u, 0u, d));
        ASSERT_FALSE(are_connected(0u, 2u, u));
        ASSERT_FALSE(are_connected(0u, 2u, d));
    }

    TEST_F(GraphTest, AreSimplicialPair) {
        using namespace as::graph;

        boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS> dir;

        as::repeat(4u, [&] () { boost::add_vertex(dir); });
        const auto a01 = boost::add_edge(0u, 1u, dir).first;
        const auto a02 = boost::add_edge(0u, 2u, dir).first;
        const auto a03 = boost::add_edge(0u, 3u, dir).first;
        boost::add_edge(1u, 2u, dir).first;

        ASSERT_TRUE(is_simplicial_pair(a01, a02, dir));
        ASSERT_FALSE(is_simplicial_pair(a01, a03, dir));
    }

    TEST_F(GraphTest, ComplementGraph) {
        using namespace as::graph;

        const auto comp = complementary(u);

        ASSERT_TRUE(boost::edge(0u, 2u, comp).second);
        ASSERT_TRUE(boost::edge(1u, 3u, comp).second);

        for(auto i = 0u; i < 4u; ++i) {
            EXPECT_FALSE(boost::edge(i, (i + 1) % 4u, comp).second);
        }
    }

    class MwisTest : public ::testing::Test {
    public:
        boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> u;
        std::vector<std::uint32_t> weights;

        MwisTest() {
            for(auto i = 0u; i < 4u; ++i) {
                boost::add_vertex(u);
            }
            for(auto i = 0u; i < 4u; ++i) {
                boost::add_edge(i, (i + 1) % 4u, u);
            }
            weights = { 1u, 2u, 1u, 2u };
        }
    };

    TEST_F(MwisTest, MaxWeightStableSetIsFound) {
        const std::vector<unsigned long> w = { 1u, 3u };

        ASSERT_EQ(as::mwis::mwis(weights, u), w);
    }

    TEST(NumericTest, ValueFitting) {
        using namespace as::numeric;

        ASSERT_FALSE(can_type_fit_value<int>(static_cast<unsigned long>(std::numeric_limits<int>::max()) + 1ul));
        ASSERT_FALSE(can_type_fit_value<unsigned int>(int{-1}));
        ASSERT_FALSE(can_type_fit_value<unsigned int>(std::numeric_limits<long unsigned int>::max()));
        ASSERT_TRUE(can_type_fit_value<unsigned int>(int{1}));
        ASSERT_TRUE(can_type_fit_value<int>((unsigned int){1u}));
    }

    TEST(GeoTest, EuclideanDistance) {
        using namespace as::geo;

        const TwoDimPoint p1{0.0f, 0.0f};
        const TwoDimPoint p2{1.0f, 1.0f};
        const TwoDimPoint p3{2.0f, 0.0f};

        ASSERT_FLOAT_EQ(euclidean_distance(p1, p2), M_SQRT2);
        ASSERT_FLOAT_EQ(euclidean_distance(p1, p3), 2.0f);
        ASSERT_FLOAT_EQ(euclidean_distance(p2, p3), M_SQRT2);
        ASSERT_FLOAT_EQ(euclidean_distance(p2, p3), euclidean_distance(p3, p2));
    }

    TEST(RandomTest, SampleEmpty) {
        using namespace as::rnd;

        const std::vector<int> v = {};

        ASSERT_TRUE(sample(v, 1u).empty());
    }

    TEST(RandomTest, SampleMoreEntriesThanInContainer) {
        using namespace as::rnd;

        const std::vector<int> v = { 1, 2, 3 };
        const std::vector<int> s1 = sample(v, 3u);
        const std::vector<int> s2 = sample(v, 5u);

        const std::unordered_set<int> u(v.begin(), v.end());
        const std::unordered_set<int> u1(s1.begin(), s1.end());
        const std::unordered_set<int> u2(s2.begin(), s2.end());

        ASSERT_EQ(u1, u);
        ASSERT_EQ(u2, u);
    }

    TEST(RandomTest, RouletteWheel) {
        using namespace as::rnd;

        const std::vector<float> v1 = { 0.5f, 0.5f };
        const auto r1 = roulette_wheel(v1);

        ASSERT_TRUE(r1 == 0u || r1 == 1u);

        const std::vector<float> v2 = { 0.0f, 1.0f };
        std::vector<std::size_t> r(10u);
        std::generate(r.begin(), r.end(), [&]() {return roulette_wheel(v2);});

        ASSERT_TRUE(std::all_of(r.begin(), r.end(), [](std::size_t n) {return n == 1u;}));
    }

    TEST(StringTest, LeftTrim) {
        std::string s{"  abc  "};
        as::string::left_trim(s);

        ASSERT_EQ(s, "abc  ");
    }

    TEST(StringTest, RightTrim) {
        std::string s{"  abc  "};
        as::string::right_trim(s);

        ASSERT_EQ(s, "  abc");
    }

    TEST(StringTest, Trim) {
        std::string s{"  abc  "};
        as::string::trim(s);

        ASSERT_EQ(s, "abc");
    }

    TEST(StringTest, TrimTab) {
        std::string s{"\tabc\t"};
        as::string::trim(s);

        ASSERT_EQ(s, "abc");
    }

    TEST(StringTest, TrimNewline) {
        std::string s{"\nabc\n"};
        as::string::trim(s);

        ASSERT_EQ(s, "abc");
    }

    class CombinatorialTest : public ::testing::Test {
    public:
        std::vector<std::uint32_t> numbers;
        std::function<void(std::vector<bool>&)> visitor;

        CombinatorialTest() {
            visitor = [this] (const std::vector<bool>& v) -> void {
                numbers.push_back(std::accumulate(v.rbegin(), v.rend(), 0u, [] (int x, int y) { return (x << 1) + y; }));
            };
        }
    };

    TEST_F(CombinatorialTest, SubsetEnum) {
        using namespace as::combi;

        visit_subsets(3u, &visitor);

        std::set<std::uint32_t> n(numbers.begin(), numbers.end());

        ASSERT_EQ(n.size(), 8u);
        for(auto i = 0u; i < 8u; ++i) {
            EXPECT_EQ(n.count(i), 1u);
        }
    }

    TEST_F(CombinatorialTest, SubsetEnumSmallToLarge) {
        using namespace as::combi;

        std::vector<std::uint32_t> n(8u);
        std::iota(n.begin(), n.end(), 0u);

        visit_subsets(3u, &visitor);

        ASSERT_EQ(numbers, n);
    }

    TEST_F(CombinatorialTest, SubsetEnumLargeToSmall) {
        using namespace as::combi;

        std::vector<std::uint32_t> n(8u);
        std::iota(n.begin(), n.end(), 0u);
        std::reverse(n.begin(), n.end());

        visit_subsets(3u, &visitor, false);

        ASSERT_EQ(numbers, n);
    }

    TEST(TspTest, SolvePr10) {
        using namespace as::tsplib;
        using namespace as::tsp;

        const TSPInstance instance("../test/tsplib/pr10.tsp");
        const auto discorde_solution = discorde_solve_tsp(instance);
        const std::set<std::uint32_t> discorde_v(discorde_solution.begin(), discorde_solution.end());

        ASSERT_EQ(discorde_solution.size(), 10u);

        for(auto i = 0u; i < 10u; ++i) {
            EXPECT_EQ(discorde_v.count(i), 1u);
        }

        const auto mtz_solution = mtz_solve_tsp(instance);
        const std::set<std::uint32_t> mtz_v(mtz_solution.begin(), mtz_solution.end());

        ASSERT_EQ(discorde_v, mtz_v);
    }

    TEST(TspTest, SolvePr10Subset) {
        using namespace as::tsplib;
        using namespace as::tsp;

        const TSPInstance instance("../test/tsplib/pr10.tsp");
        const std::vector<std::uint32_t> vertices = { 0, 1, 5, 6, 8 };
        const std::set<std::uint32_t> vertices_set(vertices.begin(), vertices.end());
        const auto discorde_solution = discorde_solve_tsp(instance, vertices);
        const std::set<std::uint32_t> discorde_v(discorde_solution.begin(), discorde_solution.end());
        const auto mtz_solution = mtz_solve_tsp(instance, vertices);
        const std::set<std::uint32_t> mtz_v(mtz_solution.begin(), mtz_solution.end());

        ASSERT_EQ(vertices_set, discorde_v);
        ASSERT_EQ(vertices_set, mtz_v);
    }

    TEST(TspTest, DiscordeCrashesOn4Vertices) {
        using namespace as::tsplib;
        using namespace as::tsp;

        const TSPInstance instance("../test/tsplib/pr10.tsp");
        const std::vector<std::uint32_t> vertices = { 0, 1, 5, 6 };

        ASSERT_THROW(discorde_solve_tsp(instance, vertices), std::runtime_error);
    }

    TEST(RepeatTest, Repeats5Times) {
        using namespace as;

        std::vector<int> v = {};
        std::vector<int> expected = { 0, 0, 0, 0, 0 };

        repeat(5u, [&] () { v.push_back(0); });

        ASSERT_EQ(v, expected);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}