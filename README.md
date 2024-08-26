# Q1
## A
### File:Q1_a.py
Suppose you get a list of words a_list. For each word, you need to identify if it starts with the letter b.
If so, the word should be changed so that it starts with an uppercase letter followed by lowercase letters, and
Save the corrected words (and only them) in b_list.
Solution:
I set a_list to an example of input.
b_list-I set to empty list.
In for I check if the word is'nt empty for know if in word[0] there is something.
If the word is empty I do'nt need it and if not i want check if in word[0] there is b and I know I can call it because there is something in it.
## B
### File:Q1_b.py
Is it possible to build a single Python line that will perform a similar operation and produce the requested list
list_b.
Solution:
I can write the code in a in one line.
I set a_list to an example of input.
Than, I write the line.
## C
### File:Q1_c.py
Given a line containing a sentence, for example:
line = 'This line contains words and some of them have the letter o'
Write a program that breaks the sentence into words, identify those that contain the letter o
and move them completely to uppercase letters. The words must be written separated by commas
and profit.
Solution:
line is like the example.
words-the words in line set in list.
upper_words is set to empty list.
in for if word has "o" I enter it to upper_words in upper case.
at the end I add , between the words with join.
# Q2
## A
### File:fruit.py
Create a class named AppleBasket whose constructor accepts two external arguments: a string
representing color, and a number representing quantity. The constructor needs to initialize two instance variables: color_apple
and quantity_apple. Write a method called increase that increases the quantity in that instance by 1
in each session. Also write a method called __str __
Solution:
Constructor- quantity can't be negative number and color can't be empty string, so i checked it
function:
increase-increases the quantity in that instance by 1.
__str__-for printing
## B
### File:fruit.py
Create an inheritance class called GreenAppleBasket for a basket of green apples, construct constructor
For this class that invokes the base class constructor with a Green data color.
Solution:
GreenAppleBasket get quantity_apple and construct AppleBasket with it and Green color.
# Q3
## A
### File:pros.py
In this question, the following classes must be constructed, with an appropriate constructor for each class:
- Users, who have a name and profession.
- Successor class according to different professions: engineers, technicians, politicians.
- Departments for different types of engineers must also be built: electrical engineers, computer engineers,
machines.
- The program will receive input from the users during runtime, which will allow it to add additional classes
to the program at runtime, while receiving the following information from the user: the new class name, name
A new variable for the class, the name of a new method for the class and it will be created at runtime (can be defined
A variable with a random integer value, and a method that executes one print command.
- The user will be able to define a name of a mother class, so that the new class will be defined as an inheritance
from this department. Make sure that the parent class does exist.
- After the class has been created, the content of the field and the dictionary must be printed: ,__name_.newClass
 .newClass.__dict__.
Solution:
Clases:
- User
  - Class variable: name-string, profession-string
  - Constructor-Get name and profession, check they are not empty string, and build user.
- Technician
  - inherits from: User
    




