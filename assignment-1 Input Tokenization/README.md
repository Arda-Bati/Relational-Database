# ECE141b Spring 2019 - Assignment1
## Tokenization and Command Processing
### Due Monday, April 8 -- by 11:15pm!

## Preliminary tools
Building a relational database is a non-trival execise. A common pattern found in the most powerful automated systems, a RDMBS includes its own programming language called SQL (structured query language).  This assignment will lay the foundation for our own version of a SQL interpreter. 

## Part 1 -- Building a Tokenizer

For part 1, you need to enable command input tokenization in your environment. This will be used by the command parsing and routing system when a user provides input (via the terminal or a script).  The `Tokenizer` converts raw textual input into small units (called Tokens) suitable for parsing.  Here's a snippet from your `main.cpp` file, showing how your `Tokenizer` is invoked:

```
    std::string theUserInput;
    bool running=true;
    
    std::cout << std::endl << "> ";
    while (running && std::getline(std::cin, theUserInput)) {
      if(theUserInput.length()) {
        std::stringstream theStream(theUserInput);
        ECE141::Tokenizer theTokenizer(theStream);
        theTokenizer.tokenize();
        theResult=theProcessor.processInput(theTokenizer);
      }
      if(ECE141::userTerminated==theResult.code)
        running=false;
      else std::cout << std::endl << "> ";
    }
```    

After tokenization, your `CommandProcessor` will attempt to make sense of the given user input (tokens), and attempt to run code associated with the given input commands. (This is what you'll handle in Part2 of this assignment).  

### Tokenizer.hpp and Tokenizer.cpp

Please review the `Tokenizer` class found in the Tokenizer.hpp and Tokenizer.cpp files.  You'll notice that the interface is pretty straightforward (as shown in `Tokenizer.hpp`).  However, important functionality is missing. Sadly, the Tokenizer is having an existential crisis, and doesn't actually do anything. It REALLY wants to, but needs your help.  

One thing worth pointing out are the various keywords that comprise our various command languages. Please take a moment to review the complete list of keywords in the `keywords.hpp` file.

Your first task will implement the method, `Tokenizer::Tokenize`. This is necessary to convert user input from the command line (or a SQL script) into a form that your RDBMS can execute. Woot! 

### What kind of input do you need to tokenize?

There are really three types of input you have to be able to deal with:

1. Meta-commands that control the system, such as the `Quit` command which terminates the app. 
2. Database-level commands (like "Create database xxx", or "Show databases")
3. Data-oriented commands (like "Create table...", "Show tables", "insert (...) into tablename", ...)

Below you will find a list of inputs that your `Tokenizer` class will need to be able to handle to complete this assignment. 

#### Basic System Commands

Here is a list of sample system commands your tokenizer must be able to consume and tokenize. Each is a single keyword:

```
  version
  help
  quit
```

#### Database-level Commands

Database level commands are used to manage the database files used by your system. Below is a list of statements that are used by your database command processor (given in a future assignment). Your `Tokenizer` has to be able to tokenize these statements as well.  NOTE: below when you see a word in brackets, e.g. <databasename>, it's important to realize that the brackets are only shown here for clarity. They won't actually appear in your input buffer.  In reality, the word inside the brackets is usually interpreted as an "identifier", like the name of a table or database. 

```
  create database <dbname>  
  show databases
  use database <dbname>
  drop database <dbname>
```

#### Table-related Commands

Table related commands are the most common type of input your system will receive. A user of your RDMBS will generally choose a database, and input table-related commands to insert, update, search and delete records. Below is a list of tabale-related statements your `Tokenizer` will need to be able to process:

```
  create table <tablename> (id into auto_increment primary key, name varchar(20) not NULL);  
  insert into <tablename> (field1, field2,...) values (value1, value2...) 
  select * from <tablename> where field1=const1 
  drop table <tablename>
  describe <tablename>
  show tables
  update <tablename> set amount=3.14 where id=2
  delete from <tablename> where id = 2
```

## Part 2 -- Basic Command Routing

As we discussed in class, command routing refers to the process of:

1. tokenizing raw user input (from command line or script)
2. parsing given tokens into valid statements that may be routed and exectuted (interpreted)
3. routing the given statement to the proper command processor to be executed 
4. executing the given statement (perform associated work, log and report errors)

In `main.cpp`, user input is tokenized, and the tokenizer is passed to the `CommandProcessor::processInput` method.  The `processInput` method attempts to create and exectute one or more valid statements. First, it calls `getStatement`, where you will write code to examine (parse) the given tokens, and attempt to match them with a known statement (e.g. version, help, quit). If you got valid input, you'll create and return an associated statement, which `processInput` will then pass to a the `interpret(statement)` method for execution.  If a valid statement can't be created, then an error is returned.

NOTE: For this part of the assignment, you need to implement the `getStatement` and `interpret` methods in `CommandProcessor.cpp`.

### Version Command

Your first system command is "version". The version command will display the current version number of your system. For this class, your version number will reflect the week -- so your first version will be "ECEDB - version 1". 

In order to handle this command, you must do two things:
1. In `CommandProcessor::getStatement` you must be able to create and return a `Statement` object that is constructed with an argument that represents a "version" token. 
2. In `CommandProcessor::interpret` you need to recognize that the `Statement` object was initialized with a "quit" keyword, and then call a your own method to handle the Quit command.

### Help Command

The second system command is "help". Like most command-line tools, your "help" command will print a list of available commands to the standard output terminal.  For now, all you need to show is this:

```
  version -- shows the current version of this application
  help -- shows this list of commands
  quit -- terminates the execution of this DB application
```

Once again, you'll update `CommandProcessor::getStatement` to handle the "help" keyword (and token), and update the `CommandProcessor::interpet` method to deal with `Statement` objects that as initialized with a "help" keyword. 

### Quit Command

No doubt you guessed that the quit command will cause your applicate to terminate. The question is how?  Take another look at the main loop in your `main.cpp` file:

```
    std::string theUserInput;
    bool running=true;
    
    std::cout << std::endl << "> ";
    while (running && std::getline(std::cin, theUserInput)) {
      if(theUserInput.length()) {
        std::stringstream theStream(theUserInput);
        ECE141::Tokenizer theTokenizer(theStream);
        theTokenizer.tokenize();
        theResult=theProcessor.processInput(theTokenizer);
      }
      if(ECE141::userTerminated==theResult.code)
        running=false;
      else std::cout << std::endl << "> ";
    }
```

Notice that the main loop will continue as long as the `running` variable is true (as it is by default).  When the user enters the "quit" command, you are expected to create a quit-statement. The quit-statement will be executed by your `CommandProcessor::interpret(statement)` method. When that method sees a quit-statement, is returns a special error value:  `userTerminated`. That value is seen in your main function loop, and the application will automatically terminate.

Now go make it so by handling the quit-command in you `CommandProcessor::getStatement` and `CommandProcessor::interpret` methods. 

## Commit your code to github by Monday, April 8 -- by 11:15pm!

After you're code is working, you'll check it into github for grading.  Your homework is graded automatically by our hostile and partially sentient grader "Vlad-the-compiler".  Vlad is generally easy going, but becomes hostile when he has to wait for late assignemnts, as it interferes with watching Stephen Colbert. It's best not to get him upset. 

### Students.json

Remember to update the all the properties in your students.json file. That includes:

1. names of students in your group
2. what platform(s) you developed your code on (windows, mac, linux, amiga, beos, plan9, ...)
3. how much time you spend (collectively) on this assignment




