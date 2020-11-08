# ECE141b -- Spring 2019
## Assignment #4 - Due Tuesday, May 7 , 2019, 11:15pm
<HR>
  
In this assignment, you will implement the first four table commands:

```
- show tables
- create table |name| |attributes|
- describe table |name|
- drop table |name|
```

## Part 1 - Getting your codebase ready for assignment4 

### New Files

Your new codebase (version 4) contains a number of new files related to table management:

```
- Attribute.[*] : represents one field in an Entity (which defines a table)
- BlockVisitor.hpp : a utility class used with Storage::eachBlock()
- Entity.[*] : represents a table
- EntityDescriptionView.[*] : used to "describe" a table
- Filters.[*] : Mixin for statements with expressions with where-clause (select, delete)
- RecordsView.hpp : used to show a list of records (e.g. select * from Names)
- SQLInterpreter.[*] : a new CommandProcessor for table-oriented statements
- SQLStatement.[*] : many Statement classes for various table commands
```

### 1.1: Copy files you have changed from Assignment3 to Assignment4

In Assignment3, you added/changed a few files that you'll want to bring over to Assignment4:

```
- Value.[*]
- DBManager.[*]
- FolderReader.[*]
- Row.[*]
- any other files you created yourself from Assignemnt3
```

### 1.2: In addition, some of the existing files have changed:

> If you have made changes to the files listed below, you need to make updates.  Copy your old version of the file you changed from Assignment3 to Assignment4. Then make the changes shown below on a "per-file" basis.  

#### 1.2.1: Update your Tokenizer

A new method called `remaining()` has been added to the `Tokenizer` class.  Make sure to __add__ `Tokenizer::remaining()` to `Tokenizer.hpp` if it isn't already there: 

```
size_t remaining() {return index<size() ? size()-index :0;}
```

#### 1.2.2: `CommandProcessor::processInput()` has changed. 

If you haven't made any changes to `CommandProcessor.cpp`, then __skip this step__, it's already been updated.

If you __have__ made changes to `CommandProcessor.cpp`, then copy your old version from Assignment3, and replace the `processInput` method with this version:

```
  StatusResult CommandProcessor::processInput(Tokenizer &aTokenizer) {
    if(Statement *theStatement=getStatement(aTokenizer)) {
      return interpret(*theStatement);
    }
    else if(next) {
      return next->processInput(aTokenizer);
    }
    return StatusResult{ECE141::unknownCommand};
  }
```

#### 1.2.3:  `Helpers.hpp` added a method called `toOperator(std::string)`:

If you haven't made changes to Helpers.hpp, then __skip this step__, it's already been updated.

If you __have__ made changes, then copy your old version (from Assignment3) over to Assignment4, and add this method:

```
    static Operators toOperator(std::string aString) {
      auto theIter = gOperators.find(aString);
      if (theIter != gOperators.end()) {
        return theIter->second;
      }
      return Operators::unknown_op;
    }
```

#### 1.2.4: Make some small changes to `Row.hpp`: 

Inside of your Row.hpp file, please add the following Friend classes to `Row`:

```
  friend class        RowCollection;
  friend class        Database;
  friend class        RecordsView;
  friend class        Filters;
```

Also, if you haven't done so already, please make sure that your `Row` class implements the following method:

```
  Row& addColumn(const std::string &aString, const Value &aValue);
```

#### 1.2.5: `DBStatement.cpp` was changed to use `Tokenizer::remaining()`:

If you haven't made any changes to `DBStatement.cpp`, then __skip this step__.

If you __have__ made changes then be aware we have provided you an updated version of `DBStatement.cpp`.  You 'll need to merge your changes from Assignment3 to Assignment4.  

#### 1.2.6: `Database.[*]` was changed to include many new methods that support Tables

If you didn't make any changes to your `Database.[*]` files, then __skip this step__.

If you changed `Database.[*]`, then copy `Database.[*]` files from Assignment3 to Assignment4.  
Next, add the following methods to the `Database` class:

```
- StatusResult    deleteRows(const Entity &anEntity, const Filters &aFilters);
- StatusResult    insertRow(const Row &aRow, const std::string &aTableName);
- StatusResult    selectRows(RowCollection &aColl, const Entity &anEntity, 
                             const Filters &aFilters, const StringList *aList=nullptr);
- StatusResult    updateRows(KeyValues &aList, const Entity &anEntity, const Filters &aFilters);
```

And finally, include these files in `Database.hpp`
```
#include "Entity.hpp"
#include "Filters.hpp"```
```

#### 1.2.7: The `main.cpp` was changed to fix a bug, and support command line scripts:

If you didn't change `main()` at all, then __skip this step__. 

If you did change `main()`, please review the updated version of `main.cpp` before copying your old file over from Assignment#3.  

## Part 2 - Implement `show tables`
Example usage:
```
show tables
```

Open the `SQLInterpreter`, and find the `showTables()` method.  You'll notice that it retrieves the active database, then calls `Database::showTables()`.  In the `Database::showTables()` method, you need to construct a `ShowTablesView` object (declared at the top of the database file). Next you'll call the `show()` method on that view.  Delete the view when you're done so you don't leak memory.

You'll notice this step was really simple. Statement parsing, creation, routing are handled for you. All you must do is create a view, and call the `show()` method on it. This is so you get comfortable with routing of SQLStatements, building views, and rendering them.  Apart from command construction and routing, the remaining tasks require that you implement things yourself. 

## Part 3 - Implement `create table |name| (|attributes|)`
Example usage:
```
CREATE TABLE Names (id integer AUTO_INCREMENT PRIMARY KEY, first_name VARCHAR(30) NOT NULL)
```

Tokenizing and parsing of a `create table...` statement, and routing them to be processed is non-trivial. Fortunately, all of this is handled for you by the new `SQLInterpreter` and `SQLStatement.[*]` classes. Please take a moment to open those files and get familiar with them before proceeding.

When a `create table...` statement is received, the `SQLInterpreter::createTable(Entity *anEntity)` method is called. This method retrieves the current database, and calls the `Database::createTable(Entity *anEntity)` method.  That is where _your task begins_.

Here's the high-level set of actions you must perform to create a table:

1. Given an entity, encode the attributes into a block
2. Call Storage.addEntity(block) to save that block
3. Presuming the storage system successfully saved the `Entity`, then cache the `Entity` (and storage block number) in the Database.entities collection.

Your primary challenge here is to write the encode/decode methods for `Entity`. This is the same concept that you handled for your `Value` class.  As before, you can use your own encode/decode process, or you can consider using the `BufferReader` and `BufferWriter` functionaliy provided to you in `MemoryStream.[*]`. 


## Part 4 - Implement `describe table |name|` 
Example usage:
```
decribe table Names
```

As with the other table-related commands, tokenization, parsing, Statement construction, and routing are all handled. Your task begins in the `SQLInterpreter::describeTable(std::string &name)` method.  As with other commands that present a result to the user, this command will create a new `EntityDescriptionView` that can describe a table.  Your task is to implement `EntityDescriptionView.show(std::ostream &anOutput)` method so that when someone asks for a table description, they see something like this example (similar to MySQL):

```
> use database foo
> describe Names
+-------------------+--------------+------+------+----------------+------------------+
| Field             | Type         | Null | Key  | Default        | Extra            |
+-------------------+--------------+------+------+----------------+------------------+
| id                | integer      | N    | PRI  |                | auto             | 
| first_name        | varchar      | Y    |      |                |                  | 
+-------------------+--------------+------+------+----------------+------------------+
```

## Part 5 - Implement `drop table |name|` 
Example usage:
```
drop table Names
```

Dropping a table is simple. When the user issues the `drop table |name|` command, `SQLInterpreter::dropTable(std::string &aName)` will eventually be called. This method will send the message `dropTable(name)` to the active database. This is where your task begins.  

When `Database::dropTable(std::string &aName)` is called, the following actions must be taken:

1. find the entity in the storage TOC
2.  if found, ask storage to drop the entity (which removes the entity from the TOC and marks the block as free)
3. find and remove all data blocks related to this entity (if you delete the table, the rows are removed too)
4. remove the entity from the Database.entities cache if it is present

## Testing These Commands

Naturally, you can use the system to test itself. First, you can simply type input commands, and let the system process them for you.  This works fine for small tasks, but can become tedius.

In version 4 of your database, the system supports command line scripts, if you choose to use them. A script file is a text file that contains system commands in the same format as you have been typing them. Note that if you use a script file, each command must be seprated by a semicolon. Here's an example:

```
create database foo;
show tables;
CREATE TABLE Names (id integer AUTO_INCREMENT PRIMARY KEY, first_name VARCHAR(30) NOT NULL);
show tables;
describe Names;
drop table Names;
show tables;
describe database foo;
drop database foo;
```

## Submitting your work

### Due Tuesday, May 7, 2019 - at 11:15pm (PST)

Make sure to change your CommandProcessor.cpp file so the `version` command reports version "0.4".
Make sure to update your `students.json` file before submitting your homework. Good luck!
