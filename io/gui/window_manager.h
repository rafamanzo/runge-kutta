namespace runge_kutta{
  class WindowManager{
    public:
      WindowManager(Scene scene);
      void loop();
      static void display();
      static void reshape(int width, int height);
      static void idle();
      static void key_pressed (unsigned char key, int x, int y);
      static void key_pressed_special (int key, int x, int y);
  };
}
