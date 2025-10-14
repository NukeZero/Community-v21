//
// Shader Example.
// Copyleft (c) 2007 Heesung Oh(Galic Studio). All rights Not reserved.
////////////////////////////////////////////////////////////////////////////

//  c0: World * View * Projection matrix
//	c9: color
//	c27: Scale


#define	Scl	c27


vs_1_1

def c25, 0.0, 0.0, 0.0, 1.0

dcl_position  v0		// 정점 위치 벡터 레지스터 선언 v0
dcl_normal	  v1		// 정점 법선 벡터 레지스터 선언 v1

mov r0, v1				// 법선 벡터
mad r0, r0, Scl, v0		// 윤곽선 위치' =  법선 벡터 * 스케일 + 위치
mov	r0.w, c25.w			// 윤곽선 위치' w = 1.0

m4x4 oPos, r0, c0		// 변환

mov	oD0,	c9
