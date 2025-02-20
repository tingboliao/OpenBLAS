/***************************************************************************
Copyright (c) 2020, The OpenBLAS Project
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the OpenBLAS project nor the names of
its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "common.h"

#if !defined(DOUBLE)
#define VSETVL(n)               __riscv_vsetvl_e32m8(n)
#define VSETVL_MAX              __riscv_vsetvlmax_e32m8()
#define FLOAT_V_T               vfloat32m8_t
#define VLEV_FLOAT              __riscv_vle32_v_f32m8
#define VLSEV_FLOAT             __riscv_vlse32_v_f32m8
#define VSEV_FLOAT              __riscv_vse32_v_f32m8
#define VSSEV_FLOAT             __riscv_vsse32_v_f32m8
#define VFMULVF_FLOAT           __riscv_vfmul_vf_f32m8
#define VFMVVF_FLOAT            __riscv_vfmv_v_f_f32m8
#else
#define VSETVL(n)               __riscv_vsetvl_e64m8(n)
#define VSETVL_MAX              __riscv_vsetvlmax_e64m8()
#define FLOAT_V_T               vfloat64m8_t
#define VLEV_FLOAT              __riscv_vle64_v_f64m8
#define VLSEV_FLOAT             __riscv_vlse64_v_f64m8
#define VSEV_FLOAT              __riscv_vse64_v_f64m8
#define VSSEV_FLOAT             __riscv_vsse64_v_f64m8
#define VFMULVF_FLOAT           __riscv_vfmul_vf_f64m8
#define VFMVVF_FLOAT            __riscv_vfmv_v_f_f64m8
#endif

int CNAME(BLASLONG n, BLASLONG dummy0, BLASLONG dummy1, FLOAT da, FLOAT *x, BLASLONG inc_x, FLOAT *y, BLASLONG inc_y, FLOAT *dummy, BLASLONG dummy2)
{
    if ( (n <= 0) || (inc_x <= 0)) return(0);

    FLOAT_V_T v0;
 
    if(inc_x == 1) {
        if(dummy2 == 0 && da == 0.0) {
            int gvl = VSETVL_MAX;
            v0 = VFMVVF_FLOAT(0.0, gvl);
            for (size_t vl; n > 0; n -= vl, x += vl) {
                vl = VSETVL(n);
                VSEV_FLOAT(x, v0, vl);
            }
        }
        else {
            for (size_t vl; n > 0; n -= vl, x += vl) {
                vl = VSETVL(n);
                v0 = VLEV_FLOAT(x, vl);
                v0 = VFMULVF_FLOAT(v0, da, vl);
                VSEV_FLOAT(x, v0, vl);
            }
        }
    } else {
        BLASLONG stride_x = inc_x * sizeof(FLOAT);

        if(dummy2 == 0 && da == 0.0) {
            int gvl = VSETVL_MAX;
            v0 = VFMVVF_FLOAT(0.0, gvl);
            for (size_t vl; n > 0; n -= vl, x += vl*inc_x) {
                vl = VSETVL(n);
                VSSEV_FLOAT(x, stride_x, v0, vl);
            }
        }
        else {
            for (size_t vl; n > 0; n -= vl, x += vl*inc_x) {
                vl = VSETVL(n);
                v0 = VLSEV_FLOAT(x, stride_x, vl);
                v0 = VFMULVF_FLOAT(v0, da, vl);
                VSSEV_FLOAT(x, stride_x, v0, vl);
            }
        }
    }

    return 0;
}
