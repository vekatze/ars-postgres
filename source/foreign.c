#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include <postgresql/server/catalog/pg_type_d.h>

__attribute__((always_inline))
int neut_ars_postgres_v0_1_CONNECTION_OK() {
  return CONNECTION_OK;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_EMPTY_QUERY() {
  return PGRES_EMPTY_QUERY;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_COMMAND_OK() {
  return PGRES_COMMAND_OK;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_TUPLES_OK() {
  return PGRES_TUPLES_OK;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_COPY_OUT() {
  return PGRES_COPY_OUT;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_COPY_IN() {
  return PGRES_COPY_IN;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_BAD_RESPONSE() {
  return PGRES_BAD_RESPONSE;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_NONFATAL_ERROR() {
  return PGRES_NONFATAL_ERROR;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_PGRES_FATAL_ERROR() {
  return PGRES_FATAL_ERROR;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_OID_SIZE() {
  return sizeof(Oid);
}


__attribute__((always_inline))
int neut_ars_postgres_v0_1_INT8OID() {
  return INT8OID;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_FLOAT8OID() {
  return FLOAT8OID;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_TEXTOID() {
  return TEXTOID;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_TIMEOID() {
  return TIMEOID;
}

__attribute__((always_inline))
int neut_ars_postgres_v0_1_TIMESTAMPTZOID() {
  return TIMESTAMPTZOID;
}
