#ifndef __EWA_MATH_DEF_H__
#define __EWA_MATH_DEF_H__

#include "ewa_base/math/tiny_cpx.h"
#include "ewa_base/math/tiny_vec.h"
#include "ewa_base/math/tiny_mat.h"
#include "ewa_base/math/tiny_box.h"

#define M_LIGHTSPEED		(2.99792458e8)

#define M_EPS0				(8.854187817e-12)
#define M_XMU0				(M_PI*4.0e-07)

#define M_HBAR				(1.0545887e-34)
#define M_EV				(1.60217653e-19)

#define M_DEG2RAD			(M_PI / 180.0)
#define M_RAD2DEG			(180.0 / M_PI)


EW_ENTER

typedef tiny_vec<size_t, 2> vec2ui;
typedef tiny_vec<size_t, 3> vec3ui;
typedef tiny_vec<size_t, 4> vec4ui;

typedef tiny_vec<int32_t,2> vec2i;
typedef tiny_vec<int32_t,3> vec3i;
typedef tiny_vec<int32_t,4> vec4i;

typedef tiny_vec<double,2> vec2d;
typedef tiny_vec<double,3> vec3d;
typedef tiny_vec<double,4> vec4d;

typedef tiny_vec<float,2> vec2f;
typedef tiny_vec<float,3> vec3f;
typedef tiny_vec<float,4> vec4f;

typedef tiny_vec<dcomplex, 2> vec2dc;
typedef tiny_vec<dcomplex, 3> vec3dc;
typedef tiny_vec<dcomplex, 4> vec4dc;

typedef tiny_box<int32_t,1> box1i;
typedef tiny_box<double,1> box1d;
typedef tiny_box<float,1> box1f;

typedef tiny_box<int32_t,2> box2i;
typedef tiny_box<double,2> box2d;
typedef tiny_box<float,2> box2f;

typedef tiny_box<int32_t,3> box3i;
typedef tiny_box<double,3> box3d;
typedef tiny_box<float,3> box3f;

typedef tiny_box<int32_t, 2> box2i;
typedef tiny_box<double, 2> box2d;
typedef tiny_box<float, 2> box2f;

typedef type_mat<double, 3, 3> mat3d;
typedef type_mat<double, 4, 4> mat4d;


typedef tiny_storage<String,3> vec3s;
typedef tiny_storage<String,2> vec2s;

typedef tiny_box<String, 3> box3s;
typedef tiny_box<String, 2> box2s;

bool is_nan(double d);
bool is_nan(float d);

double round_digit(double v, unsigned n);


EW_LEAVE
#endif
