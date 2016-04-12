#include "stdafx.h"

#include "ImageLoader.h"


// JPEG filter factory

GrFmtJpeg::GrFmtJpeg()
{
    m_sign_len = 3;
    m_signature = "\xFF\xD8\xFF";
    m_description = "JPEG files (*.jpeg;*.jpg;*.jpe)";
}


GrFmtJpeg::~GrFmtJpeg()
{
}


GrFmtReader* GrFmtJpeg::NewReader( const char* filename )
{
    return new GrFmtJpegReader( filename );
}


GrFmtWriter* GrFmtJpeg::NewWriter( const char* filename )
{
    return new GrFmtJpegWriter( filename );
}



/****************************************************************************************\
    This part of the file implements JPEG codec on base of IJG libjpeg library,
    in particular, this is the modified example.doc from libjpeg package.
    See otherlibs/_graphics/readme.txt for copyright notice.
\****************************************************************************************/

#include <stdio.h>
#include <setjmp.h>

#ifdef WIN32

#define XMD_H // prevent redefinition of INT32
#undef FAR  // prevent FAR redefinition

#endif

#if defined WIN32 && defined __GNUC__
typedef unsigned char boolean;
#endif

extern "C" {
	#include "jpeglib.h"
}

/////////////////////// Error processing /////////////////////

typedef struct GrFmtJpegErrorMgr
{
    struct jpeg_error_mgr pub;    /* "parent" structure */
    jmp_buf setjmp_buffer;        /* jump label */
}
GrFmtJpegErrorMgr;


METHODDEF(void)
error_exit( j_common_ptr cinfo )
{
    GrFmtJpegErrorMgr* err_mgr = (GrFmtJpegErrorMgr*)(cinfo->err);
    
    /* Return control to the setjmp point */
    longjmp( err_mgr->setjmp_buffer, 1 );
}


/////////////////////// GrFmtJpegReader ///////////////////


GrFmtJpegReader::GrFmtJpegReader( const char* filename ) : GrFmtReader( filename )
{
    m_cinfo = 0;
    m_f = 0;
}


GrFmtJpegReader::~GrFmtJpegReader()
{
}


void  GrFmtJpegReader::Close()
{
    if( m_f )
    {
        fclose( m_f );
        m_f = 0;
    }

    if( m_cinfo )
    {
        jpeg_decompress_struct* cinfo = (jpeg_decompress_struct*)m_cinfo;
        GrFmtJpegErrorMgr* jerr = (GrFmtJpegErrorMgr*)m_jerr;

        jpeg_destroy_decompress( cinfo );
        delete cinfo;
        delete jerr;
        m_cinfo = 0;
        m_jerr = 0;
    }
    GrFmtReader::Close();
}


bool  GrFmtJpegReader::ReadHeader()
{
    bool result = false;
    Close();

    jpeg_decompress_struct* cinfo = new jpeg_decompress_struct;
    GrFmtJpegErrorMgr* jerr = new GrFmtJpegErrorMgr;

    cinfo->err = jpeg_std_error(&jerr->pub);
    jerr->pub.error_exit = error_exit;

    m_cinfo = cinfo;
    m_jerr = jerr;

    if( setjmp( jerr->setjmp_buffer ) == 0 )
    {
        jpeg_create_decompress( cinfo );

        m_f = fopen( m_filename, "rb" );
        if( m_f )
        {
            jpeg_stdio_src( cinfo, m_f );
            jpeg_read_header( cinfo, TRUE );

            m_width = cinfo->image_width;
            m_height = cinfo->image_height;
            m_iscolor = cinfo->num_components > 1;

            result = true;
        }
    }

    if( !result )
        Close();

    return result;
}


bool  GrFmtJpegReader::ReadData( uchar* data, int step, int color )
{
    bool result = false;

    color = color > 0 || (m_iscolor && color < 0);
    
    if( m_cinfo && m_jerr && m_width && m_height )
    {
        jpeg_decompress_struct* cinfo = (jpeg_decompress_struct*)m_cinfo;
        GrFmtJpegErrorMgr* jerr = (GrFmtJpegErrorMgr*)m_jerr;
        JSAMPARRAY buffer = 0;
        
        if( setjmp( jerr->setjmp_buffer ) == 0 )
        {
            int planes = m_iscolor ? 3 : 1;
            int width_n = m_width * planes;

            jpeg_start_decompress( cinfo );

            buffer = (*cinfo->mem->alloc_sarray)((j_common_ptr)cinfo,
                                              JPOOL_IMAGE, width_n, 1 );

            for( ; m_height--; data += step )
            {
                jpeg_read_scanlines( cinfo, buffer, 1 );
                if( color )
                {
                    if( m_iscolor )
                        icvCvt_RGB2BGR_8u_C3R( buffer[0], 0, data, 0, cvSize(m_width,1) );
                    else
                        icvCvt_Gray2BGR_8u_C1C3R( buffer[0], 0, data, 0, cvSize(m_width,1) );
                }
                else
                {
                    if( m_iscolor )
                        icvCvt_BGR2Gray_8u_C3C1R( buffer[0], 0, data, 0, cvSize(m_width,1), 2 );
                    else
                        memcpy( data, buffer[0], m_width );
                }
            }
            result = true;
        }

        jpeg_finish_decompress( cinfo );
    }

    Close();
    return result;
}


/////////////////////// GrFmtJpegWriter ///////////////////

GrFmtJpegWriter::GrFmtJpegWriter( const char* filename ) : GrFmtWriter( filename )
{
}


GrFmtJpegWriter::~GrFmtJpegWriter()
{
}


bool  GrFmtJpegWriter::WriteImage( const uchar* data, int step,
                                   int width, int height, bool isColor )
{
    const int default_quality = 90;
    struct jpeg_compress_struct cinfo;
    GrFmtJpegErrorMgr jerr;

    bool result = false;
    FILE* f = 0;
    int channels = isColor ? 3 : 1;
    uchar* buffer = 0; // temporary buffer for row flipping
    
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = error_exit;

    if( setjmp( jerr.setjmp_buffer ) == 0 )
    {
        jpeg_create_compress(&cinfo);
        f = fopen( m_filename, "wb" );

        if( f )
        {
            jpeg_stdio_dest( &cinfo, f );

            cinfo.image_width = width;
            cinfo.image_height = height;
            cinfo.input_components = channels;
            cinfo.in_color_space = channels > 1 ? JCS_RGB : JCS_GRAYSCALE;

            jpeg_set_defaults( &cinfo );
            jpeg_set_quality( &cinfo, default_quality,
                              TRUE /* limit to baseline-JPEG values */ );
            jpeg_start_compress( &cinfo, TRUE );

            if( channels > 1 )
                buffer = new uchar[width*channels];

            for( ; height--; data += step )
            {
                uchar* ptr = (uchar*)data;
            
                if( channels > 1 )
                {
                    icvCvt_RGB2BGR_8u_C3R( data, 0, buffer, 0, cvSize(width,1) );
                    ptr = buffer;
                }

                jpeg_write_scanlines( &cinfo, &ptr, 1 );
            }

            jpeg_finish_compress( &cinfo );
            result = true;
        }
    }

    if(f) fclose(f);
    jpeg_destroy_compress( &cinfo );

    delete[] buffer;
    return result;
}



