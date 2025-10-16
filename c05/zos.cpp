#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>

constexpr uint32_t Magic = 0x12345678;

#pragma pack(push, 1)

struct Bootsector {
  char magic[4];
  char descriptor[30];
  uint32_t inode_count;
  uint32_t bitmap[4];
  uint32_t block_offset;
};

#pragma pack(pop)

int main() {
  std::cout << sizeof(Bootsector) << std::endl;

  std::ofstream disk("disk.bin", std::ios::out | std::ios::binary);

  Bootsector bs;
  *reinterpret_cast<uint32_t *>(bs.magic) = Magic;
  strcpy(bs.descriptor, "AHOJ!");
  bs.inode_count = 42;
  bs.bitmap[2] = 0b0111010110100101;

  disk.write(reinterpret_cast<const char *>(&bs), sizeof(bs));

  //
  //

  std::ifstream d("disk.bin", std::ios::in | std::ios::binary);

  Bootsector b;

  d.read(reinterpret_cast<char *>(&b), sizeof(Bootsector));

  d.seekg(b.block_offset);

  std::cout << b.descriptor << std::endl; // tady neco nefunguje

  return 0;
}
