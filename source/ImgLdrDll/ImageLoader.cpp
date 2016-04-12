#include "stdafx.h"
#include <ctype.h>

#include "ImageLoader.h"

IplImage* LoadFImage( const char* filename )
{
  	CvImageFilters  img_Filters;
    GrFmtReader* reader = 0;
    IplImage* image = 0;

    if( !filename || strlen(filename) == 0 )
        return NULL;

    reader = img_Filters.FindReader( filename );
    if( !reader )
        return NULL;

    if( !reader->ReadHeader() )
        return NULL;

    CvSize size;
    size.width = reader->GetWidth();
    size.height = reader->GetHeight();

    image = cvCreateImage( size, IPL_DEPTH_8U, 3 );

    if( !reader->ReadData( (unsigned char*)(image->imageData),
                           image->widthStep, 1 ))
    {
        cvReleaseImage( &image );
        return NULL;
    }
    delete reader;

    return image;
}

bool SaveFImage( const char* filename, const IplImage* arr )
{
 	CvImageFilters  img_Filters;

	int origin = 0;
    int did_flip = 0;
    GrFmtWriter* writer = 0;

    CvMat stub, *image;
    int channels;

    if( !filename || strlen(filename) == 0 )
        return false;

    image = cvGetMat( arr, &stub );
    origin = ((IplImage*)arr)->origin;
    channels = arr->nChannels;

	writer = img_Filters.FindWriter( filename );
    if( !writer )
        return false;
    if( origin )  {
        cvFlip( image, image, 0 );
        did_flip = 1;
    }

    if( !writer->WriteImage( image->data.ptr, image->step, image->width,
                             image->height, channels > 1 ))
        return false;

    delete writer;

    if( origin && did_flip )
        cvFlip( arr, (void*)arr, 0 );

    return true;
}


int LoadImageToBuf(const char *filename, IplImage *img, int sizeofbuf)
{
	GrFmtReader* reader = 0;

	CvImageFilters  img_Filters;
	reader = img_Filters.FindReader( filename );
	if( !reader ) {
		return IMG_LOAD_ERR;
	}
	
	if( !reader->ReadHeader() ) {
		delete reader;
		return IMG_LOAD_ERR;
	}

	CvSize size;
	size.width = reader->GetWidth();
	size.height = reader->GetHeight();
	if ((size.width+3)*size.height*3>sizeofbuf) {
		delete reader;
		return BUF_SIZE_SMALL;
	}
	
	//image = cvCreateImageHeader( size,IPL_DEPTH_8U,3);
	//image->imageData=buf;
	img->nSize=sizeof(IplImage);
	img->ID=0;
	img->nChannels=3;
	img->depth=IPL_DEPTH_8U;
	img->dataOrder=0;
	img->origin=0;
	img->width=size.width;
	img->widthStep=(img->width*3+3)/4*4;
	img->height=size.height;
	img->roi=NULL;
	img->maskROI=NULL;
	img->imageId=NULL;
	img->tileInfo=NULL;
	img->imageSize=img->widthStep*img->height;
	img->imageDataOrigin=img->imageData;

	if( !reader->ReadData( (uchar *) img->imageData, img->widthStep, true ))
	{
		delete reader;
		return IMG_LOAD_ERR;
	}
	delete reader;
	return LOAD_SUC;


}


CvImageFilters::CvImageFilters()
{
    m_factories = new GrFmtFactoriesList;

    m_factories->AddFactory( new GrFmtBmp() );
    m_factories->AddFactory( new GrFmtJpeg() );
}


CvImageFilters::~CvImageFilters()
{
    delete m_factories;
}


GrFmtReader* CvImageFilters::FindReader( const char* filename ) const
{
    return m_factories->FindReader( filename );
}


GrFmtWriter* CvImageFilters::FindWriter( const char* filename ) const
{
    return m_factories->FindWriter( filename );
}


GrFmtReader::GrFmtReader( const char* filename )
{
    strncpy( m_filename, filename, sizeof(m_filename) - 1 );
    m_filename[sizeof(m_filename)-1] = '\0';
    m_width = m_height = 0;
    m_iscolor = false;
}


GrFmtReader::~GrFmtReader()
{
    Close();
}


void GrFmtReader::Close()
{
    m_width = m_height = 0;
    m_iscolor = false;
}


GrFmtWriter::GrFmtWriter( const char* filename )
{
    strncpy( m_filename, filename, sizeof(m_filename) - 1 );
    m_filename[sizeof(m_filename)-1] = '\0';
}



GrFmtFilterFactory::GrFmtFilterFactory()
{
    m_description = m_signature = 0;
    m_sign_len = 0;
}


bool GrFmtFilterFactory::CheckSignature( const char* signature )
{
    return m_sign_len > 0 && signature != 0 &&
           memcmp( signature, m_signature, m_sign_len ) == 0;
}


static int GetExtensionLength( const char* buffer )
{
    int len = 0;

    if( buffer )
    {
        const char* ext = strchr( buffer, '.');
        if( ext++ )
            while( isalnum(ext[len]) && len < _MAX_PATH )
                len++;
    }

    return len;
}


bool GrFmtFilterFactory::CheckExtension( const char* format )
{
    const char* descr = 0;
    int len = 0;
        
    if( !format || !m_description )
        return false;

    // find the right-most extension of the passed format string
    for(;;)
    {
        const char* ext = strchr( format + 1, '.' );
        if( !ext ) break;
        format = ext;
    }

    len = GetExtensionLength( format );

    if( format[0] != '.' || len == 0 )
        return false;

    descr = strchr( m_description, '(' );

    while( descr )
    {
        descr = strchr( descr + 1, '.' );
        int i, len2 = GetExtensionLength( descr ); 

        if( len2 == 0 )
            break;

        if( len2 == len )
        {
            for( i = 0; i < len; i++ )
            {
                int c1 = tolower(format[i+1]);
                int c2 = tolower(descr[i+1]);

                if( c1 != c2 )
                    break;
            }
            if( i == len )
                return true;
        }
    }

    return false;
}



///////////////////// GrFmtFilterList //////////////////////////

GrFmtFactoriesList::GrFmtFactoriesList()
{
    m_factories = 0;
    RemoveAll();
}


GrFmtFactoriesList::~GrFmtFactoriesList()
{
    RemoveAll();
}


void  GrFmtFactoriesList::RemoveAll()
{
    if( m_factories )
    {
        for( int i = 0; i < m_curFactories; i++ ) delete m_factories[i];
        delete[] m_factories;
    }
    m_factories = 0;
    m_maxFactories = m_curFactories = 0;
}


bool  GrFmtFactoriesList::AddFactory( GrFmtFilterFactory* factory )
{
    assert( factory != 0 );
    if( m_curFactories == m_maxFactories )
    {
        // reallocate the factorys pointers storage
        int newMaxFactories = 2*m_maxFactories;
        if( newMaxFactories < 16 ) newMaxFactories = 16;

        GrFmtFilterFactory** newFactories = new GrFmtFilterFactory*[newMaxFactories];

        for( int i = 0; i < m_curFactories; i++ ) newFactories[i] = m_factories[i];

        delete[] m_factories;
        m_factories = newFactories;
        m_maxFactories = newMaxFactories;
    }

    m_factories[m_curFactories++] = factory;
    return true;
}


ListPosition  GrFmtFactoriesList::GetFirstFactoryPos()
{
    return (ListPosition)m_factories;
}


GrFmtFilterFactory* GrFmtFactoriesList::GetNextFactory( ListPosition& pos )
{
    GrFmtFilterFactory* factory = 0;
    GrFmtFilterFactory** temp = (GrFmtFilterFactory**)pos;

    assert( temp == 0 || (m_factories <= temp && temp < m_factories + m_curFactories));
    if( temp )
    {
        factory = *temp++;
        pos = (ListPosition)(temp < m_factories + m_curFactories ? temp : 0);
    }
    return factory;
}


GrFmtReader* GrFmtFactoriesList::FindReader( const char* filename )
{
    FILE*  f = 0;
    char   signature[4096];
    int    sign_len = 0;
    GrFmtReader* reader = 0;
    ListPosition pos = GetFirstFactoryPos();

    if( !filename ) return 0;

    while( pos )
    {
        GrFmtFilterFactory* tempFactory = GetNextFactory(pos);
        int cur_sign_len = tempFactory->GetSignatureLength();
        if( sign_len < cur_sign_len ) sign_len = cur_sign_len;
    }

    assert( sign_len <= (int)sizeof(signature) );
    f = fopen( filename, "rb" );

    if( f )
    {
        sign_len = fread( signature, 1, sign_len, f );
        fclose(f);

        pos = GetFirstFactoryPos();
        while( pos )
        {
            GrFmtFilterFactory* tempFactory = GetNextFactory(pos);
            int cur_sign_len = tempFactory->GetSignatureLength();
            if( cur_sign_len <= sign_len && tempFactory->CheckSignature(signature))
            {
                reader = tempFactory->NewReader( filename );
                break;
            }
        }
    }

    return reader;
}


GrFmtWriter* GrFmtFactoriesList::FindWriter( const char* filename )
{
    GrFmtWriter* writer = 0;
    ListPosition pos = GetFirstFactoryPos();

    if( !filename ) return 0;

    while( pos )
    {
        GrFmtFilterFactory* tempFactory = GetNextFactory(pos);
        if( tempFactory->CheckExtension( filename ))
        {
            writer = tempFactory->NewWriter( filename );
            break;
        }
    }

    return writer;
}


