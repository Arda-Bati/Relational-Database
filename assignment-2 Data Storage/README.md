# ECE141 - Spring 2019 -- Assignment #2
### Encoding/Decoding Dynamic Data Values
### Due, April 15 2019 @ 11:15pm

In this assignment you're going to be building out some fundamental classes (models) to deal with data handling. All the assignments after this one deal with core database programming.

### Quick note on working with project files

> In assignment one, you worked on a `Tokenizer` and simple `Statement` construction and command-routing.  Assignment2, and all subsequent assignments, will build upon the prior assignments.

> For most assignments,  new files (with new capabilities) will be introducted into the system. Keeping your files in sync from one assignment to the next could be huge hassle. This is because the project files you get from an assignment don't include the changes you made to files in your last assignment.

> Our answer to "keeping our sanity" as we incrementally update our system, is simple. With each new assignment, we expect that you'll take ALL your code files (.hpp and .cpp) from the _prior_ assignment, and copy them into your new project folder. You can safely overwrite all the files in your project, and the NEW project files won't have a counterpart in the prior assignment. 

Just follow this process:

1. Check out your new assignment from github classroom
2. Copy all the source files (.cpp, .hpp, student.json) from your LAST assignment, on top of files from NEW assignment

If you're using an IDE, make sure to rescan the files in your project folder to pick up new files added in the new assignment

## Part 1 -- Dynamic Types with a `Value` Class

One of the key capabilities of your database is to manage (store/retrieve) data in a variety of types (int, varchar, date,...). Python makes easy work of this by supporting dynamic typing. C++, on the other hand, is quite demanding about strong data types. Since your DB needs to support multiple data types, you'll need to provide a solution to deal with this problem.

Generally speaking, you have a few alternatives: 

1. A generic base class, with nested sub-classes for each actual type you want to handle (bool, int, float, unix_timestamp, varchar,...)
2. A dynamic class type that can intrinsically store any type of data you want to handle
3. Others alternatives you can think of?

Any of these strategies to data handling can be made to work, and it's up to you to decide which you want to implement. No alternative is perfect, and all will present trade-offs in terms of performance and complexity. 

### Requirements

1. You will create one or more classes (in `Value.hpp` and `Value.cpp`) that allow you to store data (in memory) that hold data-values described by a database table schema.  As we discussed in class, a schema describes all the fields in a given table. For a `Contact` schema, that might include properties like `[name, address, city, state, zip, phone, email, birthday...]`. 

2. Check the comments in the existing `Value.hpp` file to see what methods you MUST implement (also shown below)

3. Your `Value` class(es) must be support the following types: bool, int, float, unix_timestamp, varchar (others if you like). 

4. Your solution must not leak memory, and demonstrate reasonable memory and performance efficiency 

5. Implement `Value::debugDump(std::ostream &anOutput)` that outputs a description of the current state of your value, in the format shown below (in this example your Value is holding a float):

```
   type: float, value: 3.14
```

### Creating a Value class

The Value class will provide functionality to allow your to system to store dynamic user values.

```
  class Value {
  
    //YOU have to fill out this class according to your strategy for holding variant data types...
    
    //This class needs multiple ctors() for each type (book, timestamp, int, float, varchar)...
    //This class needs operator=, for each basic type...
    //This class needs conversion operator for each basic type

    size_t getSize();
    DataType getType();
    
    StatusResult become(DataType aType); //cause the value to assume given type

    friend bool operator < (const Value &arg1, const Value &arg2) {}  //your data must be comparable...

  };  
```

## Part 2 -- Complete the `Row` class

In your project files, you'll see a new class called `Row` (discussed in lecture). Each row is a collection of `Value` objects associated with a table, that conforms to a schema.  Part of this assignment requires that your Values can be stored (in-memory) in a row.  We use the row as the medium to hold values to be encoded into a storage block buffer, and to be decoded (and revived) into values FROM an encoded storage block. 

Data in a `Row` will occasionally change, and need to be encoded into a data structure and saved to persistent storage.  When that happens, a new `Block` object is created, using conversion constructor that accepts your `Row` object.  

For this step, you will implement the `Block::Block(const Row &aRow)` conversion constructor, found in `Storage.cpp`. 

## Part 3 -- Complete the `Block` class

If data can be **encoded**, it will need to be **decoded** to maintain symmetry. Encoded data will live in the storage system, encoded as objects of type `Block`.  When we read `Block`'s from storage, they will (usually) need to be **decoded** back into a `Row` for in-memory processing.  For this step, you will write the logic to **decode** data from a block.

Implement the `Row::Row(const Block &aBlock)` conversion constructor, found in `Row.cpp`.  This method is where raw data in the data buffer of a `Block` object is turned back into a `Row`, which contains a list of `Value` objects. 


## Part 4 -- Testing 

In the following section, we describe a general mechanism for testing your `Value` class. The examples we provide make assumptions about how you have implemented your class. If our assumptions are wrong, you are free to adjust the testing code to be compatible with your implementation of the `Value` class. The most important result of testing is validation output you write. Since that is merely text in an outputstream, the implementation details of your `Value` class should not matter. 

You may freely update the testing code, but your code must conform the _process_ of the testing scenarios we provide, and output logging information where we show in the example tests.

You should also note that the code we provide you may not compile, until you have completed the implementation details we describe in this assignment. 

### 4a -- Basic Testing 

#### -- Testing Constructors

In the `Assignment2Tests.cpp` file, there is a function called `runConstructorTests()`. There are comments in the function that illustrate the construction of variations of your Value class for each of the underlying types we must support. After constructing your class, you will emit a description of that current state of that object (type,value) to an outputstream by implementing `Value::debugDump(ostream):

```
  std::string theString("hello");
  //Value theValue1(theString);  //build varchar from std::string
  //theValue1.debugDump();
```

#### -- Testing Assignment operators

In the `Assignment2Tests.cpp` file, there is a function called `runAssignmentTests()`. There are comments in the function that illustrate how to assign values your Value class for each of the underlying types we must support. After each test, you will emit a description of that current state of that object (type,value) to an outputstream by implementing `Value::debugDump(ostream).


### 4b -- Testing "Encode" of Values

In the `Assignment2Tests.cpp` file, there are serveral tests we've prepared for you.  One of tests is called `runEncodeDecodeTests()`. Starting with a Row (collection of values), you will first write (encode) the values to a buffer. This method automatically generates a random collection of values and stores them in a Row.  You need implement the conversion constructor for Block `Block::Block(const Row &aRow)`, where encoding takes place. The code to do most of the legwork for testing is already written and waiting for you to use.

### 4c -- Testing "Decode" of Values

After you have successfully encoded data in the previous step, your next task is decode that same data, to recreate a copy of your original row. Do that by implementing the `Row::Row(const Block &aBlock)` conversion constructor in the `Row` class. 

Testing for this is already setup in the `runEncodeDecodeTests()` method in the `Assignment2Tests.cpp` file.

### 4d -- Comparing Rows

Our process so far:

1. The testing code auto-generated a row, containing 20 random values
2. The Row was encoded into a new block (in the `runEncodeDecodeTests` function in `Assignment2Tests.cpp`)
3. The encoded block is decoded _back_ into a `Row` object

Your final task is to write a `Row::operator==` that tests whether two `Row` objects are equivalent.  In our test, if our original row and our new row _are_ equivalent, it means our encode/decode phase worked correctly. This method must compare the number of values stored in the rows (original vs. decoded), along with the type and actual value of each `Value` object stored in the rows.


### Submit your assignment via github

Vlad-the-compiler is eagerly awaiting your submission.


