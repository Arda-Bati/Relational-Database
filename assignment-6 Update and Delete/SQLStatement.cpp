//
//  SQLStatement.cpp
//  Datatabase4
//
//  Created by rick gessner on 4/13/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "SQLStatement.hpp"
#include "SQLInterpreter.hpp"
#include "RGTokenizer.hpp"
#include "Entity.hpp"
#include "Database.hpp"
#include "Value.hpp"
#include <exception>
#include "EntityDescriptionView.hpp"

namespace ECE141 {
  
  //Base class for SQL-oriented statements ...
  
  bool skipIf(Tokenizer &aTokenizer, Keywords aKeyword) {
    if (aTokenizer.more() && (aKeyword==Helpers::getKeywordId(aTokenizer.current().data))) {
      aTokenizer.next(); //eat the target...
      return true;
    }
    return false;
  }
  
  bool skipIf(Tokenizer &aTokenizer, TokenType aTokenType) {
    if(aTokenizer.more() && (aTokenizer.current().type==aTokenType)) {
      aTokenizer.next(); //eat the target...
      return true;
    }
    return false;
  }
  
  //---------------------------------------------------------------------------
  
  SQLStatement::SQLStatement(SQLInterpreter &anInterpreter, Keywords aStatementType)
    : Statement(aStatementType), name(""), interpreter(anInterpreter) {}
    
  StatusResult SQLStatement::parse(Tokenizer &aTokenizer) {return StatusResult{};}
    
  StatusResult SQLStatement::run(std::ostream &aStream) {return StatusResult{noError};}
  
  //USE: parse a comma-sep list of (unvalidated) field names...
  StatusResult SQLStatement::parseIdentifierList(StringList &aList, Tokenizer &aTokenizer) {
    StatusResult theResult{noError};
    
    while(theResult && aTokenizer.more()) {
      Token &theToken=aTokenizer.current();
      if(TokenType::identifier==aTokenizer.current().type) {
        aList.push_back(theToken.data);
        aTokenizer.next(); //skip identifier...
        skipIf(aTokenizer, TokenType::comma);
      }
      else if(skipIf(aTokenizer, TokenType::rparen)){
        break;
      }
      else theResult.code=syntaxError;
    }
    return theResult;
  }
  
  //** USE: get a list of values (identifiers, strings, numbers...)
  StatusResult SQLStatement::parseValueList(StringList &aList, Tokenizer &aTokenizer) {
    StatusResult theResult{noError};
    
    while(theResult && aTokenizer.more()) {
      Token &theToken=aTokenizer.current();
      if(TokenType::identifier==theToken.type || TokenType::number==theToken.type) {
        aList.push_back(theToken.data);
        aTokenizer.next(); //skip identifier...
        skipIf(aTokenizer, TokenType::comma);
      }
      else if(skipIf(aTokenizer,TokenType::rparen)) {
        break;
      }
      else theResult.code=syntaxError;
    }
    return theResult;
  }
  
  //USE: parse a fieldId=value list...
  StatusResult SQLStatement::parseIdValueList(KeyValues &aList, Tokenizer &aTokenizer) {
    StatusResult theResult{noError};
    
    while(theResult && aTokenizer.more()) {
      Token &theToken=aTokenizer.current();
      if(TokenType::identifier==theToken.type) {
        std::string theName(theToken.data);
        theToken = aTokenizer.peek(1); //should be operator...
        if(theToken.data=="=") {
          theToken = aTokenizer.peek(2); //should be value...
          aList[theName]=theToken.data;
          aTokenizer.next(3);
          theToken = aTokenizer.current(); //should be operator...
          skipIf(aTokenizer, TokenType::comma);
        }
      }
      else if(TokenType::keyword==theToken.type && Keywords::where_kw==theToken.keyword) {
        break;
      }
      else theResult.code=syntaxError;
    }
    return theResult;
  }
  
  //============ CREATE TABLE STATEMENT =============================

  CreateTableStatement::CreateTableStatement(SQLInterpreter &anInterpreter)
    : SQLStatement(anInterpreter, Keywords::create_kw)  {}
  
  //consume varchar decl [varchar "(" NUMBER ")"]
  StatusResult parseVarcharSize(Attribute &anAttribute, Tokenizer &aTokenizer) {
    Token &theToken=aTokenizer.current();
    if(TokenType::lparen==theToken.type) {
      aTokenizer.next();
      theToken=aTokenizer.current();
      if(TokenType::number==theToken.type) {
        int theSize = atoi(theToken.data.c_str());
        anAttribute.setSize(theSize);
        aTokenizer.next();
        if(skipIf(aTokenizer, TokenType::rparen)) {
          return StatusResult{noError};
        }
      }
    }    
    return StatusResult{syntaxError}; //means we have a problem...
  }
  
  StatusResult CreateTableStatement::parseAttributeOptions(Attribute &anAttribute, Tokenizer &aTokenizer) {
    bool options=true;
    StatusResult theResult{noError};
    
    while(theResult && options && aTokenizer.more()) {
      Token &theToken=aTokenizer.current();
      switch(theToken.type) {
        case TokenType::keyword:
          switch(theToken.keyword) {
            case Keywords::auto_increment_kw:
              anAttribute.setAutoIncrement(true);
              break;
            case Keywords::primary_kw:
              anAttribute.setPrimaryKey(true);
              break;
            case Keywords::not_kw:
            {
              aTokenizer.next();
              theToken=aTokenizer.current();
              if(Keywords::null_kw==theToken.keyword) {
                anAttribute.setNullable(false);
              }
              else theResult.code=syntaxError;
            }
            break;
              
            default: break;
          }
          break;
          
        case TokenType::rparen: //fall thru...
        case TokenType::comma:
          options=false;
          break;
        default:
          options=false;
          theResult.code=syntaxError;
      }
      aTokenizer.next(); //skip ahead...
    } //while
    return theResult;
  }
  
  //USE : parse an individual attribute (name type [options])
  StatusResult CreateTableStatement::parseAttribute(Attribute &anAttribute, Tokenizer &aTokenizer) {
    StatusResult theResult{noError};
    
    if(aTokenizer.more()) {
      Token &theToken=aTokenizer.current();
      if(Helpers::isDatatype(theToken.keyword)) {
        DataType theType = Helpers::getDataTypeForKeyword(theToken.keyword);
        anAttribute.setType(theType);
        aTokenizer.next(); //skip
        
        if(DataType::varchar_type==theType) {
          theResult=parseVarcharSize(anAttribute,aTokenizer);
        }
        
        if(theResult) {
          theResult=parseAttributeOptions(anAttribute, aTokenizer);
          if(anAttribute.isValid()) {
            attributes.push_back(anAttribute);
          }
          else theResult.code=invalidAttribute;
        }
        
      } //if
      else theResult.code=invalidAttribute;
    } //if
    return theResult;
  }
  
  //USE: consume collection of attributes for create table...
  StatusResult CreateTableStatement::parseAttributes(Tokenizer &aTokenizer) {
    StatusResult theResult{noError};
    
    while(theResult && aTokenizer.more()) {
      Token &theToken=aTokenizer.current();
      if(TokenType::identifier==theToken.type) {
        aTokenizer.next(); //skip identifier...
        Attribute theAttribute(theToken.data, DataType::varchar_type);
        theResult=parseAttribute(theAttribute, aTokenizer); //will already consume ')' for last attr...
      }
      else if(TokenType::semicolon==theToken.type) {
        aTokenizer.next(); //skip semi...
        break;
      }
      else theResult.code=syntaxError;
    }
  
    return StatusResult{noError};
  }
  
  // USE: create table |name| (|attrs|)...
  StatusResult CreateTableStatement::parse(Tokenizer &aTokenizer) {
    if(8<aTokenizer.remaining()) {
      Token &theToken = aTokenizer.peek(1); //should be 'table_kw'...
      if(Keywords::table_kw==theToken.keyword) {
        Token &theNextToken = aTokenizer.peek(2);
        if(TokenType::identifier==theNextToken.type) {
          aTokenizer.next(3); //skip CREATE TABLE {name}...
          name=theNextToken.data; //hang on to name of new table...
          
          //next expect '(', attribute-list, ')', ';'
          if(skipIf(aTokenizer, TokenType::lparen)) {
            return parseAttributes(aTokenizer);
          }
          return StatusResult{noError}; //no error
        }
      }
    }
    return StatusResult{Errors::syntaxError};
  }
  
  //USE: called to execute a CreateDB statement...
  StatusResult CreateTableStatement::run(std::ostream &aStream) const {
      //Convert stored statement.attributes to an entity...
    Entity *theEntity = new Entity(name);
    for(auto theAttr : attributes) {
      theEntity->addAttribute(theAttr);
    }
    return interpreter.createTable(theEntity); //DB will "own" the entity...
  }

  //============ DELETE STATEMENT =============================
  
  DeleteStatement::DeleteStatement(SQLInterpreter &anInterpreter)
    : SQLStatement(anInterpreter, Keywords::delete_kw), Filtered()  {}
  
  StatusResult DeleteStatement::parse(Tokenizer &aTokenizer) {
    if(2<aTokenizer.remaining()) {
      StatusResult theResult{noError};
      Token &theToken = aTokenizer.peek(1); //should be 'from'...
      
      if(Keywords::from_kw==theToken.keyword) {
        theToken = aTokenizer.peek(2);
        if(TokenType::identifier==theToken.type) {
          name=theToken.data; //tablename...
          
          if(Entity *theEntity = interpreter.getActiveDatabase()->getEntity(name)) {
            aTokenizer.next(3); //DELETE FROM {tablename}...
            
            while(theResult && aTokenizer.more()) { //look for WHERE or semicolon...
              theToken=aTokenizer.current();
              
              switch(theToken.type) {
                case TokenType::keyword:
                  if(Keywords::where_kw==theToken.keyword) {
                    aTokenizer.next(1); //yank the 'where' token...
                    theResult = parseFilters(aTokenizer, *theEntity);
                  }
                  else theResult.code=syntaxError;
                  break;
                  
                case TokenType::semicolon:
                  return theResult;
                  
                default:
                  aTokenizer.next(); //yank the name...
                  break;
              }
            }
            return theResult;
          }
          else theResult.code=unknownTable;
          
        }
      }
    }
    return StatusResult{syntaxError};
  }
  
  // USE: this statement describes an entity...
  StatusResult DeleteStatement::run(std::ostream &aStream) const {
    return interpreter.deleteRows(name, filters);
  }
  
  //============ DESCRIBE STATEMENT =============================
  
  DescribeStatement::DescribeStatement(SQLInterpreter &anInterpreter)
  : SQLStatement(anInterpreter, Keywords::update_kw)  {}
  
  StatusResult DescribeStatement::parse(Tokenizer &aTokenizer) {
    if(1<aTokenizer.remaining()) {
      Token &theToken = aTokenizer.peek(1); //should be 'into_kw'...
      if(TokenType::identifier==theToken.type) {
        name=theToken.data; //hang on to name of new table...
        aTokenizer.next(2); //skip identifier...
        return StatusResult{noError};
      }      
    }
    return StatusResult{Errors::unknownCommand};
  }
  
  // USE: this statement describes an entity...
  StatusResult DescribeStatement::run(std::ostream &aStream) const {
    return interpreter.describeTable(name, aStream);
  }
  
  //============ DROP STATEMENT =============================
  
  DropTableStatement::DropTableStatement(SQLInterpreter &anInterpreter)
  : SQLStatement(anInterpreter, Keywords::update_kw)  {}
  
  StatusResult DropTableStatement::parse(Tokenizer &aTokenizer) {
    if(2<aTokenizer.remaining()) {
      Token &theToken = aTokenizer.peek(1); //should be 'table_kw'...
      if(Keywords::table_kw==theToken.keyword) {
        Token &theNextToken = aTokenizer.peek(2);
        if(TokenType::identifier==theNextToken.type) {
          aTokenizer.next(3); //skip DROP TABLE {name}...
          name=theNextToken.data; //hang on to name of new table...
          return StatusResult{noError}; //no error
        }
      }
    }
    return StatusResult{Errors::unknownCommand};
  }
  
  StatusResult DropTableStatement::run(std::ostream &aStream) const {
    return interpreter.dropTable(name);
  }
  
  
  //========= INSERT STATEMENT =============================
  
  //USE: build a row (stored on statement) to be persisted later...
  StatusResult createRow(InsertStatement &aStatement, std::map<std::string,std::string> &aKVList, Entity &anEntity) {

    StatusResult theResult{noError};
    
    //every row contains all the fields specified in the entity, even if empty...
    KeyValues theData;

    const AttributeList &theAttributes=anEntity.getAttributes();
    for(auto &theAttr : theAttributes) {
      const std::string &theName=theAttr.getName();
      Value theValue("");
      if(aKVList.count(theName)) {
        theValue=aKVList[theName]; //use the value we were given...
      }
      theValue.become(theAttr.getType());
      theData[theName]=theValue;
      //std::cout << "size " << theData.size() << "\n";
    }

    Row theRow(theData);
    aStatement.rows.push_back(theRow);

    return theResult;
  }
  
  
  InsertStatement::InsertStatement(SQLInterpreter &anInterpreter)
    : SQLStatement(anInterpreter, Keywords::insert_kw)  {}
  
  //USE: consume (value1,value2,...), (...)... -- for building rows...
  StatusResult InsertStatement::makeRowsFromValueLists(Tokenizer &aTokenizer, Entity &anEntity, StringList &aFields) {
    StatusResult theResult{noError};
    
    while(theResult && (TokenType::lparen==aTokenizer.current().type)) {
      aTokenizer.next(); //skip lparen...
      StringList theValues;
      theResult=parseValueList(theValues, aTokenizer);
      
      skipIf(aTokenizer, TokenType::comma); //ignore comma...
      
      //let's combine the keys+values, so we can build a row...
      if(theResult) {
        
        size_t theSize=aFields.size();
        if(theValues.size()==theSize) {
          std::map<std::string, std::string> theKVs;
          for(size_t i=0;i<theSize;i++) {theKVs[aFields[i]]=theValues[i];}
          theResult = createRow(*this, theKVs, anEntity);
        }
        else theResult.code=keyValueMismatch;
      }
    }
    
    return theResult;
  }
  
  //USE: parse [insert INTO "table" (field, field...) VALUES (value, value...)
  StatusResult InsertStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult{syntaxError};

    if(2<aTokenizer.remaining()) {      
      Token &theToken = aTokenizer.peek(1); //should be 'into_kw'...
      if(Keywords::into_kw==theToken.keyword) {
        theToken = aTokenizer.peek(2);
        if(TokenType::identifier==theToken.type) {
          
          name=theToken.data; //hang on to name of new table...
          aTokenizer.next(3); //skip INSERT INTO {tablename}...
          
          if(Database *theDatabase=interpreter.getActiveDatabase()) {
            if(Entity *theEntity=theDatabase->getEntity(name)) {
              StringList theFields;
              if(skipIf(aTokenizer,TokenType::lparen)) {
                theResult=parseIdentifierList(theFields, aTokenizer);
                if(theResult && 0==theFields.size()) theResult.code=identifierExpected;
              }
              
              if(skipIf(aTokenizer, Keywords::values_kw)) {
                theResult=makeRowsFromValueLists(aTokenizer, *theEntity, theFields); //get values, and try to make rows...
                return theResult;
              }
              else theResult.code=syntaxError;
            }
            else theResult.code=unknownTable;
          }
          else theResult.code=unknownDatabase;
        }
      }
    }
    
    return theResult;
  }
  
  StatusResult InsertStatement::run(std::ostream &aStream) const {
    StatusResult theResult{noError};
    for(auto &theRow : rows) {
      theResult = interpreter.insertRow(theRow, name);
      if(!theResult) break;
    }
    return theResult;
  }
  
  
  //============ SELECT STATEMENT =============================
  
  SelectStatement::SelectStatement(SQLInterpreter &anInterpreter)
    : SQLStatement(anInterpreter, Keywords::select_kw), Filtered(), selectAll(false)  {}
  
  StatusResult SelectStatement::parseFields(Attribute &anAttribute, Tokenizer &aTokenizer) {
    StatusResult theResult(notImplemented);
    return theResult;
  }
  
  StatusResult SelectStatement::parseValues(Tokenizer &aTokenizer) {
    StatusResult theResult(notImplemented);
    return theResult;
  }
  
  //group by a,b... (NOT WORKING YET)...
  StatusResult SelectStatement::parseGroupBy(Tokenizer &aTokenizer) {
    Token &theToken = aTokenizer.peek(1); //should be '*' or field-list...
    if(Keywords::group_kw==theToken.keyword) {
      aTokenizer.next(1);
      theToken=aTokenizer.current();
      if(TokenType::keyword==theToken.type && Keywords::by_kw==theToken.keyword) {
        std::vector<Token> theTokens;
        StringList theList;
        StatusResult theResult=parseIdentifierList(theList, aTokenizer);
        if(theResult) {
          for(auto theIter = theTokens.begin(); theIter != theTokens.end(); theIter++) {
            //groupBy.push_back(new Property((*theIter).data));
          }
          return theResult;
        }
      }
    }
    return StatusResult{syntaxError};
  }
  
  //order by a,b...
  StatusResult SelectStatement::parseOrderBy(Tokenizer &aTokenizer) {
    Token &theToken = aTokenizer.current(); //order by
    if(Keywords::order_kw==theToken.keyword) {
      aTokenizer.next(1);
      theToken=aTokenizer.current();
      if(TokenType::keyword==theToken.type && Keywords::by_kw==theToken.keyword) {
        std::vector<Token> theTokens;
        StringList theFieldList;
        aTokenizer.next(); //skip by...
        StatusResult theResult=parseIdentifierList(theFieldList, aTokenizer);
        if(theResult) {
          for(auto &theField : theFieldList) {
            orderBy.push_back(Property(theField.data()));
          }
          return theResult;
        }
      }
    }
    return StatusResult{syntaxError};
  }
  
  StatusResult SelectStatement::parse(Tokenizer &aTokenizer) {
    
    if(3<aTokenizer.remaining()) {
      StatusResult theResult{noError};
      
      Token &theToken = aTokenizer.peek(1); //should be '*' or field-list...
      if(TokenType::operators==theToken.type) {
        selectAll=true;
      }
      else {
        theResult= SQLStatement::parseIdentifierList(fields, aTokenizer);
      }
      
      if(theResult) {
        theToken = aTokenizer.peek(2); //should be 'from_kw'...
        if(Keywords::from_kw==theToken.keyword) {
          theToken = aTokenizer.peek(3);
          if(TokenType::identifier==theToken.type) {
            name=theToken.data; //tablename...
            
            if(Database *theDatabase=interpreter.getActiveDatabase()) {
              if(Entity *theEntity = theDatabase->getEntity(name)) {
                aTokenizer.next(4); //SELECT (fields) FROM {tablename}...
                
                //scan for WHERE or select statement keywords (where, groupby, orderby...)
                while(theResult && aTokenizer.more()) { //look for WHERE, ORDER BY, GROUP BY...
                  theToken=aTokenizer.current();
                  switch(theToken.type) {
                    case TokenType::keyword:
                      
                      switch(theToken.keyword) {
                        case Keywords::where_kw:
                          aTokenizer.next(1); //yank the 'where' token...
                          theResult = parseFilters(aTokenizer, *theEntity);
                          break;
                          
                        case Keywords::order_kw:
                          theResult=parseOrderBy(aTokenizer);
                          break;
                          
                        case Keywords::group_kw:
                          theResult=parseGroupBy(aTokenizer);
                          break;
                          
                        default: break;
                      }
                      //aParser.skip(1); //yank the name...
                      break;
                      
                    case TokenType::semicolon:
                      return theResult;
                      
                    default:
                      aTokenizer.next(); //yank the name...
                      break;
                  }
                }
                return theResult;
              }
              else theResult.code=unknownTable;
            }
            else theResult.code=unknownDatabase;
          }
        }
        return theResult;
      }
    }
    return StatusResult{syntaxError};
  }
  
  
  StatusResult SelectStatement::run(std::ostream &aStream) const {
    return interpreter.selectRows(name, filters, orderBy);    
  }

  
  //============ SHOW TABLES STATEMENT =============================
  
  ShowTablesStatement::ShowTablesStatement(SQLInterpreter &anInterpreter)
  : SQLStatement(anInterpreter, Keywords::update_kw)  {}
  
  StatusResult ShowTablesStatement::parse(Tokenizer &aTokenizer) {
    if(1<aTokenizer.remaining()) {
      Token &theToken = aTokenizer.peek(1); //should be 'tables_kw'...
      if(Keywords::tables_kw==theToken.keyword) {
        aTokenizer.next(2); //skip identifier...
        return StatusResult{noError};
      }
    }
    return StatusResult{syntaxError};
  }
  
  // USE: this statement describes an entity...
  StatusResult ShowTablesStatement::run(std::ostream &aStream) const {
    return interpreter.showTables();
  }
  
  //============ UPDATE STATEMENT =============================
  
  UpdateStatement::UpdateStatement(SQLInterpreter &anInterpreter)
  : SQLStatement(anInterpreter, Keywords::update_kw)  {}
  
  StatusResult UpdateStatement::parseFields(Tokenizer &aTokenizer) {
    StatusResult theResult(notImplemented);
    return theResult;
  }
  
  StatusResult UpdateStatement::parse(Tokenizer &aTokenizer) {
    StatusResult theResult(syntaxError);
    
    if(6<aTokenizer.remaining()) {
      Token &theToken = aTokenizer.current(); //should be 'update'...
      if(Keywords::update_kw==theToken.keyword) {
        theToken = aTokenizer.peek(1); //should be identifier...
        if(TokenType::identifier==theToken.type) {
          name=theToken.data;
          
          if(Database *theDatabase=interpreter.getActiveDatabase()) {
            if(Entity *theEntity = theDatabase->getEntity(name)) {
              
              theToken = aTokenizer.peek(2); //should be 'set'...
              if(Keywords::set_kw==theToken.keyword) {
                aTokenizer.next(3);
                theResult=parseIdValueList(keyValues, aTokenizer);
                if(theResult) {
                  theToken = aTokenizer.current(); //check if where...
                  if(TokenType::keyword==theToken.type && Keywords::where_kw==theToken.keyword) {
                    aTokenizer.next(1); //yank the 'where' token...
                    theResult = parseFilters(aTokenizer, *theEntity);
                  }
                }
              }
            }
          }
        }
      }
    }

    return theResult;
  }
  
  StatusResult UpdateStatement::run(std::ostream &aStream)  const{
    return interpreter.updateRows(name, keyValues, filters);
  }
}
