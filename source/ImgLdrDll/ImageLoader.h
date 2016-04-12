#pragma once
#include "cv.h"
#include "misc.h"
#include "bitstrm.h"
//#include "grfmts.h"
//#include "ImgLdr.h"

/////////////////////////////Image Loader////////////////////////////////////////

#define IMG_LOAD_ERR -1
#define BUF_SIZE_SMALL -2
#define LOAD_SUC 0

IplImage* LoadFImage( const char* filename );
bool SaveFImage( const char* filename, const IplImage* arr );

int LoadImageToBuf(const char *filename, IplImage *img, int sizeofbuf);



///////////////////////////////// base class for readers ////////////////////////
class   GrFmtReader
{
public:
    
    GrFmtReader( const char* filename );
    virtual ~GrFmtReader();

    int   GetWidth()  { return m_width; };
    int   GetHeight() { return m_height; };
    bool  IsColor()   { return m_iscolor; };
    
    virtual bool  ReadHeader() = 0;
    virtual bool  ReadData( uchar* data, int step, int color ) = 0;
    virtual void  Close();

protected:

    bool    m_iscolor;
    int     m_width;    // width  of the image ( filled by ReadHeader )
    int     m_height;   // height of the image ( filled by ReadHeader )
    char    m_filename[_MAX_PATH]; // filename
};


///////////////////////////// base class for writers ////////////////////////////
class   GrFmtWriter
{
public:

    GrFmtWriter( const char* filename );
    virtual ~GrFmtWriter() {};

    virtual bool  WriteImage( const uchar* data, int step,
                              int width, int height,
                              bool isColor ) = 0;
protected:
    char    m_filename[_MAX_PATH]; // filename
};

/////////////////////////////////////////////////////////////////////////////////
class   GrFmtFilterFactory
{
public:

    GrFmtFilterFactory();
    virtual ~GrFmtFilterFactory() {};

    const char*  GetDescription() { return m_description; };
    int     GetSignatureLength()  { return m_sign_len; };
    virtual bool CheckSignature( const char* signature );
    virtual bool CheckExtension( const char* filename );
    virtual GrFmtReader* NewReader( const char* filename ) = 0;
    virtual GrFmtWriter* NewWriter( const char* filename ) = 0;

protected:
    const char* m_description;
           // graphic format description in form:
           // <Some textual description>( *.<extension1> [; *.<extension2> ...]).
           // the textual description can not contain symbols '(', ')'
           // and may be, some others. It is safe to use letters, digits and spaces only.
           // e.g. "Targa (*.tga)",
           // or "Portable Graphic Format (*.pbm;*.pgm;*.ppm)"

    int          m_sign_len;    // length of the signature of the format
    const char*  m_signature;   // signature of the format
};

typedef void* ListPosition;

class   GrFmtFactoriesList
{
public:

    GrFmtFactoriesList();
    virtual ~GrFmtFactoriesList();
    void  RemoveAll();
    bool  AddFactory( GrFmtFilterFactory* factory );
    int   FactoriesCount() { return m_curFactories; };
    ListPosition  GetFirstFactoryPos();
    GrFmtFilterFactory*  GetNextFactory( ListPosition& pos );
    virtual GrFmtReader*  FindReader( const char* filename );
    virtual GrFmtWriter*  FindWriter( const char* filename );

protected:

    GrFmtFilterFactory** m_factories;
    int  m_maxFactories;
    int  m_curFactories;
};
class  CvImageFilters
{
public:

    CvImageFilters();
    ~CvImageFilters();

    GrFmtReader* FindReader( const char* filename ) const;
    GrFmtWriter* FindWriter( const char* filename ) const;

    //const CvFilePath& Path() const { return (const CvFilePath&)m_path; };
    //CvFilePath& Path() { return m_path; };

protected:

    GrFmtFactoriesList*  m_factories;
};

////////////////////////////////////BITMAP////////////////////////////////////////////
enum BmpCompression
{
    BMP_RGB = 0,
    BMP_RLE8 = 1,
    BMP_RLE4 = 2,
    BMP_BITFIELDS = 3
};

// Windows Bitmap reader
class GrFmtBmpReader : public GrFmtReader
{
public:
    
    GrFmtBmpReader( const char* filename );
    ~GrFmtBmpReader();
    
    bool  ReadData( uchar* data, int step, int color );
    bool  ReadHeader();
    void  Close();

protected:
    
    RLByteStream    m_strm;
    PaletteEntry    m_palette[256];
    int             m_bpp;
    int             m_offset;
    BmpCompression  m_rle_code;
};


// ... writer
class GrFmtBmpWriter : public GrFmtWriter
{
public:
    
    GrFmtBmpWriter( const char* filename );
    ~GrFmtBmpWriter();
    
    bool  WriteImage( const uchar* data, int step,
                      int width, int height, bool isColor );
protected:

    WLByteStream  m_strm;
};
//... factory
class GrFmtBmp : public GrFmtFilterFactory
{
public:
    
    GrFmtBmp();
    ~GrFmtBmp();

    GrFmtReader* NewReader( const char* filename );
    GrFmtWriter* NewWriter( const char* filename );

};

///////////////////////////////JPG////////////////////////////////////////////
class GrFmtJpegReader : public GrFmtReader
{
public:
    
    GrFmtJpegReader( const char* filename );
    ~GrFmtJpegReader();

    bool  ReadData( uchar* data, int step, int color );
    bool  ReadHeader();
    void  Close();

protected:

    void* m_cinfo; // pointer to IJG JPEG codec structure
    void* m_jerr; // pointer to error processing manager state
    FILE* m_f;
};


class GrFmtJpegWriter : public GrFmtWriter
{
public:
    
    GrFmtJpegWriter( const char* filename );
    ~GrFmtJpegWriter();

    bool  WriteImage( const uchar* data, int step,
                      int width, int height, bool isColor );
};


class GrFmtJpeg : public GrFmtFilterFactory
{
public:
    
    GrFmtJpeg();
    ~GrFmtJpeg();

    GrFmtReader* NewReader( const char* filename );
    GrFmtWriter* NewWriter( const char* filename );
};