vs.1.0
; Constants:
;
;  c0-c3  - View+Projection matrix
;
;  c4.x   - time
;  c4.y   - 0
;  c4.z   - 0.5
;  c4.w   - 1.0
;
;  c7.x   - pi
;  c7.y   - 1/2pi
;  c7.z   - 2pi
;  c7.w   - 0.05
;
;  c10    - first 4 taylor coefficients for sin(x)



; Decompress position
mov r0.x, v0.x
mov r0.y, c4.w       ; 1
mov r0.z, v0.y
mov r0.w, c4.w       ; 1

add r4.x, r0.x,c4.x

; Clamp theta to -pi..pi
add r4.x, r4.x, c7.x
mul r4.x, r4.x, c7.y
frc r4.xy, r4.x
mul r4.x, r4.x, c7.z
add r4.x, r4.x,-c7.x

; Compute first 4 values in sin and cos series
mul r5.y, r4.x, r4.x ; d^2  r5.y=r4.x^2
mul r4.y, r4.x, r5.y ; d^3  r4.y=r4.x^3
mul r4.z, r4.y, r5.y ; d^5  r4.z=r4.x^5
mul r4.w, r4.z, r5.y ; d^7  r4.w=r4.x^7

mul r4, r4, c10      ; sin
dp4 r4.x, r4, c4.w

mov r0.y,r4.x



add r4.x, r0.z,c4.x

; Clamp theta to -pi..pi
add r4.x, r4.x, c7.x
mul r4.x, r4.x, c7.y
frc r4.xy, r4.x
mul r4.x, r4.x, c7.z
add r4.x, r4.x,-c7.x

; Compute first 4 values in sin and cos series
mul r5.y, r4.x, r4.x ; d^2  r5.y=r4.x^2
mul r4.y, r4.x, r5.y ; d^3  r4.y=r4.x^3
mul r4.z, r4.y, r5.y ; d^5  r4.z=r4.x^5
mul r4.w, r4.z, r5.y ; d^7  r4.w=r4.x^7

mul r4, r4, c10      ; sin
dp4 r4.x, r4, c4.w

add r0.y,r0.y,r4.x



; Scale height
mul r0.y, r0.y, c7.w
; Set color
;add r1.w,r0.y,c4.w
;mul oD0, r1.w,c4.z ; * 0.5
mov oD0,c4.y

add r0.y, r0.y, c7.z

mul r1.x,v0.x,c4.z
mul r1.y,v0.y,c4.z
mul r1.x,r1.x,c4.z
mul r1.y,r1.y,c4.z
mul r1.x,r1.x,c4.z
mul r1.y,r1.y,c4.z
mul oT0.x,r1.x,c4.z
mul oT0.y,r1.y,c4.z
; Transform position
dp4 oPos.x, r0, c0
dp4 oPos.y, r0, c1
dp4 oPos.z, r0, c2
dp4 oPos.w, r0, c3

