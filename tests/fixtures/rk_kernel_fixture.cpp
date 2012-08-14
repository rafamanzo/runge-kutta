#include<cstdlib>
#include<cmath>
#include<dataset.h>

#include<gtest/gtest.h>
#include<rk_kernel_fixture.h>

using namespace runge_kutta;

void RKKernelFixture::SetUp(){
  vector_field vf, vf2;
  int i, j, k, index;
  
  vf = (vector_field) malloc(8*sizeof(vector));
  for(i = 0; i < 8; i++){
    vf[i].x = 1.0;
    vf[i].y = 1.0;
    vf[i].z = 1.0;
  }

  _dataset = DataSet(2, 2, 2, vf);
  
  vf2 = (vector_field) malloc(8*sizeof(vector));
  for(i = 0; i < 2; i++){
    for(j = 0; j < 2; j++){
      for(k = 0; k < 2; k++){
        index = i + 2*j + 4*k;
        if(j == 0){
          vf2[index].x = 1.0; vf2[index].y = 0; vf2[index].z = 0;
        }else{
          vf2[index].x = 0; vf2[index].y = 1.0; vf2[index].z = 0;
        }
      }
    }
  }
   
  _dataset2 = DataSet(2, 2, 2, vf2);
  
  _v1.x = 10; _v1.y = 5; _v1.z = 0;
  _v2.x = 0; _v2.y = 5; _v2.z = 10;
  _v1_oposite.x = -10; _v1_oposite.y = -5; _v1_oposite.z = 0;
  _v2_oposite.x = 0; _v2_oposite.y = -5; _v2_oposite.z = -10;
  _v_zero.x = _v_zero.y = _v_zero.z = 0;
  _v_unity.x = _v_unity.y = _v_unity.z = 1;
  _v_middle.x = _v_middle.y = _v_middle.z = 0.5;
}
