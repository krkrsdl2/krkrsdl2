//---------------------------------------------------------------------------
/*
	Risa [りさ]      alias 吉里吉里3 [kirikiri-3]
	 stands for "Risa Is a Stagecraft Architecture"
	Copyright (C) 2000 W.Dee <dee@kikyou.info> and contributors

	See details of license at "license.txt"
*/
//---------------------------------------------------------------------------
//! @file
//! @brief 数学関数群
//---------------------------------------------------------------------------

#ifndef TVP_MATHALGOLITHMS_H
#define TVP_MATHALGOLITHMS_H

#define _USE_MATH_DEFINES
#include <math.h>


//---------------------------------------------------------------------------

// 共通

//---------------------------------------------------------------------------
/**
 * atan2 の高速版 (1x float, C言語版)
 * @note	精度はあまり良くない。10bitぐらい。 @r
 *			原典: http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
 */
static inline float VFast_arctan2(float y, float x)
{
   static const float coeff_1 = (float)(M_PI/4);
   static const float coeff_2 = 3*coeff_1;
   float angle;
   float abs_y = fabs(y)+(float)1e-10;     // kludge to prevent 0/0 condition
   if (x>=0)
   {
      float r = (x - abs_y) / (x + abs_y);
      angle = coeff_1 - coeff_1 * r;
   }
   else
   {
      float r = (x + abs_y) / (abs_y - x);
      angle = coeff_2 - coeff_1 * r;
   }
   if (y < 0)
     return(-angle);     // negate if in quad III or IV
   else
     return(angle);
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
static inline float VFast_atan2_madd(float a, float b, float c) { return a*b+c; }
static inline float VFast_atan2_nmsub(float a, float b, float c) { return -(a*b-c); }
static inline float VFast_atan2_round(float a) { return (float)((a>0)?(int)(a+0.5):(int)(a-0.5)); }

/**
 * sincos の高速版 (1x float, C言語版)
 * @note	原典: http://arxiv.org/PS_cache/cs/pdf/0406/0406049.pdf
 */
static inline void VFast_sincos(float v, float &sin, float &cos)
{
	const float  ss1 =  1.5707963235f  ;
	const float  ss2 =  -0.645963615f  ;
	const float  ss3 =  0.0796819754f  ;
	const float  ss4 =  -0.0046075748f ;
	const float  cc1 =  -1.2336977925f ;
	const float  cc2 =  0.2536086171f  ;
	const float  cc3 =  -0.0204391631f ;

	float s1, s2, c1, c2, fixmag1;
	float x1=VFast_atan2_madd(v, (float)(1.0/(2.0*3.1415926536)), (float)(0.0));
	/* q1=x/2pi reduced onto (-0.5,0.5), q2=q1**2 */
	float q1=VFast_atan2_nmsub(VFast_atan2_round(x1), (float)(1.0), x1);
	float q2=VFast_atan2_madd(q1, q1, (float)(0.0));
	s1= VFast_atan2_madd(q1,
			VFast_atan2_madd(q2,
				VFast_atan2_madd(q2,
					VFast_atan2_madd(q2, (float)(ss4),
								(float)(ss3)),
									(float)( ss2)),
							(float)(ss1)),
						(float)(0.0));
	c1= VFast_atan2_madd(q2,
			VFast_atan2_madd(q2,
				VFast_atan2_madd(q2, (float)(cc3),
				(float)(cc2)),
			(float)(cc1)),
		(float)(1.0));

	/* now, do one out of two angle-doublings to get sin & cos theta/2 */
	c2=VFast_atan2_nmsub(s1, s1, VFast_atan2_madd(c1, c1, (float)(0.0)));
	s2=VFast_atan2_madd((float)(2.0), VFast_atan2_madd(s1, c1, (float)(0.0)), (float)(0.0));

	/* now, cheat on the correction for magnitude drift...
	if the pair has drifted to (1+e)*(cos, sin),
	the next iteration will be (1+e)**2*(cos, sin)
	which is, for small e, (1+2e)*(cos,sin).
	However, on the (1+e) error iteration,
	sin**2+cos**2=(1+e)**2=1+2e also,
	so the error in the square of this term
	will be exactly the error in the magnitude of the next term.
	Then, multiply final result by (1-e) to correct */

	/* this works with properly normalized sine-cosine functions, but un-normalized is more */
	fixmag1=VFast_atan2_nmsub(s2,s2, VFast_atan2_nmsub(c2, c2, (float)(2.0)));

	c1=VFast_atan2_nmsub(s2, s2, VFast_atan2_madd(c2, c2, (float)(0.0)));
	s1=VFast_atan2_madd((float)(2.0), VFast_atan2_madd(s2, c2, (float)(0.0)), (float)(0.0));
	cos=VFast_atan2_madd(c1, fixmag1, (float)(0.0));
	sin=VFast_atan2_madd(s1, fixmag1, (float)(0.0));
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * Phase Wrapping(radianを-PI～PIにラップする) (1x float, C言語版)
 */
static inline float WrapPi_F1(float v)
{
	int rad_unit = static_cast<int>(v*(1.0/M_PI));
	if (rad_unit >= 0) rad_unit += rad_unit&1;
	else rad_unit -= rad_unit&1;
	v -= (float)(M_PI*(double)rad_unit);
	return v;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 窓関数を適用しながらのインターリーブ解除
 * @param dest		格納先(複数)
 * @param src		ソース
 * @param win		窓関数
 * @param numch		チャンネル数
 * @param destofs	destの処理開始位置
 * @param len		処理するサンプル数
 *					(各チャンネルごとの数; 実際に処理されるサンプル
 *					数の総計はlen*numchになる)
 */
void DeinterleaveApplyingWindow(float * __restrict dest[], const float * __restrict src,
					float * __restrict win, int numch, size_t destofs, size_t len);
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
/**
 * 窓関数を適用しながらのインターリーブ+オーバーラッピング
 * @param dest		格納先
 * @param src		ソース(複数)
 * @param win		窓関数
 * @param numch		チャンネル数
 * @param srcofs	srcの処理開始位置
 * @param len		処理するサンプル数
 *					(各チャンネルごとの数; 実際に処理されるサンプル
 *					数の総計はlen*numchになる)
 */
void  InterleaveOverlappingWindow(float * __restrict dest,
	const float * __restrict const * __restrict src,
	float * __restrict win, int numch, size_t srcofs, size_t len);
//---------------------------------------------------------------------------


#endif
