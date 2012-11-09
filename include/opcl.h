#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif

#ifdef cl_khr_fp64
  #pragma OPENCL EXTENSION cl_khr_fp64 : enable
  #define TYPE double
#elif defined(cl_amd_fp64)
  #define TYPE double
  #pragma OPENCL EXTENSION cl_amd_fp64 : enable
#else
  #define TYPE float
#endif

namespace runge_kutta{
  class RK_OpenCL{
    private:
      cl_platform_id _platform;
      cl_context _context;
      cl_device_id* _devices;
      cl_command_queue _queue;
      cl_kernel _kernel;
      cl_program _program;
      cl_event _event;
      cl_mem _opencl_points, _opencl_n_points, _opencl_v0, _opencl_count_v0, _opencl_h, _opencl_n_x, _opencl_n_y, _opencl_n_z, _opencl_max_points, _opencl_field;
      unsigned int _devices_found;
      unsigned int _device_used;
      void opencl_create_platform(unsigned int num_platforms);
      void opencl_get_devices_id();
      void opencl_create_context();
      void opencl_create_queue();
      char* load_program_from_source(int *size);
      void opencl_build_program();
      void opencl_create_program();
      void opencl_create_kernel(char* kernel_name);
      void prepare_kernel(vector *v0, unsigned int count_v0, float h, int n_x,int n_y,int n_z, vector_field field, unsigned int max_points);
      void opencl_run_kernel(unsigned int count_v0, unsigned int max_points, runge_kutta::Fiber **fibers);
    public:
      RK_OpenCL();
      void opencl_init(char* kernel_name, vector *v0, int count_v0, double h, int n_x, int n_y, int n_z, vector_field field, Fiber **fibers);
  };
}
