#include "fast_psql.h"
#include <string.h>

//#define sparc

inline void WriteBytes (void *dst, void *src, int32_t size_)
{
#ifdef sparc
    memcpy (dst, src, size_);
#else
    for (int i = 0; i < size_; i++)
    {
        ((char *)dst)[i] = ((char *)src)[size_ - i - 1];
    }
#endif
}

fast_psql::fast_psql()
{
    size_t size_ = 4*1024*1024;
    buf = new char[size_];
    size = size_;
    res = 0;
}

fast_psql::fast_psql( size_t size_ )
{
    buf = new char[size_];
    size = size_;
    res = 0;
}

fast_psql::~fast_psql()
{
}

void fast_psql::SetParam( string host, int port, string DBname, string user_name, string user_password )
{
    _host = host;
    _port = port;
    _DBname = DBname;
    _user_name = user_name;
    _user_password = user_password;
}

bool fast_psql::Connect()
{
    string _conninfo = "host="+_host+" port="+to_string(_port)+" dbname="+_DBname+" user="+_user_name+" password="+_user_password+" connect_timeout=10";
    conn = PQconnectdb(_conninfo.c_str());
    if (conn)
        if (PQstatus(conn) == CONNECTION_OK)
            return true;
    return false;
}

bool fast_psql::Connect( string host, int port, string DBname, string user_name, string user_password )
{
    SetParam( host, port, DBname, user_name, user_password );
    return Connect();
}

void fast_psql::Close()
{
    if (conn)
        PQfinish (conn);
}

bool fast_psql::Exec( string query )
{
    lastError = "";
    lastQuery = query;
    if(!conn)
    {
        lastError = "No database connection.";
        return false;
    }
    if (res)
    {
        PQclear(res);
        res = 0;
    }
    res = PQexec(conn, query.c_str());
    ExecStatusType status = PQresultStatus(res);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    {
        lastError = PQresultErrorMessage(res);
        PQclear(res);
        res = 0;
        return false;
    }
    return true;
}

bool fast_psql::Prepare( string query )
{
    lastError = "";
    lastQuery = query;
    if(!conn)
    {
        lastError = "No database connection.";
        return false;
    }
    if (res)
    {
        PQclear(res);
        res = 0;
    }
    currentSize=0;
    preparedQuery = query;
    const char *cquery =  preparedQuery.c_str();
    res = PQexec(conn, cquery);
    b_init();
    return true;
}

void fast_psql::SetColumns( int columns_ )
{
    columns = columns_;
}

bool fast_psql::Exec()
{
    exec_t();
    lastError = "";
    lastQuery = "";
    if(!conn)
    {
        lastError = "No database connection.";
        return false;
    }

    if (PQresultStatus(res) == PGRES_COPY_IN)
    {

        if (PQputCopyEnd(conn, NULL) == 1)
        {
            PQclear(res);
            res = PQgetResult(conn);
            if (PQresultStatus(res) != PGRES_COMMAND_OK)
            {
                lastError = PQresultErrorMessage(res);
                PQclear(res);
                res = 0;
                return false;
            }
        }
        else
        {
            lastError = PQresultErrorMessage(res);
            PQclear(res);
            res = 0;
            return false;
        }
    }
    else
    {
        lastError = PQresultErrorMessage(res);
        PQclear(res);
        res = 0;
        return false;
    }
    PQclear(res);
    res = 0;
    return true;
}


void fast_psql::exec_t()
{
    if (PQresultStatus(res) == PGRES_COPY_IN)
        PQputCopyData(conn, buf, currentSize);

}

void fast_psql::Clear()
{
    currentSize = 0;
}

string fast_psql::LastQuery()
{
    return lastQuery;
}

string fast_psql::LastError()
{
    return lastError;
}

void fast_psql::b_init()
{
    char init_buf[] = {'P','G','C','O','P','Y','\n','\377','\r','\n','\0',
                       '\0','\0','\0','\0',
                       '\0','\0','\0','\0' };
    memcpy(buf,(void *)(&init_buf),19);
    currentSize = 19;
}

void fast_psql::b_end()
{
    unsigned char end_buf[] = { 0xff, 0xff };
    memcpy(buf+currentSize,(void *)(&end_buf),2);
    currentSize += 2;
}

void fast_psql::b_newLine()
{
    WriteBytes(buf+currentSize,(void *)(&columns),2);
    currentSize += 2;
}

inline void fast_psql::b_check()
{
    all++;
    if(currentColumn%columns==0)
    {
        currentColumn =0;
        if(currentSize>=size*0.75)
        {
            all=0;
            exec_t();
            currentSize=0;
        }
        b_newLine();
    }
}


void fast_psql::AddValue( int32_t value )
{
    b_check();
    currentColumn++;
    int32_t size = sizeof(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    WriteBytes(buf+currentSize,&value,size);
    currentSize += size;
}

void fast_psql::AddValue( int64_t value )
{
    b_check();
    currentColumn++;
    int32_t size = sizeof(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    WriteBytes(buf+currentSize,&value,size);
    currentSize += size;
}

void fast_psql::AddValue( int16_t value )
{
    b_check();
    currentColumn++;
    int32_t size = sizeof(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    WriteBytes(buf+currentSize,&value,size);
    currentSize += size;
}

void fast_psql::AddValue( double value )
{
    b_check();
    currentColumn++;
    int32_t size = sizeof(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    WriteBytes(buf+currentSize,&value,size);
    currentSize += size;
}

void fast_psql::AddValue( float value )
{
    b_check();
    currentColumn++;
    int32_t size = sizeof(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    WriteBytes(buf+currentSize,&value,size);
    currentSize += size;
}

void fast_psql::AddValue( char * value )
{
    b_check();
    currentColumn++;
    int32_t size = strlen(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    memcpy(buf+currentSize,value,size);
    currentSize += size;
}

void fast_psql::AddValue( bool value )
{
    b_check();
    currentColumn++;
    int32_t size = sizeof(value);
    WriteBytes(buf+currentSize,(void *)(&size),4);
    currentSize += 4;

    WriteBytes(buf+currentSize,&value,size);
    currentSize += size;
}
