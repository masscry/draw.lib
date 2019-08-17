/**
 * @file frameStage_t.hpp
 * @author your name (you@domain.com)
 * @brief simple frame stage class
 * @version 0.0.1.1
 * @date 2019-08-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#pragma once
#ifndef __DRAW_LIB_FRAME_STAGE_HEADER__
#define __DRAW_LIB_FRAME_STAGE_HEADER__

#include <cassert>

namespace draw
{

  class system_t;

  class frameStage_t
  {
    friend class system_t;

    int index; /**< Index is set, when frame stage is registered in system_t */

    /**
     * @brief Called when frame stage must be rendered.
     */
    virtual void OnRender()  = 0;

    /**
     * @brief Must return true if this frame stage is not a singleton and
     * allocated on heap
     * 
     * @return true frame stage is allocated on heap
     * @return false frame stage is singleton
     */
    virtual bool IsDynamic() = 0;

    frameStage_t(const frameStage_t&) = delete;
    frameStage_t& operator = (const frameStage_t&) = delete;

    frameStage_t(frameStage_t&&) = delete;
    frameStage_t& operator = (frameStage_t&&) = delete;

    /**
     * @brief Set the ID to object
     * 
     * This function called in AddFrameStage/RemoveFrameStage
     * 
     * @param index new ID
     */
    void SetRegisteredID(int index)
    {
      this->index = index;
    }

  public:

    /**
     * @brief Get registered ID
     * 
     * @return int if not registered yet returns -1
     */
    int RegisteredID() const 
    {
      return this->index;
    }

    void Render() noexcept
    {
      this->OnRender();
    }

    void Delete() noexcept
    {
      if (this->IsDynamic())
      {
        delete this;
      }
    }

    frameStage_t() noexcept
    :index(-1) {
      ;
    }
    
    virtual ~frameStage_t()
    { // Must unregister this stage before its termination 
      assert(this->RegisteredID() < 0);
    }
  };

} // namespace draw

#endif /* __DRAW_LIB_FRAME_STAGE_HEADER__ */