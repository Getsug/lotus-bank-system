## lotus-bank-system
## About The Project

<p float="left">
   <img src="/assets/splash-screen.png" width="49%"/>&nbsp;&nbsp;
   <img src="/assets/login-page.png" width="49%"/>
</p><br>

<p float="left">
   <img src="/assets/admin-page.png" width="49%">&nbsp;&nbsp;
   <img src="/assets/customer-page.png" width="49%">
</p><br>


Lotus Bank system is a Linux terminal based program that is modelled after real bank systems in order to provide basic services such deposit, withdrawal and transfer of funds to clients.<br>
The system has two interfaces:

   - admin - accessed by authorized bank staff such as bank cashiers and other bank administrators
   - general user(client) interface - accessed by customers who are registered to Maze Bank.
### How to run
Install all the dependencies on the requirements.txt for the code to run smoothly.<br>
Compile

      gcc Banksystem.c -o bank -lcurses

Run

      ./bank
Default admin ID

      001
Default admin password

      1
    

### Note
You can modify the code to make the superadmin login credentials more secure
