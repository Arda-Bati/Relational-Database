# ECE141b -- Spring 2019
## Assignment #6 - Due Tuesday, May 21 , 2019, 11:15pm
<HR>
  
In this assignment, you will implement the following table commands:

```
- UPDATE |table| set fieldA="valueA", fieldB="valueB" where |filters|
- DELETE FROM |table| where |filters|
```

## Part 1 - Setting up your project

Unlike last week, where setting up your environment got _busy_, this week's assignment is really simple.  Copy all your files from Assignment 5 into this folder, __EXCEPT__ the following files.  It's unlikely you changed these files, but if you did, merge your changes into these newer versions.

```
SQLInterpreter.[*] //change to selectRows() api, and added updateRows method
SQLStatement.[*]   //bug fix with semi-colon in selectStatement; added updateStatement...
Filters.[*]        //bug fix with semi-colon in parseExpression
makefile 
students.json
```

## Part 2- Implement the `UPDATE |table|...` command

### Part 2a -- Added "updateRows" to your `SQLInterpreter` class (both the .hpp and .cpp)

We've added a new method `SQLInterpreter`:

```
StatusResult SQLInterpreter::updateRows(const std::string &aTableName,     
                                        const KeyValues &aKeyValues, 
                                        const Filters &aFilters);
```

If a user provides an "update..." command, a new `updateStatement` object is automatically created for you.  When that statement runs, it will call `SQLInterpreter::updateRows()`. 

### Part 2b -- Other Changes

As we discussed in class, here's what you need to do in order to update records:

1. Call `selectRows` to get a `RowCollection` based on the filters provided for the `update` command.  Note: make sure you keep track of the `block#` where each row was read from storage, because you'll need it in step 3.
2. Iterate each row in the `RowCollection`, and change the row data based values in the `aKeyValues` list
3. Re-save each changed row to storage, using to the same `block#` where the row was read from the storage file
4. Destroy the RowCollection when you're done with it

## Part 3 - Implement `DELETE FROM |table| WHERE |filters|`

Again, the system is mostly ready for you to implement this feature. If the user issues a `DELETE FROM |table|...` command, the system will automatically tokenize, parse, and create a DeleteStatement for you. When that is run, it will call `SQLInterpreter::deleteRows()`.   Here's where you need to add your own code:

1. Call `selectRows` to get a `RowCollection` based on the filters provided for the `update` command.  Note: make sure you keep track of the `block#` where each row was read from storage, because you'll need it in step 2.
2. Call the `Storage` class (via `Database`), to "release" the block number associated with each `Row` in your collection.
3. Destroy the RowCollection when you're done with it

## Testing  

For this assignment, you'll definitely want to use scripts to help with testing.  You can reuse your test script from last week, and add new commands to `update` and `delete` rows from your tables. 

Another key to testing is to make sure things work, even if you quit, restart, make changes, quit, and restart your code. When your application is quit, certain state is persisted. Starting, testing, stopping, and restarting helps you confirm that your system is working fully, including the storage of state of the storage system and the database. 

## Submit Your Work -- Tuesday, May 21, 11:15pm

- Update your students.json file.  
- Change the version number you report with the `version` command to 0.6
- Check in your work.  

Good luck everyone!

