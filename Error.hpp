#ifndef __ERROR_HPP__
#define __ERROR_HPP__

#include "Types.hpp"

enum Errors {
  undefined_op,
  undefined_typ,
  invalid_token,
  wrong_syntax,
  keyword,

  database_not_chosen,

  database_not_found,
  table_not_found,
	record_not_found,

  could_not_add_record,

  could_not_delete_table,
  could_not_delete_database,
  could_not_create_table,
  could_not_create_database,

  no_columns,

  zero_length_name,

  none,
};

struct Result {
  bool res;
  Errors error;
  Result(bool r, Errors e) : res(r), error(e) {}
  Result(): res(false) {}
};

/*template<typename T>
union Maybe {
  Errors error;
  T result;
};

template<typename T>
struct Result {
  Maybe<T> result;
  bool stat;
};*/

#endif

