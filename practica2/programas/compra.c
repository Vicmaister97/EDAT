#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"


double fct_calc(SQLHDBC dbc, char *isbn) {
  SQLHSTMT stmt;
  SQLRETURN ret; /* ODBC API return status */
  double fct;
  char query[512];

    /* Allocate a statement handle */
  SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
  
  sprintf(query, "select exp(sum(ln(1-o.descuento/100))) from oferta as o, aplica as a where o.of_id=a.of_id and a.isbn='%s';", isbn);
  printf("%s\n", query);
  
  ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
  if (SQL_SUCCEEDED(ret)) printf("Query ejecutada\n");
  
  SQLBindCol(stmt, 1, SQL_C_DOUBLE, &fct, sizeof(double), NULL);
  
  if (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    return fct;	
  }
  else {
    return 1.0;
  }
  
}
    

SQLRETURN oferta(SQLHDBC dbc, int argc, char const *argv[]){
	SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLINTEGER venta_id, cl_id;
    char query[512];
	int i;

	/* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	/* Select unused id */
	SQLExecDirect(stmt, (SQLCHAR*) "select max(venta_id) from venta", SQL_NTS);
	SQLBindCol(stmt, 1, SQL_C_SLONG, &venta_id, sizeof(SQLINTEGER), NULL);

	if (!SQL_SUCCEEDED(ret = SQLFetch(stmt))){
		venta_id = 0;
	}

	/* Inserts a new sell */
	SQLCloseCursor(stmt);

	sprintf(query, "select cl_id from cliente where cliente.scrname = '%s';", argv[1]);
	printf("%s\n", query);

	SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
	SQLBindCol(stmt, 1, SQL_C_SLONG, &cl_id, sizeof(SQLINTEGER), NULL);

	if (SQL_SUCCEEDED(ret = SQLFetch(stmt))){
    	printf ("cl_id correcto\n");
	}

	else{
		cl_id = 0;
	}

	sprintf(query, "insert into venta values ('%d', '%s', '%d')", venta_id + 1, current_date, cl_id);
	printf("%s\n", query);

	ret = SQLExecDirect(stmt, (SQLCHAR*) query, SQL_NTS);
	if (SQL_SUCCEEDED(ret)) printf("Venta insertada\n");

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



}


int main(int argc, char const *argv[]){
	SQLHENV env;
    SQLHDBC dbc;
	SQLRETURN ret;

	if (argc < 3){
		printf("Use ./compra <screen_name> <isbn> <isbn> .... <isbn>\n");
		return 1;
	}

	/* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    //    char isbn[] = "0393028127";

    ret = compra(dbc, argc, argv);

    double fct = fct_calc(dbc,argv[2]);
    printf("%5.3f\n", fct);
	
    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

	return ret;
}
