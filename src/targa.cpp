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
    uint8_t  alpha:4; // 3-0 
    uint8_t  _zero:1; // 4
    uint8_t  orig:1;  // 5
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

  bool tgaIsTrueColor(uint8_t imtype)
  {
    return (imtype & 0x3) == 0x2;
  }

  bool tgaIsRLE(uint8_t imtype)
  {
    return (imtype & 0x8) == 0x8;
  }

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

    if (tgaIsTrueColor(head.imtype) == false)
    {
      THROW_ERROR("TGA: only TrueColor images supported");
    }

    if (head.cms.size != 0) {
      THROW_ERROR("TGA: invalid cms.size");
    }

    if (head.is.depth != 32) {
      THROW_ERROR("TGA: invalid bit depth");
    }

    std::vector<uint8_t> pixels;

    pixels.resize(head.is.width*head.is.height*(head.is.depth/8));

    if (tgaIsRLE(head.imtype) == false)
    {
      if (fread(pixels.data(), 4, head.is.width*head.is.height, input)
        != head.is.width*head.is.height) 
      {
        THROW_ERROR("TGA: fread failed");
      }
    }
    else
    {
      uint8_t* cursor = pixels.data();
      while (cursor - pixels.data() < head.is.width*head.is.height*4)
      {
        int rc = fgetc(input);
        if (rc == EOF)
        {
          THROW_ERROR("TGA: unexpected EOF");
        }
  
        int pixCount = (rc & 0x7F) + 1;
  
        if ((rc & 0x80) != 0) // RLE-packet
        {
          uint32_t copyPixel;
          if (fread(&copyPixel, 4, 1, input) != 1)
          {
            THROW_ERROR("TGA: fread failed");
          }
          while(pixCount-->0)
          {
            memcpy(cursor, &copyPixel, sizeof(uint32_t));
            cursor += 4;
          }
        }
        else // RAW-packet
        {
          if (fread(cursor, 4, pixCount, input) != pixCount)
          {
            THROW_ERROR("TGA: fread failed");
          }
          cursor += 4*pixCount;
        }
      }
    }

    uint8_t* bgra = pixels.data();

    for (int index = 0; index < head.is.width*head.is.height; ++index) {
      uint8_t temp = bgra[index * 4];
      bgra[index * 4] = bgra[index * 4 + 2];
      bgra[index * 4 + 2] = temp;
    }

    if (head.is.orig == 0) {
      // image origin in lower left corner
      // so loader must change invert line order
      std::vector<uint8_t> tempLine;
      tempLine.resize(head.is.width*4);

      size_t byteWidth = head.is.width*4;

      for (int line = 0; line < head.is.height/2; ++line) {
        memcpy(tempLine.data(), pixels.data() + line*byteWidth, byteWidth);
        memcpy(pixels.data() + line*byteWidth, pixels.data() + (head.is.height - line - 1)*byteWidth, byteWidth);
        memcpy(pixels.data() + (head.is.height - line - 1)*byteWidth, tempLine.data(), byteWidth);
      }
    }

    return std::make_shared<glTexture_t>(glm::ivec2{head.is.width, head.is.height}, pixels.data());
  }

}