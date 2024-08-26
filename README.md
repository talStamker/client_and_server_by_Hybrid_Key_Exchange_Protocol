# Q1
## A
### File:Q1_a.py
Suppose you get a list of words a_list. For each word, you need to identify if it starts with the letter b.\
If so, the word should be changed so that it starts with an uppercase letter followed by lowercase letters, and\
Save the corrected words (and only them) in b_list.
#### Solution:
I set a_list to an example of input.\
b_list-I set to empty list.\
In for I check if the word is'nt empty for know if in word[0] there is something.\
If the word is empty I do'nt need it and if not i want check if in word[0] there is b and I know I can call it because there is something in it.
## B
### File:Q1_b.py
Is it possible to build a single Python line that will perform a similar operation and produce the requested list
list_b.
#### Solution:
I can write the code in a in one line.\
I set a_list to an example of input.\
Than, I write the line.
## C
### File:Q1_c.py
Given a line containing a sentence, for example:\
line = 'This line contains words and some of them have the letter o'\
Write a program that breaks the sentence into words, identify those that contain the letter o\
and move them completely to uppercase letters. The words must be written separated by commas\
and profit.
#### Solution:
line is like the example.\
words-the words in line set in list.\
upper_words is set to empty list.\
in for if word has "o" I enter it to upper_words in upper case.\
at the end I add , between the words with join.
# Q2
## A
### File:fruit.py
Create a class named AppleBasket whose constructor accepts two external arguments: a string\
representing color, and a number representing quantity. The constructor needs to initialize two instance variables: color_apple\
and quantity_apple. Write a method called increase that increases the quantity in that instance by 1
in each session. Also write a method called __str __
#### Solution:
Constructor- quantity can't be negative number and color can't be empty string, so i checked it
function:
increase-increases the quantity in that instance by 1.
__str__-for printing
## B
### File:fruit.py
Create an inheritance class called GreenAppleBasket for a basket of green apples, construct constructor
For this class that invokes the base class constructor with a Green data color.
#### Solution:
GreenAppleBasket get quantity_apple and construct AppleBasket with it and Green color.
# Q3
## A
### File:pros.py
In this question, the following classes must be constructed, with an appropriate constructor for each class:
- Users, who have a name and profession.
- Successor class according to different professions: engineers, technicians, politicians.
- Departments for different types of engineers must also be built: electrical engineers, computer engineers,
machines.
- The program will receive input from the users during runtime, which will allow it to add additional classes\
to the program at runtime, while receiving the following information from the user: the new class name, name\
A new variable for the class, the name of a new method for the class and it will be created at runtime (can be defined\
A variable with a random integer value, and a method that executes one print command.
- The user will be able to define a name of a mother class, so that the new class will be defined as an inheritance
from this department. Make sure that the parent class does exist.
- After the class has been created, the content of the field and the dictionary must be printed: ,__name_.newClass
 .newClass.__dict__.
#### Solution:
Clases:
- User
  - Class variable: name-string, profession-string
  - Constructor-Get name and profession, check they are not empty string, and build user.
- Technician
  - inherits from: User
  - Class variable: none (except user).
  - Constructor-Get name and build user with the profession Technician.
- Politician
  - inherits from: User
  - Class variable: none (except user).
  - Constructor-Get name and build user with the profession Politician.
- Engineer
  - inherits from: User
  - Class variable: type_engineer -string (except user).
  - Constructor-Get name and type_engineer.
    Check if type_engineer not empty string or there is no type of specialization in it. 
    Build user with the profession Engineer, and hold type_engineer.
- ElectricalEngineer
  - inherits from: Engineer.
  - Class variable: none (except Engineer).
  - Constructor-Get name and build user with the type_engineer ElectricalEngineer.
- ComputerEngineer
  - inherits from: Engineer.
  - Class variable: none (except Engineer).
  - Constructor-Get name and build user with the type_engineer ComputerEngineer.
- MechanicalEngineer
  - inherits from: Engineer.
  - Class variable: none (except Engineer).
  - Constructor-Get name and build user with the type_engineer MechanicalEngineer.\
 ##### main:
     - if the user want set more classes
      while True: 
     - I Get the input from the user
        -new_class_name = input("Please enter the name of the new class: ")
        base_class_name = input("Please enter the name of the base class (blank if none): ")
        method_name = input(f"Please enter the name of a new method for class {new_class_name}: ")
        attribute_name = input(f"Please enter the name of a new attribute for class {new_class_name}: ")
     - I Define the base class for building the type of class
        if base_class_name:
            base_class = type(base_class_name, (object,), {})
        else:
            base_class = object
     - I Create a new class dynamically for what the user put
        new_class = type(new_class_name, (base_class,), {
            attribute_name: None,  # Define the new attribute
            method_name: lambda self: print(f"{method_name} called")  # Define the new method
        })
     - I Print the __name__ and __dict__ of the new class\
        print(f"\nClass {new_class.__name__} created with base class: {base_class_name if base_class_name else 'None'}")\
        print(f"Class __name__ is: {new_class.__name__}")\
        print(f"Class __dict__ is: {new_class.__dict__}\n")\
     - Ask the user if they want to repeat or stop\  
        repeat = input("Would you like to create another class? (yes/no): ").strip().lower()\
        if repeat != 'yes':\
            print("Exiting...")\
            break\



