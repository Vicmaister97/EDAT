#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

SQLRETURN usuario_mas(SQLHDBC dbc, const char *scr_name, const char *full_name){
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLINTEGER cl_id;
    char query[512];

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    /* Looks if the screen name is already taken */
    sprintf(query, "select cl_id from cliente where scrname = '%s' and borrado = FALSE", scr_name);
	printf("%s\n", query);

    ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) printf("Query ejecutada\n");

    SQLBindCol(stmt, 1, SQL_C_SLONG, &cl_id, sizeof(SQLINTEGER), NULL);
	
	if (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
		printf("El screen name está ocupado.\n");
		return EXIT_FAILURE;	
	}
    
	/* Selects an unused id for the new user */
	SQLCloseCursor(stmt);
	SQLExecDirect(stmt, (SQLCHAR*) "select max(cl_id) from cliente", SQL_NTS);
	SQLBindCol(stmt, 1, SQL_C_SLONG, &cl_id, sizeof(SQLINTEGER), NULL);
	
	if (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
		/* Inserts a new user */
		SQLCloseCursor(stmt);

		sprintf(query, "insert into cliente values ('%d', '%s', '%s', NULL, NULL, current_date, FALSE)", cl_id + 1, scr_name, full_name);
		printf("%s\n", query);

		ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
		if (SQL_SUCCEEDED(ret)) printf("Usuario insertado\n");
	}
	
	/* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return EXIT_SUCCESS;
}

SQLRETURN usuario_menos(SQLHDBC dbc, const char *scr_name){
	SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    char query[512];

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	sprintf(query, "update cliente set borrado = TRUE where scrname = '%s' and borrado = FALSE", scr_name);
	printf("%s\n", query);

	ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) printf("Usuario borrado\n");

	/* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return ret;
}

int main(int argc, char const *argv[]){
	SQLHENV env;
    SQLHDBC dbc;
	SQLRETURN ret;

	if (argc < 3){
		printf("Use ./usuario + <screen_name> \"<full name>\"\n");
		printf("\tor ./usuario - <screen_name>\n");
		return 1;
	}

	/* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

	if (!strcmp(argv[1], "+")) ret = usuario_mas(dbc, argv[2], argv[3]);
	if (!strcmp(argv[1], "-")) ret = usuario_menos(dbc, argv[2]);
	
	
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

	return ret;
}
