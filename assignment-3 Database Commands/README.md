# ECE141b -- Spring 2019
## Assignment #3 - Due Tuesday, April 30, 2019, 11:15pm

## 0. Setup your configuration

Before working with database files, you must configure the system to know _where_ you want the files to be stored. On our testing machines (mac/unix), we generally store these files in the `/tmp` folder. Windows users may want to choose a different location.  

### 0.1. Setting up your configuration

There are a few configuration settings you'll need to setup.

#1. Default Storage Path 

`Storage::getDefaultStoragePath()` is a method that provides a path to the location on your computer where you want your DB files to be stored.  Your first task is to choose a location for your files, and update this method to return that path as a `const char*`. 

#2. Indicate Your Platform

Next, you need to setup `FolderReader::getDefaultPlatform()` to return a suitable value for your machine. By default, this is set to `Mac`. Change this to match your platform: (mac, unix, windows).

### 0.2. Import `Value.hpp` and `Value.cpp`

Copy `Value.hpp/.cpp` files from Assignment2 into the Assignment3 directory.  Depending on how you wrote them, you may have to tweak them to use them for Assignement3.  We tried to keep the changes to a minimum.  All the processes are the same (`Value`s will be stored in memory, and encoded/decode from `Block` for persistence.

### 0.3. Import `Tokenizer.hpp/Tokenizer.cpp`

If your `Tokenizer` class is working great, then copy it over from Assignment2, into the Assignment3 directory.  If you're not happy with your `Tokenizer`, you can use the `RGTokenizer.hpp/.cpp` class we provided for you.  To use that class, you have to make sure it is imported into main(). Somewhere around line 50 in `main.cpp` is where we construct a `Tokenizer`. You can either use yours, or construct an instance of `RGTokenizer`.  The `RGTokenizer` was designed to handle tokenization challenges your class hasn't had to deal with yet, and it can save you time in later assignments. We prefer that you keep your `Tokenizer`. 

### NOTE

The underlying database platform has been extended and updated since version 2. It's been tested on unix/mac, but not much on Windows. If you see any windows specific problems, let us know right away and we'll correct the issues.

In particular, checkout the `FolderReader` class. We tried to create a windows compatible version. If anyone finds that the `WindowsFolderReader` methods don't work, please report the problem on Piazza.  If you're the first to fix it, we'll give you a few extra points.


## 1. `Create Database` Command 
### Implications
Creating a database refers to the process of creating a file to hold the database contents.  This file will be empty, except for the a table-of-contents block at block zero.  Opening/Creating/Reading/Writing block-stream storage files is handled by the `Storage` class. You may want to take a look at the `Storage.hpp` file now, to get familiar with the storage interface it provides. It's pretty small, about 15 methods. 

### Handling `DBManager::CreateDatabase()`

When you run your app, the main event loop is created (main.cpp), and an input console is available for your user to type commands. The first command we'll handle is `create table <name>`.  The `DBManager` class will automatically create a `CreateDatabase` statement object, route it, and attempt to run it.  You need to focus code starting in `DBManager::createDatabase`. 

Creating a new Database storage file is easy, in involves simply creating a new `Database` object with the right flags. 
The `Database` class can be constructed using either of these 'mode' tags: `CreateNewDatabase` or `OpenExistingDatabase`.  In this case, we want to use the tag: `CreateNewDatabase{}`.  

Here is an outline of the tasks you need to accomplish in the `DBManager::createDatabase(name)` method:

1. If activeDB isn't empty, call releaseDB (you can only have 1 activeDB at a time)
2. Create a new database on the heap (using `CreateNewDatabase` tag).
3. Set `activeDB` to the new DB you just created
4. Show the user a message indicating success or failure
5. Return the appropriate error code

### If this works (and it should)...

1. Assign the member-pointer `active` the value of your new `Database` object.  
2. Write a message to `std::cout` indicating the command succeeded.
3. Return `StatusResult{noError}`.


### If it failed...

Return an appropriate error code to the caller to indicate the problem. 

## 2. `Show Databases` Command 
### What this command does

The  `Show Databases` command is used to print a list of known database files to the console. It's a convenient way for your user to see which databases already exist. It's also useful in testing whether the `create database <name>` command worked.  We've already done the work of tokenization, automatically created a valid `ShowDatabasesStatement` object for you, and routed it. Start by looking at the method: `DBManager::ShowDatabases()`.

### `FolderView` 

This application uses the MVC design pattern, so whenever you want to present data to your user, you'll want to create a view.  Since we're trying to see a list of files in a folder, we want to create a `FolderView`. Fortunately, the `FolderView` class has been created for you (top of `DBManager.cpp`).  `FolderView` will use the `FolderReader` class to get a list of files, and then `FolderView` will present those files when you call the `FolderView::show()` method.

### `FolderReader` 

Showing a list of existing databases requires little more than checking your standard storage location for the presence of files with the extension `.db`. The `FolderReader` class was designed for this task: it scans a list of files in a given folder, and calls a `callback function` with the name of each file it finds, using the `Visitor` design pattern.

We have provided you a default implementation (for mac). If you're a unix or windows user, you need to customize/complete the implementation for `UnixFolderReader` or `WindowsFolderReader` (in `FolderReader.cpp`) so they can read a directory on your platform.  Don't worry --- every OS makes this easy (a very few lines of code).  Use the Google Machine or StackOverflow to find out how to read files in a directory for your platform. 

### Writing the `ShowDatabases` method

Now that we have working `FolderView` and `FolderReader` objects for your platform, all you need to do this create these objects, connect them, and let them do the work:

1. Call the `FolderReader::factory` method to create a `FolderReader` for your platform.
2. Construct a new folderView object (with given reader) on the heap
3. Tell the view to show itself on std::cout ...
4. Make sure you aren't leaking resources you've created when this method is finished

If all goes well, you'll see your list of database files like this example:
```
1. foo.db
2. bar.db
3. baz.db
... (others you created)...
```

## 3. `Describe Database` Command 
The  `describe database <name>` command is custom command we created that will "dump out" the contents of a database storage-stream file for debugging purposes. It's a convenient way for you to test if your database code is working correctly.  As before, we've already done the work of tokenization, automatically created a valid `DescribeDatabaseStatement` object for you, and routed it.

Take a look at the `DBManager::describeDatabase` method in `DBManager.cpp`.  You'll see that it accepts the name of a database file to describe, and attemps to load that `Database` object into memory. Presuming that works, it calls, `Database::describe(output)` to the `Database` object you loaded. After the `Database::describe(std::ostream)` finishes execution, the associated `Database` object is __deleted__ from memory.

Clearly, the real work is done by the `Database` class, so let's look at `Database::describe(std::ostream&)` in `Database.cpp`.  Just like before (in `show databases` command), we create a new view, this time `DescribeDatabaseView` and pass it a reference the database member `storage`.  The `storage` member manages low-level block storage, and is used by our view to iterate the blocks in a storage file. 

### `DescribeDatabaseView` 

Again, since this is an MVC application, we need to create a view, in this case, a `DescribeDatabaseView`. This view talks to the `Storage::each()` method, to iterate the list of blocks in a storage file.  You'll notice that `Storage::each()` method accepts a callback function. In this case the `DescribeDataView` provides the callback method itself, `DescribeDatabaseView::operator()`.  The `Storage::each` method iterates the list of blocks in the associated container, and sends each block to the `DescribeDatabaseView` object for processing. The view prints the block type (and other information) to the output stream for the user to see.

To complete this part of the assignment, implement the `Database::describe()` method, by doing the following:

1. Create a new `DescribeDatabaseView` object
2. Call show(outputStream)  on the new view
3. Don't forget to clean up any memory you used
4. Return the proper `StatusResult` to the caller

If all goes as planned, your output should look like this example:

```
0. block TOC
1. block Entity
2. block Data
3. block Data
...(more)...
```


## 4. `Drop Database` Command 
### What this command does

The  `drop database <name>` command is used to delete a database file (<name>.db) from your storage folder. As before, tokenization, statement creation, and routing are all done for you. Your job starts in the `DBManager::dropDatabase()` method in `DBManager.cpp`. You've been passed an argument that indicates the name of the database to be deleted.
  
The outline of your task is:

1. Call `Storage::getDatabasePath(name)` to get  a full path to the db file
2. Create a `FolderReader` for your platform, and call `FolderReader->exists(name)` to see if the system knows about the named file. If it doesn't, return an error.
3. Assuming the name is a real database, call std::remove(name) to delete the file
4. Make sure you clean up any objects you created
5. Display a message (std::cout) to your user indicating the you deleted their file
6. Return the appropriate error code



## 5. `Use Database` Command 
### What this command does

The  `use database <name>` command is used to load a named database file into memory, for use in subsequent commands. As before, tokenization, statement creation, and routing are all done for you. Your job starts in the `DBManager::useDatabase()` method in `DBManager.cpp`. You've been passed an argument that indicates the name of the database to be loaded/used.
  
The outline of your task is:

1. Try to load the database with the given name (Call `DBManager::load(name)`)
2. If that succeeds, set your internal `activeDB` member to this new DB* object
3. Display a message to your user saying it worked (if it did), otherwise, show an error message

NOTE: You can only have 1 open (active) database at a time. If another database is already active, make sure you release it first by calling `DBManager::releaseDB()`.


## Submit  Your Work
### Your assignment is due Tuesday, April 30, at 11:15pm

Make sure to update your students.json file. 
Vlad-the-compiler is eagerly awaiting your submission.
