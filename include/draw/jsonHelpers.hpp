#pragma once
#ifndef __DRAW_LIB_JSON_HELPERS_HEADER__
#define __DRAW_LIB_JSON_HELPERS_HEADER__

namespace draw
{

  /**
   * @brief Parse '#RRGGBBAA' strings as color vector
   * 
   * @param hexColStr color string text
   * @return glm::vec4 color vector
   */
  glm::vec4 ParseColor(const char* hexColStr);

  glm::vec3 ParseVector3D(J2VAL vec);

  template<typename T>
  struct jsonCXXTypeTraits
  {
    enum
    {
      JSON_TYPE = J2_UNDEF
    };
  };

  template<>
  struct jsonCXXTypeTraits<glm::vec3>
  {
    enum
    {
      JSON_TYPE = J2_ARRAY
    };

    static inline glm::vec3 unbox(J2VAL value)
    {
      return ParseVector3D(value);
    }
  };

  template<>
  struct jsonCXXTypeTraits<glm::vec4>
  {
    enum
    {
      JSON_TYPE = J2_STRING
    };

    static inline glm::vec4 unbox(J2VAL value)
    {
      return ParseColor(j2ValueString(value));
    }
  };

  template<>
  struct jsonCXXTypeTraits<const char*>
  {
    enum
    {
      JSON_TYPE = J2_STRING
    };
    
    static inline const char* unbox(J2VAL value)
    {
      return j2ValueString(value);
    }
  };

  template<>
  struct jsonCXXTypeTraits<double>
  {
    enum
    {
      JSON_TYPE = J2_NUMBER
    };

    static inline double unbox(J2VAL value)
    {
      return j2ValueNumber(value);
    }
  };

  template<>
  struct jsonCXXTypeTraits<keyInputCodes_t>
  {
    enum
    {
      JSON_TYPE = J2_NUMBER
    };

    static inline keyInputCodes_t unbox(J2VAL value)
    {
      return static_cast<keyInputCodes_t>(j2ValueNumber(value));
    }

  };
  
} // namespace draw


#endif /* __DRAW_LIB_JSON_HELPERS_HEADER__ */