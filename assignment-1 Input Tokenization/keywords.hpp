//
//  keywords.hpp
//  Database
//
//  Created by rick gessner on 3/21/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef keywords_h
#define keywords_h

#include <string>
#include <map>
#include "Errors.hpp"

namespace ECE141 {
  
  const char left_paren = '(';
  const char right_paren = ')';
  const char left_bracket = '[';
  const char right_bracket = ']';
  const char left_brace = '{';
  const char right_brace = '}';
  const char colon = ':';
  const char comma = ',';
  const char semicolon = ';';
  const char apostrophe = '\'';
  const char quote = '"';
  const char star = '*';
  
  //This enum defines each of the keywords we need to handle across our multiple languages...
  enum class Keywords {
    add_kw=1, all_kw, alter_kw, and_kw, avg_kw, auto_increment_kw,
    between_kw, by_kw,
    char_kw, column_kw, count_kw, create_kw,
    database_kw, databases_kw, datetime_kw, decimal_kw, delete_kw,
    describe_kw, distinct_kw, double_kw, drop_kw, explain_kw,
    float_kw, foreign_kw, from_kw, group_kw, help_kw,
    in_kw, insert_kw, integer_kw, into_kw, key_kw,
    max_kw, min_kw, not_kw,  null_kw, or_kw, order_kw, primary_kw, quit_kw,
    references_kw, select_kw, set_kw, show_kw, sum_kw,
    table_kw, tables_kw,
    unknown_kw, unique_kw, update_kw, use_kw,
    values_kw, varchar_kw, version_kw, where_kw,
  };
  
  //This enum defines operators that will be used in SQL commands...
  enum class Operators {
    equal_op=1, notequal_op, lt_op, lte_op, gt_op, gte_op, between_op,
    or_op, nor_op, and_op, not_op, unknown_op
  };
  
  //This map binds a keyword string with a Keyword (token)...
  static std::map<std::string,  Keywords> gDictionary = {
    std::make_pair("add",       Keywords::add_kw),
    std::make_pair("all",       Keywords::all_kw),
    std::make_pair("alter",     Keywords::alter_kw),
    std::make_pair("and",       Keywords::and_kw),
    std::make_pair("avg",       ECE141::Keywords::avg_kw),
    std::make_pair("auto_increment", Keywords::auto_increment_kw),
    std::make_pair("between",   ECE141::Keywords::between_kw),
    std::make_pair("by",        ECE141::Keywords::by_kw),
    std::make_pair("char",      ECE141::Keywords::char_kw),
    std::make_pair("column",    ECE141::Keywords::column_kw),
    std::make_pair("count",     ECE141::Keywords::count_kw),
    std::make_pair("create",    ECE141::Keywords::create_kw),
    std::make_pair("database",  ECE141::Keywords::database_kw),
    std::make_pair("databases", ECE141::Keywords::databases_kw),
    std::make_pair("datetime",  ECE141::Keywords::datetime_kw),
    std::make_pair("decimal",   ECE141::Keywords::decimal_kw),
    std::make_pair("delete",    ECE141::Keywords::delete_kw),
    std::make_pair("describe",  ECE141::Keywords::describe_kw),
    std::make_pair("distinct",  ECE141::Keywords::distinct_kw),
    std::make_pair("double",    ECE141::Keywords::double_kw),
    std::make_pair("drop",      ECE141::Keywords::drop_kw),
    std::make_pair("explain",   ECE141::Keywords::explain_kw),
    std::make_pair("float",     ECE141::Keywords::float_kw),
    std::make_pair("foreign",   ECE141::Keywords::foreign_kw),
    std::make_pair("from",      ECE141::Keywords::from_kw),
    std::make_pair("group",     ECE141::Keywords::group_kw),
    std::make_pair("help",      ECE141::Keywords::help_kw),
    std::make_pair("in",        ECE141::Keywords::in_kw),
    std::make_pair("insert",    ECE141::Keywords::insert_kw),
    std::make_pair("integer",   ECE141::Keywords::integer_kw),
    std::make_pair("into",      ECE141::Keywords::into_kw),
    std::make_pair("key",       ECE141::Keywords::key_kw),
    std::make_pair("max",       ECE141::Keywords::max_kw),
    std::make_pair("min",       ECE141::Keywords::min_kw),
    std::make_pair("not",       ECE141::Keywords::not_kw),
    std::make_pair("null",      ECE141::Keywords::null_kw),
    std::make_pair("or",        ECE141::Keywords::or_kw),
    std::make_pair("order",     ECE141::Keywords::order_kw),
    std::make_pair("primary",   ECE141::Keywords::primary_kw),
    std::make_pair("quit",      ECE141::Keywords::quit_kw),
    std::make_pair("references", ECE141::Keywords::references_kw),
    std::make_pair("select",    ECE141::Keywords::select_kw),
    std::make_pair("set",       ECE141::Keywords::set_kw),
    std::make_pair("show",      ECE141::Keywords::show_kw),
    std::make_pair("sum",       ECE141::Keywords::sum_kw),
    std::make_pair("table",     ECE141::Keywords::table_kw),
    std::make_pair("unique",    ECE141::Keywords::unique_kw),
    std::make_pair("update",    ECE141::Keywords::update_kw),
    std::make_pair("use",       ECE141::Keywords::use_kw),
    std::make_pair("values",    ECE141::Keywords::values_kw),
    std::make_pair("varchar",   ECE141::Keywords::varchar_kw),
    std::make_pair("version",   ECE141::Keywords::version_kw),
    std::make_pair("where",     ECE141::Keywords::where_kw)
  };


  //a list of known functions...
  static std::map<std::string,int> gFunctions = {
    std::make_pair("avg", 10),
    std::make_pair("count", 20),
    std::make_pair("max", 30),
    std::make_pair("min", 40)
  };

  //a list of known punctuation...
  static std::map<char,int> gPunctuation = {
    std::make_pair(',', 1),
    std::make_pair(':', 2),
    std::make_pair(';', 3),
    std::make_pair('\'', 6),
    std::make_pair('(', 7),
    std::make_pair(')', 8),
    std::make_pair('[', 9),
    std::make_pair(']', 10),
    std::make_pair('{', 11),
    std::make_pair('}', 12),
    std::make_pair('/', 13),
    std::make_pair('*', 14),
    std::make_pair('=', 15),
    std::make_pair('+', 16),
    std::make_pair('-', 17)
  };

  //a list of known functions...
  static std::map<std::string,int> gOperators = {
    std::make_pair("not", 10),
    std::make_pair("and", 20),
    std::make_pair("or", 30),
    std::make_pair("between", 40),
    std::make_pair("<>", 10),
    std::make_pair(">", 20),
    std::make_pair("<", 30),
    std::make_pair("=", 40),
    std::make_pair("<=", 10),
    std::make_pair(">=", 20),
  };
   
}
 
#endif /* keywords_h */
