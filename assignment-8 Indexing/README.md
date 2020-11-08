# ECE141b -- Spring 2019
## Assignment #8 - Due Thursday, June 6 , 2019, 11:15pm
<hr>

In this assignment, you will implement indexing for the primary key of new tables.


## Part 1 - Setting up your project

Copy all your files from Assignment 7 into this folder, __EXCEPT__ the following files.  

```
makefile 
students.json
```

## Part 2- Implement Indexing

### Part 2a -- Create an Index class

In lecture, we discussed a solution (shown below) that addresses requirement of homogenous types in STL containers. You are not required to use this approach, and are encouraged to develop your own original solution.

```
class Index {
  public:
  
    Index(const std::string &aField, uint32_t aTableId, DataType aKeyType)
      : field(aField), tableId(aTableId), keyType(aKeyType) {}
    
    Index&   addKeyValue(const Value &aKey, uint32_t aValue);   
    Index&   removeKeyValue(const Value &aKey);
    bool     contains(const Value &aKey);
    uint32_t getValue(const Value &aKey);
    
    using IndexType = std::map<value, uint32_t>; //c++ typedef...

  protected:    
    IndexType   index;
    std::string field;    //name of the field being indexed
    DataType    keyType;  //what Value datatype are we using for this field?
    uint32_t    tableId;
  };
```

### Part 2b -- Add an "indexes" data member (container) 

Somewhere in your application, you need a collection of Indexes that are in use. There are two excellent candidates.  First, you could store your indexes on the `Database`, in the same way that you hold a list of `Entity` objects.  Second, you could store the indexes collection on the associated `Entity'. You may have a better idea, and you're free to choose whatever approach suits you.

Choose a suitable STL container type to hold the indexes you load and use during the execution of commands. Consider handling loading and using indexes in the same way that the `Database` handles __entities__ today; they get lazy-loaded on demand when your user issues commands that require and `Entity` to be loaded.

### Part 2c -- Creating and Loading Your Indexes

It's completely up to you to determine when/how to create and load your index files. A simple approach might be to automatically create an index for the primary key of a table (that includes a primary key) when the table is being created. Then, you could automatically load the primary key index when that `Entity` is loaded. 

### Part 2d -- Add a method to retrieve an Index for a table/field

As your user adds/removes records, you need to update the associated index. You'll need a method that creates and loads the associated Index (if it exists). How you do that is up to you, but an example is shown below. How this method does its job depends on how you decide to store your index data (see part 2g).  For example, if you decide to store in-memory `Indexes` in the `Database`, you might offer the method shown below. 

```
class Database {
 ...skip a bunch of stuff...
   
   Index* getIndex(Entity *anEntity, const std::string &aFieldName);
   
 ...
};
```

> NOTE: This is just an alternative, and shouldn't be seen as "the way" to do this.

### Part 2e -- Adding Records to an Index

When a user issues an insert command (example below), one or more records is added to the database. 

```
insert into Names (first_name) values ("bob"), ("joe")
```

Your `Datbase::insertRow()` method already knows how to create a unique id for each row, and how to add the row to the storage system in a block. In this step, you will capture the blocknumber and record-id values, and add them to the primary-key index for that table. If you used an Index interface like the one we described in class (shown above), you might update `Database::insertRow()` like this:

```
...
      StatusResult  theResult = theEntity->encodeData(theRow.columns, theBlock);
      if(theResult) {
         theResult=storage.addBlock(theBlock);
         if(Index* theIndex=getIndex(theEntity, "id")) { //see step 2d
           theIndex->addKeyValue(theNewRowBlockNumber, theNewRowId);   
         }
      }
...
```

> NOTE: For grading, all the tables we ask your DB to create will include a primary key (autoincrementing integer).


### Part 2f -- Using your index

All of the work you've completed in this assignment is aimed at one thing: making retrieval of records faster. This step in the assignment is to improve your `Database::selectRows()` method to use an Index. Rather than using the `Storage::eachBlock()` method of finding and loading records, use the block numbers from your index to directly access data rows for the associated table.

It's possible that most of the code you'll write to use an `Index` for quick row retrieval will be added to `Database::selectRows()`. In our test system, we added class that uses the __facade pattern__ to load block/rows from `Storage`. If an Index exists for the table/field, the __facade__ uses that Index to load rows directly. Otherwise, the __facade__ uses the `Storage::eachBlock()` method to scan for associated data blocks, as it does today. 


### Part 2g -- Saving Index Data

In this step, you will store your index values for a given table. In lecture we talked about several approaches you could take to managing your index:

1. As a separate json file
2. As a custom block-type in the storage system

Which approach you take is up to you, as both approaches have merit. Note, however, that if you choose to use external index files, managing the files is your responsibility. In particular, if a user deletes a table that has an external index file, you must remember to automatically delete the index file as well. 

In our test system, we automatically save indexes that have changed when the database is closed. This occurs when the user "uses" a different database, or when the user issues a "quit" command to terminate execution of the app.  In either event, we rely on the `~Database()` destructor to signal us to save indexes that have been changed. 

## Part 3 -- Submitting Your Work
### Due June 6, 2019 at 11:15pm

As usual, please submit your final work using git.  Good luck.







