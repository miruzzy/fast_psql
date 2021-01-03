#ifndef FAST_PSQL_H
#define FAST_PSQL_H
#include "libpq-fe.h"
#include <fstream>
using namespace std;

class fast_psql
{
public:
    fast_psql();                        // С дефолтным размером буфера ( 4 Мб )
    fast_psql( size_t size_ );          // Указываем размер буфера, который будет заполняться
    ~fast_psql();

    void SetParam( string host, int port, string DBname, string user_name, string user_password );
    bool Connect();
    bool Connect( string host, int port, string DBname, string user_name, string user_password );
    void Close();

    bool Exec( string query );          // Выполнение обычного запроса без возврата

    bool Prepare( string query );       // Подготавливаем БД к приёму строк в бинарном формате
    void SetColumns( int columns_ );    // Указываем, сколько столбцов будем записывать ( количество столбцов в входной строке )
    bool Exec();                        // Завершаем COPY-запрос
    void Clear();                       // Очищаем буфер
    string LastQuery();                 // Возвращает последний запрос
    string LastError();                 // Возвращает последнюю ошибку

    // Перегруженные функции для добавления данных
    void AddValue( int32_t value );
    void AddValue( int64_t value );
    void AddValue( int16_t value );
    void AddValue( double value );
    void AddValue( float value );
    void AddValue( char * value );
    void AddValue( bool value );


private:
    /// Данные для подключения к БД
    string  _host;
    int     _port;
    string  _DBname;
    string  _user_name;
    string  _user_password;

    PGconn *conn;
    pg_result *res;
    bool isOpen();

    string lastError;
    string lastQuery;
    string preparedQuery;

    bool isPrepared;
    char *buf;          // Буфер для данных
    size_t size;       // Размер буфера
    size_t currentSize;// Текущий размер буфера ( записанные данные для отправки )
    int16_t currentColumn;  // Текущий столбец
    int16_t columns;        // Всего столбцов для записи

    int all=0;

    // функции для управления буфером
    void b_init();
    void b_end();
    void b_newLine();
    void b_check();
    void exec_t();                        // Передаём очередную пачку данных
};

#endif // FAST_PSQL_H
