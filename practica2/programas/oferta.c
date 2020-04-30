#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

SQLRETURN oferta(SQLHDBC dbc, int argc, char const *argv[]){
	SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLINTEGER of_id;
    char query[512];
	int i;

	/* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	SQLExecDirect(stmt, (SQLCHAR*) "select max(of_id) from oferta", SQL_NTS);
	SQLBindCol(stmt, 1, SQL_C_SLONG, &of_id, sizeof(SQLINTEGER), NULL);

	if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))){
		of_id = 0;
	}

	/* Inserts a new offer */
	SQLCloseCursor(stmt);

	sprintf(query, "insert into oferta values ('%s', '%d', '%s', '%s')", argv[1], of_id + 1, argv[2], argv[3]);
	printf("%s\n", query);

	ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) printf("Oferta insertada\n");

	/* Relates offer and isbns */
	for (i = 4; i < argc; i++){
		SQLCloseCursor(stmt);

		sprintf(query, "insert into aplica values ('%d', '%s')", of_id + 1, argv[i]);
		printf("%s\n", query);

		ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
		if (SQL_SUCCEEDED(ret)) printf("Tupla insertada\n");
	}

	/* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return EXIT_SUCCESS;
}

int main(int argc, char const *argv[]){
	SQLHENV env;
    SQLHDBC dbc;
	SQLRETURN ret;

	if (argc < 5){
		printf("Use ./oferta <descuento> <de> <a> <isbn> <isbn> .... <isbn>\n");
		return 1;
	}

	/* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

	ret = oferta(dbc, argc, argv);
	
	
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

	return ret;
}
