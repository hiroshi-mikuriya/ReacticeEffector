/// @file      common/dma_mem.cpp
/// @author    SATOH GADGET
/// @copyright Copyright© 2021 SATOH GADGET
///
/// DO NOT USE THIS SOFTWARE WITHOUT THE SOFTWARE LICENSE AGREEMENT.

#include "dma_mem.h"
#include <cstdint>

#define DTCM_MEM __attribute__((section(".dtcm"))) __attribute__((aligned(4)))

namespace
{
/// メモリ全体のサイズ
constexpr std::size_t TOTAL_SIZE = 24000;
/// DTCM領域に確保したDMA専用メモリ（D-Cacheの影響を受けない）
std::uint8_t s_dmaMem[TOTAL_SIZE] DTCM_MEM;
/// 現在位置
size_t s_pos = 0;
} // namespace

void *satoh::allocDmaMem(std::size_t size) noexcept
{
  size = (size + 3) / 4 * 4;
  if (size + s_pos <= TOTAL_SIZE)
  {
    auto res = &s_dmaMem[s_pos];
    s_pos += size;
    return res;
  }
  return 0;
}
void satoh::freeDmaMem(void *ptr) noexcept
{
  static_cast<void>(ptr); // unused
}
size_t satoh::getFreeDmaMemSize() noexcept
{
  return TOTAL_SIZE - s_pos;
}
