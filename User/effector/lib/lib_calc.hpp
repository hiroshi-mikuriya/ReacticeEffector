#pragma once

#include <cmath>

namespace satoh
{
float gainToDb(float x);
float dbToGain(float x);
float logPot(uint16_t pot, float dBmin, float dBmax);
float mixPot(uint16_t pot, float dBmin);
} // namespace satoh

/// @brief 使用範囲0.00001(-100dB)～1(0dB) 最大誤差0.016dB
inline float satoh::gainToDb(float x)
{
  x = sqrtf(sqrtf(sqrtf(sqrtf(sqrtf(x)))));
  return -559.57399f + 995.83468f * x - 591.85129f * x * x + 155.60596f * x * x * x;
}

/// @brief 使用範囲±128dB 最大誤差0.015dB
inline float satoh::dbToGain(float x)
{
  // Decibel to Voltage-ratio lookup table, size 257
  static constexpr float dbToGainTable[257] = {
      3.9810709e-7f,   4.46683686e-7f,  5.01187003e-7f, 5.62341313e-7f,  6.30957629e-7f,  7.07945617e-7f,  7.943284e-7f,    8.91250522e-7f,  9.99999997e-7f,
      1.12201894e-6f,  1.25892507e-6f,  1.41253781e-6f, 1.58489252e-6f,  1.7782794e-6f,   1.99526312e-6f,  2.23872075e-6f,  2.51188703e-6f,  2.81838174e-6f,
      3.16227761e-6f,  3.54813551e-6f,  3.98107068e-6f, 4.4668368e-6f,   5.01187014e-6f,  5.62341302e-6f,  6.30957629e-6f,  7.07945628e-6f,  7.94328389e-6f,
      8.91250511e-6f,  9.99999975e-6f,  1.12201897e-5f, 1.25892511e-5f,  1.41253786e-5f,  1.58489256e-5f,  1.77827933e-5f,  1.9952633e-5f,   2.23872066e-5f,
      2.51188703e-5f,  2.81838165e-5f,  3.16227779e-5f, 3.54813528e-5f,  3.98107077e-5f,  4.46683698e-5f,  5.01187023e-5f,  5.62341338e-5f,  6.30957657e-5f,
      7.07945655e-5f,  7.94328371e-5f,  8.91250529e-5f, 9.99999975e-5f,  0.000112201829f, 0.000125892519f, 0.000141253782f, 0.000158489333f, 0.00017782794f,
      0.000199526214f, 0.000223872063f, 0.0002511887f,  0.000281838322f, 0.000316227757f, 0.000354813354f, 0.000398107077f, 0.000446683698f, 0.000501187285f,
      0.000562341302f, 0.000630957307f, 0.00070794567f, 0.0007943284f,   0.000891251024f, 0.00100000005f,  0.00112201832f,  0.00125892507f,  0.00141253788f,
      0.00158489333f,  0.00177827943f,  0.00199526199f, 0.00223872066f,  0.00251188688f,  0.00281838328f,  0.00316227763f,  0.00354813342f,  0.00398107106f,
      0.00446683681f,  0.00501187285f,  0.00562341325f, 0.00630957261f,  0.00707945647f,  0.00794328377f,  0.00891251024f,  0.00999999978f,  0.0112201832f,
      0.0125892553f,   0.0141253751f,   0.0158489328f,  0.0177827943f,   0.0199526213f,   0.0223872121f,   0.0251188632f,   0.0281838328f,   0.0316227749f,
      0.0354813337f,   0.0398107171f,   0.0446683541f,  0.0501187295f,   0.0562341325f,   0.0630957261f,   0.0707945824f,   0.0794328153f,   0.0891251042f,
      0.100000001f,    0.112201847f,    0.12589255f,    0.14125374f,     0.158489317f,    0.177827939f,    0.199526235f,    0.223872125f,    0.251188636f,
      0.281838298f,    0.316227764f,    0.354813397f,   0.398107171f,    0.446683586f,    0.501187205f,    0.562341332f,    0.630957365f,    0.707945764f,
      0.794328213f,    0.891250968f,    1.0f,           1.12201846f,     1.25892544f,     1.41253757f,     1.58489323f,     1.77827942f,     1.99526238f,
      2.23872113f,     2.5118866f,      2.81838274f,    3.1622777f,      3.54813409f,     3.98107195f,     4.4668355f,      5.01187229f,     5.62341309f,
      6.30957365f,     7.07945824f,     7.94328213f,    8.91250896f,     10.0f,           11.2201834f,     12.5892544f,     14.1253748f,     15.8489332f,
      17.782795f,      19.9526215f,     22.3872128f,    25.1188622f,     28.1838322f,     31.622776f,      35.4813347f,     39.8107185f,     44.6683578f,
      50.1187286f,     56.2341309f,     63.095726f,     70.7945862f,     79.4328156f,     89.1251068f,     100.0f,          112.201836f,     125.892517f,
      141.253784f,     158.489334f,     177.827942f,    199.526215f,     223.87207f,      251.188705f,     281.838318f,     316.227783f,     354.813354f,
      398.107086f,     446.683685f,     501.187286f,    562.341309f,     630.957275f,     707.945618f,     794.32843f,      891.251038f,     1000.0f,
      1122.01831f,     1258.92517f,     1412.53784f,    1584.89331f,     1778.27942f,     1995.26208f,     2238.7207f,      2511.88696f,     2818.3833f,
      3162.27759f,     3548.13354f,     3981.0708f,     4466.83691f,     5011.87305f,     5623.41309f,     6309.57275f,     7079.45605f,     7943.28418f,
      8912.51074f,     10000.0f,        11220.1895f,    12589.252f,      14125.3789f,     15848.9248f,     17782.7949f,     19952.6328f,     22387.207f,
      25118.8691f,     28183.8164f,     31622.7773f,    35481.3555f,     39810.707f,      44668.3672f,     50118.7031f,     56234.1328f,     63095.7617f,
      70794.5625f,     79432.8438f,     89125.0547f,    100000.0f,       112201.891f,     125892.516f,     141253.781f,     158489.25f,      177827.938f,
      199526.312f,     223872.062f,     251188.703f,    281838.156f,     316227.75f,      354813.562f,     398107.094f,     446683.688f,     501187.0f,
      562341.312f,     630957.625f,     707945.625f,    794328.375f,     891250.562f,     1000000.0f,      1122019.0f,      1258925.12f,     1412537.88f,
      1584892.5f,      1778279.38f,     1995263.25f,    2238720.75f,     2511887.0f,
  };
  const uint8_t x0 = static_cast<uint8_t>(x + 128);
  return dbToGainTable[x0] + (dbToGainTable[x0 + 1] - dbToGainTable[x0]) * ((x + 128) - x0);
}

inline float satoh::logPot(uint16_t pot, float dBmin, float dBmax)
{
  // パラメータの値0～100を最小dB～最大dB倍率へ割り当てる
  float p = (dBmax - dBmin) * pot * 0.01f + dBmin;
  return dbToGain(p); // dBから倍率へ変換
}

inline float satoh::mixPot(uint16_t pot, float dBmin)
{
  // パラメータの値0～100をMIX倍率へ割り当てる
  // dBminはdB変化の初期値で-6未満の値
  float a = (-6.0206f - dBmin) * 0.02f; // dB変化の傾きを計算
  if (pot == 0)
  {
    return 0.0f;
  }
  if (pot < 50)
  {
    return dbToGain(pot * a + dBmin); // dBmin ～ -6dB
  }
  if (pot < 100)
  {
    return 1.0f - dbToGain((100 - pot) * a + dBmin);
  }
  return 1.0f;
}
