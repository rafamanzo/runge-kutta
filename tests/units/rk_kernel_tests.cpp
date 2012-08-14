#include<cstdlib>
#include<ctime>
#include<cmath>
#include<cstdio>
#include<dataset.h>
#include<fiber.h>
#include<rk_c_kernel.h>

#include<gtest/gtest.h>
#include<rk_kernel_fixture.h>
#include<rk_kernel_extra.h>

using namespace runge_kutta;

TEST_F(RKKernelFixture, sum){
  EXPECT_EQ(_v_zero.x, (sum(_v2, _v2_oposite)).x);
  EXPECT_EQ(_v_zero.y, (sum(_v2, _v2_oposite)).y);
  EXPECT_EQ(_v_zero.z, (sum(_v2, _v2_oposite)).z);
  
  EXPECT_EQ(_v_unity.x, (sum(_v_zero, _v_unity)).x);
  EXPECT_EQ(_v_unity.y, (sum(_v_zero, _v_unity)).y);
  EXPECT_EQ(_v_unity.z, (sum(_v_zero, _v_unity)).z);
  
  EXPECT_EQ(10, (sum(_v1, _v2)).x);
  EXPECT_EQ(10, (sum(_v1, _v2)).y);
  EXPECT_EQ(10, (sum(_v1, _v2)).z);
  
  EXPECT_EQ((sum(_v2, _v1)).x, (sum(_v1, _v2)).x);
  EXPECT_EQ((sum(_v2, _v1)).y, (sum(_v1, _v2)).y);
  EXPECT_EQ((sum(_v2, _v1)).z, (sum(_v1, _v2)).z);
}

TEST_F(RKKernelFixture, mult_scalar){
  EXPECT_EQ(15, (mult_scalar(_v1, 1.5)).x);
  EXPECT_EQ(7.5, (mult_scalar(_v1, 1.5)).y);
  EXPECT_EQ(0, (mult_scalar(_v1, 1.5)).z);
  
  EXPECT_EQ(_v_zero.x, (mult_scalar(_v_zero, 1.5)).x);
  EXPECT_EQ(_v_zero.y, (mult_scalar(_v_zero, 1.5)).y);
  EXPECT_EQ(_v_zero.z, (mult_scalar(_v_zero, 1.5)).z);
  
  EXPECT_EQ(_v2.x, (mult_scalar(_v2, 1)).x);
  EXPECT_EQ(_v2.y, (mult_scalar(_v2, 1)).y);
  EXPECT_EQ(_v2.z, (mult_scalar(_v2, 1)).z);
}

TEST_F(RKKernelFixture, set){
  vector aux;
  
  set(&aux, _v1);
  
  EXPECT_EQ(_v1.x, aux.x);
  EXPECT_EQ(_v1.y, aux.y);
  EXPECT_EQ(_v1.z, aux.z);
}

TEST_F(RKKernelFixture, module){
  EXPECT_EQ(0, module(_v_zero));
  EXPECT_EQ(sqrt(3), module(_v_unity)); 
}

TEST_F(RKKernelFixture, distance){
  EXPECT_EQ(0, distance(_v_zero, _v_zero));
  EXPECT_EQ(module(_v2), distance(_v_zero, _v2));
}

TEST_F(RKKernelFixture, nearest_neighbour){
  EXPECT_EQ(_dataset.field(0,0,0).x, (nearest_neighbour(_v_zero, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).x);
  EXPECT_EQ(_dataset.field(0,0,0).y, (nearest_neighbour(_v_zero, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).y);
  EXPECT_EQ(_dataset.field(0,0,0).z, (nearest_neighbour(_v_zero, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).z);
  
  EXPECT_EQ(_v_zero.x, (nearest_neighbour(_v1, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).x);
  EXPECT_EQ(_v_zero.y, (nearest_neighbour(_v1, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).y);
  EXPECT_EQ(_v_zero.z, (nearest_neighbour(_v1, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).z);
  
  EXPECT_EQ(_dataset.field(0,0,0).x, (nearest_neighbour(_v_middle, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).x);
  EXPECT_EQ(_dataset.field(0,0,0).y, (nearest_neighbour(_v_middle, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).y);
  EXPECT_EQ(_dataset.field(0,0,0).z, (nearest_neighbour(_v_middle, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).z);
}

TEST_F(RKKernelFixture, trilinear_interpolation){
  vector aux;
  aux.x = aux.y = aux.z = 1;
  
  EXPECT_EQ(aux.x, (trilinear_interpolation(_v_middle, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).x);
  EXPECT_EQ(aux.y, (trilinear_interpolation(_v_middle, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).y);
  EXPECT_EQ(aux.z, (trilinear_interpolation(_v_middle, _dataset.n_x(), _dataset.n_y(), _dataset.n_z(), _dataset.field())).z);
  
  aux.x = 1; aux.y = 0.5; aux.z = 0;
  
  EXPECT_EQ(aux.x, (trilinear_interpolation(_v_middle, _dataset2.n_x(), _dataset2.n_y(), _dataset2.n_z(), _dataset2.field())).x);
  EXPECT_EQ(aux.y, (trilinear_interpolation(_v_middle, _dataset2.n_x(), _dataset2.n_y(), _dataset2.n_z(), _dataset2.field())).y);
  EXPECT_EQ(aux.z, (trilinear_interpolation(_v_middle, _dataset2.n_x(), _dataset2.n_y(), _dataset2.n_z(), _dataset2.field())).z);
}
