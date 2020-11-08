# ECE141b -- Spring 2019
## Assignment #5 - Due Tuesday, May 14 , 2019, 11:15pm
<HR>
  
In this assignment, you will implement the following table commands:

```
- INSERT INTO |table| (fields) VALUES (...), (...)
- SELECT (fields) FROM |table| where |filters|
```

## Part 1 - Setting up your project

Unlike last week, where setting up your environment got _busy_, this week's assignment is really simple.  Copy all your files from Assignment 4 into this folder, __EXCEPT__ the following files.  If you changed your versions of these files, please update them them manually. 

```
makefile 
RGTokenizer.cpp //it has a bug fix in the isOperator() method (ONLY)
Storage.[*]    //makes it easier to change your block size (removed references to kBlockSize) 
Filters.[*]     //Improvements to expression handling; dealing with more logical operators...
```

## Part 2- Implement `insert into |table| (fields) VALUES (...), (...)`

As we discussed in class, in order to implement this feature, you'll have to make some design choices, and implement a few features:

1. Validating that given data conforms to Attribute constraints from Entity
2. Ensuring that each attribute has a value (at least a default value)
3. If the row requires an auto-incrementin primary key, obtaining one from the Entity
4. Encode the Row data into a storage block; with a default value for each field if the row doesn't have one
5. Store the block, and ensure that you have way to associate the block with the given Entity

## Part 3 - Implement `SELECT (fields) FROM |table| WHERE |filters|`

Again, the system is mostly ready for you to implement this feature. Here's where you need to add your own code:

1. Tokenizing, parsing, statement creation, command routing (done for you)
2. Iterating storage to retrieve rows (blocks) associated with the "Names" table
3. Applying filters to remove unwanted records
4. Assembling (returning) a "record set" (implemented as RowCollection)
5. Presenting selected rows in a tabular view (RecordsView?)
6. Destroy the RowCollection when you're done with it

## Testing  

For this assignment, you'll definitely want to use scripts to help with testing.  At a minimum, something like this:

```
create database foo;
CREATE TABLE Names (id integer AUTO_INCREMENT PRIMARY KEY, first_name VARCHAR(30) NOT NULL)
show tables
quit 
(now stop and restart your program)
use database foo
show tables
DESCRIBE Names
DESCRIBE database foo
insert into Names (first_name) VALUES ("rick"), ("chanhini")
select * from Names
select * from Names where id=2
quit
(now stop and restart your program)
use database foo
select * from Names
drop table foo   
show tables //should be 0 tables now...
quit
```

Another key to testing is to make sure things work, even if you quit, restart, make changes, quit, and restart your code. When your application is quit, certain state is persisted. Starting, testing, stopping, and restarting helps you confirm that your system is working fully, including the storage of state of the storage system and the database. 

## Submit Your Work -- Tuesday, May 14, 11:15pm

- Update your students.json file.  
- Change the version number you report with the `version` command to 0.5
- Check in your work.  

Good job!

