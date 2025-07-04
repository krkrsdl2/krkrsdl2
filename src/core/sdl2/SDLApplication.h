/* SPDX-License-Identifier: MIT */
/* Copyright (c) Kirikiri SDL2 Developers */

#pragma once
#include "tjsCommHead.h"

extern void krkrsdl2_pre_init_platform(void);
extern void krkrsdl2_set_args(int argc, tjs_char **argv);
extern void krkrsdl2_convert_set_args(int argc, char **argv);
extern bool krkrsdl2_init_platform(void);
extern void krkrsdl2_run_main_loop(void);
extern void krkrsdl2_cleanup(void);
