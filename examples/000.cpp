#include <draw.hpp>
#include <cstdio>

class sampleFrame:public draw::frameStage_t
{
  void OnRender() override
  {
    glClear(GL_COLOR_BUFFER_BIT);
  }
  
  bool IsDynamic() override
  {
    return false;
  }
};

int main(int argc, char* argv[])
{
  try {
    draw::system_t& instance = draw::system_t::Instance();
    sampleFrame frame;

    int stageID = instance.AddFrameStage(&frame);

    double start = instance.Timestamp();
    double mark  = start;

    fprintf(stderr, "Start Draw: %f\n", start);
    while (instance.IsCloseExpected() == 0)
    {
      double now = instance.Timestamp();
      instance.Render();
      if (now - mark >= 1.0)
      {
        fprintf(stderr, "Time Passed: %f\n", now - start);
        mark = now;
      }
    }
    fprintf(stderr, "After Draw Finished: %f\n", instance.Timestamp());
    instance.RemoveFrameStage(stageID);
  }
  catch(const draw::error_t& err)
  {
    fprintf(stderr, "%s:%d: %s\n", err.whatFile(), err.whatLine(), err.what());
    return -1;
  }
  return 0;
}