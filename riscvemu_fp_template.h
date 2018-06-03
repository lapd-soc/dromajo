/*
 * RISCV emulator
 *
 * Copyright (c) 2016 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#if F_SIZE == 32
#define OPID 0
#define F_HIGH F32_HIGH
#elif F_SIZE == 64
#define OPID 1
#define F_HIGH F64_HIGH
#elif F_SIZE == 128
#define OPID 3
#define F_HIGH 0
#else
#error unsupported F_SIZE
#endif

#define isboxed(r) (((r) & F_HIGH) == F_HIGH)
#define unbox(r) (isboxed(r) ? (r) : F_QNAN)
#define F_QNAN glue(F_QNAN, F_SIZE)
extern glue(sfloat, F_SIZE) F_QNAN;

#define FSIGN_MASK glue(FSIGN_MASK, F_SIZE)

            case (0x00 << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                write_fp_reg(rd, glue(add_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                      unbox(read_fp_reg(rs2)),
                                                      rm, &s->fflags) | F_HIGH);
                s->fs = 3;
                break;
            case (0x01 << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                write_fp_reg(rd, glue(sub_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                      unbox(read_fp_reg(rs2)),
                                                      rm, &s->fflags) | F_HIGH);
                s->fs = 3;
                break;
            case (0x02 << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                write_fp_reg(rd, glue(mul_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                      unbox(read_fp_reg(rs2)),
                                                      rm, &s->fflags) | F_HIGH);
                s->fs = 3;
                break;
            case (0x03 << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                write_fp_reg(rd, glue(div_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                      unbox(read_fp_reg(rs2)),
                                                      rm, &s->fflags) | F_HIGH);
                s->fs = 3;
                break;
            case (0x0b << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0 || rs2 != 0)
                    goto illegal_insn;
                write_fp_reg(rd, glue(sqrt_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                       rm, &s->fflags) | F_HIGH);
                s->fs = 3;
                break;
            case (0x04 << 2) | OPID:
                switch(rm) {
                case 0: /* fsgnj */
                    write_fp_reg(rd,
                                 (unbox(read_fp_reg(rs1)) & ~FSIGN_MASK) |
                                 (unbox(read_fp_reg(rs2)) &  FSIGN_MASK) | F_HIGH);
                    break;
                case 1: /* fsgnjn */
                    write_fp_reg(rd,
                                 (unbox(read_fp_reg(rs1)) & ~FSIGN_MASK) |
                                 ((unbox(read_fp_reg(rs2)) & FSIGN_MASK) ^ FSIGN_MASK) | F_HIGH);
                    break;
                case 2: /* fsgnjx */
                    write_fp_reg(rd, (unbox(read_fp_reg(rs1)) ^
                        (unbox(read_fp_reg(rs2)) & FSIGN_MASK)) | F_HIGH);
                    break;
                default:
                    goto illegal_insn;
                }
                s->fs = 3;
                break;
            case (0x05 << 2) | OPID:
                switch(rm) {
                case 0: /* fmin */
                    write_fp_reg(rd, glue(min_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                   unbox(read_fp_reg(rs2)),
                                                   &s->fflags) | F_HIGH);
                    break;
                case 1: /* fmax */
                    write_fp_reg(rd, glue(max_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                   unbox(read_fp_reg(rs2)),
                                                   &s->fflags) | F_HIGH);
                    break;
                default:
                    goto illegal_insn;
                }
                s->fs = 3;
                break;
            case (0x18 << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                switch(rs2) {
                case 0: /* fcvt.w.[sdq] */
                    val = (int32_t)glue(glue(cvt_sf, F_SIZE), _i32)(unbox(read_fp_reg(rs1)), rm,
                                                          &s->fflags);
                    break;
                case 1: /* fcvt.wu.[sdq] */
                    val = (int32_t)glue(glue(cvt_sf, F_SIZE), _u32)(unbox(read_fp_reg(rs1)), rm,
                                                          &s->fflags);
                    break;
#if XLEN >= 64
                case 2: /* fcvt.l.[sdq] */
                    val = (int64_t)glue(glue(cvt_sf, F_SIZE), _i64)(unbox(read_fp_reg(rs1)), rm,
                                                          &s->fflags);
                    break;
                case 3: /* fcvt.lu.[sdq] */
                    val = (int64_t)glue(glue(cvt_sf, F_SIZE), _u64)(unbox(read_fp_reg(rs1)), rm,
                                                          &s->fflags);
                    break;
#endif
#if XLEN >= 128
                /* XXX: the index is not defined in the spec */
                case 4: /* fcvt.t.[sdq] */
                    val = glue(glue(cvt_sf, F_SIZE), _i128)(unbox(read_fp_reg(rs1)), rm,
                                                          &s->fflags);
                    break;
                case 5: /* fcvt.tu.[sdq] */
                    val = glue(glue(cvt_sf, F_SIZE), _u128)(unbox(read_fp_reg(rs1)), rm,
                                                          &s->fflags);
                    break;
#endif
                default:
                    goto illegal_insn;
                }
                if (rd != 0)
                    write_reg(rd, val);
                break;
            case (0x14 << 2) | OPID:
                switch(rm) {
                case 0: /* fle */
                    val = glue(le_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                              unbox(read_fp_reg(rs2)),
                                              &s->fflags);
                    break;
                case 1: /* flt */
                    val = glue(lt_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                              unbox(read_fp_reg(rs2)),
                                              &s->fflags);
                    break;
                case 2: /* feq */
                    val = glue(eq_quiet_sf, F_SIZE)(unbox(read_fp_reg(rs1)),
                                                    unbox(read_fp_reg(rs2)),
                                                    &s->fflags);
                    break;
                default:
                    goto illegal_insn;
                }
                if (rd != 0)
                    write_reg(rd, val);
                break;
            case (0x1a << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                switch(rs2) {
                case 0: /* fcvt.[sdq].w */
                    write_fp_reg(rd, glue(cvt_i32_sf, F_SIZE)(read_reg(rs1), rm,
                                                       &s->fflags) | F_HIGH);
                    break;
                case 1: /* fcvt.[sdq].wu */
                    write_fp_reg(rd, glue(cvt_u32_sf, F_SIZE)(read_reg(rs1), rm,
                                                       &s->fflags) | F_HIGH);
                    break;
#if XLEN >= 64
                case 2: /* fcvt.[sdq].l */
                    write_fp_reg(rd, glue(cvt_i64_sf, F_SIZE)(read_reg(rs1), rm,
                                                       &s->fflags) | F_HIGH);
                    break;
                case 3: /* fcvt.[sdq].lu */
                    write_fp_reg(rd, glue(cvt_u64_sf, F_SIZE)(read_reg(rs1), rm,
                                                       &s->fflags) | F_HIGH);
                    break;
#endif
#if XLEN >= 128
                /* XXX: the index is not defined in the spec */
                case 4: /* fcvt.[sdq].t */
                    write_fp_reg(rd, glue(cvt_i128_sf, F_SIZE)(read_reg(rs1), rm,
                                                       &s->fflags) | F_HIGH);
                    break;
                case 5: /* fcvt.[sdq].tu */
                    write_fp_reg(rd, glue(cvt_u128_sf, F_SIZE)(read_reg(rs1), rm,
                                                       &s->fflags) | F_HIGH);
                    break;
#endif
                default:
                    goto illegal_insn;
                }
                s->fs = 3;
                break;

            case (0x08 << 2) | OPID:
                rm = get_insn_rm(s, rm);
                if (rm < 0)
                    goto illegal_insn;
                switch(rs2) {
#if F_SIZE == 32 && FLEN >= 64
                case 1: /* cvt.s.d */
                    write_fp_reg(rd, cvt_sf64_sf32(read_fp_reg(rs1), rm, &s->fflags) | F32_HIGH);
                    break;
#if FLEN >= 128
                case 3: /* cvt.s.q */
                    write_fp_reg(rd, cvt_sf128_sf32(read_fp_reg(rs1), rm, &s->fflags) | F32_HIGH);
                    break;
#endif
#endif /* F_SIZE == 32 */
#if F_SIZE == 64
                case 0: /* cvt.d.s */
                    write_fp_reg(rd, cvt_sf32_sf64(unbox(read_fp_reg(rs1)), &s->fflags) | F64_HIGH);
                    break;
#if FLEN >= 128
                case 1: /* cvt.d.q */
                    write_fp_reg(rd, cvt_sf128_sf64(read_fp_reg(rs1), rm, &s->fflags) | F64_HIGH);
                    break;
#endif
#endif /* F_SIZE == 64 */
#if F_SIZE == 128
                case 0: /* cvt.q.s */
                    write_fp_reg(rd, cvt_sf32_sf128(unbox(read_fp_reg(rs1)), &s->fflags));
                    break;
                case 1: /* cvt.q.d */
                    write_fp_reg(rd, cvt_sf64_sf128(unbox(read_fp_reg(rs1)), &s->fflags));
                    break;
#endif /* F_SIZE == 128 */

                default:
                    goto illegal_insn;
                }
                s->fs = 3;
                break;

            case (0x1c << 2) | OPID:
                if (rs2 != 0)
                    goto illegal_insn;
                switch(rm) {
#if F_SIZE <= XLEN
                case 0: /* fmv.x.w */
#if F_SIZE == 32
                    val = (int32_t)read_fp_reg(rs1);
#elif F_SIZE == 64
                    val = (int64_t)read_fp_reg(rs1);
#else
                    val = (int128_t)read_fp_reg(rs1);
#endif
                    break;
#endif /* F_SIZE <= XLEN */
                case 1: /* fclass */
                    val = glue(fclass_sf, F_SIZE)(unbox(read_fp_reg(rs1)));
                    break;
                default:
                    goto illegal_insn;
                }
                if (rd != 0)
                    write_reg(rd, val);
                break;

#if F_SIZE <= XLEN
            case (0x1e << 2) | OPID: /* fmv.w.x */
                if (rs2 != 0 || rm != 0)
                    goto illegal_insn;
#if F_SIZE == 32
                write_fp_reg(rd, (int32_t)read_reg(rs1) | F_HIGH);
#elif F_SIZE == 64
                write_fp_reg(rd, (int64_t)read_reg(rs1) | F_HIGH);
#else
                write_fp_reg(rd, (int128_t)read_reg(rs1) | F_HIGH);
#endif
                s->fs = 3;
                break;
#endif /* F_SIZE <= XLEN */

#undef F_SIZE
#undef F_HIGH
#undef OPID
#undef FSIGN_MASK
#undef isboxed
#undef unbox
#undef F_QNAN
