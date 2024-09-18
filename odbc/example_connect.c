#include <stdio.h>
#include <stdlib.h>
#include <sql.h>
#include <sqlext.h>

void check_error(SQLRETURN ret, SQLHANDLE handle, SQLSMALLINT type, char *msg) {
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        SQLCHAR sqlState[1024];
        SQLCHAR message[1024];
        SQLINTEGER nativeError;
        SQLSMALLINT messageLen;
        SQLGetDiagRec(type, handle, 1, sqlState, &nativeError, message, sizeof(message), &messageLen);
        printf("%s: SQL Error: %s, %s\n", msg, sqlState, message);
        exit(-1);
    }
}

int main() {
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt;
    SQLRETURN ret;

    // Allocate environment handle
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    check_error(ret, henv, SQL_HANDLE_ENV, "Allocate environment handle");

    // Set the ODBC version to 3.0
    ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void *)SQL_OV_ODBC3, 0);
    check_error(ret, henv, SQL_HANDLE_ENV, "Set ODBC version");

    // Allocate connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    check_error(ret, hdbc, SQL_HANDLE_DBC, "Allocate connection handle");

    // Connect to the data source using DSN
    ret = SQLConnect(hdbc, (SQLCHAR *)"DSN=mydsn", SQL_NTS, (SQLCHAR *)"username", SQL_NTS, (SQLCHAR *)"password", SQL_NTS);
    check_error(ret, hdbc, SQL_HANDLE_DBC, "Connect to the data source");

    printf("Connected to the database successfully!\n");

    // Allocate statement handle
    ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
    check_error(ret, hstmt, SQL_HANDLE_STMT, "Allocate statement handle");

    // Execute a query
    ret = SQLExecDirect(hstmt, (SQLCHAR *)"SELECT id, name FROM mytable", SQL_NTS);
    check_error(ret, hstmt, SQL_HANDLE_STMT, "Execute query");

    // Fetch and display the data
    SQLINTEGER id;
    SQLCHAR name[64];
    while ((ret = SQLFetch(hstmt)) == SQL_SUCCESS) {
        SQLGetData(hstmt, 1, SQL_C_LONG, &id, 0, NULL);
        SQLGetData(hstmt, 2, SQL_C_CHAR, name, sizeof(name), NULL);
        printf("ID: %d, Name: %s\n", id, name);
    }

    // Clean up
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);

    return 0;
}
