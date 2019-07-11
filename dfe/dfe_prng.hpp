// SplitMix64
//
// Written in 2015 by Sebastiano Vigna (vigna@acm.org)
//
// To the extent possible under law, the author has dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// See <http://creativecommons.org/publicdomain/zero/1.0/>.

// xoshiro256**
//
// Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
//
// To the extent possible under law, the author has dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// See <http://creativecommons.org/publicdomain/zero/1.0/>.

// Port to C++11-compatible interface.
//
// Copyright 2019 Moritz Kiehn
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file
/// \brief   UniformRandomBitGenerator-compatible Pseudorandom number generators
/// \author  Moritz Kiehn <msmk@cern.ch>
/// \date    2019-07-10, Initial port of existing implementation to C++

#include <cstdint>

#pragma once

namespace dfe {

/// The SplitMix64 64bit pseudorandom number generator.
///
/// This is a fixed-increment version of Java 8's SplittableRandom generator
/// See http://dx.doi.org/10.1145/2714064.2660195 and
/// http://docs.oracle.com/javase/8/docs/api/java/util/SplittableRandom.html
///
/// It is a very fast generator passing BigCrush, and it can be useful if
/// for some reason you absolutely want 64 bits of state; otherwise, we
/// rather suggest to use a xoroshiro128+ (for moderately parallel
/// computations) or xorshift1024* (for massively parallel computations)
/// generator.
///
/// See also http://xoshiro.di.unimi.it
class SplitMix64 {
public:
  using result_type = uint64_t;

  constexpr SplitMix64(result_type seed)
    : m_state(seed)
  {
  }
  constexpr result_type operator()()
  {
    uint64_t z = (m_state += 0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
    z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
    return z ^ (z >> 31);
  }
  static constexpr result_type min() { return UINT64_C(0); }
  static constexpr result_type max() { return UINT64_MAX; }

private:
  uint64_t m_state;
};

/// The xoshiro256** 64bit pseudorandom number generator.
///
/// This is xoshiro256** 1.0, our all-purpose, rock-solid generator. It has
/// excellent (sub-ns) speed, a state (256 bits) that is large enough for
/// any parallel application, and it passes all tests we are aware of.
///
/// For generating just floating-point numbers, xoshiro256+ is even faster.
///
/// The state must be seeded so that it is not everywhere zero. If you have
/// a 64-bit seed, we suggest to seed a SplitMix64 generator and use its
/// output to fill the state.
///
/// See also http://xoshiro.di.unimi.it
class Xoshiro256StarStar {
public:
  using result_type = uint64_t;

  constexpr Xoshiro256StarStar(result_type seed)
    : m_state{UINT64_C(0), UINT64_C(0), UINT64_C(0), UINT64_C(0)}
  {
    // expand 64bit seed to 256bit state as suggested by original author
    SplitMix64 seq(seed);
    m_state[0] = seq();
    m_state[1] = seq();
    m_state[2] = seq();
    m_state[3] = seq();
  }
  constexpr result_type operator()()
  {
    const uint64_t z = rotate_left(m_state[1] * 5, 7) * 9;
    const uint64_t t = m_state[1] << 17;
    m_state[2] ^= m_state[0];
    m_state[3] ^= m_state[1];
    m_state[1] ^= m_state[2];
    m_state[0] ^= m_state[3];
    m_state[2] ^= t;
    m_state[3] = rotate_left(m_state[3], 45);
    return z;
  }
  static constexpr result_type min() { return UINT64_C(0); }
  static constexpr result_type max() { return UINT64_MAX; }

private:
  static constexpr uint64_t rotate_left(uint64_t x, int k)
  {
    return (x << k) | (x >> (64 - k));
  }

  uint64_t m_state[4];
};

} // namespace dfe
