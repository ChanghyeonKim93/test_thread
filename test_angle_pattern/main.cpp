#include <immintrin.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#define SIMD_STEPSIZE_FLOAT 8
#define SIMD_DATA_FLOAT __m256
#define SIMD_ADD_FLOAT _mm256_add_ps
#define SIMD_SUB_FLOAT _mm256_sub_ps
#define SIMD_MUL_FLOAT _mm256_mul_ps
#define SIMD_DIV_FLOAT _mm256_div_ps
#define SIMD_RCP_FLOAT _mm256_rcp_ps
#define SIMD_LOAD_FLOAT _mm256_load_ps
#define SIMD_STORE_FLOAT _mm256_store_ps

int main() {
  const int kAnglePatternRadius = 15;

  std::vector<int> u_max_for_each_v_list;
  for (int v = 0; v <= kAnglePatternRadius; ++v) {
    // [u,v] = [15*cos, 15*sin]
    const double cos_theta =
        sqrt(1.0 - std::pow(static_cast<double>(v) /
                                static_cast<double>(kAnglePatternRadius),
                            2));
    int u_max = static_cast<int>(
        round(static_cast<double>(kAnglePatternRadius) * cos_theta));
    std::cout << u_max << "," << v << std::endl;
    u_max_for_each_v_list.push_back(u_max);
  }

  std::vector<std::vector<int>> u_list;
  for (int v = 0; v <= kAnglePatternRadius; ++v) {
    u_list.push_back(std::vector<int>(0));
    for (int u = 0; u < u_max_for_each_v_list[v]; ++u) u_list[v].push_back(u);
  }
  for (int v = 0; v <= kAnglePatternRadius; ++v) {
    for (const auto& u : u_list[v]) std::cout << u;
    std::cout << std::endl;
  }

  std::vector<std::string> pattern;
  pattern.resize(kAnglePatternRadius * 2 + 1,
                 std::string("                               "));
  for (int v = 0; v <= kAnglePatternRadius; ++v) {
    const int& u = u_max_for_each_v_list[v];
    pattern[kAnglePatternRadius + v][kAnglePatternRadius + u] = 'x';
    pattern[kAnglePatternRadius + v][kAnglePatternRadius - u] = 'x';
    pattern[kAnglePatternRadius - v][kAnglePatternRadius + u] = 'x';
    pattern[kAnglePatternRadius - v][kAnglePatternRadius - u] = 'x';
  }

  for (const auto& str : pattern) {
    std::cout << str << std::endl;
  }

  /*
   */
  // precalculate pattern index
  const int image_width = 640;
  const int image_height = 480;
  uint8_t image[image_width * image_height] = {0};

  float* buf_u_list = new float[100000];
  float* buf_v_list = new float[100000];
  float* buf_vwidth_list = new float[100000];
  float* buf_Iru_local_index_list = new float[100000];
  float* buf_Ilu_local_index_list = new float[100000];
  float* buf_Ild_local_index_list = new float[100000];
  float* buf_Ird_local_index_list = new float[100000];
  int buf_index = 0;
  for (int v = 1; v <= kAnglePatternRadius; ++v) {
    const int vw = image_width * v;
    for (int u = 1; u < u_max_for_each_v_list[v]; ++u) {
      buf_u_list[buf_index] = (u);
      buf_v_list[buf_index] = (v);
      buf_vwidth_list[buf_index] = (vw);
      buf_Iru_local_index_list[buf_index] = (u + vw);   //  u + vw
      buf_Ilu_local_index_list[buf_index] = (-u + vw);  // -u + vw
      buf_Ild_local_index_list[buf_index] = (-u - vw);  // -u - vw
      buf_Ird_local_index_list[buf_index] = (u - vw);   //  u - vw
      ++buf_index;
    }
  }
  std::cout << buf_index << std::endl;

  // std::vector<int> buf_Iru_list;
  // std::vector<int> buf_Ilu_list;
  // std::vector<int> buf_Ild_list;
  // std::vector<int> buf_Ird_list;

  return 0;
}