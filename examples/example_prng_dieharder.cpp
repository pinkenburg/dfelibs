/// \file
/// \brief Generator program to test random number generators with dieharder
///
/// This generates random bytes using a selectable pseudorandom number
/// generator and writes them to stdout. The data is intended to be consumed
/// by a statistical test tool, such as dieharder, to validate the quality, e.g.
///
///     example_prng_dieharder 'xoshiro256**' 1024 123 | dieharder -g 200 -d 201
///

#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <string>

#include <dfe/dfe_prng.hpp>

static constexpr uint64_t block_size = 1024;

template<typename Rng>
void
write_random_bytes_to_stdout(uint64_t seed, uint64_t bytes)
{
  Rng rng(seed);
  uint64_t written = 0;
  uint64_t data[block_size];

  // block-wise data generation to speed up write out
  while (written < bytes) {
    for (size_t i = 0; i < block_size; ++i) { data[i] = rng(); }
    std::fwrite(data, sizeof(uint64_t), block_size, stdout);
    written += sizeof(uint64_t) * block_size;
  }
}

struct RngRegistryEntry {
  std::string name;
  std::function<void(uint64_t, uint64_t)> func;
};

static RngRegistryEntry registry[] = {
  {"splitmix64", write_random_bytes_to_stdout<dfe::SplitMix64>},
  {"xoshiro256**", write_random_bytes_to_stdout<dfe::Xoshiro256StarStar>},
};

int
main(int argc, char* argv[])
{
  if ((argc < 3) or (4 < argc)) {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "  %s name mebibytes [seed]\n", argv[0]);
    fprintf(stderr, "\n");
    fprintf(stderr, "available rngs:\n");
    for (const auto& entry : registry) {
      fprintf(stderr, "  %s\n", entry.name.c_str());
    }
    return EXIT_FAILURE;
  }

  // read cmdline arguments
  std::string rng = argv[1];
  uint64_t seed = 1234567890;
  uint64_t bytes = 1024 * 1024 * strtoull(argv[2], nullptr, 0);
  if (argc == 4) { seed = strtoull(argv[3], nullptr, 0); }

  // run selected rng to generate bytes
  for (const auto& entry : registry) {
    if (entry.name == rng) {
      fprintf(stderr, "rng: %s\n", rng.c_str());
      fprintf(stderr, "seed: %" PRIu64 "\n", seed);
      fprintf(stderr, "bytes: %" PRIu64 "\n", bytes);
      entry.func(seed, bytes);
      return EXIT_SUCCESS;
    }
  }

  fprintf(stderr, "unknown rng '%s'\n", rng.c_str());
  return EXIT_SUCCESS;
}
