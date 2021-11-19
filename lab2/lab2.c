/*+
 * Module:  lab2.c
 *
 * Purpose: Skeleton solution to ELEC 377 Lab2.
 *
 * written by: Trevor Kirton - 20160010 - 18tk30
 * written by: Travis Stephney - 20172675 - 18tms9
 *
-*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

static struct task_struct * firstTask, *theTask;

int cnt;

//#define PAGE_SIZE=10;

int my_read_proc(char * page, char **start, off_t fpos, int blen, int * eof, void * data){

    int Kpage=(PAGE_SIZE>>10);

    int numChars;
    if (fpos == 0){

        //this is the code to printn 'Hello World' used to demonstrate printing
        //numChars=sprintf(page, "Hello");
        //numChars+=sprintf(page+numChars, " World\n");

        numChars=sprintf(page, "PID\tUID\tVSZ\tRSS\n"); // print column headers

        theTask=&init_task; //set theTask to the PCD of the init_task

        while((theTask->pid)==0){//keep moving to next task until a task is reached that does not have a PID of 0
            theTask=theTask->next_task;
        }
        firstTask=theTask; //store first task since all the tasks are linked cyclicly. Each task will be compared to this one 
                           //to determine if the program has looped back to the begining

        numChars+=sprintf(page+numChars,"%4d\t%4d\t", theTask->pid, theTask->uid); //print the task's pid and uid. Use %4d to print
                                                                                   //in 4 charector wide field

        if (theTask->mm==NULL){//mm stores virtual memory information. If process is a kernel process then it does not use virtual
                               //memory so its mm will be NULL. In this case print a 0 for VSZ and RSS
            numChars+=sprintf(page+numChars, "%4d\t%4d\n", 0, 0);
        }else{ //if mm is not NULL then print the corresponding values for VSZ and RSS
            numChars+=sprintf(page+numChars, "%4d\t%4d\n", theTask->mm->total_vm*Kpage, theTask->mm->rss*Kpage);
        }

        do{//this loop keeps moving to the next task until it comes across a task with a PID that is not 0. This is the next task
           //to be printed
            theTask=theTask->next_task;
        }while((theTask->pid)==0);

    } else {
        if (theTask==firstTask){//check to see if the program has looped around to the first task again. If so then break 
                                //out of loop
            *eof = 0;
            *start = page;
            return 0;
        }
        
        numChars=sprintf(page,"%4d\t%4d\t", theTask->pid, theTask->uid);//print PID and UID for next task

        if (theTask->mm==NULL){//once again, check if task is a kernel process and print memory information accordingly 
            numChars+=sprintf(page+numChars, "%4d\t%4d\n", 0, 0);
        }else{
            numChars+=sprintf(page+numChars, "%4d\t%4d\n", theTask->mm->total_vm*Kpage, theTask->mm->rss*Kpage);
        }
        do{//move to next task
            theTask=theTask->next_task;
        }while((theTask->pid)==0);
    }
    *eof = 1;
    *start = page;
    return numChars;
}

int init_module(){
   struct proc_dir_entry * proc_entry;

    //code from lab 0:

    // create a proc ntry in the directory
    printk("<> Loading lab0 into the kernel\n");

    // create a proc entry directly in /proc
    proc_entry = create_proc_entry("lab2",0444,NULL); //create new proc entry with the first argument as the name, second argument 
    //specifies permission of the file (0444 means readable by anyone and cannot be written to) the final argument gives the 
    //directory and NULL means /proc directory
    if (proc_entry == NULL){
       return -1;
    }
    proc_entry->read_proc = my_read_proc;
    return 0;

}

void cleanup_module(){
        remove_proc_entry("lab2", NULL);
        printk("<> lab2 - removed module from the kernel\n");

}
