#include <draw.hpp>

#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <cstdio>

namespace
{

  enum tgaErrors {
    TGA_OK  = 0, /**< No errors */
    TGA_IO  = 1, /**< IO error  */
    TGA_SIG = 2
  };

#pragma pack(push, 1)

  struct tgaColorMapSpec 
  {
    uint16_t first;
    uint16_t len;
    uint8_t  size;
  };
  
  struct tgaImageSpec
  {
    uint16_t xorig;
    uint16_t yorig;
    uint16_t width;
    uint16_t height;
    uint8_t  depth;
    uint8_t  alpha:4;
    uint8_t  orig:2;
    uint8_t  _res:2;
  };
  
  struct tgaHeader
  {
    uint8_t         idlen;
    uint8_t         cmtype;
    uint8_t         imtype;
    tgaColorMapSpec cms;
    tgaImageSpec    is;
  };
  
  struct tgaFooter
  {
    uint32_t extoff;
    uint32_t devoff;
    char     sig[18];
  };

#pragma pack(pop)

}

const char TGA_SIGNATURE[] = "TRUEVISION-XFILE.";

namespace draw {

  glSharedResource_t LoadTGA(const char* filename)
  {
    FILE* input = fopen(filename, "rb");
    
    if (fseek(input, 0, SEEK_END) != 0) 
    {
      THROW_ERROR("TGA: fseek failed");
    }

    int32_t len = ftell(input);
    if (len < 0) 
    {
      THROW_ERROR("TGA: ftell failed");
    }

    if (fseek(input, len - sizeof(tgaFooter), SEEK_SET) != 0) 
    {
      THROW_ERROR("TGA: fseek failed");
    }

    tgaFooter foot;

    if (fread(&foot, sizeof(tgaFooter), 1, input) != 1) 
    {
      THROW_ERROR("TGA: fread failed");
    }

    if (strcmp(foot.sig, TGA_SIGNATURE) != 0) 
    {
      THROW_ERROR("TGA: invalid signature");
    }

    clearerr(input);
    if (fseek(input, 0, SEEK_SET) != 0) {
      THROW_ERROR("TGA: fseek failed");
    }

    tgaHeader head;

    if (fread(&head, sizeof(tgaHeader), 1, input) != 1) {
      THROW_ERROR("TGA: fread failed");
    }

    if (head.idlen != 0) 
    {
      THROW_ERROR("TGA: invalid idlen");
    }

    if (head.cmtype != 0) {
      THROW_ERROR("TGA: invalid cmtype");
    }

    if (head.imtype != 2) {
      THROW_ERROR("TGA: invalid imtype");
    }

    if (head.cms.size != 0) {
      THROW_ERROR("TGA: invalid cms.size");
    }

    if (head.is.depth != 32) {
      THROW_ERROR("TGA: invalid bit depth");
    }

    std::vector<uint8_t> pixels;

    pixels.resize(head.is.width*head.is.height*(head.is.depth/8));

    if (fread(pixels.data(), 4, head.is.width*head.is.height, input)
      != head.is.width*head.is.height) 
    {
      THROW_ERROR("TGA: fread failed");
    }

    uint8_t* bgra = pixels.data();

    for (int index = 0; index < head.is.width*head.is.height; ++index) {
      uint8_t temp = bgra[index * 4];
      bgra[index * 4] = bgra[index * 4 + 2];
      bgra[index * 4 + 2] = temp;
    }

    return std::make_shared<glTexture_t>(glm::ivec2{head.is.width, head.is.height}, pixels.data());
  }

}