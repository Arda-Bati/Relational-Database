# ECE141b -- Spring 2019
## Assignment #7 - Due Tuesday, May 28 , 2019, 11:15pm
<HR>
  
In this assignment, you will implement the following table commands:

```
- select * from |table| where |filters| order by |field_name|
```

You will also add a `Block Cache` to your version of the `Storage` class, to improve system performance.

## Part 1 - Setting up your project

Copy all your files from Assignment 6 into this folder, __EXCEPT__ the following files.  

```
makefile 
students.json
```

NOTE: There is a minor bug in the  `updateStatement::parse` method. Please make the following (small change):

Change this (near line 635)...
```
if(6<aTokenizer.remaining()) {}
```

To this:
```
if(5<aTokenizer.remaining()) {}
```


## Part 2- Implement `SELECT ... from |table| ... order by |field|`

### Part 2a -- Add "reorder" to your `RowCollection` class (both the .hpp and .cpp)

```
RowCollection& reorder(const PropertyList &anOrder, Entity &anEntity);
```

If a user provides an "order by" specifier in the `select` command, the `SQLInterpreter` will call this method to cause the `RowCollection` to be reordered prior to being displayed.


### Part 2b -- Call `RowCollection::reorder()` from `SQLInterpreter::selectRows()`

As we discussed in class, if the user provides you with an `order by` clause, you need to update the `SQLInterpreter::selectRows()` method to call `RowCollection::reorder()` __before__ you show the selected rows. 

### Part 2c -- Implement row ordering

In this step, implement the logic in `RowCollection::reorder()` so that your rows are properly sorted (ascending order only) based on ONE given field from a user-given `select` statement.

## Part 3 - Add a `Block Cache` to your `Storage` class

We discussed this at length in two recent lectures. How you implement your `BlockCache` is up to you. You'll need to add block-caching to the `Storage` class methods: `readBlock()` and `writeBlock()`. 

Your block-caching strategy should allow for a variable size of cache (defined at compile time). Your cache should contain _at least_ 10 blocks, but you may store more if you wish.


## Testing  

For this assignment, you'll continue to use script-based tests to validate your work.  Testing `select ... order by ...` is easily done visually.  However, you should give some thought as to how to validate the your ordering code is working correctly using automated tests.

In our grader, we will test the performance of your `block cache` using a timer class that measures how long it takes to perform 1000's of reads/writes with caching enabled. We'll compare that against a baseline version of `Storage` that doesn't have caching enabled.

## Submit Your Work -- Tuesday, May 28, 11:15pm

- Update your students.json file.  
- Change the version number you report with the `version` command to 0.7
- Check in your work.  

Good luck everyone!
