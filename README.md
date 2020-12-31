# fast_psql

// Endglish by Google translate
This library is designed for fast loading of data into PostgreSQL DBMS using the 'COPY' request and transferring the buffer in binary format.

The main advantages of the library:
1. Fast loading
2. Data is not lost (for example double)

The library is written in C ++, which makes it possible to insert it into projects of other programming languages.

Library functionality is scarce. It is implemented specifically for loading data, so it does not support many standard functions.

-------------------------------------------------- -----------------
How to use the library.

1. Connect the library
#include "fast_psql.h"

2. Create an instance of the class
As a parameter, the function should be passed the size of the buffer that will be reserved
fast_psql p (5000);

3. Connect to the server
p.Connect (<host>, <port>, <dbname>, <user>, <password>);

4. Run the commands required for the current connection
p.Exec ("create temp table tmptable (id serial, testdata text);");

5. Prepare a 'COPY' request (documentation https://www.postgresql.org/docs/9.6/sql-copy.html)
p.Prepare ("COPY tmptable (testdata) FROM STDIN (format binary);");

6. Specify the number of columns to add
p.SetColumns (1);

7. Add data
7.1 Via functions
p.AddValue (<value>);
7.2 through arrows
p << <value>;

8. Finally, save the data
p.Exec ();

-------------------------------------------------- -----------------
An example of error handling:
if (p.Exec ("create table testtable5 (id serial, trtt text)"))
{
    qDebug () << "true" << p.LastQuery (). c_str () << p.LastError (). c_str ();
}
else
{
    qDebug () << "false" << p.LastQuery (). c_str () << p.LastError (). c_str ();
}

-------------------------------------------------- -----------------
If you are using a processor on the SPARC architecture:
In the file 'fast_psql.cpp' uncomment the line '// # define sparc'


In the future, I will update the functionality of the software. If you need something - write to me.
///-------------------------------------------------------------------------------------------------------

// Русская версия 

Данная библиотека предназначена для быстрой загрузки в СУБД PostgreSQL данных, используя запрос 'COPY'  и передачу буфера в бинарном формате.

Основные плюсы бибилиотеки:
1. Быстрая загрузка
2. Не теряются данные ( на примере double )

Библиотка написана на С++, что даёт возможность встаивать её в проекты не других языках программирования.

Функционал библиотеки скудный. Он реализован именно для загрузки данных,  поэтому множетво стандартных функций он не поддерживает.

-------------------------------------------------------------------
Как пользоваться библиотекой.

1. Подключите библиотеку 
#include "fast_psql.h"

2. Создайте экземпляр класса
В качестве параметра в функцию следует передавать размер буфера, который будет зарезервирован
fast_psql p(5000); 

3. Подключитесь к серверу
p.Connect(<host>, <port>,<dbname>, <user>, <password>);

4. Выполните команды, которые нужны для текущего подключения
p.Exec("create temp table tmptable( id serial, testdata text) ;");

5. Подготовьте 'COPY' запрос ( докуменатция https://www.postgresql.org/docs/9.6/sql-copy.html )
p.Prepare("COPY tmptable( testdata ) FROM STDIN (format binary); ");

6. Укажите количество столбцов  для добавления 
p.SetColumns(1);

7. Добавляйте данные 
7.1 Через функции
p.AddValue(<value>);
7.2 через стрелки
p << <value>;

8. В конце сохраните данные
p.Exec();

-------------------------------------------------------------------
Пример обработки ошибок:
if(p.Exec("create table testtable5 ( id serial, trtt text) "))
{
    qDebug()<<"true"<<p.LastQuery().c_str()<<p.LastError().c_str();
}
else
{
    qDebug()<<"false"<<p.LastQuery().c_str()<<p.LastError().c_str();
}

-------------------------------------------------------------------
Если используете процессор на архитектуре SPARС:
В файле 'fast_psql.cpp' расскоментируйте строку '//#define sparc'


В дальнейшем буду обновлять функционал программного обеспечения. Если что-то требуется - пишите мне.
