namespace runge_kutta{
  class RKKernelFixture : public testing::Test{
    protected:
      DataSet _dataset;
      vector _v1, _v2, _v_zero, _v_unity, _v1_oposite, _v2_oposite, _v_middle;
      
      virtual void SetUp();
  };
}
