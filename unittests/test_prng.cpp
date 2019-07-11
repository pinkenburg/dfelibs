/// \file
/// \brief Unit tests for pseudo random number generators

#include <algorithm>
#include <iterator>
#include <random>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

#include <dfe/dfe_prng.hpp>

using Generators = boost::mpl::list<dfe::SplitMix64, dfe::Xoshiro256StarStar>;

static constexpr int num_tests = (1 << 15);

BOOST_AUTO_TEST_CASE_TEMPLATE(sequence_neighbors, Generator, Generators)
{
  Generator rng(123);

  auto prev = rng();
  for (int n = num_tests; 0 < n; --n) {
    auto curr = rng();
    BOOST_CHECK(curr != prev);
    prev = curr;
  }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(distributions, Generator, Generators)
{
  Generator rng(123);

  auto rnd_int = std::uniform_int_distribution<int>(-10, 20);
  auto rnd_flt = std::uniform_real_distribution<float>(-10, 20);

  for (int n = num_tests; 0 < n; --n) {
    auto i = rnd_int(rng);
    BOOST_CHECK((-10 <= i) and (i <= 20));
    auto f = rnd_flt(rng);
    BOOST_CHECK((-10.0f <= f) and (f < 20.0f));
  }
}
